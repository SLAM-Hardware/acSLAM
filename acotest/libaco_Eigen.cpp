#include <chrono>
#include <Eigen/Dense>
#include "aco.h"
#include <stdio.h>
#include <iostream>
#include "aco_assert_override.h"

using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using namespace std;
using namespace Eigen;
struct GaussSolver
{
    MatrixXd A;
    VectorXd b;
    VectorXd x;
    GaussSolver(int _n, int _m)
    {
        A.resize(_n, _m);
        x.resize(_m);
        b.resize(_m);
    }
} Instance(1000, 1000);

inline auto getTimeSince(const time_point<high_resolution_clock> &start)
{
    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;
    return elapsed.count();
}

void IO()
{
    int *i = new int;
    struct GaussSolver *arg_ptr = (struct GaussSolver *)(aco_get_arg());
    for ((*i) = 0; (*i) < 10; (*i)++)
    {
        arg_ptr->b.setRandom();
        arg_ptr->A.setRandom();
        arg_ptr->A = arg_ptr->A + arg_ptr->A.transpose().eval();
        aco_yield();
    }
    delete i;
    aco_exit();
}

int main()
{
    //Initializing...
    aco_thread_init(NULL);
    aco_t *main_co = aco_create(NULL, NULL, 0, NULL, NULL);
    aco_share_stack_t *sstk = aco_share_stack_new(16 * 1024 * 1024);
    aco_t *co = aco_create(main_co, sstk, 0, IO, &Instance);

    //Solving...
    for (int i = 0; i < 10; i++)
    {
        aco_resume(co);
        Instance.x = Instance.A.ldlt().solve(Instance.b);
        if ((Instance.A * Instance.x).isApprox(Instance.b, 1e-6))
        {
            cout << Instance.x;
            printf("\n");
        }
    }
    aco_resume(co);
    // Cleaning...
    assert(co->is_end);
    aco_destroy(co);
    co = NULL;
    aco_share_stack_destroy(sstk);
    sstk = NULL;
    aco_destroy(main_co);
    main_co = NULL;

    return 0;
}
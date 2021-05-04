#include "LinearSolver.hpp"
#include <chrono>
#include <Eigen/Dense>
#include "aco.h"
#include <stdio.h>
#include "aco_assert_override.h"

using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using namespace std;
struct GaussSolver
{
    vector<vector<double>> A;
    vector<double> b;
    vector<double> x;
    GaussSolver(int _n, int _m)
    {
        A.resize(_n);
        for (auto &a : A)
            a.resize(_m);
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

// If we use it as a coroutine, we can never call it directly(get Seg Fault)
void IO()
{
    // It would be much better to use allocated variables rather than local variables
    // Too many local variables in the private stack would significantly affect the performance.
    int *i = new int;
    // Get co->arg. The caller of `aco_get_arg()` must be a non-main co.
    // Also, it should be folded in a struct pointer
    struct GaussSolver *arg_ptr = (struct GaussSolver *)(aco_get_arg());
    for ((*i) = 0; (*i) < 10; (*i)++)
    {
        generate(arg_ptr->b.begin(), arg_ptr->b.end(), mt19937{random_device{}()});
        for (auto &a : arg_ptr->A)
            generate(a.begin(), a.end(), mt19937{random_device{}()});
        printf("Input %d done\n", *i);
        // Yield the execution of current co and resume the execution of
        // `co->main_co`. The caller of `aco_yield()` must be a non-main co.
        aco_yield();
    }
    printf("Input exit\n");
    delete i;
    // In addition do the same as `aco_yield()`, `aco_exit()` also set 
    // `co->is_end` to `1` thus to mark the `co` at the status of "END".
    aco_exit();
}

int main()
{
    /*
    * Initializing...
    */
    // Initialize the aco environment in the current thread, mainly for fpu and mvsc control words
    aco_thread_init(NULL);

    // Create a main coroutine whose "share stack" is the default stack 
    // of the current thread. And it doesn't need any private save stack 
    // since it is definitely a standalone coroutine (which coroutine 
    // monopolizes it's share stack).
    aco_t *main_co = aco_create(NULL, NULL, 0, NULL, NULL);
    
    // Create a share stack with the default size of 2MB and also with a 
    // read-only guard page for the detection of stack overflow.
    aco_share_stack_t *sstk = aco_share_stack_new(16 * 1024 * 1024);

    // Create a non-main coroutine whose share stack is `sstk` and has a
    // default 64 bytes size private save stack. The entry function of the 
    // coroutine is `IO`. Set `co->arg` to the address of the int 
    // variable `&Instance`.
    aco_t *co = aco_create(main_co, sstk, 0, IO, &Instance);

    /*
    * Solving...
    */
    for (int i = 0; i < 10; i++)
    {
        // Start or continue the execution of `co`. The caller of this function
        // must be main_co.
        aco_resume(co);
        if (solveLinear(Instance.A, Instance.b, Instance.x) != -1)
        {
            //for (auto &xi : Instance.x)
            //    printf("%lf ", xi);
            printf("Pizzle %d solved\n", i);
        }
        else
            printf("Pizzle %d has no answer\n", i);
    }
    aco_resume(co);
    printf("Co should be ended\n");
    
    /* 
    * Cleaning...
    */
    // The value of `co->is_end` must be `1` now since it just suspended 
    // itself by calling `aco_exit()`.
    assert(co->is_end);
    // Destroy co and its private save stack.
    aco_destroy(co);
    co = NULL;
    // Destroy the share stack sstk.
    aco_share_stack_destroy(sstk);
    sstk = NULL;
    // Destroy the main_co.
    aco_destroy(main_co);
    main_co = NULL;

    return 0;
}
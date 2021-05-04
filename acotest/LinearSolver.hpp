#include <bits/extc++.h>
#include <bits/stdc++.h>
using namespace std;

const double eps = 1e-12;
int solveLinear(vector<vector<double>> &A, vector<double> &b, vector<double> &x)
{
    int n = A.size(), m = x.size(), rank = 0, br, bc;
    if (n)
        assert(A[0].size() == m);
    vector<int> col(m);
    // col[i] = i;
    iota(begin(col), end(col), 0);
    for (int i = 0; i < n; i++)
    {
        // Find the biggest |Aij| and exchange rows and columns
        double v, bv = 0;
        for (int r = i; r < n; r++)
            for (int c = i; c < m; c++)
                if ((v = fabs(A[r][c])) > bv)
                    br = r,
                    bc = c, bv = v;
        if (bv <= eps)
        {
            for (int j = i; j < n; j++)
                if (fabs(b[j]) > eps)
                    return -1;
            break;
        }
        swap(A[i], A[br]);
        swap(b[i], b[br]);
        swap(col[i], col[bc]);
        for (int j = 0; j < n; j++)
            swap(A[j][i], A[j][bc]);

        bv = 1 / A[i][i];
        for (int j = i + 1; j < n; j++)
        {
            double fac = A[j][i] * bv;
            b[j] -= fac * b[i];
            for (int k = i + 1; k < m; k++)
                A[j][k] -= fac * A[i][k];
        }
        rank++;
    }
    x.assign(m, 0);
    for (int i = rank; i--;)
    {
        b[i] /= A[i][i];
        x[col[i]] = b[i];
        for (int j = 0; j < i; j++)
            b[j] -= A[j][i] * b[i];
    }
    return rank; // (multiple solutions if rank < m)
}

int solveBinaryLinear(vector<bitset<1000>> &A, vector<int> &b, bitset<1000> &x, int m)
{
    int n = A.size(), rank = 0, br;
    assert(m <= x.size());
    vector<int> col(m);
    iota(begin(col), end(col), 0);
    for (size_t i = 0; i < n; i++)
    {
        for (br = i; br < n; ++br)
            if (A[br].any())
                break;
        if (br == n)
        {
            for (size_t j = i; j < n; j++)
                if (b[j])
                    return -1;
            break;
        }
        int bc = (int)A[br]._Find_next(i - 1);
        swap(A[i], A[br]);
        swap(b[i], b[br]);
        swap(col[i], col[bc]);
        for (size_t j = 0; j < n; j++)
            if (A[j][i] != A[j][bc])
            {
                A[j].flip(i);
                A[j].flip(bc);
            }
        for (size_t j = i + 1; j < n; j++)
            if (A[j][i])
            {
                b[j] ^= b[i];
                A[j] ^= A[i];
            }
        rank++;
    }
    x = bitset<1000>();
    for (int i = rank; i--;)
    {
        if (!b[i])
            continue;
        x[col[i]] = 1;
        for (size_t j = 0; j < i; j++)
            b[j] ^= A[j][i];
    }
    return rank; // (multiple solutions if rank < m)
}
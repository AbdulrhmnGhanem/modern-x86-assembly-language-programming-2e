//------------------------------------------------
//               Ch09_03.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <memory>

using namespace std;

extern "C" size_t c_NumRowsMax = 1024 * 1024;
extern "C" size_t c_NumColsMax = 1024 * 1024;

extern "C" bool AvxCalcColumnMeans_(const double* x, size_t nrows, size_t ncols, double* col_means);

void Init(double* x, size_t n, unsigned int seed)
{
    uniform_int_distribution<> ui_dist {1, 2000};
    default_random_engine rng {seed};

    for (size_t i = 0; i < n; i++)
        x[i] = (double)ui_dist(rng) / 10.0;
}

bool AvxCalcColumnMeansCpp(const double* x, size_t nrows, size_t ncols, double* col_means)
{
    // Make sure nrows and ncols are valid
    if (nrows == 0 || nrows > c_NumRowsMax)
        return false;
    if (ncols == 0 || ncols > c_NumColsMax)
        return false;

    // Set initial column means to zero
    for (size_t i = 0; i < ncols; i++)
        col_means[i] = 0.0;

    // Calculate column means
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
            col_means[j] += x[i * ncols + j];
    }

    for (size_t j = 0; j < ncols; j++)
        col_means[j] /= nrows;

    return true;
}

void AvxCalcColumnMeans(void)
{
    const size_t nrows = 20;
    const size_t ncols = 11;
    unique_ptr<double[]> x {new double[nrows * ncols]};
    unique_ptr<double[]> col_means1 {new double[ncols]};
    unique_ptr<double[]> col_means2 {new double[ncols]};

    Init(x.get(), nrows * ncols, 47);

    bool rc1 = AvxCalcColumnMeansCpp(x.get(), nrows, ncols, col_means1.get());
    bool rc2 = AvxCalcColumnMeans_(x.get(), nrows, ncols, col_means2.get());

    cout << "Results for AvxCalcColumnMeans\n";

    if (!rc1 || !rc2)
    {
        cout << "Invalid return code: ";
        cout << "rc1 = " << boolalpha << rc1 << ", ";
        cout << "rc2 = " << boolalpha << rc2 << '\n';
        return;
    }

    cout << "\nTest Matrix\n";
    cout << fixed << setprecision(1);

    for (size_t i = 0; i < nrows; i++)
    {
        cout << "row " << setw(2) << i;

        for (size_t j = 0; j < ncols; j++)
            cout << setw(7) << x[i * ncols + j];
        cout << '\n';
    }

    cout << "\nColumn Means\n";
    cout << setprecision(2);

    for (size_t j = 0; j < ncols; j++)
    {
        cout << "col_means1[" << setw(2) << j << "] =";
        cout << setw(10) << col_means1[j] << "   ";
        cout << "col_means2[" << setw(2) << j << "] =";
        cout << setw(10) << col_means2[j] << '\n';
    }
}

int main()
{
    AvxCalcColumnMeans();
    return 0;
}

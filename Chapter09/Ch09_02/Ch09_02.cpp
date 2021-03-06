//------------------------------------------------
//               Ch09_02.cpp
//------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

extern "C" void AvxCalcSphereAreaVolume_(float* sa, float* vol, const float* r, size_t n);

extern "C" float c_PI_F32 = (float)M_PI;
extern "C" float c_QNaN_F32 = numeric_limits<float>::quiet_NaN();

void Init(float* r, size_t n, unsigned int seed)
{
    uniform_int_distribution<> ui_dist {1, 100};
    default_random_engine rng {seed};

    for (size_t i = 0; i < n; i++)
        r[i] = (float)ui_dist(rng) / 10.0f;

    // Set invalid radii for test purposes
    if (n > 2)
    {
        r[2] = -r[2];
        r[n / 4] = -r[n / 4];
        r[n / 2] = -r[n / 2];
        r[n / 4 * 3] = -r[n / 4 * 3];
        r[n - 2] = -r[n - 2];
    }
}

void AvxCalcSphereAreaVolumeCpp(float* sa, float* vol, const float* r, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (r[i] < 0.0f)
            sa[i] = vol[i] = c_QNaN_F32;
        else
        {
            sa[i] = r[i] * r[i] * 4.0f * c_PI_F32;
            vol[i] = sa[i] * r[i] / 3.0f;
        }
    }
}

void AvxCalcSphereAreaVolume(void)
{
    const size_t n = 21;
    alignas(32) float r[n];
    alignas(32) float sa1[n];
    alignas(32) float vol1[n];
    alignas(32) float sa2[n];
    alignas(32) float vol2[n];

    Init(r, n, 93);
    
    AvxCalcSphereAreaVolumeCpp(sa1, vol1, r, n);
    AvxCalcSphereAreaVolume_(sa2, vol2, r, n);

    cout << "\nResults for AvxCalcSphereAreaVolume\n";
    cout << fixed;

    const float eps = 1.0e-6f;

    for (size_t i = 0; i < n; i++)
    {
        cout << setw(2) << i << ":  ";
        cout << setprecision(2);
        cout << setw(5) << r[i] << " | ";
        cout << setprecision(6);
        cout << setw(12) << sa1[i] << "   ";
        cout << setw(12) << sa2[i] << " | ";
        cout << setw(12) << vol1[i] << "   ";
        cout << setw(12) << vol2[i];

        bool b0 = (fabs(sa1[i] - sa2[i]) > eps);
        bool b1 = (fabs(vol1[i] - vol2[i]) > eps);

        if (b0 || b1)
            cout << " Compare discrepancy";
        cout << '\n';
    }
}

int main()
{
    AvxCalcSphereAreaVolume();
    return 0;
}

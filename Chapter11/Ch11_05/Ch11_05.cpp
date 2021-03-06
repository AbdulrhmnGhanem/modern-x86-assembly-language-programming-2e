//------------------------------------------------
//               Ch11_05.cpp
//------------------------------------------------

#include "stdafx.h"
#include <cstdint>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

extern "C" void SingleToHalfPrecision_(uint16_t x_hp[8], float x_sp[8], int rc);
extern "C" void HalfToSinglePrecision_(float x_sp[8], uint16_t x_hp[8]);

int main()
{
    float x[8];

    x[0] = 4.125f;
    x[1] = 32.9f;
    x[2] = 56.3333f;
    x[3] = -68.6667f;
    x[4] = 42000.5f;
    x[5] = 75600.0f;
    x[6] = -6002.125f;
    x[7] = 170.0625f;

    uint16_t x_hp[8];
    float rn[8], rd[8], ru[8], rz[8];

    SingleToHalfPrecision_(x_hp, x, 0);
    HalfToSinglePrecision_(rn, x_hp);
    SingleToHalfPrecision_(x_hp, x, 1);
    HalfToSinglePrecision_(rd, x_hp);
    SingleToHalfPrecision_(x_hp, x, 2);
    HalfToSinglePrecision_(ru, x_hp);
    SingleToHalfPrecision_(x_hp, x, 3);
    HalfToSinglePrecision_(rz, x_hp);

    unsigned int w = 15;
    string line(76, '-');

    cout << fixed << setprecision(4);
    cout << setw(w) << "x";
    cout << setw(w) << "RoundNearest";
    cout << setw(w) << "RoundDown";
    cout << setw(w) << "RoundUp";
    cout << setw(w) << "RoundZero";
    cout << '\n' << line << '\n';

    for (int i = 0; i < 8; i++)
    {
        cout << setw(w) << x[i];
        cout << setw(w) << rn[i];
        cout << setw(w) << rd[i];
        cout << setw(w) << ru[i];
        cout << setw(w) << rz[i];
        cout << '\n';
    }

    return 0;
}

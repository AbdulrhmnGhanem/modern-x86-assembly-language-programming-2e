//------------------------------------------------
//               Ch07_07.cpp
//------------------------------------------------

#include "stdafx.h"
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "Ch07_07.h"
#include "AlignedMem.h"
#include "ImageMatrix.h"

using namespace std;

extern "C" uint32_t c_NumPixelsMax = 16777216;

bool AvxBuildImageHistogramCpp(uint32_t* histo, const uint8_t* pixel_buff, uint32_t num_pixels)
{
    // Make sure num_pixels is valid
    if ((num_pixels == 0) || (num_pixels > c_NumPixelsMax))
        return false;

    if (num_pixels % 32 != 0)
        return false;

    // Make sure histo is aligned to a 16-byte boundary
    if (!AlignedMem::IsAligned(histo, 16))
        return false;

    // Make sure pixel_buff is aligned to a 16-byte boundary
    if (!AlignedMem::IsAligned(pixel_buff, 16))
        return false;

    // Build the histogram
    memset(histo, 0, 256 * sizeof(uint32_t));

    for (uint32_t i = 0; i < num_pixels; i++)
        histo[pixel_buff[i]]++;

    return true;
}

void AvxBuildImageHistogram(void)
{
    const wchar_t* image_fn = L"..\\Ch07_Data\\TestImage1.bmp";
    const wchar_t* csv_fn = L"Ch07_07_AvxBuildImageHistogram_Histograms.csv";

    ImageMatrix im(image_fn);
    uint32_t num_pixels = im.GetNumPixels();
    uint8_t* pixel_buff = im.GetPixelBuffer<uint8_t>();
    AlignedArray<uint32_t> histo1_aa(256, 16);
    AlignedArray<uint32_t> histo2_aa(256, 16);

    bool rc1 = AvxBuildImageHistogramCpp(histo1_aa.Data(), pixel_buff, num_pixels);
    bool rc2 = AvxBuildImageHistogram_(histo2_aa.Data(), pixel_buff, num_pixels);

    cout << "\nResults for AvxBuildImageHistogram\n";

    if (!rc1 || !rc2)
    {
        cout << "Bad return code: ";
        cout << "rc1 = " << rc1 << ", rc2 = " << rc2 << '\n';
        return;
    }

    ofstream ofs(csv_fn);

    if (ofs.bad())
        cout << "File create error - " << csv_fn << '\n';
    else
    {
        bool compare_error = false;
        uint32_t* histo1 = histo1_aa.Data();
        uint32_t* histo2 = histo2_aa.Data();
        const char* delim = ", ";

        for (uint32_t i = 0; i < 256; i++)
        {
            ofs << i << delim;
            ofs << histo1[i] << delim << histo2[i] << '\n';

            if (histo1[i] != histo2[i])
            {
                compare_error = true;
                cout << "  Histogram compare error at index " << i << '\n';
                cout << "  counts: " << histo1[i] << delim << histo2[i] << '\n';
            }
        }

        if (!compare_error)
            cout << "  Histograms are identical\n";

        ofs.close();
    }
}

int main()
{
    try
    {
        AvxBuildImageHistogram();
        AvxBuildImageHistogram_BM();
    }

    catch (...)
    {
        cout << "Unexpected exception has occurred\n";
        cout << "File = " << __FILE__ << '\n';
    }

    return 0;
}

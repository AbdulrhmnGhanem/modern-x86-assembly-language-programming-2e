//------------------------------------------------
//               Ch07_08.cpp
//------------------------------------------------

#include "stdafx.h"
#include <cstdint>
#include <iostream>
#include <iomanip>
#include "Ch07_08.h"
#include "AlignedMem.h"
#include "ImageMatrix.h"

using namespace std;

extern "C" uint32_t c_NumPixelsMax = 16777216;

bool IsValid(uint32_t num_pixels, const uint8_t* pb_src, const uint8_t* pb_mask)
{
    const size_t alignment = 16;

    // Make sure num_pixels is valid
    if ((num_pixels == 0) || (num_pixels > c_NumPixelsMax))
        return false;
    if ((num_pixels % 64) != 0)
        return false;

    // Make sure image buffers are properly aligned
    if (!AlignedMem::IsAligned(pb_src, alignment))
        return false;
    if (!AlignedMem::IsAligned(pb_mask, alignment))
        return false;

    return true;
}

bool AvxThresholdImageCpp(ITD* itd)
{
    uint8_t* pb_src = itd->m_PbSrc;
    uint8_t* pb_mask = itd->m_PbMask;
    uint8_t threshold = itd->m_Threshold;
    uint32_t num_pixels = itd->m_NumPixels;

    // Verify pixel count and buffer alignment
    if (!IsValid(num_pixels, pb_src, pb_mask))
        return false;

    // Threshold the image
    for (uint32_t i = 0; i < num_pixels; i++)
        *pb_mask++ = (*pb_src++ > threshold) ? 0xff : 0x00;

    return true;
}

bool AvxCalcImageMeanCpp(ITD* itd)
{
    uint8_t* pb_src = itd->m_PbSrc;
    uint8_t* pb_mask = itd->m_PbMask;
    uint32_t num_pixels = itd->m_NumPixels;

    // Verify pixel count and buffer alignment
    if (!IsValid(num_pixels, pb_src, pb_mask))
        return false;

    // Calculate mean of masked pixels
    uint32_t sum_masked_pixels = 0;
    uint32_t num_masked_pixels = 0;

    for (uint32_t i = 0; i < num_pixels; i++)
    {
        uint8_t mask_val = *pb_mask++;
        num_masked_pixels += mask_val & 1;
        sum_masked_pixels += (*pb_src++ & mask_val);
    }

    itd->m_NumMaskedPixels = num_masked_pixels;
    itd->m_SumMaskedPixels = sum_masked_pixels;
    
    if (num_masked_pixels > 0)
        itd->m_MeanMaskedPixels = (double)sum_masked_pixels / num_masked_pixels;
    else
        itd->m_MeanMaskedPixels = -1.0;

    return true;
}

void AvxThreshold(void)
{
    const wchar_t* fn_src = L"..\\Ch07_Data\\TestImage2.bmp";
    const wchar_t* fn_mask1 = L"Ch07_08_AvxThreshold_TestImage2_Mask1.bmp";
    const wchar_t* fn_mask2 = L"Ch07_08_AvxThreshold_TestImage2_Mask2.bmp";

    ImageMatrix im_src(fn_src);
    int im_h = im_src.GetHeight();
    int im_w = im_src.GetWidth();
    ImageMatrix im_mask1(im_h, im_w, PixelType::Gray8);
    ImageMatrix im_mask2(im_h, im_w, PixelType::Gray8);
    ITD itd1, itd2;

    itd1.m_PbSrc = im_src.GetPixelBuffer<uint8_t>();
    itd1.m_PbMask = im_mask1.GetPixelBuffer<uint8_t>();
    itd1.m_NumPixels = im_src.GetNumPixels();
    itd1.m_Threshold = c_TestThreshold;

    itd2.m_PbSrc = im_src.GetPixelBuffer<uint8_t>();
    itd2.m_PbMask = im_mask2.GetPixelBuffer<uint8_t>();
    itd2.m_NumPixels = im_src.GetNumPixels();
    itd2.m_Threshold = c_TestThreshold;

    // Threshold image
    bool rc1 = AvxThresholdImageCpp(&itd1);
    bool rc2 = AvxThresholdImage_(&itd2);

    if (!rc1 || !rc2)
    {
        cout << "\nInvalid return code: ";
        cout << "rc1 = " << rc1 << ", rc2 = " << rc2 << '\n';
        return;
    }

    im_mask1.SaveToBitmapFile(fn_mask1);
    im_mask2.SaveToBitmapFile(fn_mask2);

    // Calculate mean of masked pixels
    rc1 = AvxCalcImageMeanCpp(&itd1);
    rc2 = AvxCalcImageMean_(&itd2);

    if (!rc1 || !rc2)
    {
        cout << "\nInvalid return code: ";
        cout << "rc1 = " << rc1 << ", rc2 = " << rc2 << '\n';
        return;
    }

    // Print results
    const int w = 12;
    cout << fixed << setprecision(4);

    cout << "\nResults for AvxThreshold\n\n";
    cout << "                            C++       X86-AVX\n";
    cout << "---------------------------------------------\n";
    cout << "SumPixelsMasked:   ";
    cout << setw(w) << itd1.m_SumMaskedPixels << "  ";
    cout << setw(w) << itd2.m_SumMaskedPixels << '\n';
    cout << "NumPixelsMasked:   ";
    cout << setw(w) << itd1.m_NumMaskedPixels << "  ";
    cout << setw(w) << itd2.m_NumMaskedPixels << '\n';
    cout << "MeanMaskedPixels:  ";
    cout << setw(w) << itd1.m_MeanMaskedPixels << "  ";
    cout << setw(w) << itd2.m_MeanMaskedPixels << '\n';
}

int main()
{
    try
    {
        AvxThreshold();
        AvxThreshold_BM();
    }

    catch (...)
    {
        cout << "Unexpected exception has occurred\n";
    }

    return 0;
}

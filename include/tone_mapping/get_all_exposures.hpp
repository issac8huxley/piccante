/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_TONE_MAPPING_GET_ALL_EXPOSURES_HPP
#define PIC_TONE_MAPPING_GET_ALL_EXPOSURES_HPP

#include "../base.hpp"
#include "../image.hpp"
#include "../histogram.hpp"

#include "../util/indexed_array.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_simple_tmo.hpp"

namespace pic {

/**
 * @brief getMinMaxFstops computes the minimum and maximum f-stop values of an image.
 * @param imgIn is an image.
 * @param minFstop is the mininum f-stop of imgIn, output.
 * @param maxFstop is the maximum f-stop of imgIn, output.
 */
PIC_INLINE void getMinMaxFstops(Image *imgIn, int &minFstop, int &maxFstop)
{
    if(imgIn == NULL) {
        return;
    }

    Image *img_lum = NULL;
    bool bAllocated = false;

    if(imgIn->channels == 1) {
        img_lum = imgIn;
    } else {
        bAllocated = true;
        img_lum = FilterLuminance::Execute(imgIn, NULL, LT_CIE_LUMINANCE);
    }

    int nData = img_lum->width * img_lum->height;

    IntCoord coord;
    IndexedArray::findSimple(img_lum->data, nData, IndexedArray::bFuncNotNeg, coord);

    float commonMin = IndexedArray::min(img_lum->data, coord);
    float commonMax = IndexedArray::max(img_lum->data, coord);

    float tminFstop = logf(commonMin) / logf(2.0f);
    float tmaxFstop = logf(commonMax) / logf(2.0f);

    minFstop = int(lround(tminFstop));
    maxFstop = int(lround(tmaxFstop));

    int halfFstops = (maxFstop - minFstop + 1) >> 1;
    minFstop = -halfFstops + 1;
    maxFstop =  halfFstops - 1;

    if(minFstop == maxFstop) {
        minFstop--;
        maxFstop++;
    }

    if(bAllocated) {
        delete img_lum;
    }
}

/**
 * @brief getAllExposures converts an image into a stack of exposure values which
 * generates all required exposure images for reconstructing the input image.
 * @param imgIn is an input image.
 * @return It returns an array of exposure values encoding the
 */
PIC_INLINE std::vector<float> getAllExposures(Image *imgIn) {
    std::vector<float> exposures;

    if(imgIn == NULL) {
        return exposures;
    }

    if(!imgIn->isValid()) {
        return exposures;
    }

    Image *lum = FilterLuminance::Execute(imgIn, NULL);

    Histogram m(lum, VS_LOG_2, 1024);
    exposures = m.exposureCovering();

    delete lum;

    return exposures;
}


/**
 * @brief getAllExposuresImages converts an image into a stack of images.
 * @param imgIn is an input image.
 * @param fstops a vector with fstops.
 * @param gamma is the gamma correction value for the output stack.
 * @return It returns an ImageVec of images which encode imgIn at different
 * exposure values.
 */
PIC_INLINE ImageVec getAllExposuresImages(Image *imgIn, std::vector<float> &fstops, float gamma = 2.2f)
{
    ImageVec ret;
    FilterSimpleTMO flt(gamma, 0.0f);

    ImageVec input = Single(imgIn);

    for(unsigned int i = 0; i < fstops.size(); i++) {
        flt.Update(gamma, fstops[i]);
        Image *expo = flt.ProcessP(input, NULL);

        expo->exposure = powf(2.0f, fstops[i]);

        ret.push_back(expo);
    }

    return ret;
}

/**
 * @brief getAllExposuresImages converts an image into a stack of images.
 * @param imgIn is an input image.
 * @param gamma is the gamma correction value for the output stack.
 * @return It returns an ImageVec of images which encode imgIn at different
 * exposure values.
 */
PIC_INLINE ImageVec getAllExposuresImages(Image *imgIn, float gamma = 2.2f)
{
    std::vector<float> fstops = getAllExposures(imgIn);
    return getAllExposuresImages(imgIn, fstops, gamma);
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_GET_ALL_EXPOSURES_HPP */


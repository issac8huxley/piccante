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

#ifndef PIC_METRICS_LOG_RMSE_HPP
#define PIC_METRICS_LOG_RMSE_HPP

#include <math.h>
#include "image.hpp"
#include "metrics/base.hpp"

namespace pic {

/**
 * @brief logRMSE computes root mean square error (RMSE) in the log_e domain.
 * @param ori is the original image.
 * @param cmp is the distorted image.
 * @return It returns RMSE in the log_e domain.
 */
double logRMSE(Image *ori, Image *cmp)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->isSimilarType(cmp)) {
        return -1.0;
    }

    int size = ori->size();
    int counter = 0;

    double acc = 0.0;
    double val;

    for(int i = 0; i < size; i += ori->channels) {
        for(int j = 0; j < ori->channels; j++) {
            if(ori->data[i + j] > 0.0f && cmp->data[i + j] > 0.0f) {
                val  = log(ori->data[i + j] / cmp->data[i + j]);
                acc += val * val;
                counter++;
            }
        }
    }

    if(counter > 0) {
        acc = acc / double(counter);
        return sqrt(acc);
    } else {
        return -3.0;
    }
}

} // end namespace pic

#endif /* PIC_METRICS_LOG_RMSE_HPP */


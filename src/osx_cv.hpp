/*
Copyright (c) 2012, Kevin Han
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <opencv2/opencv.hpp>
#include <ApplicationServices/ApplicationServices.h>

#include "osx.hpp"
#include "kmath.hpp"

/*! Functions for interoperability between ApplicationServices and OpenCV. */
namespace osx_cv {
    /*! Convert a `CGImageRef` to a `cv::Mat`.
     *
     * @param image
     *      Must have 32 bits per pixel.
     * @param out
     *      Destination whose type will be either `CV_8UC4` or `CV_8UC3`
     *      depending on `destType`.
     * @param destType
     *      The pixel format of the resulting `cv::Mat`.
     *
     * @throws invalid_argument
     *      Thrown if `image` is not 32 bits per pixel.
     */
    cv::Mat &toMat(
            const CGImageRef &image,
            cv::Mat &out,
            osx::disp::RGBType destType=osx::disp::COLOR_BGR
            );

    /*! Convert a `cv::Point` to a `CGPoint`. */
    inline CGPoint toCGPt(cv::Point pt) {
        return CGPointMake(pt.x, pt.y);
    }

    /*! Convert a `CGPoint` to a `cv::Point`. */
    inline cv::Point toCVPt(CGPoint pt) {
        return cv::Point(kmath::iround(float(pt.x)), kmath::iround(float(pt.y)));
    }
}

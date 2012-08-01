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

#include <stdexcept>

#include <opencv2/opencv.hpp>
#include <ApplicationServices/ApplicationServices.h>

#include "osx.hpp"
#include "osx_cv.hpp"

static cv::Mat &_cvtRGBColor(
        const cv::Mat &in,
        cv::Mat &out,
        CGImageAlphaInfo srcAlpha,
        osx::disp::RGBType destFmt
        ) {
    if (in.data == out.data) {
        throw invalid_argument("`in` must be different from `out`");
    }

    switch (srcAlpha) {
        case kCGImageAlphaPremultipliedLast:
            // fall through
        case kCGImageAlphaLast:
            // fall through
        case kCGImageAlphaNoneSkipLast:
            switch (destFmt) {
                case osx::disp::COLOR_BGR:
                    cvtColor(in, out, cv::COLOR_RGBA2BGR);
                    break;
                case osx::disp::COLOR_ARGB: {
                    out.create(in.size(), CV_8UC4);
                    const int fromTo[] = {0,1, 1,2, 2,3, 3,0};
                    mixChannels(&in, 1, &out, 1, fromTo, 4);
                    break;
                }
                case osx::disp::COLOR_RGBA:
                    out = in;
                    break;
                default:
                    throw invalid_argument("invalid destFmt");
                    break;
            }
            break;
        case kCGImageAlphaPremultipliedFirst:
            // fall through
        case kCGImageAlphaFirst:
            // fall through
        case kCGImageAlphaNoneSkipFirst:
            switch (destFmt) {
                case osx::disp::COLOR_BGR: {
                    out.create(in.size(), CV_8UC3);
                    const int fromTo[] = {1,2, 2,1, 3,0};
                    mixChannels(&in, 1, &out, 1, fromTo, 3);
                    break;
                }
                case osx::disp::COLOR_ARGB:
                    out = in;
                    break;
                case osx::disp::COLOR_RGBA: {
                    out.create(in.size(), CV_8UC4);
                    const int fromTo[] = {0,3, 1,0, 2,1, 3,2};
                    mixChannels(&in, 1, &out, 1, fromTo, 4);
                    break;
                }
                default:
                    throw invalid_argument("invalid destFmt");
                    break;
            }
            break;
        default:
            throw invalid_argument("invalid srcAlpha");
            break;
    }

    return out;
}

cv::Mat &osx_cv::toMat(
        const CGImageRef &image,
        cv::Mat &out,
        osx::disp::RGBType destType
        ) {
    cv::Mat temp;

    size_t w = CGImageGetWidth(image), h = CGImageGetHeight(image);

    if (CGImageGetBitsPerPixel(image) != 32) {
        throw invalid_argument("`image` must be 32 bits per pixel");
    }

    temp.create(int(h), int(w), CV_8UC4);

    CGColorSpaceRef colorSpace = CGImageGetColorSpace(image);

    CGContextRef context = CGBitmapContextCreate(
            temp.data,
            w,
            h,
            CGImageGetBitsPerComponent(image),
            CGImageGetBytesPerRow(image),
            colorSpace,
            CGImageGetAlphaInfo(image)
    );

    CGContextDrawImage(
            context,
            CGRectMake(0, 0, (CGFloat)w, (CGFloat)h),
            image
            );

    _cvtRGBColor(temp, out, CGImageGetAlphaInfo(image), destType);

    CGContextRelease(context);

    return out;
}

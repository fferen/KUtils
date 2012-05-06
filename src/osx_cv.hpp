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

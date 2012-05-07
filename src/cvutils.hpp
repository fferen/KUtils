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

#include <iostream>
#include <stdexcept>

#include <opencv2/opencv.hpp>

#include "krandom.hpp"
#include "kmath.hpp"
#include "seq.hpp"

using namespace std;

/*! OpenCV utilities. */
namespace cvutils {
    //@{
    /*! Specifies positions for affixing `cv::Rect`s or `cv::Size`s. */
    enum XLoc { FIX_CENTER_X, FIX_LEFT, FIX_RIGHT };
    enum YLoc { FIX_CENTER_Y, FIX_TOP, FIX_BOTTOM };
    //@}

    /*! Get the number of bytes of a type at compile-time.
     *
     * This is done with `cv::DataDepth<>`, but for some reason
     * `cv::DataDepth<uchar>::value` == 0, so we override it here with 1.
     */
    template <typename T>
    struct _DataDepth_Fixed {
        static const int value = cv::DataDepth<T>::value;
    };

    template <>
    struct _DataDepth_Fixed<uchar> {
        static const int value = 1;
    };

    /*! Represents a video input source (either a video file or camera). */
    struct VideoReader
    {
        cv::VideoCapture cap;
        cv::Size size;

        /*! Construct from either a device id (int) or filename.
         *
         * Specify `size` to manually set the size of the returned frames.
         *
         * @throw invalid_argument
         * Thrown if unable to open input source.
         */
        template <typename T>
        VideoReader(T input, cv::Size size=cv::Size(0, 0)):
            cap(input), size(size)
        {
            if (not cap.isOpened()) {
                throw invalid_argument("unable to open device or file");
            }

            if (size.area()) {
                cap.set(CV_CAP_PROP_FRAME_WIDTH, size.width);
                cap.set(CV_CAP_PROP_FRAME_HEIGHT, size.height);

                // the first image read is always messed up for some reason when
                // you set the size manually... could be a bug in OpenCV
                cv::Mat temp;
                cap >> temp;
            }
        }

        /*! Read next frame from input. */
        void operator>>(cv::Mat &im);
    };

    /*! Block until a key is pressed. Focus must be in an OpenCV window. */
    void waitForKeypress();

    /*! For each pixel in `m1` and `m2`, call `func` like so:
     *
     *      func(int row, int col, T1 *ptr (pointer to element in m1), T2 *ptr (pointer to element in m2))
     *
     * @throws runtime_error
     * Thrown if matrices are not the same size.
     */
    template <typename T1, typename T2, typename _FuncT>
    void applyBinaryOp(
            const _FuncT &func,
            cv::Mat_<T1> &m1,
            cv::Mat_<T2> &m2
            ) {
        int cols = m1.cols, rows = m1.rows;

        if (cols != m2.cols or rows != m2.rows) {
            throw runtime_error("matrices must be the same size");
        }

        if (m1.isContinuous() and m2.isContinuous()) {
            cols *= rows;
            rows = 1;
        }
        for (int i = 0; i < rows; i++) {
            auto *i1 = m1[i];
            auto *i2 = m2[i];
            for (int j = 0; j < cols; j++) {
                func(i, j, i1 + j, i2 + j);
            }
        }
    }

    /*! Partly const version of applyBinaryOp(). */
    template <typename T1, typename T2, typename _FuncT>
    void applyBinaryOp(
            const _FuncT &func,
            const cv::Mat_<T1> &m1,
            cv::Mat_<T2> &m2
            ) {
        cv::Mat_<T1> temp(m1);
        applyBinaryOp(func, temp, m2);
    }

    /*! Partly const version of applyBinaryOp(). */
    template <typename T1, typename T2, typename _FuncT>
    void applyBinaryOp(
            const _FuncT &func,
            cv::Mat_<T1> &m1,
            const cv::Mat_<T2> &m2
            ) {
        cv::Mat_<T2> temp(m2);
        applyBinaryOp(func, m1, temp);
    }

    /*! Const version of applyBinaryOp(). */
    template <typename T1, typename T2, typename _FuncT>
    void applyBinaryOp(
            const _FuncT &func,
            const cv::Mat_<T1> &m1,
            const cv::Mat_<T2> &m2
            ) {
        cv::Mat_<T1> temp1(m1);
        cv::Mat_<T2> temp2(m2);
        applyBinaryOp(func, temp1, temp2);
    }

    /*! Return a random RGB color. */
    inline cv::Scalar randColor() {
        return cv::Scalar(
                krandom::randint(0, 255),
                krandom::randint(0, 255),
                krandom::randint(0, 255),
                255
                );
    }

    /*! Draw the outline of a closed polygon specified by `pts` on `im`.
     *
     * If `thickness` < 0, draw a filled polygon. This uses `fillConvexPoly` if
     * `convex` is true, else `fillPoly` (the flag `convex` is only examined if
     * `thickness` < 0).
     */
    void drawPolygon(
            cv::Mat &im,
            const vector<cv::Point> &pts,
            const cv::Scalar &color=cv::Scalar(255, 255, 255),
            int thickness=1,
            int lineType=8,
            bool convex=true
            );

    /*! Pad `src` to a larger size and store in `dest`, filling extra space with
     * `fillVal`. This is O(1) as no data is copied.
     *
     * This version takes a point `offset` where the top-left of `src` will go.
     */
    void expandMat(const cv::Mat &src, cv::Mat &dest, cv::Size newSize, cv::Point offset, cv::Scalar fillVal=cv::Scalar());

    /*! Pad `src` to a larger size and store in `dest`, filling extra space with
     * `fillVal`. This is O(1) as no data is copied.
     *
     * This version takes location flags to affix `src` in a certain position.
     */
    void expandMat(const cv::Mat &src, cv::Mat &dest, cv::Size newSize, cv::Scalar fillVal=cv::Scalar(), XLoc xFlag=FIX_CENTER_X, YLoc yFlag=FIX_CENTER_Y);

    /*! Convert a matrix from BGR color space to rg chromaticity space, with the
     * 3rd channel unused.
     */
    void cvtBGR2RG(const cv::Mat &src, cv::Mat &dest);

    /*! Generic naive clustering algorithm (`O(n^2)` worst case). An item is
     * added to the cluster if it is within `maxDist` from all other items in
     * the cluster.
     *
     * `getDist` should take two items and return their distance.
     */
    template <
            class InSeqT,
            template <class T=InSeqT, typename... Args> class OutSeqT,
            class DistFuncT
            >
    void cluster(const InSeqT &items, const DistFuncT &getDist, float maxDist, OutSeqT<InSeqT> &clusters) {
        clusters.clear();
        if (items.empty()) {
            return;
        }
        for (auto &item : items) {
            bool added = false;
            for (auto &cluster : clusters) {
                if (seq::functional::all(
                        [&](decltype(items[0]) &cItem) {
                            return getDist(cItem, item) <= maxDist;
                        },
                        cluster
                       )) {
                    cluster.push_back(item);
                    added = true;
                    break;
                }
            }
            if (not added) {
                clusters.push_back(InSeqT{item});
            }
        }
    }

    /*! Save Mat in file. */
    void matwrite(const char *fileName, const cv::Mat &m);

    /*! Read Mat from file saved with `matwrite()`. */
    cv::Mat matread(const char *fileName);

    /*! Return the sum of all pixels within `roi` using an integral image.
     *
     * @param integralMat
     * Supported types: `CV_32SC1`, `CV_32FC1`, `CV_64FC1`.
     *
     * @throws invalid_argument
     * Thrown if type of `integralMat` is not supported.
     */
    double integralMatSum(const cv::Mat &integralMat, cv::Rect roi);

    /*! Call default `erode()` `times` times, then default `dilate()` `times`
     * times.
     *
     * This is useful to remove small areas of noise while preserving area.
     */
    void erodilate(const cv::Mat &src, cv::Mat &dest, unsigned times);

    /*! Call default `dilate()` `times` times, then default `erode()` `times`
     * times.
     *
     * This is useful to fill in small gaps while preserving area.
     */
    void dilerode(const cv::Mat &src, cv::Mat &dest, unsigned times);

    /*! Convert a binary mask (non-zero == on) to a GrabCut mask, using this
     * pixel mapping:
     *
     * nonzero -> `GC_PR_FGD`
     *
     * zero -> `GC_PR_BGD`
     */
    void binMaskToGCMask(const cv::Mat_<uchar> &mask, cv::Mat_<uchar> &out);

    /*! Convert a GrabCut mask to a binary mask, using this pixel mapping:
     *
     * {`GC_PR_BGD`, `GC_BGD`} -> 0
     *
     * {`GC_PR_FGD`, `GC_FGD`} -> 255
     */
    void gcMaskToBinMask(const cv::Mat_<uchar> &mask, cv::Mat_<uchar> &out);

    /*! Operations on `cv::Point`s, `cv::Vec`s, and `cv::Rect`s. */
    namespace geom {
        /*! Convert a `kmath::Point<T>` to a `cv::Point`. */
        template <typename T>
        inline cv::Point toCVPt(kmath::Point<T> pt) {
            return cv::Point(pt.x, pt.y);
        }

        /*! Convert a `cv::Point` to a `kmath::Point<T>`. */
        template <typename T>
        inline kmath::Point<T> toPt(cv::Point pt) {
            return kmath::Point<T>(pt.x, pt.y);
        }

        /*! Return the midpoint (duh). */
        template <typename T>
        inline cv::Point_<T> midpoint(cv::Point_<T> a, cv::Point_<T> b) {
            a.x = (a.x + b.x) / 2;
            a.y = (a.y + b.y) / 2;
            return a;
        }

        /*! Return the magnitude of `p`, squared. */
        template <typename T>
        inline float ptNormSqrd(const cv::Point_<T> &p) {
            return float(p.x * p.x + p.y * p.y);
        }

        /*! Return the magnitude of `v`, squared. */
        template <typename ElemT, int cn>
        inline float vecNormSqrd(const cv::Vec<ElemT, cn> &v) {
            float s = 0;
            for (int i = 0; i < cn; i++) {
                s += float(v[i] * v[i]);
            }
            return s;
        }

        /*! Return the angle from vector `a` to vector `b`, in radians from -π to π, CCW
         * positive.
         */
        float vecAngle(const cv::Vec2i &a, const cv::Vec2i &b);

        /*! Return the angle from the vector `(a - b)` to the vector `(c - b)`, in
         * radians from -π to π, CCW positive.
         */
        float ptAngle(const cv::Point &a, const cv::Point &b, const cv::Point &c);

        /*! Clamp `pt` inside `r`. */
        cv::Point clampPt(cv::Point pt, const cv::Rect &r);

        /*! Return the top-left offset in `big` of `small` given location flags.
         *
         * @throws invalid_argument
         * Thrown if `xFlag` or `yFlag` is invalid.
         */
        cv::Point getOffsetFromLocFlags(
                cv::Size small,
                cv::Size big,
                XLoc xFlag=FIX_CENTER_X,
                YLoc yFlag=FIX_CENTER_Y
                );

        /*! Return the cv::Rect with the average bounds of both input rects. */
        cv::Rect getAvgRect(const cv::Rect &a, const cv::Rect &b);

        /*! Return the cv::Rect with the average bounds of all input rects. */
        cv::Rect getAvgRect(const vector<cv::Rect> &rects);

        /*! Expand Rect by a certain size on all sides while preserving the
         * center.
         */
        cv::Rect expandRect(const cv::Rect &r, const cv::Size &s);
    }

    /*! Functions related to affine transformations. */
    namespace transform {
        /*! Store part of `src` in `dest` with the ROI specified by `r`, and
         * return `dest`.
         */
        cv::Mat &getRotatedROI(const cv::Mat &src, const cv::RotatedRect &r, cv::Mat &dest);

        /*! Scale and rotate `src` and store in `dest` (must not be the same).
         *
         * `angle` is in degrees CCW.
         *
         * If `transMat` is provided, store the 2x3 transformation matrix in it.
         * Provide `NULL` if not needed.
         *
         * Extra space in `dest` will be filled to 0.
         */
        void rotozoomMat(const cv::Mat &src, cv::Mat &dest, float angle, float scale=1.0, cv::Mat *transMat=NULL);

        /*! Return the size bounding the given size rotated by `angle`, given in
         * degrees.
         *
         * It will always have a width and height >= those of the given size.
         */
        cv::Size2f rotateSize(cv::Size2f s, float angle);

        /*! Apply 2x3 affine transform matrix to `pt` and return new point. */
        cv::Point2f warpAffinePt(const cv::Mat_<float> &transMat, cv::Point2f pt);
    }
}

/* Printers */
ostream &operator<<(ostream &out, const cv::Rect r);
ostream &operator<<(ostream &out, const cv::Size s);
ostream &operator<<(ostream &out, const cv::Size2f s);
ostream &operator<<(ostream &out, const cv::Scalar s);
ostream &operator<<(ostream &out, const uchar c);

template <typename T, int cn>
ostream &operator<<(ostream &out, const cv::Vec<T, cn> v) {
    out << "<Vec depth=" << cvutils::_DataDepth_Fixed<T>::value << " data=" << "(";
    int i;
    for (i = 0; i < cn - 1; i++) {
        out << v[i] << ", ";
    }
    out << v[i] << ")>";

    return out;
}

/* Overload common operators for cv::Size, because the documentation is WRONG,
 * and cv::Size does NOT support the same operators as cv::Point. >:(
 */

cv::Size2f operator*(const cv::Size2f &s, float a);
cv::Size2f operator*(float a, const cv::Size2f &s);
cv::Size2f operator/(const cv::Size2f &s, float a);

bool operator==(const cv::Size2f &a, const cv::Size2f &b);
bool operator!=(const cv::Size2f &a, const cv::Size2f &b);

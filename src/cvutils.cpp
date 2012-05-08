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

#include <cstdint>

#include <algorithm>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "cvutils.hpp"
#include "kmath.hpp"

using namespace std;
using namespace cv;
using namespace cvutils;
using namespace cvutils::geom;

void cvutils::VideoReader::operator>>(cv::Mat &im) {
    this->cap >> im;

    // Sometimes setting the capture properties doesn't work, so ensure that it
    // is sized properly.
    if (this->size.area() and im.size() != this->size) {
        resize(im, im, this->size);
    }
}

void cvutils::waitForKeypress() {
    while (waitKey(30) < 0);
}

void cvutils::drawPolygon(
        Mat &im,
        const vector<Point> &pts,
        const Scalar &color,
        int thickness,
        int lineType,
        bool convex
        ) {
//    const Point *ptArr = new Point[pts.size()];
    int nPts = int(pts.size());
//    for (int i = 0; i < nPts; i++) {
//        *(ptArr + i) = pts[i];
//    }

    const Point *startPt = &pts[0];
    if (thickness < 0) {
        if (convex) {
            fillConvexPoly(im, startPt, nPts, color, lineType);
        } else {
            fillPoly(im, &startPt, &nPts, 1, color, lineType);
        }
    } else {
        polylines(im, &startPt, &nPts, 1, true, color, thickness, lineType);
    }
}

void cvutils::expandMat(const Mat &src, Mat &dest, Size newSize, Point offset, Scalar fillVal) {
    Mat temp(newSize, src.type(), fillVal);

    Mat roi = temp(Rect(offset, src.size()));
    src.copyTo(roi);
    dest = temp;
}

void cvutils::expandMat(const Mat &src, Mat &dest, Size newSize, Scalar fillVal, cvutils::XLoc xFlag, cvutils::YLoc yFlag) {
    cvutils::expandMat(src, dest, newSize, geom::getOffsetFromLocFlags(src.size(), newSize, xFlag, yFlag), fillVal);
}

void cvutils::matwrite(const char *fileName, const Mat &m) {
    CvMat cvM = CvMat(m);
    cvSave(fileName, &cvM);
}

Mat cvutils::matread(const char *fileName) {
    return Mat((CvMat *)cvLoad(fileName));
}

double cvutils::integralMatSum(const Mat &integralMat, Rect roi) {
    switch (integralMat.type()) {
        case CV_32SC1:
            return integralMat.at<int32_t>(roi.br())
                - integralMat.at<int32_t>(roi.y + roi.height, roi.x)
                - integralMat.at<int32_t>(roi.y, roi.x + roi.width)
                + integralMat.at<int32_t>(roi.tl());
        case CV_32FC1:
            return integralMat.at<float>(roi.br())
                - integralMat.at<float>(roi.y + roi.height, roi.x)
                - integralMat.at<float>(roi.y, roi.x + roi.width)
                + integralMat.at<float>(roi.tl());
        case CV_64FC1:
            return integralMat.at<double>(roi.br())
                - integralMat.at<double>(roi.y + roi.height, roi.x)
                - integralMat.at<double>(roi.y, roi.x + roi.width)
                + integralMat.at<double>(roi.tl());
        default:
            throw invalid_argument("unsupported type of integralMat");
    }
}

void cvutils::erodilate(const Mat &src, Mat &dest, unsigned times) {
    src.copyTo(dest);

    erode(dest, dest, Mat(), Point(-1, -1), int(times));
    dilate(dest, dest, Mat(), Point(-1, -1), int(times));
}

void cvutils::dilerode(const Mat &src, Mat &dest, unsigned times) {
    src.copyTo(dest);

    dilate(dest, dest, Mat(), Point(-1, -1), int(times));
    erode(dest, dest, Mat(), Point(-1, -1), int(times));
}

void cvutils::binMaskToGCMask(const Mat_<uchar> &mask, Mat_<uchar> &out) {
    out.create(mask.size());
    auto outIt = out.begin();
    for (auto it = mask.begin(); it != mask.end(); it++, outIt++) {
        *outIt = (*it == 0) ? GC_PR_BGD : GC_FGD;
    }
}

void cvutils::gcMaskToBinMask(const Mat_<uchar> &mask, Mat_<uchar> &out) {
    out.create(mask.size());
    auto outIt = out.begin();
    for (auto it = mask.begin(); it != mask.end(); it++, outIt++) {
        *outIt = (*it == GC_PR_BGD || *it == GC_BGD) ? 0 : 255;
    }
}

/*** namespace `geom` ***/

float geom::vecAngle(const Vec2i &a, const Vec2i &b) {
    float dp = float(a.dot(b));
    float cross = float(b[1] * a[0] - b[0] * a[1]);
    dp /= sqrt(float((vecNormSqrd(a) * vecNormSqrd(b)))); // get cos(angle)

    return acos(dp) * float(kmath::sgn(cross));
}

float geom::ptAngle(const Point &a, const Point &b, const Point &c) {
    return vecAngle((a - b), (c - b));
}

Point geom::clampPt(Point pt, const Rect &r) {
    pt.x = int(kmath::Interval(float(r.x), float(r.x + r.width)).closest(float(pt.x)));
    pt.y = int(kmath::Interval(float(r.y), float(r.y + r.height)).closest(float(pt.y)));
    return pt;
}

Point geom::getOffsetFromLocFlags(Size small, Size big, cvutils::XLoc xFlag, cvutils::YLoc yFlag) {
    Point offset;

    switch (xFlag) {
        case FIX_CENTER_X:
            offset.x = (big.width - small.width) / 2;
            break;
        case FIX_LEFT:
            offset.x = 0;
            break;
        case FIX_RIGHT:
            offset.x = big.width - small.width;
            break;
        default:
            throw invalid_argument("xFlag not one of: FIX_CENTER_X, FIX_LEFT, FIX_RIGHT");
    }

    switch (yFlag) {
        case FIX_CENTER_Y:
            offset.y = (big.height - small.height) / 2;
            break;
        case FIX_TOP:
            offset.y = 0;
            break;
        case FIX_BOTTOM:
            offset.y = big.height - small.height;
            break;
        default:
            throw invalid_argument("yFlag not one of: FIX_CENTER_Y, FIX_TOP, FIX_BOTTOM");
    }

    return offset;
}

Rect geom::getAvgRect(const Rect &a, const Rect &b)
{
    return Rect((a.x + b.x) / 2, (a.y + b.y) / 2, (a.width + b.width) / 2, (a.height + b.height) / 2);
}

Rect geom::getAvgRect(const vector<Rect> &rects) {
    int xSum = 0, ySum = 0, wSum = 0, hSum = 0;
    for (auto r : rects) {
        xSum += r.x;
        ySum += r.y;
        wSum += r.width;
        hSum += r.height;
    }
    return Rect(xSum / int(rects.size()), ySum / int(rects.size()), wSum / int(rects.size()), hSum / int(rects.size()));
}

Rect geom::expandRect(const Rect &r, const Size &s) {
    return Rect(r.x - s.width, r.y - s.height, r.width + 2 * s.width, r.height + 2 * s.height);
}

void cvutils::cvtBGR2RG(const cv::Mat_<cv::Vec3b> &src, cv::Mat_<cv::Vec3b> &dest) {
    dest.create(src.size());
    float sum;
    cv::Vec3b srcPx;
    applyBinaryOp(
            [&](int row, int col, const cv::Vec3b *srcIt, cv::Vec3b *destIt) {
                srcPx = *srcIt;
                sum = srcPx[0] + srcPx[1] + srcPx[2];
                (*destIt)[0] = 255 * srcPx[2] / sum;
                (*destIt)[1] = 255 * srcPx[1] / sum;
            },
            src,
            dest
            );
}

/*** namespace `transform` ***/

Mat &cvutils::transform::getRotatedROI(const Mat &src, const RotatedRect &r, Mat &dest) {
    Mat transMat;
    transform::rotozoomMat(src, dest, r.angle, 1.0, &transMat);
    Point newPt = transform::warpAffinePt(transMat, r.center);
    dest = dest(Rect(Point2f(float(newPt.x) - r.size.width * 0.5f, float(newPt.y) - r.size.height * 0.5f), r.size));
    return dest;
}

void cvutils::transform::rotozoomMat(const Mat &src, Mat &dest, float angle, float scale, Mat *transMat) {
    Size2f newSize = rotateSize(src.size() * scale, angle);

    Mat tempTransMat;
    if (transMat == NULL) {
        transMat = &tempTransMat;
    }

    *transMat = getRotationMatrix2D(
            Point2f(float(src.cols) * 0.5f, float(src.rows) * 0.5f),
            angle,
            scale
            );
    transMat->at<double>(0, 2) += (newSize.width - float(src.cols)) / 2.0f;
    transMat->at<double>(1, 2) += (newSize.height - float(src.rows)) / 2.0f;

    warpAffine(src, dest, *transMat, newSize);
}

Point2f cvutils::transform::warpAffinePt(const Mat_<float> &transMat, Point2f pt) {
    pt.x = transMat(0, 0) * pt.x + transMat(0, 1) * pt.y + transMat(0, 2);
    pt.y = transMat(1, 0) * pt.y + transMat(1, 1) * pt.y + transMat(1, 2);

    return pt;
}

Size2f cvutils::transform::rotateSize(Size2f s, float angle) {
    float w = s.width, h = s.height;
    angle = float(fabs(angle / 180 * kmath::PI));
    float diagAngle = atan(h / w);
    float diagLen = sqrt(w * w + h * h);
    return Size2f(
            kmath::fround(fabs(diagLen * cos(diagAngle - angle))),
            kmath::fround(fabs(diagLen * sin(diagAngle + angle)))
            );
}

ostream &cv::operator<<(ostream &out, const Rect r) {
    out << "<Rect x=" << r.x << " y=" << r.y << " width=" << r.width << " height=" << r.height << ">";
    return out;
}

ostream &cv::operator<<(ostream &out, const Size s) {
    out << "<Size width=" << s.width << " height=" << s.height << ">";
    return out;
}

ostream &cv::operator<<(ostream &out, const Size2f s) {
    out << "<Size2f width=" << s.width << " height=" << s.height << ">";
    return out;
}

ostream &cv::operator<<(ostream &out, const Scalar s) {
    out << "<Scalar (" << s[0] << ", " << s[1] << ", " << s[2] << ", " << s[3] << ")>";
    return out;
}

ostream &cv::operator<<(ostream &out, const uchar c) {
    out << unsigned(c);
    return out;
}

Size2f cv::operator*(const Size2f &s, float a) {
    return Size2f(kmath::fround(s.width * a), kmath::fround(s.height * a));
}

Size2f cv::operator*(float a, const Size2f &s) {
    return Size2f(kmath::fround(s.width * a), kmath::fround(s.height * a));
}

Size2f cv::operator/(const Size2f &s, float a) {
    return Size2f(kmath::fround(s.width / a), kmath::fround(s.height / a));
}

bool cv::operator==(const Size2f &a, const Size2f &b) {
    return (a.width == b.width and a.height == b.height);
}

bool cv::operator!=(const Size2f &a, const Size2f &b) {
    return not (a == b);
}

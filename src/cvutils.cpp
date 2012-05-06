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
    while (true) {
        if (waitKey(30) >= 0) {
            break;
        }
    }
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

void cvutils::faceDetectScaled(const Mat &src, float scale, vector<Rect> &out) {
    Mat scaled;
    static CascadeClassifier cascade("/Users/kevinhan/Documents/pure/haarcascades/haarcascade_frontalface_alt.xml");

    resize(src, scaled, Size(), scale, scale);

    cascade.detectMultiScale(scaled, out);

    for (auto &rect : out) {
        rect.x = int(float(rect.x) / scale);
        rect.y = int(float(rect.y) / scale);
        rect.width = int(float(rect.width) / scale);
        rect.height = int(float(rect.height) / scale);
    }
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

float geom::vecAngle(const Vec2i a, const Vec2i b) {
    float dp = float(a.dot(b));
    float cross = float(b[1] * a[0] - b[0] * a[1]);
    dp /= sqrt(float((vecNormSqrd(a) * vecNormSqrd(b)))); // get cos(angle)

    return acos(dp) * float(kmath::sgn(cross));
}

float geom::ptAngle(const Point a, const Point b, const Point c) {
    return vecAngle((a - b), (c - b));
}

void geom::kCurvatures(const vector<Point> &pts, unsigned k, vector<float> &out) {
    out.resize(max<size_t>(pts.size() - 2 * k, 0));

    Point off1, off2;
    for (unsigned i = 2 * k; i < pts.size(); i++) {
        off1 = pts[i - 2 * k] - pts[i - k];
        off2 = pts[i] - pts[i - k];

        float dp = float(off1.dot(off2));
        dp /= sqrt(float((ptNormSqrd(off1) * ptNormSqrd(off2)))); // get cos(angle)

        float angle = acos(dp);
        if (off2.y * off1.x - off2.x * off1.y < 0) {
            // cross product is negative
            angle = -angle;
        }
        out[i - 2 * k] = angle;
    }
}

Point geom::boundPtInRect(Point pt, Rect r) {
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

void cvutils::cvtBGR2RG(const Mat &src, Mat &dest) {
    dest.create(src.size(), CV_8UC3);
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            Vec3b px = src.at<Vec3b>(i, j);
            float sum = float(px[0] + px[1] + px[2]);
            px[0] = uchar(float(255 * px[2]) / sum);
            px[1] = uchar(float(255 * px[1]) / sum);
            dest.at<Vec3b>(i, j) = px;
        }
    }
}

/*** namespace `transform` ***/

Mat &cvutils::transform::getRotatedROI(const Mat &src, RotatedRect &r, Mat &dest) {
    Mat transMat;
    transform::rotozoomMat(src, dest, r.angle, 1.0, &transMat);
    Point newPt = transform::warpAffinePt(transMat, r.center);
    dest = dest(Rect(Point2f(float(newPt.x) - r.size.width * 0.5f, float(newPt.y) - r.size.height * 0.5f), r.size));
    return dest;
}

Mat &cvutils::transform::rotozoomMat(const Mat &src, Mat &dest, float angle, float scale, Mat *transMat) {
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
    return dest;
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

ostream &operator<<(ostream &out, const Rect r) {
    out << "<Rect x=" << r.x << " y=" << r.y << " width=" << r.width << " height=" << r.height << ">";
    return out;
}

ostream &operator<<(ostream &out, const Size s) {
    out << "<Size width=" << s.width << " height=" << s.height << ">";
    return out;
}

ostream &operator<<(ostream &out, const Size2f s) {
    out << "<Size2f width=" << s.width << " height=" << s.height << ">";
    return out;
}

ostream &operator<<(ostream &out, const Scalar s) {
    out << "<Scalar (" << s[0] << ", " << s[1] << ", " << s[2] << ", " << s[3] << ")>";
    return out;
}

ostream &operator<<(ostream &out, const uchar c) {
    out << unsigned(c);
    return out;
}

Size2f operator*(const Size2f &s, float a) {
    return Size2f(kmath::fround(s.width * a), kmath::fround(s.height * a));
}

Size2f operator*(float a, const Size2f &s) {
    return Size2f(kmath::fround(s.width * a), kmath::fround(s.height * a));
}

Size2f operator/(const Size2f &s, float a) {
    return Size2f(kmath::fround(s.width / a), kmath::fround(s.height / a));
}

bool operator==(const Size2f &a, const Size2f &b) {
    return (a.width == b.width and a.height == b.height);
}

bool operator!=(const Size2f &a, const Size2f &b) {
    return not (a == b);
}

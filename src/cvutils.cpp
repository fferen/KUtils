#include <iostream>

#include <opencv2/opencv.hpp>

#include "cvutils.hpp"

using namespace cv;
using namespace cvutils;

void cvutils::waitForKeypress() {
    while (true) {
        if (waitKey(30) >= 0) {
            break;
        }
    }
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
    out << (unsigned)c;
    return out;
}

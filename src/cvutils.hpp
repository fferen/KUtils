#pragma once

#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;

/*! \brief OpenCV utilities. */
namespace cvutils {
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

    /*! Wait until a key is pressed. Focus must be in an OpenCV window. */
    void waitForKeypress();
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

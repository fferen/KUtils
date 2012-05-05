#pragma once

#ifdef __GNUC__
#define UNUSED_VAR __attribute__ ((unused))
#else
#define UNUSED_VAR
#endif

#include <cstdint>

#include <iostream>

#include <opencv2/opencv.hpp>
#include <ApplicationServices/ApplicationServices.h>

#include "io.hpp"

using namespace std;

/*! \brief OSX-specific utilities. */
namespace osx {
    /* "Reverse enum" for the enum type `CGColorSpaceModel`, used for display
     * purposes. Starting index in the enum is -1.
     */
    UNUSED_VAR static const char *COLOR_SPACE_MODEL_NAMES[] = {
        "kCGColorSpaceModelUnknown",
        "kCGColorSpaceModelMonochrome",
        "kCGColorSpaceModelRGB",
        "kCGColorSpaceModelCMYK",
        "kCGColorSpaceModelLab",
        "kCGColorSpaceModelDeviceN",
        "kCGColorSpaceModelIndexed",
        "kCGColorSpaceModelPattern"
    };

    /* "Reverse enum" for the enum type `CGImageAlphaInfo`. */
    UNUSED_VAR static const char *ALPHA_INFO_NAMES[] = {
        "kCGImageAlphaNone",
        "kCGImageAlphaPremultipliedLast",
        "kCGImageAlphaPremultipliedFirst",
        "kCGImageAlphaLast",
        "kCGImageAlphaFirst",
        "kCGImageAlphaNoneSkipLast",
        "kCGImageAlphaNoneSkipFirst"
    };

    /*! \brief Utilities for modeling the mouse state and controlling the mouse. */
    namespace mouse {
        enum MouseButton { LEFT_BUTTON, RIGHT_BUTTON };

        /*! Simple data holder for a button flag and a position. */
        struct MouseState {
            static const uint16_t LEFT_DOWN;
            static const uint16_t RIGHT_DOWN;
            static const uint16_t SCROLL;

            /*! Can be any OR'd combination of the above flags. */
            uint16_t btn;
            cv::Point pos;

            MouseState();
            MouseState(uint16_t btn, cv::Point pos);
        };

        inline CGPoint cvPtToCGPt(cv::Point pt) {
            return CGPointMake(pt.x, pt.y);
        }

        /*! Move cursor to `pos`. */
        void move(CGPoint pos);
        void move(cv::Point pos);

        /*! Simulate a mouse down event at `pos`. */
        void down(MouseButton button, CGPoint pos);
        void down(MouseButton button, cv::Point pos);

        /*! Simulate a mouse up event at `pos`. */
        void up(MouseButton button, CGPoint pos);
        void up(MouseButton button, cv::Point pos);

        /*! Simulate a mouse click (button down, then up) event at `pos`. */
        void click(MouseButton button, CGPoint pos);
        void click(MouseButton button, cv::Point pos);

        /*! Simulate a mouse drag event at from `startPos` to `endPos`. */
        void drag(MouseButton button, CGPoint startPos, CGPoint endPos);
        void drag(MouseButton button, cv::Point startPos, cv::Point endPos);
    }

    /*! \brief Functions for extracting the display image. */
    namespace disp {
        enum RGBType {
            COLOR_BGR,
            COLOR_ARGB,
            COLOR_RGBA
        };

        /*! Return a `cv::Mat` of the current screen.
         *
         * `destType` is passed to `CGImageRefToMat` to acquire the `cv::Mat`.
         */
        cv::Mat &getScreen(cv::Mat &out, RGBType destType=COLOR_BGR);
        cv::Mat getScreen(RGBType destType=COLOR_BGR);

        /*! Convert a `CGImageRef` to a `cv::Mat`.
         *
         * `image` must have 32 bits per pixel; if it doesn't, `invalid_argument`
         * will be thrown.
         *
         * `destType` specifies the pixel format of the resulting `cv::Mat`.
         */
        cv::Mat &CGImageRefToMat(
                const CGImageRef &image,
                cv::Mat &out,
                RGBType destType=COLOR_BGR
                ) throw (invalid_argument);
    }
}

/* Printers */
ostream &operator<<(ostream &out, const osx::mouse::MouseState &m);

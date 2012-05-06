#pragma once

#ifdef __GNUC__
#define UNUSED_VAR __attribute__ ((unused))
#else
#define UNUSED_VAR
#endif

#include <cstdint>

#include <iostream>
#include <memory>

#include <ApplicationServices/ApplicationServices.h>

using namespace std;

/*! OSX-specific utilities. */
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

    /*! Utilities for modeling the mouse state and controlling the mouse. */
    namespace mouse {
        enum MouseButton { LEFT_BUTTON, RIGHT_BUTTON };

        /*! Simple data holder for a button flag and a position. */
        struct MouseState {
            static const uint16_t LEFT_DOWN;
            static const uint16_t RIGHT_DOWN;
            static const uint16_t SCROLL;

            /*! Can be any OR'd combination of the above flags. */
            uint16_t btn;
            CGPoint pos;

            MouseState();
            MouseState(uint16_t btn, CGPoint pos);

            friend ostream &operator<<(ostream &out, const MouseState &m);
        };

        /*! Move cursor to `pos`. */
        void move(CGPoint pos);

        /*! Simulate a mouse down event at `pos`. */
        void down(MouseButton button, CGPoint pos);

        /*! Simulate a mouse up event at `pos`. */
        void up(MouseButton button, CGPoint pos);

        /*! Simulate a mouse click (button down, then up) event at `pos`. */
        void click(MouseButton button, CGPoint pos);

        /*! Simulate a mouse drag event at from `startPos` to `endPos`. */
        void drag(MouseButton button, CGPoint startPos, CGPoint endPos);
    }

    /*! Functions for extracting the display image. */
    namespace disp {
        enum RGBType {
            COLOR_BGR,
            COLOR_ARGB,
            COLOR_RGBA
        };

        struct Image : public shared_ptr<CGImage> {
            Image(const CGImageRef &imRef);

            operator CGImageRef() const;
        };

        /*! Return the current screen. */
        Image getScreen();
    }
}

/*! Print `CGColorSpaceRef` info. */
ostream &operator<<(ostream &out, const CGColorSpaceRef &colorSpace);

/*! Print `CGImageRef` info. */
ostream &operator<<(ostream &out, const CGImageRef &imRef);

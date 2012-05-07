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

#ifdef __GNUC__
#define UNUSED_VAR __attribute__ ((unused))
#else
#define UNUSED_VAR
#endif

#include <cstdint>

#include <iostream>
#include <memory>

#include <ApplicationServices/ApplicationServices.h>

#include "mouse.hpp"
#include "kmath.hpp"

using namespace std;
// to avoid name conflicts
namespace ms = mouse;

/*! OSX-specific utilities. */
namespace osx {
    /*! "Reverse enum" for the enum type `CGColorSpaceModel`, used for display
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

    /*! "Reverse enum" for the enum type `CGImageAlphaInfo`. */
    UNUSED_VAR static const char *ALPHA_INFO_NAMES[] = {
        "kCGImageAlphaNone",
        "kCGImageAlphaPremultipliedLast",
        "kCGImageAlphaPremultipliedFirst",
        "kCGImageAlphaLast",
        "kCGImageAlphaFirst",
        "kCGImageAlphaNoneSkipLast",
        "kCGImageAlphaNoneSkipFirst"
    };

    /*! Convert a `kmath::Point<T>` to a `CGPoint`. */
    template <typename T>
    inline CGPoint toCGPt(kmath::Point<T> pt) {
        return CGPointMake(pt.x, pt.y);
    }

    /*! Convert a `CGPoint` to a `kmath::PointF`. */
    inline kmath::PointF toPt(CGPoint pt) {
        return kmath::PointF(pt.x, pt.y);
    }

    /*! Utilities for controlling the mouse. */
    namespace mouse {
        /*! Move cursor to `pos`. */
        void move(CGPoint pos);

        /*! Simulate a mouse down event at `pos`. */
        void down(ms::Button button, CGPoint pos);

        /*! Simulate a mouse up event at `pos`. */
        void up(ms::Button button, CGPoint pos);

        /*! Simulate a mouse click (button down, then up) event at `pos`. */
        void click(ms::Button button, CGPoint pos);

        /*! Simulate a mouse drag event at from `startPos` to `endPos`. */
        void drag(ms::Button button, CGPoint startPos, CGPoint endPos);
    }

    /*! Functions for extracting the display image. */
    namespace disp {
        enum RGBType {
            COLOR_BGR,
            COLOR_ARGB,
            COLOR_RGBA
        };

        /*! Memory-managed wrapper aronud `CGImageRef` using a `shared_ptr`. */
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

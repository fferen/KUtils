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

/* Here's a useful link to the pixel formats of graphics contexts:
 *
 * http://developer.apple.com/library/ios/#documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-BCIBHHBB
 */
#include <cstdint>

#include <iostream>
#include <stdexcept>

#include <ApplicationServices/ApplicationServices.h>

#include "osx.hpp"
#include "mouse.hpp"

using namespace std;
// to avoid name conflicts
namespace ms = mouse;

static void _mouseAction(mouse::Button button, CGPoint pos, CGEventType lEvtType, CGEventType rEvtType) {
    CGEventRef evt = CGEventCreateMouseEvent(
            nullptr,
            button == ms::LEFT_BUTTON ? lEvtType : rEvtType,
            pos,
            0
            );
    CGEventPost(kCGHIDEventTap, evt);
    CFRelease(evt);
}

/* Namespace `mouse` */

void osx::mouse::move(CGPoint pos) {
    _mouseAction(ms::LEFT_BUTTON, pos, kCGEventMouseMoved, kCGEventMouseMoved);
}

void osx::mouse::down(ms::Button button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseDown, kCGEventRightMouseDown);
}

void osx::mouse::up(ms::Button button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseUp, kCGEventRightMouseUp);
}

void osx::mouse::click(ms::Button button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseDown, kCGEventRightMouseDown);
    _mouseAction(button, pos, kCGEventLeftMouseUp, kCGEventRightMouseUp);
}

void osx::mouse::drag(ms::Button button, CGPoint startPos, CGPoint endPos) {
    _mouseAction(button, startPos, kCGEventLeftMouseDown, kCGEventRightMouseDown);
    _mouseAction(button, endPos, kCGEventMouseMoved, kCGEventMouseMoved);
    _mouseAction(button, endPos, kCGEventLeftMouseUp, kCGEventRightMouseUp);
    /* The documentation says this sends a drag with the mouse button down, but
     * I can't get it to work.
     */
//    _mouseAction(button, endPos, kCGEventLeftMouseDragged, kCGEventRightMouseDragged);
}

/* Namespace `disp` */

osx::disp::Image::Image(const CGImageRef &imRef)
        : shared_ptr<CGImage>(imRef, CGImageRelease) {
}

osx::disp::Image::operator CGImageRef() const {
    return this->get();
}

osx::disp::Image osx::disp::getScreen() {
    return CGDisplayCreateImage(CGMainDisplayID());
}

/* Printers */

ostream &operator<<(ostream &out, const CGColorSpaceRef &colorSpace) {
    out << "<CGColorSpace model=" << osx::COLOR_SPACE_MODEL_NAMES[CGColorSpaceGetModel(colorSpace) + 1] << ">";
    return out;
}

ostream &operator<<(ostream &out, const CGImageRef &imRef) {
    auto colorSpace = CGImageGetColorSpace(imRef);
    auto bpc = CGImageGetBitsPerComponent(imRef);
    out << "<CGImageRef size=(" << CGImageGetWidth(imRef) << ", " << CGImageGetHeight(imRef) << ") bitsPerComponent="
        << bpc << " chans=" << CGImageGetBitsPerPixel(imRef) / bpc << " colorSpace=" << colorSpace
        << " alphaInfo=" << osx::ALPHA_INFO_NAMES[CGImageGetAlphaInfo(imRef)] << ">";
    return out;
}


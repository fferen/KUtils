/* Here's a useful link to the pixel formats of graphics contexts:
 *
 * http://developer.apple.com/library/ios/#documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-BCIBHHBB
 */
#include <cstdint>

#include <iostream>
#include <stdexcept>

#include <ApplicationServices/ApplicationServices.h>

#include "osx.hpp"

using namespace std;
using namespace osx;

static void _mouseAction(mouse::MouseButton button, CGPoint pos, CGEventType lEvtType, CGEventType rEvtType) {
    CGEventRef evt = CGEventCreateMouseEvent(
            nullptr,
            button == mouse::LEFT_BUTTON ? lEvtType : rEvtType,
            pos,
            0
            );
    CGEventPost(kCGHIDEventTap, evt);
    CFRelease(evt);
}

/* Namespace `mouse` */

const uint16_t mouse::MouseState::LEFT_DOWN     = 0x0001;
const uint16_t mouse::MouseState::RIGHT_DOWN    = 0x0002;
const uint16_t mouse::MouseState::SCROLL        = 0x0004;

mouse::MouseState::MouseState()
        : btn(0), pos(CGPointMake(-1, -1)) {
}

mouse::MouseState::MouseState(uint16_t btn, CGPoint pos)
        : btn(btn), pos(pos) {
}

ostream &operator<<(ostream &out, const mouse::MouseState &m) {
    out << "<MouseState btn=" << m.btn << " pos=(" << m.pos.x << ", " << m.pos.y << ")>";
    return out;
}

void mouse::move(CGPoint pos) {
    _mouseAction(mouse::LEFT_BUTTON, pos, kCGEventMouseMoved, kCGEventMouseMoved);
}

void mouse::down(mouse::MouseButton button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseDown, kCGEventRightMouseDown);
}

void mouse::up(mouse::MouseButton button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseUp, kCGEventRightMouseUp);
}

void mouse::click(mouse::MouseButton button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseDown, kCGEventRightMouseDown);
    _mouseAction(button, pos, kCGEventLeftMouseUp, kCGEventRightMouseUp);
}

void mouse::drag(mouse::MouseButton button, CGPoint startPos, CGPoint endPos) {
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

disp::Image osx::disp::getScreen() {
    return CGDisplayCreateImage(CGMainDisplayID());
}

/* Printers */

ostream &operator<<(ostream &out, const CGColorSpaceRef &colorSpace) {
    out << "<CGColorSpace model=" << COLOR_SPACE_MODEL_NAMES[CGColorSpaceGetModel(colorSpace) + 1] << ">";
    return out;
}

ostream &operator<<(ostream &out, const CGImageRef &imRef) {
    auto colorSpace = CGImageGetColorSpace(imRef);
    auto bpc = CGImageGetBitsPerComponent(imRef);
    out << "<CGImageRef size=(" << CGImageGetWidth(imRef) << ", " << CGImageGetHeight(imRef) << ") bitsPerComponent="
        << bpc << " chans=" << CGImageGetBitsPerPixel(imRef) / bpc << " colorSpace=" << colorSpace
        << " alphaInfo=" << ALPHA_INFO_NAMES[CGImageGetAlphaInfo(imRef)] << ">";
    return out;
}


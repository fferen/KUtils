/* Here's a useful link to the pixel formats of graphics contexts:
 *
 * http://developer.apple.com/library/ios/#documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-BCIBHHBB
 */
#include <cstdint>

#include <iostream>
#include <stdexcept>

#include <opencv2/opencv.hpp>
#include <ApplicationServices/ApplicationServices.h>

#include "osx.hpp"
#include "cvutils.hpp"

using namespace std;
using namespace osx;

static cv::Mat &_cvtRGBColor(
        const cv::Mat &in,
        cv::Mat &out,
        CGImageAlphaInfo srcAlpha,
        disp::RGBType destFmt
        ) throw (invalid_argument) {
    if (in.data == out.data) {
        throw invalid_argument("(_cvtRGBColor) `in` must be different than `out`");
    }

    switch (srcAlpha) {
        case kCGImageAlphaPremultipliedLast:
            // fall through
        case kCGImageAlphaLast:
            // fall through
        case kCGImageAlphaNoneSkipLast:
            switch (destFmt) {
                case disp::COLOR_BGR:
                    cvtColor(in, out, cv::COLOR_RGBA2BGR);
                    break;
                case disp::COLOR_ARGB:
                    out.create(in.size(), CV_8UC4);
                    mixChannels(&in, 1, &out, 1, (int []){0,1, 1,2, 2,3, 3,0}, 4);
                    break;
                case disp::COLOR_RGBA:
                    out = in;
                    break;
                default:
                    throw invalid_argument("(_cvtRGBColor) invalid destFmt");
                    break;
            }
            break;
        case kCGImageAlphaPremultipliedFirst:
            // fall through
        case kCGImageAlphaFirst:
            // fall through
        case kCGImageAlphaNoneSkipFirst:
            switch (destFmt) {
                case disp::COLOR_BGR:
                    out.create(in.size(), CV_8UC3);
                    mixChannels(&in, 1, &out, 1, (int []){1,2, 2,1, 3,0}, 3);
                    break;
                case disp::COLOR_ARGB:
                    out = in;
                    break;
                case disp::COLOR_RGBA:
                    out.create(in.size(), CV_8UC4);
                    mixChannels(&in, 1, &out, 1, (int []){0,3, 1,0, 2,1, 3,2}, 4);
                    break;
                default:
                    throw invalid_argument("(_cvtRGBColor) invalid destFmt");
                    break;
            }
            break;
        default:
            throw invalid_argument("(_cvtRGBColor) invalid srcAlpha");
            break;
    }

    return out;
}

static void _mouseAction(mouse::MouseButton button, CGPoint pos, CGEventType lEvtType, CGEventType rEvtType) {
    CGEventRef evt = CGEventCreateMouseEvent(
            NULL,
            button == mouse::LEFT_BUTTON ? lEvtType : rEvtType,
            pos,
            0
            );
    CGEventPost(kCGHIDEventTap, evt);
    CFRelease(evt);
}

/* Printers */

ostream &operator<<(ostream &out, const mouse::MouseState &m) {
    out << "<MouseState btn=" << m.btn << " pos=(" << m.pos.x << ", " << m.pos.y << ")>";
    return out;
}

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

/* Namespace `mouse` */

const uint16_t mouse::MouseState::LEFT_DOWN     = 0x0001;
const uint16_t mouse::MouseState::RIGHT_DOWN    = 0x0002;
const uint16_t mouse::MouseState::SCROLL        = 0x0004;

mouse::MouseState::MouseState()
        : btn(0), pos(cv::Point(-1, -1)) {
}

mouse::MouseState::MouseState(uint16_t btnState, cv::Point pos)
        : btn(btn), pos(pos) {
}

void mouse::move(CGPoint pos) {
    _mouseAction(mouse::LEFT_BUTTON, pos, kCGEventMouseMoved, kCGEventMouseMoved);
}

void mouse::move(cv::Point pos) {
    _mouseAction(mouse::LEFT_BUTTON, mouse::cvPtToCGPt(pos), kCGEventMouseMoved, kCGEventMouseMoved);
}

void mouse::down(mouse::MouseButton button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseDown, kCGEventRightMouseDown);
}

void mouse::down(mouse::MouseButton button, cv::Point pos) {
    _mouseAction(button, mouse::cvPtToCGPt(pos), kCGEventLeftMouseDown, kCGEventRightMouseDown);
}

void mouse::up(mouse::MouseButton button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseUp, kCGEventRightMouseUp);
}

void mouse::up(mouse::MouseButton button, cv::Point pos) {
    _mouseAction(button, mouse::cvPtToCGPt(pos), kCGEventLeftMouseUp, kCGEventRightMouseUp);
}

void mouse::click(mouse::MouseButton button, CGPoint pos) {
    _mouseAction(button, pos, kCGEventLeftMouseDown, kCGEventRightMouseDown);
    _mouseAction(button, pos, kCGEventLeftMouseUp, kCGEventRightMouseUp);
}

void mouse::click(mouse::MouseButton button, cv::Point pos) {
    _mouseAction(button, mouse::cvPtToCGPt(pos), kCGEventLeftMouseDown, kCGEventRightMouseDown);
    _mouseAction(button, mouse::cvPtToCGPt(pos), kCGEventLeftMouseUp, kCGEventRightMouseUp);
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

void mouse::drag(mouse::MouseButton button, cv::Point startPos, cv::Point endPos) {
    _mouseAction(button, mouse::cvPtToCGPt(startPos), kCGEventLeftMouseDown, kCGEventRightMouseDown);
    _mouseAction(button, mouse::cvPtToCGPt(endPos), kCGEventMouseMoved, kCGEventMouseMoved);
    _mouseAction(button, mouse::cvPtToCGPt(endPos), kCGEventLeftMouseUp, kCGEventRightMouseUp);
//    _mouseAction(button, mouse::cvPtToCGPt(endPos), kCGEventLeftMouseDragged, kCGEventRightMouseDragged);
}

/* Namespace `disp` */

cv::Mat &disp::getScreen(cv::Mat &out, disp::RGBType destType) {
    CGImageRef screenIm(CGDisplayCreateImage(CGMainDisplayID()));
    disp::CGImageRefToMat(screenIm, out, destType);
    CGImageRelease(screenIm);
    return out;
}

cv::Mat disp::getScreen(disp::RGBType destType) {
    cv::Mat out;
    return disp::getScreen(out, destType);
}

cv::Mat &disp::CGImageRefToMat(
        const CGImageRef &image,
        cv::Mat &out,
        disp::RGBType destType
        ) throw (invalid_argument) {
//    prof::init();

    cv::Mat temp;

    size_t w = CGImageGetWidth(image), h = CGImageGetHeight(image);

    if (CGImageGetBitsPerPixel(image) != 32) {
        throw invalid_argument("(CGImageRefToMat) `image` must be 32 bits per pixel");
    }

    temp.create((int)h, (int)w, CV_8UC4);

    CGColorSpaceRef colorSpace = CGImageGetColorSpace(image);

    CGContextRef context = CGBitmapContextCreate(
            temp.data,
            w,
            h,
            CGImageGetBitsPerComponent(image),
            CGImageGetBytesPerRow(image),
            colorSpace,
            CGImageGetAlphaInfo(image)
    );

//    prof::printCycles("copied data");

    CGContextDrawImage(
            context,
            CGRectMake(0, 0, (CGFloat)w, (CGFloat)h),
            image
            );

//    prof::printCycles("drawed image");

    _cvtRGBColor(temp, out, CGImageGetAlphaInfo(image), destType);

    CGContextRelease(context);

//    prof::printCycles("released");

    return out;
}

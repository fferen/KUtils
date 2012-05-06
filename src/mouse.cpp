#include <iostream>

#include "mouse.hpp"
#include "kmath.hpp"

using namespace std;
using namespace mouse;
using namespace kmath;

const uint16_t State::LEFT_DOWN     = 0x0001;
const uint16_t State::RIGHT_DOWN    = 0x0002;
const uint16_t State::SCROLL        = 0x0004;

State::State()
        : btn(0), pos(kmath::PointI(-1, -1)) {
}

State::State(uint16_t btn, kmath::PointI pos)
        : btn(btn), pos(pos) {
}

ostream &operator<<(ostream &out, const mouse::State &m) {
    out << "<mouse::State btn=" << m.btn << " pos=(" << m.pos.x << ", " << m.pos.y << ")>";
    return out;
}

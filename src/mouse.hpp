#pragma once

#include <cstdint>

#include <iostream>

#include "kmath.hpp"

using namespace std;

/*! Utilities for modeling the mouse state. */
namespace mouse {
    enum Button { LEFT_BUTTON, RIGHT_BUTTON };

    /*! Simple data holder for a button flag and a position. */
    struct State {
        static const uint16_t LEFT_DOWN;
        static const uint16_t RIGHT_DOWN;
        static const uint16_t SCROLL;

        /*! Can be any OR'd combination of the above flags. */
        uint16_t btn;
        kmath::PointI pos;

        State();
        State(uint16_t btn, kmath::PointI pos);

        friend ostream &operator<<(ostream &out, const State &m);
    };
}

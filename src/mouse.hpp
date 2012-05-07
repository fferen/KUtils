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

#include <cstdint>

#include <iostream>

#include "kmath.hpp"

using namespace std;

/*! Utilities for modeling the mouse state. */
namespace mouse {
    enum Button { LEFT_BUTTON, RIGHT_BUTTON };

    /*! Simple data holder for a button flag and a position. */
    struct State {
        /*! Flag indicating left mouse button is down. */
        static const uint16_t LEFT_DOWN;
        /*! Flag indicating right mouse button is down. */
        static const uint16_t RIGHT_DOWN;
        /*! Flag indicating middle mouse button is down. */
        static const uint16_t MIDDLE_DOWN;

        /*! Can be any `OR`'d combination of the below flags, or `0` if no buttons
         * down.
         */
        uint16_t btn;
        /*! Mouse position in integer coordinates. */
        kmath::PointI pos;

        State();
        State(uint16_t btn, kmath::PointI pos);

        friend ostream &operator<<(ostream &out, const State &m);
    };
}

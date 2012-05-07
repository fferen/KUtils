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

#include <cmath>
#include <iostream>

using namespace std;

/*! Implements automatic differentiation.
 *
 * See: http://www.pvv.ntnu.no/~berland/resources/autodiff-triallecture.pdf
 *
 * Many but not all of the functions in the `cmath` library are implemented with
 * a `d*` prefix, eg. `dSin`.
 */
namespace autodiff {
    const double INV_LOG_10 = 1 / log(10);

    /*! Represents the "real" part of a number in member `r` and the derivative
     * in `d`.
     *
     * Can be converted to and from `double`s as required, so that generic code
     * such as:
     * 
     *      template <class NumT>
     *      NumT doIt(NumT x) {
     *          NumT y = x;
     *          for (int i = 0; i < 100; i++) {
     *              y = dSin(x + y);
     *          }
     *          return y;
     *      }
     *
     * will work for both `double`s and `DualNum`s.
     */
    struct DualNum {
        double r;
        double d;

        DualNum(double r, double d=0);

        operator double() const;
    };

    DualNum dPow(const DualNum &a, const DualNum &b);

    DualNum dSin(const DualNum &a);

    DualNum dCos(const DualNum &a);

    DualNum dTan(const DualNum &a);

    DualNum dExp(const DualNum &a);

    DualNum dLog(const DualNum &a);

    DualNum dLog10(const DualNum &a);
}

autodiff::DualNum operator+(const autodiff::DualNum &a, const autodiff::DualNum &b);

autodiff::DualNum operator-(const autodiff::DualNum &a, const autodiff::DualNum &b);

autodiff::DualNum operator-(const autodiff::DualNum &a);

autodiff::DualNum operator*(const autodiff::DualNum &a, const autodiff::DualNum &b);

autodiff::DualNum operator/(const autodiff::DualNum &a, const autodiff::DualNum &b);

ostream &operator<<(ostream &out, const autodiff::DualNum &a);

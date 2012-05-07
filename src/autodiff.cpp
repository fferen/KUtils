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

#include <cmath>
#include <iostream>

#include "autodiff.hpp"

using namespace std;
using namespace autodiff;

DualNum::DualNum(double r, double d)
        : r(r), d(d) {
}

DualNum::operator double() const {
    return this->r;
}

DualNum operator+(const DualNum &a, const DualNum &b) {
    return DualNum(a.r + b.r, a.d + b.d);
}

DualNum operator-(const DualNum &a, const DualNum &b) {
    return DualNum(a.r - b.r, a.d - b.d);
}

DualNum operator-(const DualNum &a) {
    return DualNum(-a.r, -a.d);
}

DualNum operator*(const DualNum &a, const DualNum &b) {
    return DualNum(a.r * b.r, a.d * b.r + a.r * b.d);
}

DualNum operator/(const DualNum &a, const DualNum &b) {
    return DualNum(a.r / b.r, (b.r * a.d - a.r * b.d) / (b.r * b.r));
}

DualNum autodiff::dPow(const DualNum &a, const DualNum &b) {
    double p = pow(a.r, b.r);
    return DualNum(p, p * (b.r * a.d / a.r + b.d * log(a.r)));
}

DualNum autodiff::dSin(const DualNum &a) {
    return DualNum(sin(a.r), cos(a.r) * a.d);
}

DualNum autodiff::dCos(const DualNum &a) {
    return DualNum(cos(a.r), -sin(a.r) * a.d);
}

DualNum autodiff::dTan(const DualNum &a) {
    double c = cos(a.r);
    return DualNum(tan(a.r), a.d / (c * c));
}

DualNum autodiff::dExp(const DualNum &a) {
    double e = exp(a.r);
    return DualNum(e, e * a.d);
}

DualNum autodiff::dLog(const DualNum &a) {
    return DualNum(log(a.r), a.d / a.r);
}

DualNum autodiff::dLog10(const DualNum &a) {
    return DualNum(log10(a.r), INV_LOG_10 * a.d / a.r);
}

ostream &operator<<(ostream &out, const DualNum &a) {
    out << "<DualNum r=" << a.r << " d=" << a.d << ">";
    return out;
}

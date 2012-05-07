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
#include <stdexcept>

#include "kmath.hpp"

using namespace kmath;

kmath::Interval::Interval(float low, float high, bool closeLeft, bool closeRight)
        : low(low), high(high), closeLeft(closeLeft), closeRight(closeRight), size(high - low) {
    if (high < low) {
        throw invalid_argument("high must be >= low");
    }
}

float kmath::Interval::closest(float n) const {
    if (this->contains(n)) {
        return n;
    } else if (n > high and this->closeRight) {
        return high;
    } else if (n <= low and this->closeLeft) {
        return low;
    }
    throw invalid_argument("can't get closest point of open interval");
}
double kmath::normalCDF(double x) {
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = abs(x)/sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return 0.5*(1.0 + sign*y);
}

unsigned kmath::factorial(unsigned n) {
    if (n == 0) {
        return 1;
    }

    unsigned res = 1;
    for (; n > 1; n--) {
        res *= n;
    }
    return res;
}

unsigned kmath::combinations(unsigned n, unsigned k) {
    if (k > n) {
        throw range_error("(combinations) k > n");
    }

    if (k > n / 2) {
        k = n - k;
    }

    unsigned res = 1;
    for (; n > n - k; n--) {
        res *= n;
    }
    return res / factorial(k);
}

float kmath::fround(float val, float scale, RoundType action) {
    switch (action) {
        case FLOOR:
            return scale * floor(val / scale);
        case CEIL:
            return scale * ceil(val / scale);
        case ROUND:
            return scale * float(int(val / scale + 0.5));
        default:
            throw invalid_argument("invalid action");
    }
}

int kmath::iround(float val) {
    return int(kmath::fround(val));
}

float kmath::intervalProject(
        float val,
        const Interval &domain,
        const Interval &range,
        const function<float(float)> &func,
        const Interval &funcDomain,
        const Interval &funcRange,
        bool bound
        ) {
    if (bound) {
        val = domain.closest(val);
    }

    if (not domain.contains(val)) {
        throw range_error("val not in domain");
    }

    float inScale = (val - domain.low) / domain.size;
    float outScale = (func(funcDomain.low + inScale * funcDomain.size) - funcRange.low) / funcRange.size;
    if (outScale < 0 or outScale > 1) {
        throw range_error("output of `func` not within `funcRange`");
    }

    return range.low + outScale * range.size;
}

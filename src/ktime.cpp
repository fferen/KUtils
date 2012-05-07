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

#include "ktime.hpp"
#include "io.hpp"

using namespace ktime;

float ktime::toSecs(const TimeDiff &diff) {
    auto ns = chrono::duration_cast<chrono::nanoseconds>(diff).count();
    return float(ns) / 1e9f;
}

ktime::RunningAvgTimer::RunningAvgTimer(size_t sz)
: sum(TimeDiff(0)), lastPoint(ClockT::now()) {
    for (size_t i = 0; i < sz; i++) {
        this->push(TimeDiff(0));
    }
}

void ktime::RunningAvgTimer::addTime() {
    auto now = ClockT::now();
    auto newDiff = now - lastPoint;

    sum += newDiff;
    this->push(newDiff);

    sum -= this->front();
    this->pop();

    lastPoint = now;
}

TimeDiff ktime::RunningAvgTimer::avg() {
    return sum / this->size();
}

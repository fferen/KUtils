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

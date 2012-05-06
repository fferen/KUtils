#pragma once

#include <queue>
#include <chrono>

using namespace std;

/*! Utilities for working with time. */
namespace ktime {
    typedef chrono::system_clock ClockT;

    typedef ClockT::duration TimeDiff;
    typedef ClockT::time_point TimePoint;

    /*! Convert a TimeDiff to seconds. */
    float toSecs(const TimeDiff &diff);

    /*! Keep a running average of the time between calls to `addTime()`. Useful
     * for calculating FPS.
     */
    class RunningAvgTimer : queue<TimeDiff> {
    public:
        TimeDiff sum;
        TimePoint lastPoint;

        RunningAvgTimer(size_t sz);

        void addTime();
        TimeDiff avg();
    };
}

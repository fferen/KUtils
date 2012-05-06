#pragma once

#include <iostream>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include <utility>

using namespace std;

/*! Miscellaneous mathematical functions. */
namespace kmath {
    /*! The ratio of the circumference of a circle to its diameter. */
    const double PI = 3.141592654;

    /*! Type of rounding to do, used in fround(). */
    enum RoundType { ROUND, CEIL, FLOOR };

    /*! Generic point class. Ho hum. */
    template <typename T>
    struct Point {
        T x;
        T y;

        /*! Default initializer - sets `x` and `y` to 0. */
        Point()
                :x(0), y(0) {
        }

        Point(T x, T y)
                :x(x), y(y) {
        }

        friend ostream &operator<<(ostream &out, Point p) {
            out << "<kmath::Point x=" << p.x << " y=" << p.y << ">";
            return out;
        }
    };

    typedef Point<int> PointI;
    typedef Point<float> PointF;
    typedef Point<double> PointD;

    /*! Represents an interval of real numbers. */
    struct Interval {
        float low;
        float high;
        bool closeLeft;
        bool closeRight;
        float size;

        /*! `closeLeft` and `closeRight` denote a closed left and right
         * endpoint, respectively.
         *
         * @throws invalid_argument
         * Thrown if `low` > `high`.
         */
        Interval(float low, float high, bool closeLeft=true, bool closeRight=true);

        /*! Return true if interval contains `n`. */
        inline bool contains(float n) const {
            return ((this->closeLeft ? n >= this->low : n > this->low) and
                    (this->closeRight ? n <= this->high : n < this->high));
        }

        /*! Return the closest value in the interval to the given one.
         *
         * @throws invalid_argument
         * Thrown if no closest value exists (interval is open on that side).
         */
        float closest(float n) const;
    };

    /*! Return 1 if `val` > 0, -1 if `val` < 0, or 0 if `val` == 0. */
    template <typename T>
    inline int sgn(T val) {
        return (val > 0) - (val < 0);
    }

    /*! Specialization for unsigned types. */
    template <>
    inline int sgn(unsigned val) {
        return (val > 0);
    }

    /*! Specialization for unsigned types. */
    template <>
    inline int sgn(unsigned long val) {
        return (val > 0);
    }

    /*! Specialization for unsigned types. */
    template <>
    inline int sgn(unsigned long long val) {
        return (val > 0);
    }

    /*! Return the cumulative density of a Gaussian distribution with mean = 0,
     * variance = 1.
     */
    double normalCDF(double x);

    /*! Return `n!`. */
    unsigned factorial(unsigned n);

    /*! Return `n`C`k` (n choose k).
     *
     * @throws range_error
     * Thrown if k > n.
     */
    unsigned combinations(unsigned n, unsigned k);

    /*! Round val to nearest multiple of scale.
     *
     * `action` is one of: ROUND, FLOOR, CEIL.
     *
     * Rounds up on ties.
     *
     * @throws invalid_argument
     * Thrown if `action` is invalid.
     */
    float fround(float val, float scale=1.0, RoundType action=ROUND);

    /*! Specialization of `fround` that rounds float to nearest integer. */
    int iround(float val);

    /*! Project a value within `domain` to a new Interval `range` using `func`.
     *
     * The value is mapped, using `domain`, onto the function's domain, then the
     * function is called. The result is mapped back, using `funcRange`, onto
     * the output range.
     *
     * With the default arguments, the behavior is to project the value linearly
     * from `domain` to `range`.
     *
     * @param val
     *      The value to map.
     * @param domain
     *      Domain of `val`.
     * @param range
     *      Range of output.
     * @param func
     *      Called on the result of mapping `val` onto `funcDomain`.
     * @param funcDomain
     *      Domain of `func`.
     * @param funcRange
     *      Range of `func`.
     *
     * @throws range_error
     * Thrown if `val` is not within `domain` or if `func` returns something outside of
     * `funcRange`.
     */
    float intervalProject(
            float val,
            const Interval &domain,
            const Interval &range,
            const function<float(float)> &func=[](float x) { return x; },
            const Interval &funcDomain=Interval(0, 1),
            const Interval &funcRange=Interval(0, 1)
            );
}

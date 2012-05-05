#pragma once

#include <cmath>
#include <iostream>

using namespace std;

/*! \brief Implements automatic differentiation.
 *
 * See: http://www.pvv.ntnu.no/~berland/resources/autodiff-triallecture.pdf
 *
 * Many but not all of the functions in the `cmath` library are implemented with
 * a `d*` prefix, eg. `dSin`.
 */
namespace autodiff {
    const double INV_LOG_10 = 1 / log(10);

    /*! \brief Represents the "real" part of a number in member `r` and the
     * derivative in `d`.
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

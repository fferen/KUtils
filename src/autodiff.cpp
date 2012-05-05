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

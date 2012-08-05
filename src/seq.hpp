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
#include <cstddef>

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>

#include "kmath.hpp"

using namespace std;

/*! Generic sequence operations and structures.
 *
 * NB: "sequence" here means "container" in the rest of C++. I just chose
 * "sequence" because "seq" in variable names looks better than "ctr".
 */
namespace seq {

/*! Functions commonly used in functional programming.
 */
namespace functional {
    template <class InSeqT, class OutSeqT, class FuncT>
    OutSeqT &map(const FuncT &func, const InSeqT &in, OutSeqT &out) {
        out.resize(in.size());
        transform(in.begin(), in.end(), out.begin(), func);
        return out;
    }

    template <class InSeqT, class OutSeqT=InSeqT, class FuncT>
    OutSeqT map(const FuncT &func, const InSeqT &in) {
        OutSeqT out;
        return map(func, in, out);
    }

    template <class InSeqT, class OutSeqT, class FuncT>
    OutSeqT &filter(const FuncT &test, const InSeqT &in, OutSeqT &out) {
        size_t curI = 0;
        for (auto elem : in) {
            if (test(elem)) {
                if (curI < out.size()) {
                    out[curI] = elem;
                    curI++;
                } else {
                    out.push_back(elem);
                }
            }
        }
        out.resize(curI);
        return out;
    }

    template <class InSeqT, class OutSeqT=InSeqT, class FuncT>
    OutSeqT filter(const FuncT &test, const InSeqT &in) {
        OutSeqT out;
        return filter(test, in, out);
    }

    template<class SeqT, class FuncT>
    bool any(const FuncT &test, const SeqT &in) {
        for (auto elem : in) {
            if (test(elem)) {
                return true;
            }
        }
        return false;
    }

    template<class SeqT, class FuncT>
    bool all(const FuncT &test, const SeqT &in) {
        for (auto elem : in) {
            if (not test(elem)) {
                return false;
            }
        }
        return true;
    }
}

/*! Mathematical operations on sequences, and sequence structures. */
namespace math {
    template <class NumT>
    struct XRange {
        struct iterator {
            NumT e;
            NumT step;
            bool inclusive;

            NumT operator*(void) { return e; }
            void operator++(void) { e += step; }
            bool operator!=(const iterator &other) {
                // Yes, this breaks symmetry. This will work since the standard
                // specifies that the iterators will be compared as `begin !=
                // end` instead of `end != begin`.
                if (inclusive) {
                    return e <= other.e;
                } else {
                    return e < other.e;
                }
            }
        };

        NumT start;
        NumT stop;
        NumT step;
        bool inclusive;

        iterator begin(void) { return iterator{start, step, inclusive}; }
        iterator end(void) { return iterator{stop, 0, inclusive}; }
    };

    /*! Return `XRange` sequence, similar to Python's "xrange".
     *
     * Suggested usage:
     *
     *      for (auto i : xrange(10)) {
     *          printf("%d\n", i);
     *      }
     *
     * Unlike the Python version, this can also accept floats/doubles, and can
     * include the endpoint of the range by specifying `inclusive` = `true`.
     */
    template <class NumT>
    XRange<NumT> xrange(NumT low, NumT high, NumT step=1, bool inclusive=false) {
        return XRange<NumT>{low, high, step, inclusive};
    }

    template <class NumT>
    XRange<NumT> xrange(NumT high, bool inclusive=false) {
        return xrange<NumT>(0, high, 1, inclusive);
    }

    /*! Construct a sequence of numbers from `low` to `high` with optional
     * `step`, store it in `out`, and return `out`.
     */
    template <class NumT, class SeqT>
    SeqT &range(NumT low, NumT high, SeqT &out, NumT step=1) {
        if (high <= low) {
            out.clear();
            return out;
        }

        out.resize(size_t(ceil(double((high - low) / step))));

        NumT curNum = low;
        for (size_t i = 0; curNum < high; curNum += step, i++) {
            out[i] = curNum;
        }
        return out;
    }

    /*! Construct a sequence of numbers from 0 to high with step=1, store it
     * in `out`, and return `out`.
     */
    template <class NumT, class SeqT>
    SeqT &range(NumT high, SeqT &out) {
        range((NumT)0, high, out);
    }

    /*! Return the sum of sequence elements. */
    template <class SeqT>
    typename SeqT::value_type sum(const SeqT &in) {
        typename SeqT::value_type res;
        for (auto elem : in) {
            res += elem;
        }
        return res;
    }

    /*! Return the arithmetic mean of sequence elements. */
    template <class SeqT>
    typename SeqT::value_type mean(const SeqT &in) {
        return sum(in) / in.size();
    }
}

/*! Get the wrapped index to an array of size `sz`. Throw `length_error` if `sz`
 * == 0.
 */
inline size_t _realI(int i, size_t sz) {
    if (sz == 0) {
        throw length_error("`sz` must be != 0");
    }
    i %= int(sz);
    if (i < 0) {
        i += int(sz);
    }
    return size_t(i);
}

/*! Get the wrapped distance between left and right indices in an array of size
 * `sz`. For example, `_realIDist(3, 0, 4)` == 1.
 */
inline size_t _realIDist(size_t leftI, size_t rightI, size_t sz) {
    if (rightI >= leftI) {
        return rightI - leftI;
    } else {
        return rightI - leftI + sz;
    }
}

/*! Provides a reference and an index of an element in a sequence. */
template <class SeqT>
struct ElemView {
    typename SeqT::reference e;
    long int i;

    /*! `itr` is the iterator to the desired element, `begin` is the starting
     * iterator of the sequence.
     */
    ElemView(typename SeqT::iterator itr, typename SeqT::iterator begin)
    : e(*itr), i(itr - begin) {
    }

    friend ostream &operator<<(ostream &out, const ElemView &e) {
        out << "<ElemView elem=" << e.e << " i=" << e.i << ">";
        return out;
    }
};

/*! Sequence wrapper that supports wrap-around indexing - all accessors will
 * accept negative and out-of-bound indices and wrap around.
 */
template <class SeqT>
struct WrappedSeq {
    /*! The underlying sequence. */
    SeqT &s;

    WrappedSeq(SeqT &s) : s(s) {
    }

    //@{
    /*! Wrapped accessor. */
    typename SeqT::reference at(int i) {
        return this->s.at(this->realI(i));
    }

    const typename SeqT::reference at(int i) const {
        return this->s.at(this->realI(i));
    }

    typename SeqT::reference operator[](int i) {
        return this->s[this->realI(i)];
    }

    const typename SeqT::reference operator[](int i) const {
        return this->s[this->realI(i)];
    }
    //@}

    /*! Return the size of the underlying sequence. */
    size_t size() const {
        return this->s.size();
    }

    /*! Return `true` if sequence is empty. */
    bool empty() const {
        return this->s.empty();
    }

    /*! Return the real, bounded index from a possibly out-of-bounds one. */
    size_t realI(int i) const {
        return _realI(i, this->s.size());
    }

    /*! Return the real (wrapped) distance between two indices. See
     * _realIDist.
     */
    size_t realIDist(size_t leftI, size_t rightI) const {
        return _realIDist(leftI, rightI, this->s.size());
    }

    friend ostream &operator<<(ostream &out, const WrappedSeq &s) {
        out << "<WrappedSeq seq=" << s.s << ">";
        return out;
    }

};

/*! Split a sequence into clusters based on some distance metric.
 *
 * An item is added to a cluster if it is within `maxDist` from all other items
 * in the cluster.
 *
 * `getDist` should take two items and return their distance.
 */
template <
        class InSeqT,
        class OutSeqT,
        class DistFuncT
        >
void cluster(const InSeqT &items, const DistFuncT &getDist, float maxDist, OutSeqT &clusters) {
    clusters.clear();
    if (items.empty()) {
        return;
    }
    for (auto &item : items) {
        bool added = false;
        for (auto &cluster : clusters) {
            if (functional::all(
                    [&](typename InSeqT::reference cItem) {
                        return getDist(cItem, item) <= maxDist;
                    },
                    cluster
                   )) {
                cluster.push_back(item);
                added = true;
                break;
            }
        }
        if (not added) {
            clusters.push_back(InSeqT{item});
        }
    }
}

} // namespace seq

/*! Multiply each element in a sequence by `e`. */
template <
        class NumT,
        class ElemT,
        template <class U=ElemT, class Alloc=allocator<ElemT>> class SeqT
        >
SeqT<> &operator*=(SeqT<> &s, const NumT &e) {
    for (auto &elem : s) {
        elem *= e;
    }
    return s;
}

/*! Return a new sequence with each element multiplied by `e`. */
template <
        class NumT,
        class ElemT,
        template <class U=ElemT, class Alloc=allocator<ElemT>> class SeqT
        >
SeqT<> operator*(const SeqT<> &s, NumT e) {
    SeqT<> res(s);
    return (res *= e);
}

/*! Return a new sequence with each element multiplied by `e`. */
template <
        class NumT,
        class ElemT,
        template <class U=ElemT, class Alloc=allocator<ElemT>> class SeqT
        >
SeqT<> operator*(NumT e, const SeqT<> &s) {
    SeqT<> res(s);
    return (res *= e);
}

/*! Divide each element in a sequence by `e`. */
template <
        class NumT,
        class ElemT,
        template <class U=ElemT, class Alloc=allocator<ElemT>> class SeqT
        >
SeqT<> &operator/=(SeqT<> &s, const NumT &e) {
    for (auto &elem : s) {
        elem /= e;
    }
    return s;
}

/*! Return a new sequence with each element divided by `e`. */
template <
        class NumT,
        class ElemT,
        template <class U=ElemT, class Alloc=allocator<ElemT>> class SeqT
        >
SeqT<> operator/(const SeqT<> &s, NumT e) {
    SeqT<> res(s);
    return (res /= e);
}

/*! Return a new sequence with each element divided by `e`. */
template <
        class NumT,
        class ElemT,
        template <class U=ElemT, class Alloc=allocator<ElemT>> class SeqT
        >
SeqT<> operator/(NumT e, const SeqT<ElemT> &s) {
    SeqT<> res(s);
    for (auto &n: res) {
        n = e / n;
    }
    return res;
}

/*! Print each element of a container, surrounded by `[]` and separated by
 * commas.
 */
template <
        class T,
        template <class U=T, class Alloc=allocator<T>> class SeqT
        >
ostream &operator<<(ostream &out, const SeqT<> &seq) {
    if (seq.empty()) {
        out << "[]";
        return out;
    }

    out << "[";
    auto it = seq.begin();
    for (; it + 1 != seq.end(); it++) {
        out << *it << ", ";
    }
    out << *it << "]";

    return out;
}

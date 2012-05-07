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

/*! Multiply each element in a sequence by `e`. */
template <class SeqT, typename OperandT>
SeqT &operator*=(SeqT &s, const OperandT &e) {
    for (auto &elem : s) {
        elem *= e;
    }
    return s;
}

/*! Divide each element in a sequence by `e`. */
template <class SeqT, typename OperandT>
SeqT &operator/=(SeqT &s, const OperandT &e) {
    for (auto &elem : s) {
        elem /= e;
    }
    return s;
}

/*! Generic sequence operations and structures.
 *
 * NB: "sequence" here means "container" in the rest of C++. I just chose
 * "sequence" because "seq" in variable names looks better than "ctr".
 */
namespace seq {
    /*! Get the wrapped index to an array of size `sz`. Throw `length_error`
     * if `sz` == 0.
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

    /*! Get the wrapped distance between left and right indices in an array of
     * size `sz`. For example, `_realIDist(3, 0, 4)` == 1.
     */
    inline size_t _realIDist(size_t leftI, size_t rightI, size_t sz) {
        if (rightI >= leftI) {
            return rightI - leftI;
        } else {
            return rightI - leftI + sz;
        }
    }

    /*! Provides a reference and an index of an element in a sequence. */
    template <typename SeqT>
    struct ElemView {
        typename SeqT::reference e;
        long int i;

        /*! `itr` is the iterator to the desired element, `begin` is the
         * starting iterator of the sequence.
         */
        ElemView(typename SeqT::iterator itr, typename SeqT::iterator begin)
        : e(*itr), i(itr - begin) {
        }
    };

    /*! Sequence wrapper that supports wrap-around indexing - all
     * accessors will accept negative and out-of-bound indices and wrap around.
     */
    template <class SeqT>
    struct WrappedSeq {
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

        size_t size() const {
            return this->s.size();
        }

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
    };

    template <class IterT>
    using Slice = pair<IterT, IterT>;

    /*! Get slice of sequence as a pair of iterators. Throw `range_error` if
     * `i1` > `i2`.
     */
    template <class SeqT>
    Slice<typename SeqT::iterator> sl(SeqT &in, int i1, int i2) {
        size_t realI1 = _realI(i1, in.size());
        size_t realI2 = _realI(i2, in.size());
        if (i1 > i2) {
            throw range_error("i1 must be <= i2");
        }
        return Slice<typename SeqT::iterator>(
                in.begin() + (typename SeqT::difference_type)realI1,
                in.begin() + (typename SeqT::difference_type)realI2
                );
    }

    /*! Functions commonly used in functional programming.
     *
     * These all should be fairly self-explanatory.
     */
    namespace functional {
        template <class InSeqT, class OutSeqT, typename _FuncT>
        OutSeqT &map(const _FuncT &func, const InSeqT &in, OutSeqT &out) {
            out.resize(in.size());
            transform(in.begin(), in.end(), out.begin(), func);
            return out;
        }

        template <class InSeqT, class OutSeqT=InSeqT, typename _FuncT>
        OutSeqT map(const _FuncT &func, const InSeqT &in) {
            OutSeqT out;
            return map(func, in, out);
        }

        template <class InSeqT, class OutSeqT, typename _FuncT>
        OutSeqT &filter(const _FuncT &test, const InSeqT &in, OutSeqT &out) {
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

        template <class InSeqT, class OutSeqT=InSeqT, typename _FuncT>
        OutSeqT filter(const _FuncT &test, const InSeqT &in) {
            OutSeqT out;
            return filter(test, in, out);
        }

        template<class SeqT, typename _FuncT>
        bool any(const _FuncT &test, const SeqT &in) {
            for (auto elem : in) {
                if (test(elem)) {
                    return true;
                }
            }
            return false;
        }

        template<class IterT, typename _FuncT>
        bool any(const _FuncT &test, const Slice<IterT> &its) {
            for (auto it = its.first; it != its.second; it++) {
                if (test(*it)) {
                    return true;
                }
            }
            return false;
        }

        template<class SeqT, typename _FuncT>
        bool all(const _FuncT &test, const SeqT &in) {
            for (auto elem : in) {
                if (not test(elem)) {
                    return false;
                }
            }
            return true;
        }

        template<class IterT, typename _FuncT>
        bool all(const _FuncT &test, const Slice<IterT> &its) {
            for (auto it = its.first; it != its.second; it++) {
                if (not test(*it)) {
                    return false;
                }
            }
            return true;
        }
    }

    /*! Mathematical operations on sequences, and sequence structures. */
    namespace math {
        /*! Lazy iterator used for XRange(), usually not necessary to use
         * directly.
         *
         * `this->step == 0` means that it's the end of a range.
         */
        template <typename NumT>
        class XRangeIterator {
            public:
            typedef NumT value_type;
            typedef NumT &reference_type;
            typedef NumT *pointer;
            typedef forward_iterator_tag iterator_category;
            typedef ptrdiff_t difference_type;

            NumT val;
            NumT step;

            XRangeIterator() {
            };

            XRangeIterator(NumT start, NumT step=0) {
                val = start;
                this->step = step;
            }

            template <typename OtherNumT>
            XRangeIterator(const XRangeIterator<OtherNumT> &other) {
                val = other.val;
                step = other.step;
            }

            const value_type& operator*() const {
                return val;
            }

            void operator++() {
                val += step;
            }

            template <typename OtherNumT>
            bool operator==(const XRangeIterator<OtherNumT>& it) {
                bool res = it.step == 0
                    ? (step > 0 ? val >= it.val : val <= it.val) // other one is end
                    : (it.step > 0 ? it.val >= val : it.val <= val); // this one is end
                return res;
            }

            template <typename OtherNumT>
            bool operator!=(const XRangeIterator<OtherNumT>& it) {
                bool res = not (*this == it);
                return res;
            }

            difference_type operator-(const XRangeIterator& it) {
                return (val - it.val) / it.step;
            }
        };

        /*! Represent a lazily-evaluated range of numbers.
         *
         * Analogous to Python's "xrange" objects, returned by the function
         * `xrange`. Typically only exists to provide iterators via `begin()`
         * and `end()`.
         */
        template <typename NumT=int>
        struct XRange {
            typedef XRangeIterator<NumT> iterator;
            typedef NumT value_type;

            iterator startIt;
            iterator endIt;

            XRange() {
            };

            XRange(NumT low, NumT high, NumT step=1) {
                startIt = iterator(low, step);
                endIt = iterator(high);
            }

            template <typename OtherNumT>
            XRange(const XRange<OtherNumT> &other) {
                startIt = other.begin();
                endIt = other.end();
            }

            const iterator &begin() const {
                return startIt;
            }

            const iterator &end() const {
                return endIt;
            }
        };

        /*! Return `XRange` sequence, similar to Python's "xrange".
         *
         * Suggested usage:
         *
         *      for (auto i : xrange(10)) {
         *          printf("%d\n", i);
         *      }
         *
         * Unlike the Python version, this can also accept floats/doubles, and
         * can include the endpoint of the range by specifying `inclusive` =
         * `true`.
         */
        template <typename NumT>
        XRange<NumT> xrange(NumT low, NumT high, NumT step=1, bool inclusive=false) {
            NumT highOff = 0;
            if (inclusive) {
                highOff = (NumT)(abs(step) * 0.5);
                if (highOff == 0) {
                    highOff = 1;
                }
                highOff *= NumT(kmath::sgn(step));
            }

            return XRange<NumT>(low, high + highOff, step);
        }

        template <typename NumT>
        XRange<NumT> xrange(NumT high, bool inclusive=false) {
            return xrange<NumT>(0, high, 1, inclusive);
        }

        /*! Construct a sequence of numbers from low to high with optional step.
         */
        template <typename NumT, class SeqT>
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

        /*! Construct a sequence of numbers from 0 to high with step=1.
         */
        template <typename NumT, class SeqT>
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

#ifdef _SEQ_UTILS_TEST

#include <vector>

    /*! To test: call `test()`. */
    void test() {
        vector<float> flv(10);

        for (float i = 0; i < 10; i++) {
            flv[i] = i;
        }

        io::print(flv);

        printf("mapped with (x -> x + 1)\n");

        flv = functional::map([](float i) { return i + 1; }, flv);

        io::print(flv);

        printf("all > 5?\n");

        printf("%d\n", functional::all([](float i) { return i > 5; }, flv));

        printf("all in last half > 5?\n");

        printf("%d\n", functional::all([](float i) { return i > 5; }, sl(flv, flv.size() / 2, flv.size())));

        printf("any > 5?\n");

        printf("%d\n", functional::any([](float i) { return i > 5; }, flv));

        printf("passed!\n");
    }
#endif
}

template <class SeqT>
ostream &operator<<(ostream &out, const seq::ElemView<SeqT> &e) {
    out << "<ElemView elem=" << e.e << " i=" << e.i << ">";
    return out;
}

template <class SeqT>
ostream &operator<<(ostream &out, const seq::WrappedSeq<SeqT> &s) {
    out << "<WrappedSeq seq=" << s.s << ">";
    return out;
}

/*! Print each element of a container, surrounded by `[]` and separated by
 * commas.
 */
template <
        typename ElemType,
        template <typename U, class Alloc=allocator<U>> class SeqType
        >
ostream &operator<<(ostream &out, const SeqType<ElemType> &seq) {
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

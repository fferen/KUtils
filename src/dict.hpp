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

#include <iostream>
#include <unordered_map>
#include <utility>

using namespace std;

/*! Utilities for working with `Dict`s (`unordered_map`s). */
namespace dict {
    /*! The name `unordered_map` is too long. */
    template <typename KeyType, typename ValType>
    using Dict = unordered_map<KeyType, ValType>;

    template <typename KeyType, typename ValType>
    void _makeDictIP(Dict<KeyType, ValType> &d, KeyType key, ValType val) {
        d.insert(make_pair(key, val));
    }

    template <typename KeyType, typename ValType, typename... Args>
    void _makeDictIP(Dict<KeyType, ValType> &d, KeyType key, ValType val, Args... args) {
        d.insert(make_pair(key, val));
        _makeDictIP(d, args...);
    }

    /*! Construct a `Dict` (`unordered_map`) with any number of key/value pairs. */
    template <typename KeyType, typename ValType, typename... Args>
    Dict<KeyType, ValType> makeDict(KeyType key, ValType val, Args... args) {
        Dict<KeyType, ValType> d;
        _makeDictIP(d, key, val, args...);
        return d;
    }

    /*! Construct an empty `Dict` (`unordered_map`). Must specify `KeyType` and
     * `ValType` as template parameters.
     */
    template <typename KeyType, typename ValType>
    Dict<KeyType, ValType> makeDict() {
        Dict<KeyType, ValType> d;
        return d;
    }

    /*! Given a pair of iterators, return a mapping from element to count. */
    template <typename IterT>
    Dict<typename IterT::value_type, unsigned> countElems(
            const IterT &start,
            const IterT &end
            ) {
        Dict<typename IterT::value_type, unsigned> out;

        for (auto it = start; it < end; it++) {
            if (out.count(*it)) {
                out.at(*it)++;
            } else {
                out.insert(pair<typename IterT::value_type, unsigned>(*it, 1));
            }
        }

        return out;
    }
}

/*! Print each element of a `Dict`. */
template <typename KeyT, typename ValT>
ostream &operator<<(ostream &out, const dict::Dict<KeyT, ValT> &d) {
    if (d.empty()) {
        out << "{}";
        return out;
    }

    out << "{";
    auto it = d.begin();
    while (1) {
        auto newIt = it;
        newIt++;
        if (newIt == d.end()) {
            break;
        }
        out << *it << ", ";

        it = newIt;
    }
    out << *it << "}";

    return out;
}


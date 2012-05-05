#pragma once

#include <unordered_map>
#include <utility>

using namespace std;

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

template <typename KeyType, typename ValType, typename... Args>
Dict<KeyType, ValType> makeDict(KeyType key, ValType val, Args... args) {
    Dict<KeyType, ValType> d;
    _makeDictIP(d, key, val, args...);
    return d;
}

/*! Construct a `Dict` (`unordered_map`). For an empty `Dict`, must specify
 * `KeyType` and `ValType`.
 */
template <typename KeyType, typename ValType>
Dict<KeyType, ValType> makeDict() {
    Dict<KeyType, ValType> d;
    return d;
}

/*! Print each element of a `Dict`. */
template <typename KeyT, typename ValT>
ostream &operator<<(ostream &out, const Dict<KeyT, ValT> &seq) {
    if (seq.empty()) {
        out << "{}";
        return out;
    }

    out << "{";
    auto it = seq.begin();
    while (1) {
        auto newIt = it;
        newIt++;
        if (newIt == seq.end()) {
            break;
        }
        out << *it << ", ";

        it = newIt;
    }
    out << *it << "}";

    return out;
}

/*! \brief Utilities for working with `Dict`s (`unordered_map`s). */
namespace dict {
    /*! Given a pair of iterators, store a mapping from element to count in
     * `out`, and return `out`.
     */
    template <typename IterT, typename ElemT>
    Dict<ElemT, unsigned> &countElems(const IterT &start, const IterT &end, Dict<ElemT, unsigned> &out) {
        out.clear();

        for (auto it = start; it < end; it++) {
            if (out.count(*it)) {
                out.at(*it)++;
            } else {
                out.insert(pair<ElemT, unsigned>(*it, 1));
            }
        }

        return out;
    }
}

#pragma once

#include <cstdio>
#include <string>

/*! Functional version of `snprintf`.
 *
 * Yes, macros are evil, but this is so convenient.
 */
#define strFmt(fmt, ...) (snprintf(str::_buf, 100, fmt, __VA_ARGS__), str::_buf)

using namespace std;

/*! \brief String utilities. */
namespace str {
    /*! Buffer for the `strFmt` macro. */
    extern char _buf[100];

    /*! Return a new string word-wrapped with `cols` columns. If length of word
     * exceeds cols, throw `length_error`.
     */
    string wordWrap(string in, unsigned cols=80) throw (length_error);
}

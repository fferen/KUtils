#pragma once

#include <cstdio>
#include <string>

/*! Functional version of `snprintf`.
 *
 * Yes, macros are evil, but this is so convenient.
 */
#define strFmt(fmt, ...) (snprintf(str::_buf, 100, fmt, __VA_ARGS__), str::_buf)

using namespace std;

/*! String utilities. */
namespace str {
    /*! Buffer for the `strFmt` macro. */
    extern char _buf[100];

    /*! The letters `"abcdefghijklmnopqrstuvwxyz"`. */
    const string ASCII_LOWERCASE("abcdefghijklmnopqrstuvwxyz");

    /*! The letters `"ABCDEFGHIJKLMNOPQRSTUVWXYZ"`. */
    const string ASCII_UPPERCASE("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    /*! Concatenation of `ASCII_LOWERCASE` and `ASCII_UPPERCASE`. */
    const string ASCII_LETTERS(ASCII_LOWERCASE + ASCII_UPPERCASE);

    /*! Return a new string word-wrapped with `cols` columns.
     *
     * @throws length_error
     * Thrown if length of word exceeds `cols`.
     */
    string wordWrap(string in, unsigned cols=80);
}

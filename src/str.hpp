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

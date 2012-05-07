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

/*! Define `operator<<` overloads for common types that aren't covered in other
 * modules, as well as convenience functions for printing.
 */
#pragma once

#include <iostream>

using namespace std;

/*! Print a `pair`. */
template <typename T1, typename T2>
ostream &operator<<(ostream &out, const pair<T1, T2> &p) {
    out << "<pair first=" << p.first << " second=" << p.second << ">";
    return out;
}

/*! Convenience functions for printing values. */
namespace io {
    /*! Simply print a newline.
     *
     * Used as a recursive base-case for print(...) below.
     */
    inline void print() {
        cout << endl;
    }
     
    /*! Print arguments separated by spaces, then a newline. */
    template<typename T, typename... Args>
    void print(T value, Args... args) {
        cout << value << " ";
        print(args...);
    }

    /*! Call `fflush(stdout);`.
     *
     * Used as a recursive base-case for printImm(...) below. */
    inline void printImm() {
        fflush(stdout);
    }

    /*! Print arguments separated by spaces with no newline (by default values only
     * show up on the screen when a newline is printed). There will be an extra
     * space after the last argument.
     */
    template<typename T, typename... Args>
    void printImm(T value, Args... args) {
        cout << value << " ";
        printImm(args...);
    }
}

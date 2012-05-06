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

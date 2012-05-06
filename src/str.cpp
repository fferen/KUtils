#include <string>
#include <stdexcept>

#include "str.hpp"
#include "io.hpp"

char str::_buf[100];

string str::wordWrap(string in, unsigned cols) {
    unsigned i = cols;
    while (i < in.size()) {
        for (; i > 0; i--) {
            if (isspace(int((in.at(i))))) {
                in.replace(i, 1, "\n");
                break;
            }
        }
        if (i == 0) {
            throw length_error("length of word exceeds cols");
        }
        i += cols;
    }
    return in;
}

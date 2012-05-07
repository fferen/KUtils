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

#include <cctype>

#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "argparse.hpp"
#include "dict.hpp"
#include "io.hpp"
#include "str.hpp"

using namespace std;
using namespace argparse;

string argparse::makeUsageString(
        const string &progName,
        const string &desc,
        const string &posArgs,
        const dict::Dict<string, string> &optToHelp,
        unsigned cols,
        unsigned helpCols
        ) {
    const string helpSpaces(cols - helpCols, ' ');

    stringstream usage("Usage: ");
    usage << progName;
    if (not optToHelp.empty()) {
        usage << " [options]";
    }
    usage << " " << posArgs << endl << endl;
    usage << str::wordWrap(desc, cols);

    if (optToHelp.empty()) {
        return usage.str();
    }

    usage << endl << endl << "Options:" << endl;
    for (auto elem : optToHelp) {
        string wrappedHelp = str::wordWrap(elem.second, helpCols);
        usage << elem.first << string(cols - helpCols - elem.first.size(), ' ');

        size_t pos = 0, newPos;
        while (1) {
            newPos = wrappedHelp.find('\n', pos);

            usage << elem.second.substr(pos, newPos - pos) << endl;
            if (newPos == string::npos) {
                break;
            }
            usage << helpSpaces;
            pos = newPos + 1;
        }
    }
    return usage.str();
}

ArgHolder argparse::parse(
        int argc,
        const char *argv[],
        char optChar,
        bool genHelpChker
        ) {
    ArgHolder holder{dict::Dict<string, vector<string>>(), vector<string>()};

    int i = 1;

    while (i < argc) {
        if (argv[i][0] != optChar) {
            // positional argument
            holder.posArgs.push_back(argv[i]);
            i++;
            continue;
        }

        if (genHelpChker and (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0)) {
            holder.optToArgs.insert(pair<string, vector<string>>("-h", vector<string>()));
            i++;
            continue;
        }

        // option (begins with `optChar`)
        throw invalid_argument(strFmt("unknown option %s", argv[i]));
    }

    return holder;
}

#include <cctype>

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
        const Dict<string, string> &optToHelp,
        int cols,
        int helpCols
        ) {
    const string helpSpaces(cols - helpCols, ' ');

    stringstream usage("Usage: ");
    usage << progName;
    if (not optToHelp.empty()) {
        usage << " [options]";
    }
    usage << " " << posArgs << endl << endl;
    usage << str::wordWrap(desc, 80);

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
    ArgHolder holder{Dict<string, vector<string>>(), vector<string>(), string()};

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
        stringstream errStrm;
        errStrm << "unknown option " << argv[i];
        holder.err = errStrm.str();
        return holder;
    }

    return holder;
}

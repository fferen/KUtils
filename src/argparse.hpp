#pragma once

#include <cstring>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "dict.hpp"
#include "str.hpp"

using namespace std;

/*! Command-line argument parser with a focus on brevity. */
namespace argparse {
    /*! Holds the result of parsing arguments. */
    struct ArgHolder {
        /*! Mapping from option string (eg. "-h") to its arguments. */
        Dict<string, vector<string>> optToArgs;
        /*! Any positional arguments left over. */
        vector<string> posArgs;
    };

    /*! Create a usage string.
     *
     * @param progName
     *      Name of the program as run on the command line. Typically `argv[0]`
     *      is passed.
     * @param desc
     *      Description of the program.
     * @param posArgs
     *      String representing the positional arguments.
     * @param optToHelp
     *      Mapping from option to help dialog.
     * @param cols
     *      Maximum number of columns - strings longer than this will be wrapped.
     * @param helpCols
     *      Number of columns that the option help should take up.
     *
     * Example:
     *
     *      makeUsageString(
     *              "testprog",
     *              "This is a description of the program.",
     *              "arg1 arg2 arg3",
     *              makeDict(
     *                  string("-h"), string("print a help message"),
     *                  string("-a"), string("add some numbers")
     *                  )
     *              )
     *
     * generates this help string:
     *
     *      Usage: testprog [options] arg1 arg2 arg3
     *
     *      This is a description of the program.
     *
     *      Options:
     *      -a                  add some numbers
     *      -h                  print a help message
     *
     */
    string makeUsageString(
            const string &progName,
            const string &desc,
            const string &posArgs=string(),
            const Dict<string, string> &optToHelp=Dict<string, string>(),
            unsigned cols=80,
            unsigned helpCols=60
            );

    /*! Return an ArgHolder object holding the parsed arguments.
     *
     * @param argc,argv
     *      The arguments to main().
     * @param optToChker
     *      Should map an option string (anything beginning with `optChar`) to a
     *      checker function that takes a vector of argument strings and returns
     *      the number of arguments read. The function should also throw an
     *      `invalid_argument` exception if any arguments are invalid. For
     *      example, a simple help option checker:
     *
     *          makeDict(string("-h"), [](vector<string> args) { return 0; })
     * @param optChar
     *      Signifies the start of an option.
     * @param genHelpChker
     *      If `true`, checks "-h" and "--help" options automatically.
     *
     * @throws invalid_argument
     *      Thrown if unknown option is encountered, as well as if a checker throws
     *      an `invalid_argument` exception.
     */
    template <class FuncType> 
    ArgHolder parse(
            int argc,
            const char *argv[],
            const Dict<string, FuncType> &optToChker,
            char optChar='-',
            bool genHelpChker=true
            ) {
        ArgHolder holder{Dict<string, vector<string>>(), vector<string>(), string()};

        vector<string> curArgs;
        string curOpt;
        string error;
        unsigned nargs;
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
            curArgs.clear();
            curOpt = argv[i];

            i++;
            while (i < argc and argv[i][0] != optChar) {
                curArgs.push_back(argv[i]);
                i++;
            }

            try {
                nargs = optToChker.at(curOpt)(curArgs);
            } catch (out_of_range &err) {
                throw invalid_argument(strFmt("unknown option %s", curOpt));
            }
            
            holder.posArgs.insert(holder.posArgs.end(), curArgs.begin() + nargs, curArgs.end());
            curArgs.resize(nargs);

            holder.optToArgs.insert(pair<string, vector<string>>(curOpt, curArgs));
        }

        return holder;
    }

    /*! For programs that take no options.
     *
     * See parse() above.
     */
    ArgHolder parse(
            int argc,
            const char *argv[],
            const char optChar='-',
            bool genHelpChker=true
            );
}

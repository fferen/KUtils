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

/*! \brief Command-line argument parser with a focus on brevity. */
namespace argparse {
    /*! \brief Holds the result of parsing arguments. */
    struct ArgHolder {
        /*! Mapping from option string (eg. "-h") to its arguments. */
        Dict<string, vector<string>> optToArgs;
        /*! Any positional arguments left over. */
        vector<string> posArgs;
    };

    /*! Create a usage string.
     *
     * @param cols
     * Maximum number of columns - strings longer than this will be wrapped.
     *
     * @param helpCols
     * Number of columns that the option help should take up.
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
     *      testprog [options] arg1 arg2 arg3
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
            int cols=80,
            int helpCols=60
            );

    /*! Return an ArgHolder object holding the parsed arguments.
     *
     * @param argc,argv
     * The arguments to main().
     *
     * @param optToChker
     * Should map an option string (anything beginning with `optChar`) to a
     * checker function that takes a vector of argument strings and returns the
     * number of arguments read. The function should also throw an
     * `invalid_argument` exception if any arguments are invalid. For example, a
     * simple help option checker:
     *
     *      makeDict(string("-h"), [](vector<string> args) { return 0; })
     *
     * @param optChar
     * Signifies the start of an option.
     *
     * @param genHelpChker
     * If `true`, checks "-h" and "--help" options automatically.
     *
     *
     * If a checker throws an `invalid_argument` exception, it will be thrown
     * from this function as well. An `invalid_argument` exception will also be
     * thrown if an unknown option is encountered, .
     */
    template <class FuncType> 
    ArgHolder parse(
            int argc,
            const char *argv[],
            const Dict<string, FuncType> &optToChker,
            char optChar='-',
            bool genHelpChker=true
            ) throw (invalid_argument) {
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
                stringstream errStrm;
                errStrm << "unknown option " << curOpt;
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

#ifdef _ARGPARSE_TEST

#include <cstdlib>
#include <cstdio>

#include "io.hpp"

    bool isNum(string s) {
        for (auto c : s) {
            if (c < 48 or c > 57) {
                return false;
            }
        }
        return true;
    }

    unsigned aChk(vector<string> args) {
        if (args.empty()) {
            throw "-a must have >= 1 arguments";
        }
        for (auto s : args) {
            if (not isNum(s)) {
                throw "arguments to -a must be numbers";
            }
        }
        return args.size();
    }

    unsigned hChk(vector<string> args) {
        return 0;
    }

    /*! To test: in your `main()` function, call `test()` with argc and argv and
     * return its return value.
     *
     * It simulates a command that has a help dialog "-h" and sums numbers
     * passed to it with the "-a" option. It also prints additional positional
     * arguments.
     */

    int test(int argc, const char *argv[]) {
        auto holder = parse(
                argc,
                argv,
                makeDict(
                    string("-h"), hChk,
                    string("-a"), aChk
                    )
                 );

        if (not holder.err.empty()) {
            printf("err: %s\n", holder.err.c_str());
            return 1;
        }

        if (holder.optToArgs.count("-h")) {
            print(makeUsageString(
                    argv[0],
                    "Test program for argparse. Lorem ipsum dolor sit amet, "
                    "consectetur adipisicing elit, sed do eiusmod tempor incididunt ut "
                    "labore et dolore magna aliqua.",
                    "arg1 arg2 arg3",
                    makeDict(
                        string("-h"), string("print a wonderfully detailed and amazingly stupendous help message"),
                        string("-a"), string("add some numbers")
                        )
                    ));
            return 1;
        }

        int sum = 0;
        for (auto arg : holder.optToArgs["-a"]) {
            sum += atoi(arg.c_str());
        }

        if (sum) {
            printf("summed to %d\n", sum);
        }

        for (size_t i = 0; i < holder.posArgs.size(); i++) {
            printf("pos arg %zu: %s\n", i, holder.posArgs[i].c_str());
        }

        return 0;
    }
#endif
}

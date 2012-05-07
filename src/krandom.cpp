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

#include <random>

#include "krandom.hpp"

using namespace std;

random_device krandom::engine {};

int krandom::randint(int low, int high) {
    static uniform_int_distribution<int> dist{};
    return dist(engine, uniform_int_distribution<int>::param_type{low, high});
}

long krandom::randint(long low, long high) {
    static uniform_int_distribution<long> dist{};
    return dist(engine, uniform_int_distribution<long>::param_type{low, high});
}

long long krandom::randint(long long low, long long high) {
    static uniform_int_distribution<long long> dist{};
    return dist(engine, uniform_int_distribution<long long>::param_type{low, high});
}

double krandom::random() {
    static uniform_real_distribution<double> dist(0, 1);
    return dist(engine);
}

float krandom::uniform(float low, float high) {
    static uniform_real_distribution<float> dist{};
    return dist(engine, uniform_real_distribution<float>::param_type{low, high});
}

double krandom::uniform(double low, double high) {
    static uniform_real_distribution<double> dist{};
    return dist(engine, uniform_real_distribution<double>::param_type{low, high});
}

vector<unsigned> &krandom::getSampleInds(unsigned popSize, unsigned sampSize, vector<unsigned> &inds, bool withReplacement) {
    inds.resize(sampSize);
    if (withReplacement) {
        for (auto &ind : inds) {
            ind = unsigned(krandom::randint(0l, long(popSize) - 1));
        }
    } else {
        // code from here:
        // http://stackoverflow.com/questions/311703/algorithm-for-sampling-without-replacement
        unsigned n = sampSize;
        unsigned N = popSize;

        unsigned t = 0; // total input records dealt with
        unsigned m = 0; // number of items selected so far
        double u;

        while (m < n) {
            u = krandom::random();

            if ((N - t)*u >= n - m) {
                t++;
            } else {
                inds[m] = t;
                t++; m++;
            }
        }
    }

    return inds;
}

vector<unsigned> getSampleInds(unsigned popSize, unsigned sampSize, bool withReplacement) {
    vector<unsigned> inds;
    krandom::getSampleInds(popSize, sampSize, inds, withReplacement);
    return inds;
}

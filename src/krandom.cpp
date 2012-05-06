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

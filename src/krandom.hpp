#pragma once

#include <random>
#include <algorithm>

using namespace std;

/*! \brief Simple wrapper around `<random>` with a Python interface. */
namespace krandom {
    extern random_device engine;

    int randint(int low, int high);
    long randint(long low, long high);
    long long randint(long long low, long long high);

    double random();

    float uniform(float low, float high);
    double uniform(double low, double high);

    template <class IterType>
    void shuffle(IterType start, IterType end) {
        random_shuffle(
                start,
                end,
                [](long i) { return krandom::randint(0l, i - 1); }
                );
    }

    template <class SeqType>
    void shuffle(SeqType &seq) {
        shuffle(seq.begin(), seq.end());
    }

    template <
        template <typename U, typename... Args> class SeqType,
        typename ItemType
        >
    ItemType choice(const SeqType<ItemType> &seq) {
        return seq[randint(0, seq.size() - 1)];
    }

    /*! Store `sampSize` random numbers in range [0, `popSize`) in `inds`, with
     * or without replacement.
     */
    vector<unsigned> &getSampleInds(unsigned popSize, unsigned sampSize, vector<unsigned> &inds, bool withReplacement=true);
    vector<unsigned> getSampleInds(unsigned popSize, unsigned sampSize, bool withReplacement=true);

    template <typename SeqType>
    void sample(const SeqType &seq, unsigned sampSize, SeqType &out, bool withReplacement=true) {
        vector<unsigned> inds;
        out.resize(sampSize);
        getSampleInds(seq.size(), sampSize, inds, withReplacement);
        for (auto i = 0u; i < sampSize; i++) {
            out[i] = seq[inds[i]];
        }
    }

    template <typename SeqType>
    SeqType sample(const SeqType &seq, unsigned sampSize, bool withReplacement=true) {
        SeqType out;
        sample(seq, sampSize, out, withReplacement);
        return out;
    }
}

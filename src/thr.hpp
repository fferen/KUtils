#pragma once

#include <deque>
#include <mutex>

using namespace std;

/*! \brief Common multi-threading patterns. */
namespace thr {
    /*! \brief Producer-consumer queue. */
    template <typename T>
    class Queue {
    public:
        static mutex lock;

        deque<T> q;

        Queue() {};

        /* Return `true` if next item in queue was stored in `out`. */
        bool poll(T &out) {
            lock_guard<mutex> lk(lock);
            bool found = false;
            if (not q.empty()) {
                out = q.back();
                q.pop_back();
                found = true;
            }
            return found;
        }

        /* Block until next item is stored in `out`. */
        void wait(T &out) {
            while (not poll(out));
        }

        /* Push a new item into the queue. */
        void push(const T &in) {
            lock_guard<mutex> lk(lock);
            q.push_front(in);
        }
    };

    template<typename T>
    mutex Queue<T>::lock;
}

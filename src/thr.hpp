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

#pragma once

#include <deque>
#include <mutex>

using namespace std;

/*! Multi-threading utilities. */
namespace thr {
    /*! Producer-consumer queue. */
    template <typename T>
    struct Queue {
        static mutex lock;

        deque<T> q;

        Queue() {};

        /*! Return `true` if next item in queue was stored in `out` or `false`
         * if there was no item.
         */
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

        /*! Block until next item is stored in `out`. */
        void wait(T &out) {
            while (not poll(out));
        }

        /*! Push a new item into the queue. */
        void push(const T &in) {
            lock_guard<mutex> lk(lock);
            q.push_front(in);
        }
    };

    template<typename T>
    mutex Queue<T>::lock;
}

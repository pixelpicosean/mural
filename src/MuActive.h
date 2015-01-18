/*
Copyright (c) 2015 Sean Bohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include "MuGlobals.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <thread>

using namespace std;

namespace mural {

    template <class T>
    class message_queue : public MuObject {
        public:
            message_queue(void):
                q(), m(), c()
            {}
            ~message_queue(void) {}

            void send(T t) {
                std::lock_guard<std::mutex> lock(m);
                q.push(t);
                c.notify_one();
            }

            T receive(void) {
                std::unique_lock<std::mutex> lock(m);
                while (q.empty()) {
                    c.wait(lock);
                }
                T val = q.front();
                q.pop();
                return val;
            }

        private:
            std::queue<T> q;
            mutable std::mutex m;
            std::condition_variable c;
    };

    class Active : public MuObject {
        public:
            typedef function<void()> Message;

            Active(): done(false) {
                thd = unique_ptr<thread>(
                    new thread([=] {
                        this->run();
                    })
                );
            }

            ~Active() {
                send([&] {
                    done = true;
                });
                thd->join();
            }

            void send(Message m) {
                mq.send(m);
            }

        private:
            Active(const Active&);          // no copying
            void operator=(const Active&);  // no copying

            bool done;                      // le flag
            message_queue<Message> mq;      // le queue
            unique_ptr<thread> thd;         // le thread

            void run() {
                while (!done) {
                    Message msg = mq.receive();
                    msg(); // execute message
                } // note: last message sets done to true
            }
    };

}

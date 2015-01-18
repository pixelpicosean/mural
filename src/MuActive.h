#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <thread>

using namespace std;

namespace mural {

    template <class T>
    class message_queue {
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

    class Active {
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

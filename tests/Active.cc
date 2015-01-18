#include "../src/MuGlobals.h"
#include "../src/MuActive.h"

#include <functional>
#include <chrono>
#include <thread>

class Backgrounder {
    public:
        void doSomethingLater(int ms, int idx, std::function<void(const char *)> returnCallback) {
            a.send([=] {
                // Wait for some time
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));

                // Run the callback
                String res("Defered task: ");
                res += std::to_string(idx);
                returnCallback(res.c_str());
            });
        }
    private:
        mural::Active a;
};

void task(const char *res) {
    printf("%s\n", res);
}

int main(int argc, char const *argv[]) {
    Backgrounder b;

    b.doSomethingLater(200, 1, task);
    task("Main thread task: 1");

    b.doSomethingLater(400, 2, task);
    task("Main thread task: 2");

    b.doSomethingLater(600, 3, task);
    task("Main thread task: 3");

    b.doSomethingLater(800, 4, task);
    task("Main thread task: 4");

    b.doSomethingLater(1000, 5, task);
    task("Main thread task: 5");


    return 0;
}

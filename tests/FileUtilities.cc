#include "../src/MuGlobals.h"
#include "../src/MuUtilities.h"

using namespace mural;

int main(int argc, char const *argv[]) {

    printf("getAppPath: %s\n", getAppPath().c_str());
    printf("getHomeDirectory: %s\n", getHomeDirectory().c_str());
    printf("getDocumentsDirectory: %s\n", getDocumentsDirectory().c_str());

    return 0;
}

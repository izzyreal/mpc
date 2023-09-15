#include "Mpc.hpp"

#ifndef EMSCRIPTEN_KEEPALIVE
    #define EMSCRIPTEN_KEEPALIVE
#endif

EMSCRIPTEN_KEEPALIVE

 mpc::Mpc* wasmMpc = nullptr;

int wrapper_function(int arg1, int arg2) {

    if (wasmMpc != nullptr)
    {
        printf("wasmMpc already initialized\n");
        return 1;
    }

    printf("Instantiating mpc::Mpc ...\n");

    wasmMpc = new mpc::Mpc();

    wasmMpc->init(0, 1);

    return 0;
}

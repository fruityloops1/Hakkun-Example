#include "hk/diag/diag.h"
#include "hk/hook/Trampoline.h"

extern "C" void nninitRegion();
extern "C" void nninitLocale();
extern "C" void __ctr_start_hook_continue();

extern "C" void nninitStartup() {
}

extern "C" void nnMain(int argc, const char** argv) {
    nninitRegion();
    nninitLocale();
    __ctr_start_hook_continue();
}

extern "C" void hkMain() {
}

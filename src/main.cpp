#include "hk/diag/diag.h"
#include "hk/hook/Trampoline.h"
#include "hooks/OsInit.h"
#include "hooks/SvcStub.h"
#include "module.h"

extern "C" void nninitStartup() {
}

extern "C" void hkMain() {
    installSvcStubHooks();
    installOsInitHooks();
}

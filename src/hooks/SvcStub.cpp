#include "hooks/SvcStub.h"
#include "hk/diag/diag.h"
#include "hk/hook/InstrUtil.h"
#include "hk/util/TemplateString.h"
#include "module.h"

template <hk::util::TemplateString SvcName>
static void abortSvcStub() {
    HK_ABORT("SvcStub: %s called", SvcName.value);
}

void installSvcStubHooks() {
#define STUB(NAME, OFFSET) \
    hk::hook::writeBranch(getRpModule(), OFFSET, abortSvcStub<#NAME>)

    STUB(ArbitrateAddress, 0x0018bc84);
    STUB(Break, 0x0018ec90);
    STUB(CloseHandle, 0x0000c56c);
    STUB(ControlMemory, 0x00293054);
    STUB(CreateMemoryBlock, 0x0010c574);
    STUB(CreateThread, 0x000038a0);
    STUB(ExitProcess, 0x00193614);
    STUB(SleepThread, 0x0018bc4c);
    STUB(WaitSynchronizationN, 0x0018eba8);

#undef STUB
}
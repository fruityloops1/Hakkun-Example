#include "hooks/OsInit.h"
#include "hk/types.h"
#include "nn/oe.h"
#include "nn/os.h"
#include <ctr/sead/heap/seadArena.h>

constexpr size cMemoryHeapSize = 64_MB;

void ctr::sead::Arena::initialize(size_t size) {
    mSize = size;
    uintptr_t block = 0;
    nn::os::AllocateMemoryBlock(&block, hk::alignUp(size, 2_MB));
    mStart = reinterpret_cast<u8*>(block);
}

extern "C" void nninitStartUp() {
    nn::oe::Initialize();

    nn::os::SetMemoryHeapSize(cMemoryHeapSize);
}

void installOsInitHooks() {
}
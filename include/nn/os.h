#pragma once

#include "nn/types.h"

namespace nn::os {

    Result SetMemoryHeapSize(size size);
    Result AllocateMemoryBlock(uintptr_t* out, size_t size);

} // namespace nn::os

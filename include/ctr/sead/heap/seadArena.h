#pragma once

#include "hk/types.h"

namespace ctr::sead {
    class Arena {
    public:
        Arena();
        ~Arena();

        void initialize(size_t size);
        void destroy();

        u8* mStart = nullptr;
        size_t mSize = 0;
    };

} // namespace ctr::sead

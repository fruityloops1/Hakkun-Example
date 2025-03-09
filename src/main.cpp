#include "hk/diag/diag.h"
#include "hk/hook/Trampoline.h"

namespace al {
    bool isPadHoldA(int port = -1);
}

namespace sead {
    template <typename T>
    struct Vector3 {
        T x, y, z;
    };

    using Vector3f = Vector3<f32>;
}

struct PlayerActor {
    u8 _0[0xB8];
    struct Player {
        struct PlayerProperty {
            sead::Vector3f mTrans;
            sead::Vector3f mFront;
            sead::Vector3f mUp;
            sead::Vector3f mVelocity;
        }* mProperty;
    }* mPlayer;
};

HkTrampoline<void, PlayerActor*> control = hk::hook::trampoline([](PlayerActor* player) -> void {
    control.orig(player);
    if (al::isPadHoldA()) {
        player->mPlayer->mProperty->mTrans.y += 30.0f;
        player->mPlayer->mProperty->mVelocity.y = 0;
    }
});

extern "C" void hkMain() {
    control.installAtSym<"_ZN11PlayerActor7controlEv">();
}

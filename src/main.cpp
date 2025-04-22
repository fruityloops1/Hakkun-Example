#include "hk/gfx/DebugRenderer.h"
#include "hk/gfx/ImGuiBackendNvn.h"
#include "hk/hook/Trampoline.h"

#include "agl/common/aglDrawContext.h"

#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/Application.h"
#include "game/System/GameSystem.h"

#include "al/Library/Controller/InputFunction.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/LiveActor/LiveActor.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Memory/HeapUtil.h"
#include "al/Library/Player/PlayerHolder.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/System/GameSystemInfo.h"

#include <sead/heap/seadExpHeap.h>
#include <utility>

#include "nn/hid.h"

#include "imgui.h"

static sead::Heap* sImGuiHeap = nullptr;

HkTrampoline<void, GameSystem*> gameSystemInit = hk::hook::trampoline([](GameSystem* gameSystem) -> void {
    sImGuiHeap = sead::ExpHeap::create(2_MB, "ImGuiHeap", al::getStationedHeap(), 8, sead::Heap::cHeapDirection_Forward, false);

    gameSystemInit.orig(gameSystem);

    auto* imgui = hk::gfx::ImGuiBackendNvn::instance();

    imgui->setAllocator(
        { [](size allocSize, size alignment) -> void* {
             return sImGuiHeap->tryAlloc(allocSize, alignment);
         },
            [](void* ptr) -> void {
                sImGuiHeap->free(ptr);
            } });
    imgui->tryInitialize();

    nn::hid::InitializeMouse();
});

HkTrampoline<void, al::LiveActor*> marioControl = hk::hook::trampoline([](al::LiveActor* player) -> void {
    if (al::isPadHoldA(-1)) {
        player->getPoseKeeper()->getVelocityPtr()->y = 0;
        al::getTransPtr(player)->y += 20;
    }

    marioControl.orig(player);
});

static void updateImGuiInput() {
    static nn::hid::MouseState state;
    static nn::hid::MouseState lastState;

    lastState = state;
    nn::hid::GetMouseState(&state);

    auto& io = ImGui::GetIO();
    io.AddMousePosEvent(state.mX / 1280.f * io.DisplaySize.x, state.mY / 720.f * io.DisplaySize.y);
    constexpr std::pair<nn::hid::MouseButton, ImGuiMouseButton> buttonMap[] = {
        { nn::hid::MouseButton::Left, ImGuiMouseButton_Left },
        { nn::hid::MouseButton::Right, ImGuiMouseButton_Right },
        { nn::hid::MouseButton::Middle, ImGuiMouseButton_Middle },
    };

    for (const auto& [hidButton, imguiButton] : buttonMap) {
        if (state.mButtons.Test(int(hidButton)) && !lastState.mButtons.Test(int(hidButton))) {
            io.AddMouseButtonEvent(imguiButton, true);
        } else if (!state.mButtons.Test(int(hidButton)) && lastState.mButtons.Test(int(hidButton))) {
            io.AddMouseButtonEvent(imguiButton, false);
        }
    }

    if (state.mWheelDeltaX != 0.0f)
        io.AddMouseWheelEvent(0.0f, state.mWheelDeltaX > 0.0f ? 1.5f : -1.5f);

    /* Keyboard missing */

    io.MouseDrawCursor = true;
}

HkTrampoline<void, GameSystem*> drawMainHook = hk::hook::trampoline([](GameSystem* gameSystem) -> void {
    drawMainHook.orig(gameSystem);

    auto* drawContext = Application::instance()->mDrawSystemInfo->drawContext;

    HakoniwaSequence* sequence = static_cast<HakoniwaSequence*>(gameSystem->mSequence);
    if (sequence == nullptr)
        return;

    al::Scene* scene = sequence->mCurrentScene;
    al::LiveActor* player = nullptr;
    if (scene && scene->mLiveActorKit && scene->mLiveActorKit->mPlayerHolder)
        player = scene->mLiveActorKit->mPlayerHolder->tryGetPlayer(0);

    /* ImGui */

    updateImGuiInput();

    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();

    hk::gfx::ImGuiBackendNvn::instance()->draw(ImGui::GetDrawData(), drawContext->getCommandBuffer()->ToData()->pNvnCommandBuffer);

    /* DebugRenderer */

    auto* renderer = hk::gfx::DebugRenderer::instance();

    renderer->clear();
    renderer->begin(drawContext->getCommandBuffer()->ToData()->pNvnCommandBuffer);

    renderer->setGlyphSize(0.45);

    renderer->drawQuad(
        { { 30, 30 }, { 0, 0 }, 0xef000000 },
        { { 300, 30 }, { 1.0, 0 }, 0xef000000 },
        { { 300, 100 }, { 1.0, 1.0 }, 0xef000000 },
        { { 30, 100 }, { 0, 1.0 }, 0xef000000 });

    renderer->setCursor({ 50, 50 });

    if (player) {
        const sead::Vector3f& trans = al::getTrans(player);

        renderer->printf("Pos: %.2f %.2f %.2f\n", trans.x, trans.y, trans.z);
    } else {
        renderer->printf("No player\n");
    }

    renderer->end();
});

extern "C" void hkMain() {
    marioControl.installAtSym<"_ZN19PlayerActorHakoniwa7controlEv">();
    drawMainHook.installAtSym<"_ZN10GameSystem8drawMainEv">();
    gameSystemInit.installAtSym<"_ZN10GameSystem4initEv">();

    hk::gfx::DebugRenderer::instance()->installHooks();
    hk::gfx::ImGuiBackendNvn::instance()->installHooks(false);
}

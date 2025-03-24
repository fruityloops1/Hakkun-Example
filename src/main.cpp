#include "hk/gfx/DebugRenderer.h"
#include "hk/gfx/ImGuiBackendNvn.h"
#include "hk/hook/Trampoline.h"

#include <utility>

#include "hk/nvn/nvn_Cpp.h"
#include "imgui.h"

#include "hk/diag/diag.h"
#include "hk/mem/BssHeap.h"

static void draw(nvn::CommandBuffer* commandBuffer) {
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
    ImGui::Render();

    hk::gfx::ImGuiBackendNvn::instance()->draw(ImGui::GetDrawData(), commandBuffer);

    /* DebugRenderer */

    auto* renderer = hk::gfx::DebugRenderer::instance();

    renderer->clear();
    renderer->begin(commandBuffer);

    renderer->setGlyphSize(0.45);

    renderer->drawQuad(
        { { 30, 30 }, { 0, 0 }, 0xef000000 },
        { { 300, 30 }, { 1.0, 0 }, 0xef000000 },
        { { 300, 100 }, { 1.0, 1.0 }, 0xef000000 },
        { { 30, 100 }, { 0, 1.0 }, 0xef000000 });

    renderer->setCursor({ 50, 50 });

    renderer->printf("meow\n");

    renderer->end();
}

extern "C" void hkMain() {
    hk::gfx::DebugRenderer::instance()->installHooks();
    hk::gfx::ImGuiBackendNvn::instance()->installHooks(true);
    // clang-format off
    hk::gfx::ImGuiBackendNvn::instance()->setAllocator({
        [](size_t size, size_t alignment) -> void* {
            void* ptr = aligned_alloc(alignment, size);
            HK_ABORT_UNLESS(ptr != nullptr, "no More Memory ): FreeSize: %zu", hk::mem::sMainHeap.getFreeSize());
            return ptr;
        },
        [](void* ptr) -> void {
            free(ptr);
        }
    });
    // clang-format on
}

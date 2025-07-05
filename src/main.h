#include <Windows.h>
#include <iostream>

#include <kthook/kthook.hpp>

#include <sampapi/CChatBubble.h>
#include <sampapi/CGame.h>
#include <sampapi/CNetGame.h>
#include <sampapi/CLabel.h>
#include <sampapi/CEntity.h>
#include <sampapi/CFonts.h>

#include <d3d9.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "utils.h"
#include "imgui_utils.h"
#include "settings.h"

namespace samp = sampapi::v037r1;

using CChatBubble__DrawProto = void(__thiscall*)(samp::CChatBubble*);
using CChatBubble__AddProto = int16_t(__thiscall*)(samp::CChatBubble*, uint16_t, const char*, uint32_t, float, int);

class Main {
private:
    bool isImguiInitialized = false;
    Settings settings;
    std::array<std::vector<samp::CChatBubble::Player>, 1004> g_Bubbles;
public:

    Main();
    ~Main();

    kthook::kthook_simple<CChatBubble__DrawProto> hookCChatBubble__Draw{ reinterpret_cast<void*>(sampapi::GetBase() + 0x63310) };
    kthook::kthook_simple<CChatBubble__AddProto> hookCChatBubble__Add{ reinterpret_cast<void*>(sampapi::GetBase() + 0x63250) };

    void CChatBubble__Draw(
        const decltype(hookCChatBubble__Draw)& hook,
        samp::CChatBubble* p_this
    );

    int16_t CChatBubble__Add(
        const decltype(hookCChatBubble__Add)& hook,
        samp::CChatBubble* p_this,
        uint16_t n_Player,
        const char* szText,
        uint32_t color,
        float fDrawDistance,
        int lifeSpan
    );

    void TryInitImgui();
} g_Main;

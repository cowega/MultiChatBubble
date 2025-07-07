#include "main.h"

Main::Main() {
    hookCChatBubble__Draw.set_cb(
        [this](const decltype(hookCChatBubble__Draw)& hook, samp::CChatBubble* p_this
        ) -> void { return this->CChatBubble__Draw(hook, p_this); });
    hookCChatBubble__Draw.install();
    
    hookCChatBubble__Add.set_cb(
        [this](const auto& hook, samp::CChatBubble* p_this,
            uint16_t n_Player, const char* szText,
            uint32_t color, float fDrawDistance, int lifeSpan
        ) -> int16_t {
            return this->CChatBubble__Add(hook, p_this, n_Player, szText, color, fDrawDistance, lifeSpan);
        });
    hookCChatBubble__Add.install();

    
    plugin::Events::d3dLostEvent +=
        []() { ImGui_ImplDX9_InvalidateDeviceObjects(); };


    int maxChatBubbles = settings.getMaxChatBubbles();
    for (auto& vec : g_Bubbles) {
        vec.resize(maxChatBubbles);
    }
}

Main::~Main() {
    hookCChatBubble__Add.remove();
    hookCChatBubble__Draw.remove();
}

int16_t Main::CChatBubble__Add(
    const decltype(hookCChatBubble__Add)& hook, samp::CChatBubble* p_this,
    uint16_t n_Player, const char* szText, uint32_t color, float fDrawDistance,
    int lifeSpan
) {
    auto& bubbles = g_Bubbles[n_Player];
    DWORD lastTick = GetTickCount();

    char copyText[256];
    strcpy(copyText, szText);
    clearInput(copyText);
    samp::CChatBubble::Player* bubble = nullptr;
    if (!strcmp(copyText, bubbles[0].m_szText)) {
        bubble = &bubbles[0];
    } else {
        bubble = &*std::min_element(bubbles.begin(), bubbles.end(), [lastTick](const auto& a, const auto& b) {
            bool a_dead = (a.m_creationTick + a.m_lifeSpan <= lastTick);
            bool b_dead = (b.m_creationTick + b.m_lifeSpan <= lastTick);

            if (a_dead && !b_dead) return true;
            if (!a_dead && b_dead) return false;
            if (a_dead && b_dead) return false;

            return a.m_creationTick < b.m_creationTick;
            });
    }

    auto orig = hook.get_trampoline()(p_this, n_Player, szText, color, fDrawDistance, lifeSpan);
    *bubble = p_this->m_player[n_Player];
    strcpy(bubble->m_szText, copyText);
    bubble->m_creationTick = lastTick;
    bubble->m_nMaxLineLength = 1;

    std::sort(bubbles.begin(), bubbles.end(), [](const auto& a, const auto& b) {
        return a.m_creationTick > b.m_creationTick;
        });

    return orig;
}

void Main::TryInitImgui() {
    bool isSAMPInitialized = *reinterpret_cast<void**>(sampapi::GetBase() + 0x21A0F8) != nullptr;
    if (isImguiInitialized || !isSAMPInitialized)
        return;

    HWND* hwnd = *reinterpret_cast<HWND**>(0xC17054);
    IDirect3DDevice9* pDevice = *reinterpret_cast<IDirect3DDevice9**>(0xC97C28);

    if (!hwnd || !pDevice)
        return;

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(*hwnd);
    ImGui_ImplDX9_Init(pDevice);

    ImGui::GetIO().Fonts->Clear();
    ImGui::GetIO().Fonts->AddFontDefault();

    isImguiInitialized = true;
}

void Main::CChatBubble__Draw(
    const decltype(hookCChatBubble__Draw)& hook,
    samp::CChatBubble* p_this
) {
    if (!isImguiInitialized) {
        this->TryInitImgui();
        return;
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX9_NewFrame();

    static int fontSize = 0;
    int currentFontSize = getFontSize();
    if (fontSize != currentFontSize) {
        changeFontSize(ImGui::GetIO(), currentFontSize, settings.getFontName());
        fontSize = currentFontSize;
    }

    ImGui::NewFrame();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    if (samp::RefGame() && samp::RefGame()->GetPlayerPed() && samp::RefNetGame()) {
        if (samp::RefLabel())
            samp::RefLabel()->Begin();

        CVector vecScreen;
        DWORD lastTick = GetTickCount();
        int largestId = samp::RefNetGame()->m_pPools->m_pPlayer->m_nLargestId;
        for (int i = 0; i < largestId; i++) {
            auto playerInfo = samp::RefNetGame()->m_pPools->m_pPlayer->m_pObject[i];
            if (!playerInfo)
                continue;

            auto m_player = playerInfo->m_pPlayer;
            if (!m_player)
                continue;

            auto player = m_player->m_pPed;
            if (!player)
                continue;

            auto* playerPed = player->m_pGamePed;
            float distanceToPlayer = player->GetDistanceToLocalPlayer();
            float distanceToCam = player->GetDistanceToCamera();
            float chatBubbleHeight = getChatBubbleHeight();

            RwV3d RwPos;
            playerPed->GetBonePosition(RwPos, 8, 1);
            sampapi::CVector vecPos = *(sampapi::CVector*)&RwPos;

            vecPos.z += distanceToCam * chatBubbleHeight + 0.2;
            CalcScreenCoors((CVector*)&vecPos, &vecScreen);

            for (auto& bubble : g_Bubbles[i]) {
                if (!bubble.m_bExists)
                    continue;

                if (distanceToPlayer > bubble.m_fDrawDistance)
                    continue;

                if (lastTick > (bubble.m_creationTick + bubble.m_lifeSpan)) {
                    bubble.m_bExists = false;
                    continue;
                }

                std::string utf8_ChatBubbleText = CP1251ToUTF8(bubble.m_szText);
                filterText(ImGui::GetFont(), utf8_ChatBubbleText);

                std::vector<std::string> lines;
                float totalHeight = 0.0;
                calcTotalHeight(utf8_ChatBubbleText, settings.getMaxWidth(), totalHeight, lines);

                vecScreen.y -= totalHeight;
                float y = vecScreen.y;
                if (vecScreen.z >= 0) {
                    for (auto& line : lines) {
                        ImVec2 size = ImGui::CalcTextSize(line.c_str());
                        float x = vecScreen.x - size.x / 2;
                        uint32_t color = fadeInOut(lastTick, bubble.m_creationTick, bubble.m_lifeSpan, settings.getDuration(), bubble.m_color);
                        drawShadowText(dl, x, y, line.c_str(), color);
                        y += size.y;
                    }
                }
                vecScreen.y -= settings.getMargin();
            }
        }
        if (samp::RefLabel())
            samp::RefLabel()->End();
    }

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

#include "main.h"

void OpenConsole() {
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
}

std::array<std::array<samp::CChatBubble::Player, 5>, 1004> g_Bubbles;

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

    OpenConsole();
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

    auto it = std::min_element(bubbles.begin(), bubbles.end(), [lastTick](const auto& a, const auto& b) {
        bool a_dead = (a.m_creationTick + a.m_lifeSpan <= lastTick);
        bool b_dead = (b.m_creationTick + b.m_lifeSpan <= lastTick);

        if (a_dead && !b_dead) return true;
        if (!a_dead && b_dead) return false;
        if (a_dead && b_dead) return false;

        return a.m_creationTick < b.m_creationTick;
        });

    samp::CChatBubble::Player& bubble = *it;

    auto orig = hook.get_trampoline()(p_this, n_Player, szText, color, fDrawDistance, lifeSpan);
    bubble = p_this->m_player[n_Player];
    bubble.m_creationTick = lastTick;

    std::string wrapped = wrapText(bubble.m_szText, 36);
    int lines = countLines(wrapped);

    strcpy(bubble.m_szText, wrapped.c_str());
    bubble.m_nMaxLineLength = lines;

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
    std::cout << "inited!\n";
}

void Main::CChatBubble__Draw(
    const decltype(hookCChatBubble__Draw)& hook,
    samp::CChatBubble* p_this
) {
    if (!isImguiInitialized) {
        std::cout << "try to init\n";
        this->TryInitImgui();
        return;
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX9_NewFrame();

    static int fontSize = 0;
    int currentFontSize = getFontSize();
    if (fontSize != currentFontSize) {
        changeFontSize(ImGui::GetIO(), currentFontSize);
        fontSize = currentFontSize;
        std::cout << fontSize << '\n';
    }

    ImGui::NewFrame();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();


    if (samp::RefGame() && samp::RefGame()->GetPlayerPed() && samp::RefNetGame()) {
        if (samp::RefLabel())
            samp::RefLabel()->Begin();

        DWORD lastTick = GetTickCount();
        int largestId = samp::RefNetGame()->m_pPools->m_pPlayer->m_nLargestId;
        for (int i = 0; i < largestId; i++) {
            auto playerInfo = samp::RefNetGame()->m_pPools->m_pPlayer->m_pObject[i];
            if (!playerInfo)
                continue;

            auto player = playerInfo->m_pPlayer->m_pPed;
            if (!player)
                continue;

            auto* playerPed = player->m_pGamePed;
            float distanceToPlayer = player->GetDistanceToLocalPlayer();
            float distanceToCam = player->GetDistanceToCamera();
            float chatBubbleHeight = getChatBubbleHeight();
            int fontSize = getFontSize();

            RwV3d RwPos;
            playerPed->GetBonePosition(RwPos, 8, 1);
            sampapi::CVector vecPos = *(sampapi::CVector*)&RwPos;

            CVector vecScreen;
            float margin = 5.0;
            bool didFirst = false;

            for (auto& bubble : g_Bubbles[i]) {
                if (!bubble.m_bExists)
                    continue;

                if (distanceToPlayer > bubble.m_fDrawDistance)
                    continue;

                if (lastTick > (bubble.m_creationTick + bubble.m_lifeSpan)) {
                    bubble.m_bExists = false;
                    continue;
                }

                ImVec2 textRect = ImGui::CalcTextSize(bubble.m_szText);

                if (!didFirst) {
                    float lines = std::max(0, bubble.m_nMaxLineLength - 1);
                    float heightOffset = 0.08 * lines;
                    float scaleOffset = 0.0125 * lines;

                    vecPos.z += distanceToCam * (scaleOffset + chatBubbleHeight) + heightOffset + 0.25;
                    CalcScreenCoors((CVector*)&vecPos, &vecScreen);
                    didFirst = true;
                } else 
                    vecScreen.y -= textRect.y;

                if (vecScreen.z > 0.0)
                    drawShadowText(dl, vecScreen.x - textRect.x * 0.5, vecScreen.y, bubble.m_szText, bubble.m_color);

                vecScreen.y -= margin;
            }

        }
        if (samp::RefLabel())
            samp::RefLabel()->End();
    }

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

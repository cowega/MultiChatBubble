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

    HWND hwnd = *reinterpret_cast<HWND*>(0xC17054);
    IDirect3DDevice9* pDevice = *reinterpret_cast<IDirect3DDevice9**>(0xC97C28);

    if (!hwnd || !pDevice)
        return;

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(**reinterpret_cast<HWND**>(0xC17054));
    ImGui_ImplDX9_Init(pDevice);

    ImGui::GetIO().Fonts->Clear();
    ImGui::GetIO().Fonts->AddFontDefault();
    //int fontSize = getFontSize();
    //changeFontSize(ImGui::GetIO(), fontSize);

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
    dl->AddText(ImVec2(10, 10), 0xFFFFFFFF, "TEXT FOR RENDER");

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

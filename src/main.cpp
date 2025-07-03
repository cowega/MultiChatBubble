#include "main.h"

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

void Main::CChatBubble__Draw(
    const decltype(hookCChatBubble__Draw)& hook,
    samp::CChatBubble* p_this
) {
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

            float baseZ = vecPos.z;
            short idx = 0;
            float cumulativeMargin = 0.0;
            for (auto& bubble : g_Bubbles[i]) {
                if (!bubble.m_bExists)
                    continue;

                if (distanceToPlayer > bubble.m_fDrawDistance)
                    continue;

                if (lastTick > (bubble.m_creationTick + bubble.m_lifeSpan)) {
                    bubble.m_bExists = 0;
                    continue;
                }

                float lines = std::max(0, bubble.m_nMaxLineLength - 1);
                float heightOffset = 0.08 * lines;
                float scaleOffset = 0.0125 * lines;

                float perspectiveFix = distanceToCam * 0.02;
                float bubbleHeight = perspectiveFix * bubble.m_nMaxLineLength * (fontSize * 0.075);

                vecPos.z = baseZ + distanceToCam * (scaleOffset + chatBubbleHeight) + heightOffset + 0.2 + cumulativeMargin;

                samp::RefLabel()->Draw(&vecPos, bubble.m_szText, bubble.m_color, 1, 0);

                cumulativeMargin += bubbleHeight;
            }
        }
        if (samp::RefLabel())
            samp::RefLabel()->End();
    }
}

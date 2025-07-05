#include "imgui_utils.h"

void changeFontSize(ImGuiIO& io, int size) {
    std::string font{ getenv("WINDIR") };
    font += "\\Fonts\\" + (std::string)GetFontFace() + "bd.ttf";

    io.Fonts->Clear();

    ImFont* newFont = io.Fonts->AddFontFromFileTTF(font.c_str(), size - 3.0, NULL, io.Fonts->GetGlyphRangesCyrillic());

    if (newFont) {
        io.FontDefault = newFont;
        std::cout << font << '\n';
    } else
        std::cout << "font not found\n";
    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui_ImplDX9_CreateDeviceObjects();
}

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, uint32_t color) {
    color = ñonvertARGBToRGBA(color);
    uint32_t shadowColor = color & 0xFF000000;
    text = convertCP1251ToUTF8(text);

	dl->AddText(ImVec2(x + 1.0, y), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y + 1.0), shadowColor, text.c_str());
	dl->AddText(ImVec2(x - 1.0, y), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y - 1.0), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y), color, text.c_str());
}

char* GetFontFace() {
	return reinterpret_cast<char*(__cdecl*)()>(sampapi::GetBase() + 0xB3D40)();
}

std::string convertCP1251ToUTF8(const std::string& str_cp1251) {
    int size_needed = MultiByteToWideChar(1251, 0, str_cp1251.c_str(), (int)str_cp1251.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(1251, 0, str_cp1251.c_str(), (int)str_cp1251.size(), &wstr[0], size_needed);

    int utf8_size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), size_needed, NULL, 0, NULL, NULL);
    std::string str_utf8(utf8_size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), size_needed, &str_utf8[0], utf8_size, NULL, NULL);

    return str_utf8;
}

ImU32 ñonvertARGBToRGBA(uint32_t sampColor) {
    uint8_t a = (sampColor >> 24) & 0xFF;
    uint8_t r = (sampColor >> 16) & 0xFF;
    uint8_t g = (sampColor >> 8) & 0xFF;
    uint8_t b = (sampColor) & 0xFF;
    return IM_COL32(r, g, b, a);
}

uint32_t fadeInOut(int lastTick, int creationTick, int lifeSpan, int duration, uint32_t color) {
    uint8_t origAlpha = (color >> 24) & 0xFF;
    int elapsed = lastTick - creationTick;
    uint8_t alpha = origAlpha;

    if (elapsed < duration) {
        float t = std::clamp(elapsed / (float)duration, 0.f, 1.f);
        alpha = static_cast<uint8_t>(origAlpha * t);
    } else if (elapsed > lifeSpan - duration) {
        float t = std::clamp((elapsed - (lifeSpan - duration)) / (float)duration, 0.f, 1.f);
        alpha = static_cast<uint8_t>(origAlpha * (1.0f - t));
    } else {
        alpha = origAlpha;
    }

    return (alpha << 24) | (color & 0x00FFFFFF);
}

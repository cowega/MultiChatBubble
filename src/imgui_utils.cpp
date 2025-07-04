#include "imgui_utils.h"

void changeFontSize(ImGuiIO& io, int size) {
    std::string font{ getenv("WINDIR") };
    font += "\\Fonts\\" + (std::string)GetFontFace() + ".ttf";

    io.Fonts->Clear();

    ImFont* newFont = io.Fonts->AddFontFromFileTTF(font.c_str(), size - 2.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

    if (newFont) {
        io.FontDefault = newFont;
        std::cout << font << '\n';
    } else
        std::cout << "font not found\n";
    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui_ImplDX9_CreateDeviceObjects();
}

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, uint32_t color) {
    uint32_t shadowColor = color & 0xFF000000;
    text = convertCP1251ToUTF8(text);

	dl->AddText(ImVec2(x + 1.0f, y), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y + 1.0f), shadowColor, text.c_str());
	dl->AddText(ImVec2(x - 1.0f, y), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y - 1.0f), shadowColor, text.c_str());
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
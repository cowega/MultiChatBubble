#include "imgui_utils.h"

void changeFontSize(ImGuiIO& io, int size, std::string fontName) {
    std::string font{ getenv("WINDIR") };
    font += "\\Fonts\\" + fontName;

    io.Fonts->Clear();

    ImFont* newFont = io.Fonts->AddFontFromFileTTF(font.c_str(), size - 3.0, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui::GetIO().FontGlobalScale = 1.0;

    if (newFont) {
        io.FontDefault = newFont;
    } else {
        changeFontSize(io, size, "Arialbd.ttf");
    }

    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui_ImplDX9_CreateDeviceObjects();
}

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, uint32_t color) {
    color = сonvertARGBToRGBA(color);
    uint32_t shadowColor = color & 0xFF000000;

	dl->AddText(ImVec2(x + 1.0, y), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y + 1.0), shadowColor, text.c_str());
	dl->AddText(ImVec2(x - 1.0, y), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y - 1.0), shadowColor, text.c_str());
	dl->AddText(ImVec2(x, y), color, text.c_str());
}

char* GetFontFace() {
	return reinterpret_cast<char*(__cdecl*)()>(sampapi::GetBase() + 0xB3D40)();
}

std::string CP1251ToUTF8(const std::string& cp1251)
{
    if (cp1251.empty())
        return "";

    int wlen = MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, nullptr, 0);
    std::wstring wide(wlen, 0);
    MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, wide.data(), wlen);

    int utf8len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(utf8len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, utf8.data(), utf8len, nullptr, nullptr);

    return utf8;
}

ImU32 сonvertARGBToRGBA(uint32_t sampColor) {
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

std::vector<std::string> WrapText(const std::string& text, float maxWidth) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string word, line;

    while (stream >> word) {
        std::string testLine = line.empty() ? word : line + " " + word;
        if (ImGui::CalcTextSize(testLine.c_str()).x > maxWidth && !line.empty()) {
            lines.push_back(line);
            line.clear();

            size_t i = 0;
            std::string chunk;

            while (i < word.size()) {
                unsigned char c = word[i];
                size_t charLen = 1;
                if ((c & 0xE0) == 0xC0) charLen = 2;
                else if ((c & 0xF0) == 0xE0) charLen = 3;
                else if ((c & 0xF8) == 0xF0) charLen = 4;

                std::string nextChar = word.substr(i, charLen);
                chunk += nextChar;

                if (ImGui::CalcTextSize(chunk.c_str()).x > maxWidth) {
                    chunk.erase(chunk.size() - charLen);
                    lines.push_back(chunk);
                    chunk = nextChar;
                }
                i += charLen;
            }
            if (!chunk.empty())
                line = chunk;
        } else {
            line = testLine;
        }
    }

    if (!line.empty())
        lines.push_back(line);

    return lines;
}

void calcTotalHeight(
    std::string text,
    float maxWidth, float& totalHeight,
    std::vector<std::string>& lines
) {
    lines = WrapText(text, 300);
    for (const auto& line : lines) {
        ImVec2 size = ImGui::CalcTextSize(line.c_str());
        totalHeight += size.y;
    }
}

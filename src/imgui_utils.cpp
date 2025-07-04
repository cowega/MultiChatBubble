#include "imgui_utils.h"

void changeFontSize(ImGuiIO io, std::string szFont, float size) {
	std::string font{ getenv("WINDIR") }; font += "\\Fonts\\" + szFont;
	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF(font.c_str(), size - 2.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
}

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, unsigned long color) {
	dl->AddText(ImVec2(x + 1.0f, y), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x, y + 1.0f), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x - 1.0f, y), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x, y - 1.0f), 0xFF000000, text.c_str());
	dl->AddText(ImVec2(x, y), color, text.c_str());
}
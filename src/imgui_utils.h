#pragma once

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <sampapi/sampapi.h>

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_internal.h"

void changeFontSize(ImGuiIO& io, int size, std::string fontName);

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, uint32_t color);

char* GetFontFace();

std::string CP1251ToUTF8(const std::string& cp1251);

ImU32 ñonvertARGBToRGBA(uint32_t sampColor);

uint32_t fadeInOut(int lastTick, int creationTick, int lifeSpan, int duration, uint32_t color);

std::vector<std::string> WrapText(const std::string& text, float maxWidth);

void calcTotalHeight(
    std::string text,
    float maxWidth, float& totalHeight,
    std::vector<std::string>& lines
);

void filterText(ImFont* font, std::string& text);
#pragma once

#include <iostream>
#include <sampapi/sampapi.h>
#include <Windows.h>

#include "imgui.h"
#include "imgui_impl_dx9.h"

void changeFontSize(ImGuiIO& io, int size);

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, uint32_t color);

char* GetFontFace();

std::string convertCP1251ToUTF8(const std::string& str_cp1251);

#pragma once

#include <iostream>
#include <sampapi/sampapi.h>

#include "imgui.h"
#include "imgui_impl_dx9.h"

void changeFontSize(ImGuiIO& io, int size);

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, unsigned long color);

char* GetFontFace();
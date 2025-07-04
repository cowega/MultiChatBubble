#pragma once

#include <iostream>
#include "imgui.h"

void changeFontSize(ImGuiIO io, std::string szFont, float size);

void drawShadowText(ImDrawList* dl, float x, float y, std::string text, unsigned long color);
#pragma once

#include <Windows.h>
#include <iostream>
#include "plugin.h"

#include <sampapi/CNetGame.h>

float getChatBubbleHeight();

int getFontSize();

int getMaxLineLength(char* a1, signed int a2, int a3);

std::string wrapText(const std::string& text, size_t maxLen);

int countLines(const std::string& text);

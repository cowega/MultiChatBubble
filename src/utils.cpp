#include "utils.h"

float getChatBubbleHeight() {
    return *reinterpret_cast<float*>(sampapi::GetBase() + 0xD795C);
}

int getFontSize() {
    return reinterpret_cast<int(__cdecl*)()>(sampapi::GetBase() + 0xB3C60)();
}

int getMaxLineLength(char* a1, signed int a2, int a3) {
    return reinterpret_cast<int(__cdecl*)(char*, signed int, int)>(sampapi::GetBase() + 0x13DB0)(a1, a2, a3);
}

std::string wrapText(const std::string& text, size_t maxLen) {
    std::istringstream iss(text);
    std::string word;
    std::string line;
    std::string result;

    while (iss >> word) {
        if (line.length() + word.length() + 1 >= maxLen) {
            if (!line.empty()) {
                result += line + '\n';
                line = word;
            } else {
                result += word + '\n';
            }
        } else {
            if (!line.empty())
                line += ' ';
            line += word;
        }
    }

    if (!line.empty())
        result += line;

    return result;
}

int countLines(const std::string& text) {
    if (text.empty()) return 0;

    int lines = 1;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\n') {
            if (i + 1 < text.size())
                ++lines;
        }
    }
    return lines;
}

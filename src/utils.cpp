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

void CalcScreenCoors(CVector* vecWorld, CVector* vecScreen) {
    D3DXMATRIX m((float*)(0xB6FA2C));

    DWORD* dwLenX = (DWORD*)(0xC17044);
    DWORD* dwLenY = (DWORD*)(0xC17048);

    vecScreen->x = (vecWorld->z * m._31) + (vecWorld->y * m._21) + (vecWorld->x * m._11) + m._41;
    vecScreen->y = (vecWorld->z * m._32) + (vecWorld->y * m._22) + (vecWorld->x * m._12) + m._42;
    vecScreen->z = (vecWorld->z * m._33) + (vecWorld->y * m._23) + (vecWorld->x * m._13) + m._43;

    double fRecip = (double)1.0 / vecScreen->z;
    vecScreen->x *= (float)(fRecip * (*dwLenX));
    vecScreen->y *= (float)(fRecip * (*dwLenY));
}

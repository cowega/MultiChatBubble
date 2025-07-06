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

void textFilter(char* text) {
    return reinterpret_cast<void(__cdecl*)(char*)>(sampapi::GetBase() + 0xAF780)(text);
}
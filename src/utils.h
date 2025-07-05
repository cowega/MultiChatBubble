#pragma once

#include <Windows.h>
#include <iostream>
#include "plugin.h"

#include <sampapi/CNetGame.h>

struct D3DXMATRIX {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };

    D3DXMATRIX() = default;

    explicit D3DXMATRIX(float* ptr) {
        memcpy(m, ptr, sizeof(m));
    }
};


float getChatBubbleHeight();

int getFontSize();

int getMaxLineLength(char* a1, signed int a2, int a3);

void CalcScreenCoors(CVector* vecWorld, CVector* vecScreen);

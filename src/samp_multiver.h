#pragma once

#include <sampapi/CChatBubble.h>
#include <sampapi/CGame.h>
#include <sampapi/CNetGame.h>
#include <sampapi/CLabel.h>
#include <sampapi/CEntity.h>
#include <sampapi/CFonts.h>

#if defined(BUILD_SAMP_R1)
	namespace samp = sampapi::v037r1;

	inline constexpr uintptr_t CChatBubble__DrawAddr = 0x63310;
	inline constexpr uintptr_t CChatBubble__AddAddr = 0x63250;

	inline constexpr uintptr_t GetFontFaceAddr = 0xB3D40;
	inline constexpr uintptr_t GetChatBubbleHeightAddr = 0xD795C;
	inline constexpr uintptr_t GetFontSizeAddr = 0xB3C60;
	inline constexpr uintptr_t TextFilterAddr = 0xAF780;
#elif defined(BUILD_SAMP_R3)
	namespace samp = sampapi::v037r3;

	inline constexpr uintptr_t CChatBubble__DrawAddr = 0x66760;
	inline constexpr uintptr_t CChatBubble__AddAddr = 0x666A0;

	inline constexpr uintptr_t GetFontFaceAddr = 0xC5C00;
	inline constexpr uintptr_t GetChatBubbleHeightAddr = 0xE9C3C;
	inline constexpr uintptr_t GetFontSizeAddr = 0xC5B20;
	inline constexpr uintptr_t TextFilterAddr = 0xB5830;
#else
	#error "No SAMP version defined."
#endif

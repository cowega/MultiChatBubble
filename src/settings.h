#pragma once

#include <string>
#include <SimpleIni.h>


class Settings {
private:
	std::string configFileName = "MultiChatBubble.ini";
	CSimpleIni ini;

	float fadeInOutDuration;
	std::string fontName;
	int maxChatBubbles;
	float maxWidth;
	float margin;
public:
	Settings();
	~Settings();

	int Load();
	int Create();

	float getDuration();
	std::string getFontName();
	int getMaxChatBubbles();
	float getMaxWidth();
	float getMargin();
};
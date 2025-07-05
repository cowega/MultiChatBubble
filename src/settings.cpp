#include "settings.h"

Settings::Settings() {
	ini.SetUnicode();
	Load();
}

Settings::~Settings() { }

int Settings::Load() {

	SI_Error rc = ini.LoadFile(configFileName.c_str());
	if (rc < 0) {
		return this->Create();
	}

	fadeInOutDuration = std::stof(ini.GetValue("Settings", "fadeInOutDuration", "180"));
	fontName = ini.GetValue("Settings", "fontName", "Arialbd.ttf");
	maxChatBubbles = std::stoi(ini.GetValue("Settings", "maxChatBubbles", "5"));
	maxWidth = std::stof(ini.GetValue("Settings", "maxWidth", "300.0"));
	margin = std::stof(ini.GetValue("Settings", "margin", "5.0"));

	return rc;
}

int Settings::Create() {
	ini.SetValue("Settings", "fadeInOutDuration", "180");
	ini.SetValue("Settings", "fontName", "Arialbd.ttf");
	ini.SetValue("Settings", "maxChatBubbles", "5");
	ini.SetValue("Settings", "maxWidth", "300.0");
	ini.SetValue("Settings", "margin", "5.0");

	SI_Error rc = ini.SaveFile(configFileName.c_str());
	return Load();
}

float Settings::getDuration() {
	return fadeInOutDuration;
}

std::string Settings::getFontName() {
	return fontName;
}

int Settings::getMaxChatBubbles() {
	return maxChatBubbles;
}

float Settings::getMaxWidth() {
	return maxWidth;
}

float Settings::getMargin() {
	return margin;
}

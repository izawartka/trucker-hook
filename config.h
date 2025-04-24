#pragma once
#include "common.h"

#define CONFIG_FILE_NAME "hook.cfg"

class Config {
public:
	static std::string GetConfigPath();

	static bool ReadFromConfig(const char* section, const char* key, int& value);
	static void WriteToConfig(const char* section, const char* key, int value);

	static bool ReadFromConfig(const char* section, const char* key, float& value);
	static void WriteToConfig(const char* section, const char* key, float value);

	template<typename T>
	static void ReadOrDefaultFromConfig(const char* section, const char* key, T& value, T defaultValue) {
		bool success = ReadFromConfig(section, key, value);

		if (!success) {
			value = defaultValue;
			WriteToConfig(section, key, defaultValue);
		}
	}

private:
	static std::string cachedConfigPath;
};
#include "config.h"

std::string Config::cachedConfigPath = "";

std::string Config::GetConfigPath()
{
	if (cachedConfigPath.size()) return cachedConfigPath;

	CHAR fullPath[MAX_PATH];
	GetModuleFileNameA(nullptr, fullPath, MAX_PATH);

	std::string path(fullPath);
	size_t pos = path.find_last_of("\\/");
	if (pos != std::string::npos) {
		path = path.substr(0, pos + 1);
	}
	else {
		path = ".\\";
	}

	cachedConfigPath = path + CONFIG_FILE_NAME;
	return cachedConfigPath;
}

bool Config::ReadFromConfig(const char* section, const char* key, int& value)
{
	char buffer[32];
	GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), GetConfigPath().c_str());

	if (strlen(buffer) == 0) {
		return false;
	}

	try {
		value = std::stoi(buffer);
	}
	catch (...) {
		return false;
	}

	return true;
}

void Config::WriteToConfig(const char* section, const char* key, int value)
{
	char buffer[32];
	sprintf_s(buffer, "%d", value);
	WritePrivateProfileStringA(section, key, buffer, GetConfigPath().c_str());
}

bool Config::ReadFromConfig(const char* section, const char* key, float& value)
{
	char buffer[32];
	GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), GetConfigPath().c_str());

	if (strlen(buffer) == 0) {
		return false;
	}

	try {
		value = std::stof(buffer);
	}
	catch (...) {
		return false;
	}

	return true;
}

void Config::WriteToConfig(const char* section, const char* key, float value)
{
	char buffer[32];
	sprintf_s(buffer, "%f", value);
	WritePrivateProfileStringA(section, key, buffer, GetConfigPath().c_str());
}

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace config
{
	inline static bool isCopied = false;
}

void SaveConfig(const std::string& fileName);
void LoadConfig(const std::string& fileName);
std::vector<std::string> GetFilesInDirectory(const std::string& directory);
std::vector<std::string> GetDirectoriesInDirectory(const std::string& directory);
std::string GetDirectoryName(const std::string& path);
void CopyDirectory(const std::string& sourceDir, const std::string& destDir);
#endif // CONFIG_H

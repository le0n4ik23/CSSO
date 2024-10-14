#include "../source/gui.h"
#include "../source/config.h"
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <fstream>
#include <io.h>
#include <Psapi.h>

namespace fs = std::filesystem;

// Function to get files in a directory
std::vector<std::string> GetFilesInDirectory(const std::string& directory) {
	std::vector<std::string> files;
	for (const auto& entry : fs::directory_iterator(directory)) {
		if (entry.is_regular_file()) {
			files.push_back(entry.path().filename().string());
		}
	}
	return files;
}
std::vector<std::string> GetDirectoriesInDirectory(const std::string& directory) {
	std::vector<std::string> directories;
	for (const auto& entry : fs::directory_iterator(directory)) {
		if (entry.is_directory()) {
			directories.push_back(entry.path().filename().string());
		}
	}
	return directories;
}

std::string GetDirectoryName(const std::string& path) {
	return std::filesystem::path(path).filename().string();
}
std::string join(const std::vector<int>& vec) {
	std::ostringstream oss;
	for (size_t i = 0; i < vec.size(); ++i) {
		if (i != 0) {
			oss << ',';
		}
		oss << vec[i];
	}
	return oss.str();
}


void CopyDirectory(const std::string& sourceDir, const std::string& destDir) {
	config::isCopied = false;
	try {
		// Проверяем, существует ли целевая директория, и создаем ее, если нет
		if (!fs::exists(destDir)) {
			fs::create_directories(destDir);
		}

		// Итерация по всем элементам в исходной директории
		for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
			const auto& path = entry.path();
			auto relativePathStr = path.lexically_relative(sourceDir).string();
			fs::path targetPath = fs::path(destDir) / relativePathStr;

			if (fs::is_directory(path)) {
				// Создаем директорию в целевой директории
				fs::create_directories(targetPath);
			}
			else if (fs::is_regular_file(path)) {
				// Копируем файл в целевую директорию
				fs::copy_file(path, targetPath, fs::copy_options::overwrite_existing);
			}
		}

		// Устанавливаем значение переменной в true по завершении копирования
		config::isCopied = true;
	}
	catch (fs::filesystem_error& e) {
		std::cerr << "Ошибка копирования: " << e.what() << std::endl;
	}
}

void SaveConfig(const std::string& fileName) {
	fs::path documentsFolder = "C:/hw$";
	fs::create_directories(documentsFolder);
	fs::path configPath = documentsFolder / fileName;

	std::ofstream file(configPath);
	if (file.is_open()) {
		file << std::boolalpha;
		
		
		
		file << "Bunnyhop=" << gui::settings::misc::bhop << '\n'
			<< "Radar=" << gui::settings::misc::radar << '\n'
			<< "Velocity=" << gui::settings::misc::velocity << '\n'
			<< "Stamina=" << gui::settings::misc::stamina << '\n'
			<< "Velocityposition=" << gui::settings::misc::velocityposition << '\n'
			<< "Staminaposition=" << gui::settings::misc::staminaposition << '\n'
			<< "ShowLastVelocity=" << gui::settings::misc::pretype << '\n'
			<< "Espenable=" << gui::settings::esp::enable << '\n'
			<< "EspLength=" << gui::settings::esp::length << '\n'
			<< "EspDistance=" << gui::settings::esp::espdistance << '\n'
			<< "Name=" << gui::settings::esp::name << '\n'
			<< "Health=" << gui::settings::esp::health << '\n'
			<< "Weapon=" << gui::settings::esp::weapon << '\n'
			<< "Boxes=" << gui::settings::esp::boxes << '\n'
			<< "DrawC4=" << gui::settings::esp::drawc4 << '\n'
			<< "BombInfo=" << gui::settings::misc::bombinfo << '\n'
			<< "Flags=" << gui::settings::esp::flags << '\n'
			<< "Aimbotenable=" << gui::settings::aimbot::enable << '\n'
			<< "BoxInner=" << gui::settings::esp::inner << '\n'
			<< "BoxOuter=" << gui::settings::esp::outer << '\n'
			<< "Silent=" << gui::settings::aimbot::silent << '\n'
			<< "AutoStop=" << gui::settings::aimbot::autostop << '\n'
			<< "pistolfov=" << gui::settings::aimbot::pistolfov << '\n'
			<< "pistolrcs=" << gui::settings::aimbot::pistolrcs << '\n'
			<< "pistolsmooth=" << gui::settings::aimbot::pistolsmooth << '\n'
			<< "riflefov=" << gui::settings::aimbot::riflefov << '\n'
			<< "riflercs=" << gui::settings::aimbot::riflercs << '\n'
			<< "riflesmooth=" << gui::settings::aimbot::riflesmooth << '\n'
			<< "sniperfov=" << gui::settings::aimbot::sniperfov << '\n'
			<< "sniperrcs=" << gui::settings::aimbot::sniperrcs << '\n'
			<< "snipersmooth=" << gui::settings::aimbot::snipersmooth << '\n'
			<< "Watermark=" << gui::settings::misc::watermark << '\n'
			<< "Spectatorlist=" << gui::settings::misc::spectatorlist << '\n'
			<< "FogEnable=" << gui::settings::esp::fogenable << '\n'
			<< "FogMin=" << gui::settings::esp::fogmin << '\n'
			<< "FogMax=" << gui::settings::esp::fogmax << '\n'
			<< "FogDensity=" << gui::settings::esp::fogdensity << '\n'
			
		<< "pistolhitboxes=" << join(gui::settings::aimbot::pistolhitboxes) << '\n'
			<< "riflehitboxes=" << join(gui::settings::aimbot::riflehitboxes) << '\n'
			<< "sniperhitboxes=" << join(gui::settings::aimbot::sniperhitboxes) << '\n';
		file << "MenuColor=" << gui::settings::misc::checkboxColor.x << ","
			<< gui::settings::misc::checkboxColor.y << ","
			<< gui::settings::misc::checkboxColor.z << ","
			<< gui::settings::misc::checkboxColor.w << '\n';

		file << "OOFVisColor=" << gui::settings::esp::outoffovcolor.x << ","
			<< gui::settings::esp::outoffovcolor.y << ","
			<< gui::settings::esp::outoffovcolor.z << ","
			<< gui::settings::esp::outoffovcolor.w << '\n';

		file << "OOFInvisColor=" << gui::settings::esp::outoffovicolor.x << ","
			<< gui::settings::esp::outoffovicolor.y << ","
			<< gui::settings::esp::outoffovicolor.z << ","
			<< gui::settings::esp::outoffovicolor.w << '\n';

		file << "VeloColor=" << gui::settings::misc::velocolor.x << ","
			<< gui::settings::misc::velocolor.y << ","
			<< gui::settings::misc::velocolor.z << ","
			<< gui::settings::misc::velocolor.w << '\n';

		file << "StaminaColor=" << gui::settings::misc::staminacolor.x << ","
			<< gui::settings::misc::staminacolor.y << ","
			<< gui::settings::misc::staminacolor.z << ","
			<< gui::settings::misc::staminacolor.w << '\n';

		file << "PreColor=" << gui::settings::misc::precolor.x << ","
			<< gui::settings::misc::precolor.y << ","
			<< gui::settings::misc::precolor.z << ","
			<< gui::settings::misc::precolor.w << '\n';
	}
	else {
		std::cerr << "Unable to open file " << configPath << " for writing." << std::endl;
	}
}
// Utility function to join vector of integers into a comma-separated string
std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}



// Utility function to convert a comma-separated string to a vector of integers
std::vector<int> parseVector(const std::string& s) {
	std::vector<int> result;
	std::vector<std::string> tokens = split(s, ',');
	for (const std::string& token : tokens) {
		result.push_back(std::stoi(token));
	}
	return result;
}

ImVec4 parseColor(const std::string& value) {

	std::istringstream iss(value);
	std::vector<float> components;
	std::string component;
	while (std::getline(iss, component, ',')) {
		components.push_back(std::stof(component));
	}
	if (components.size() == 4) {
		return ImVec4(components[0], components[1], components[2], components[3]);
	}
	return ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Default color if parsing fails
}


void LoadConfig(const std::string& fileName) {
	fs::path documentsFolder = "C:/hw$";
	fs::path configPath = documentsFolder / fileName;

	std::ifstream file(configPath);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			size_t pos = line.find('=');
			if (pos != std::string::npos) {
				std::string key = line.substr(0, pos);
				std::string value = line.substr(pos + 1);
				

				if (key == "Bunnyhop") gui::settings::misc::bhop = (value == "true");
				else if (key == "Radar") gui::settings::misc::radar = (value == "true");
				else if (key == "Velocity") gui::settings::misc::velocity = (value == "true");
				else if (key == "Stamina") gui::settings::misc::stamina = (value == "true");
				else if (key == "Velocityposition") gui::settings::misc::velocityposition = std::stoi(value);
				else if (key == "Staminaposition") gui::settings::misc::staminaposition = std::stoi(value);
				else if (key == "ShowLastVelocity") gui::settings::misc::pretype = std::stoi(value);
				else if (key == "Espenable") gui::settings::esp::enable = (value == "true");
				else if (key == "EspLength") gui::settings::esp::length = std::stoi(value);
				else if (key == "EspDistance") gui::settings::esp::espdistance = std::stoi(value);
				else if (key == "Name") gui::settings::esp::name = (value == "true");
				else if (key == "Health") gui::settings::esp::health = (value == "true");
				else if (key == "Weapon") gui::settings::esp::weapon = (value == "true");
				else if (key == "Boxes") gui::settings::esp::boxes = (value == "true");
				else if (key == "DrawC4") gui::settings::esp::drawc4 = (value == "true");
				else if (key == "BombInfo") gui::settings::misc::bombinfo = (value == "true");
				else if (key == "Flags") gui::settings::esp::flags = (value == "true");
				else if (key == "Aimbotenable") gui::settings::aimbot::enable = (value == "true");
				else if (key == "BoxInner") gui::settings::esp::inner = (value == "true");
				else if (key == "BoxOuter") gui::settings::esp::outer = (value == "true");
				else if (key == "Silent") gui::settings::aimbot::silent = (value == "true");
				else if (key == "AutoStop") gui::settings::aimbot::autostop = (value == "true");
				else if (key == "FogEnable") gui::settings::esp::fogenable = (value == "true");
				else if (key == "FogMin") gui::settings::esp::fogmin = std::stof(value);
				else if (key == "FogMax") gui::settings::esp::fogmax = std::stoi(value);
				else if (key == "FogDensity") gui::settings::esp::fogdensity = std::stoi(value);
				else if (key == "snipersmooth") gui::settings::aimbot::snipersmooth = std::stof(value);
				else if (key == "pistolfov") gui::settings::aimbot::pistolfov = std::stof(value);
				else if (key == "pistolrcs") gui::settings::aimbot::pistolrcs = std::stoi(value);
				else if (key == "pistolsmooth") gui::settings::aimbot::pistolsmooth = std::stof(value);
				else if (key == "riflefov") gui::settings::aimbot::riflefov = std::stof(value);
				else if (key == "riflercs") gui::settings::aimbot::riflercs = std::stoi(value);
				else if (key == "riflesmooth") gui::settings::aimbot::riflesmooth = std::stof(value);
				else if (key == "sniperfov") gui::settings::aimbot::sniperfov = std::stof(value);
				else if (key == "sniperrcs") gui::settings::aimbot::sniperrcs = std::stoi(value);
				else if (key == "snipersmooth") gui::settings::aimbot::snipersmooth = std::stof(value);
				else if (key == "Spectatorlist") gui::settings::misc::spectatorlist = (value == "true");
				else if (key == "Watermark") gui::settings::misc::watermark = (value == "true");
				else if (key == "pistolhitboxes") gui::settings::aimbot::pistolhitboxes = parseVector(value);
				else if (key == "riflehitboxes") gui::settings::aimbot::riflehitboxes = parseVector(value);
				else if (key == "sniperhitboxes") gui::settings::aimbot::sniperhitboxes = parseVector(value);
			

				else if (key == "FogColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::esp::fogcolor = loadedColor;
					}
				}
				else if (key == "OOFVisColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::esp::outoffovcolor = loadedColor;
					}
				}
				else if (key == "OOFInvisColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::esp::outoffovicolor = loadedColor;
					}
				}
				else if (key == "BoxColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::esp::boxcolor = loadedColor;
					}
				}
				else if (key == "VeloColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::misc::velocolor = loadedColor;
					}
				}
				else if (key == "StaminaColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::misc::staminacolor = loadedColor;
					}
				}
				else if (key == "PreColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::misc::precolor = loadedColor;
					}
				}
				
				else if (key == "VisibleColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::chams::visiblecolor = loadedColor;
					}
				}
				else if (key == "HiddenColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::chams::hiddencolor = loadedColor;
					}
				}
				else if (key == "MenuColor") {
					std::istringstream iss(value);
					std::string component;
					std::vector<float> components;
					while (std::getline(iss, component, ',')) {
						components.push_back(std::stof(component));
					}

					if (components.size() == 4) {
						ImVec4 loadedColor = ImVec4(components[0], components[1], components[2], components[3]);
						gui::settings::misc::checkboxColor = loadedColor;
					}

				}
				}
			}
		}
	}

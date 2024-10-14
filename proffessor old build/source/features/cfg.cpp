//#include <fstream>
//#include <stdexcept>
//#include <nlohmann/json.hpp>
//
//// Define a structure to hold GUI settings
//struct GuiSettings {
//    bool espEnabled;
//
//    GuiSettings() : espEnabled(false) {} // Initialize default values
//};
//
//// Function to save configuration to a file
//void SaveConfiguration(const GuiSettings& settings, const std::string& filename) {
//    // Open file for writing
//    std::ofstream file(filename);
//    if (!file.is_open()) {
//        throw std::runtime_error("Failed to open configuration file for writing");
//    }
//
//    // Serialize settings to JSON
//    nlohmann::json j;
//    j["espEnabled"] = settings.espEnabled;
//
//    // Write JSON to file
//    file << j;
//}
//
//// Function to load configuration from a file
//GuiSettings LoadConfiguration(const std::string& filename) {
//    // Open file for reading
//    std::ifstream file(filename);
//    if (!file.is_open()) {
//        throw std::runtime_error("Failed to open configuration file for reading");
//    }
//
//    // Parse JSON from file
//    nlohmann::json j;
//    file >> j;
//
//    // Create GuiSettings object from JSON
//    GuiSettings settings;
//    settings.espEnabled = j.value("espEnabled", false);
//
//    return settings;
//}
//
//// Function to apply settings to GUI
//void ApplySettings(const GuiSettings& settings) {
//    // Apply settings to GUI elements
//    // For example:
//    // gui::settings::esp::enable = settings.espEnabled;
//    // Apply more settings as needed...
//}
//
//// Example usage
//void gui::Setup() {
//    // Load configuration from file
//    GuiSettings settings = LoadConfiguration("cfg/gui_config.json");
//
//    // Apply settings to GUI
//    ApplySettings(settings);
//}
//
//void gui::Destroy() {
//    // Save current configuration to file
//    GuiSettings settings;
//    // Populate settings with current GUI state
//    // For example:
//    // settings.espEnabled = gui::settings::esp::enable;
//    // Populate more settings as needed...
//    SaveConfiguration(settings, "cfg/gui_config.json");
//}

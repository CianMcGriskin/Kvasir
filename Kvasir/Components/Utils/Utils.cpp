#include "Utils.h"

namespace fs = std::filesystem;
void Utils::ClearDirectory(const std::string& directoryPath) {
    try
    {
        // Check if the directory exists
        if (fs::exists(directoryPath) && fs::is_directory(directoryPath))
        {
            // Iterate and delete each item in the directory
            for (const auto& entry : fs::directory_iterator(directoryPath)){
                fs::remove_all(entry);
            }
            std::cout << "Directory cleared successfully." << std::endl;
        } else {
            std::cout << "Directory does not exist or is not a directory." << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error clearing directory: " << e.what() << std::endl;
    }
}
#include "Utils.h"

void Utils::ClearDirectory(const std::string& directoryPath) {
    try
    {
        // Check if the directory exists
        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath))
        {
            // Iterate and delete each item in the directory
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
            {
                std::filesystem::remove_all(entry);
            }
            std::cout << "Directory cleared successfully." << std::endl;
        } 
        else 
        {
            std::cout << "Directory does not exist or is not a directory." << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& e) 
    {
        std::cerr << "Error clearing directory: " << e.what() << std::endl;
    }
}
#ifndef KVASIR_UTILS_H
#define KVASIR_UTILS_H

#include <filesystem>
#include <iostream>

class Utils {
public:
    // Function used to delete the contents of a directory
    static void ClearDirectory(const std::string& directoryPath);
};


#endif //KVASIR_UTILS_H
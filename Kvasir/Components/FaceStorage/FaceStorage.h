#include <vector>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../WebserverCommunication/S3Communication.h"

#ifndef KVASIR_FACESTORAGE_H
#define KVASIR_FACESTORAGE_H

class FaceStorage {
public:
    // Constructor
    FaceStorage();

    // Retrieve face data based on index
    std::vector<float> RetrieveFace(int personIndex, int faceIndex);

    // Save face data to the faces array in the json file.
    void SaveFaceToJSON(int personIndex, const std::vector<float>& face);

    // Returns json data
    nlohmann::json GetJsonData();

private:
    // Variable used to store json data
    nlohmann::json jsonData;
};

#endif /* KVASIR_FACESTORAGE_H */
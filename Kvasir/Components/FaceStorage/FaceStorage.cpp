#include "FaceStorage.h"

FaceStorage::FaceStorage() {
    std::ifstream infile("../json/faces.json");
    if (infile.is_open()) {
        try {
            infile >> jsonData;
        }
        catch (const nlohmann::json::parse_error &e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    }
}

std::vector<float> FaceStorage::RetrieveFace(int personIndex, int faceIndex) {

    std::string key = std::to_string(personIndex);

    // Making sure the index exists before retrieval.
    if (jsonData.contains(key) && faceIndex >= 0 && faceIndex < jsonData[key]["faces"].size())
    {
        return jsonData[key]["faces"][faceIndex].get<std::vector<float>>();
    }
    else
    {
        std::cerr << "\nInvalid index or JSON object does not contain faces at the given index." << std::endl;
        return {};
    }
}

void FaceStorage::SaveFaceToJSON(int personIndex, const std::vector<float>& face) {
    std::string key = std::to_string(personIndex);

    if (!jsonData.contains(key) || jsonData[key].is_null()) {
        jsonData[key] = {{"faces", nlohmann::json::array()}};
    }

    jsonData[key]["faces"].push_back(face);

    // Write the updated JSON to the file
    std::ofstream outfile("../json/faces.json");
    outfile << std::setw(4) << jsonData << std::endl;
    outfile.close();
    S3Communication::uploadJsonFile("../json/faces.json");
}

nlohmann::json FaceStorage::GetJsonData() {
    return jsonData;
}

void FaceStorage::DeleteFace(int index) {
    if (!jsonData["faces"].empty() && index >= 0 && index < jsonData["faces"].size())
    {
        jsonData["faces"].erase(jsonData["faces"].begin() + index); // Remove the face at the given index
        writeDataToFile(); // Write the updated JSON data to the file
    }
    else
    {
        std::cerr << "Invalid index or JSON object does not contain faces at the given index.";
    }
}

void FaceStorage::writeDataToFile() {
    std::ofstream outfile("../json/faces.json");
    if (outfile.is_open())
    {
        outfile << std::setw(4) << jsonData << std::endl;
    }
    else
    {
        std::cerr << "Unable to open faces.json for writing.";
    }
}
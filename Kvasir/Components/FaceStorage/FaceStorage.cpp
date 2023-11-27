#include <iostream>
#include "FaceStorage.h"
#include <fstream>
#include <vector>

FaceStorage::FaceStorage() {
    std::ifstream infile("../faces.json");
    if (infile.is_open()) {
        infile >> jsonData;
    } else {
        std::cerr << "Unable to open faces.json";
    }
}

void FaceStorage::AddFace(const std::vector<double>& face)
{
    faces.emplace_back(face);
}

//std::vector<double> FaceStorage::RetrieveFace(int index) {
//    if(!faces.empty())
//    {
//        return faces[index];
//    }
//    else
//    {
//        std::cerr << "JSON object does not contain faces.";
//    }
//}

std::vector<double> FaceStorage::RetrieveFace(int index) {
    if (!jsonData["faces"].empty() && index >= 0 && index < jsonData["faces"].size())
    {
        std::cout << "Retrieving face at index " << index << std::endl;
        return jsonData["faces"][index].get<std::vector<double>>();
    } else {
        std::cerr << "\nInvalid index or JSON object does not contain faces at the given index." << std::endl;
        return std::vector<double>();
    }
}

void FaceStorage::SaveFaceToJSON(std::vector<double> face) {
    std::ifstream infile("../faces.json");

    if (infile.is_open())
    {
        infile >> jsonData;
    }

    // Append the face vector to the JSON array
    jsonData["faces"].push_back(face);

    // Write the updated JSON to the file
    std::ofstream outfile("../faces.json");
    outfile << std::setw(4) << jsonData << std::endl;
}

nlohmann::json FaceStorage::GetJsonData() {
    return jsonData;
}

void FaceStorage::DeleteFace(int index) {
    if (!jsonData["faces"].empty() && index >= 0 && index < jsonData["faces"].size()) {
        jsonData["faces"].erase(jsonData["faces"].begin() + index); // Remove the face at the given index
        writeDataToFile(); // Write the updated JSON data to the file
    } else {
        std::cerr << "Invalid index or JSON object does not contain faces at the given index.";
    }
}

void FaceStorage::writeDataToFile() {
    std::ofstream outfile("faces.json");
    if (outfile.is_open()) {
        outfile << std::setw(4) << jsonData << std::endl;
    } else {
        std::cerr << "Unable to open faces.json for writing.";
    }
}

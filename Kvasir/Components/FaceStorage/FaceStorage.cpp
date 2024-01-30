#include "FaceStorage.h"

FaceStorage::FaceStorage() {
    std::ifstream infile("../json/facesV2.json");
    if (infile.is_open())
    {
        try
        {
            // If the file exists but has nothing in it, initialise the json file.
            if (infile.peek() == std::ifstream::traits_type::eof())
            {
                jsonData = {{"faces", nlohmann::json::array()}};
                writeDataToFile();
            }
            else
            {
                infile >> jsonData;

                // Check if the JSON data is empty or null or if "faces" key is missing
                if (jsonData.empty() || !jsonData.contains("faces") || jsonData["faces"].empty())
                {
                    // Initialize jsonData with a default structure
                    jsonData = {{"faces", nlohmann::json::array()}};
                    writeDataToFile();
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error while parsing JSON: " << e.what() << std::endl;
        }
    } else
    {
        std::cerr << "Unable to open faces.json" << std::endl;
    }
}

std::vector<float> FaceStorage::RetrieveFace(int index) {
    // Making sure the index exists before retrieval.
    if (!jsonData["faces"].empty() && index >= 0 && index < jsonData["faces"].size())
    {
        return jsonData["faces"][index].get<std::vector<float>>();
    }
    else
    {
        std::cerr << "\nInvalid index or JSON object does not contain faces at the given index." << std::endl;
        return {};
    }
}

void FaceStorage::SaveFaceToJSON(const std::vector<float>& face) {
    std::ifstream infile("../json/faces.json");

    if (infile.is_open())
    {
        infile >> jsonData;
    }

    // Append the face vector to the JSON array
    jsonData["faces"].push_back(face);

    // Write the updated JSON to the file
    std::ofstream outfile("../json/faces.json");
    outfile << std::setw(4) << jsonData << std::endl;
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
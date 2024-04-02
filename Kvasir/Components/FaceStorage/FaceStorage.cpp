#include "FaceStorage.h"

// Constructor
FaceStorage::FaceStorage() {
    // Open the faces.json file for reading
    std::ifstream infile("../json/faces.json");

    // If file was successfully opened
    if (infile.is_open())
    {
        try
        {
            // Parse json file into jsonData
            infile >> jsonData;
        }
        catch (const nlohmann::json::parse_error &e)
        {
            // Catch and print any errors if encountered
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    }
}

// Method to retrieve a specific face embedding vector from the JSON storage based on person and face indices
std::vector<float> FaceStorage::RetrieveFace(int personIndex, int faceIndex) {
    // Convert the person index to a string to use as a key in the JSON object
    std::string key = std::to_string(personIndex);

    // Making sure the person and face index exists before retrieval.
    if (jsonData.contains(key) && faceIndex >= 0 && faceIndex < jsonData[key]["faces"].size())
    {
        // Retrieve and return the face embedding vector
        return jsonData[key]["faces"][faceIndex].get<std::vector<float>>();
    }
    else
    {
        // Report an error if the indices are invalid or the data doesn't exist
        std::cerr << "\nInvalid index or JSON object does not contain faces at the given index." << std::endl;
        return {};
    }
}

// Method to save a face embedding vector to the JSON storage for a specific person index
void FaceStorage::SaveFaceToJSON(int personIndex, const std::vector<float>& face) {
    // Convert the person index to a string to use as a key in the JSON object
    std::string key = std::to_string(personIndex);

    // Add the face embedding vector to the array of faces for the specified person in the JSON data
    jsonData[key]["faces"].push_back(face);

    // Write the updated JSON to the file
    std::ofstream outfile("../json/faces.json");

    if (outfile.is_open())
    {
        outfile << std::setw(4) << jsonData << std::endl;
        outfile.close();
    }

    // Upload the updated JSON file to AWS S3
    S3Communication::uploadFile("PeopleInformation.json", "../json/faces.json");
}

nlohmann::json FaceStorage::GetJsonData() {
    return jsonData;
}
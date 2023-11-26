#include <iostream>
#include "FaceStorage.h"

void FaceStorage::AddFace(const std::vector<double>& face)
{
    faces.emplace_back(face);
}

std::vector<double> FaceStorage::RetrieveFace(int index) {
    if(!faces.empty())
    {
        return faces[index];
    }
    else
    {
        std::cerr << "JSON object does not contain faces.";
    }
}

void FaceStorage::SaveFaceToJSON(std::vector<double> face) {

}

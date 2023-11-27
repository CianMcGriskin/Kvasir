#include <vector>
#include "nlohmann/json.hpp"

#ifndef KVASIR_FACESTORAGE_H
#define KVASIR_FACESTORAGE_H


class FaceStorage {
public:
    FaceStorage();
    void AddFace(const std::vector<double>& face);
    std::vector<double> RetrieveFace(int index);
    void SaveFaceToJSON(std::vector<double> face);
    nlohmann::json GetJsonData();
    void DeleteFace(int index);
    void writeDataToFile();
private:
    std::vector<std::vector<double>> faces;
    nlohmann::json jsonData;

};

#endif /* KVASIR_FACESTORAGE_H */
#include <vector>

#ifndef KVASIR_FACESTORAGE_H
#define KVASIR_FACESTORAGE_H


class FaceStorage {
public:
    FaceStorage();
    void AddFace(const std::vector<double>& face);
    std::vector<double> RetrieveFace(int index);
    static void SaveFaceToJSON(std::vector<double> face);

private:
    std::vector<std::vector<double>> faces;


};

#endif /* KVASIR_FACESTORAGE_H */
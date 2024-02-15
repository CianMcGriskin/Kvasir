#ifndef KVASIR_S3COMMUNICATION_H
#define KVASIR_S3COMMUNICATION_H

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <memory>
#include <fstream>
#include <filesystem>

class S3Communication {
public:
    static void initAws();
    static void readJsonFile(std::string filePath);
    static void shutdownAWS();
    static void uploadJsonFile(const std::string& filePath);
    static void uploadVideoSegment(const std::string& fileName);
private:
    static Aws::SDKOptions options;
    static std::shared_ptr<Aws::S3::S3Client> s3_client;

};


#endif //KVASIR_S3COMMUNICATION_H

#ifndef KVASIR_S3COMMUNICATION_H
#define KVASIR_S3COMMUNICATION_H

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/ListObjectsV2Result.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <memory>
#include <fstream>
#include <filesystem>

class S3Communication {
public:
    static void initAws();
    static void downloadFile(const std::string& key, const std::string& filePath);
    static void deleteFile(const std::string& key);
    static void uploadJsonFile(const std::string& filePath);
    static void uploadVideoSegment(const std::string& fileName);
    static std::vector<std::string> getFileNames(const std::string& folderName);
    static void shutdownAWS();
private:
    static Aws::SDKOptions options;
    static std::shared_ptr<Aws::S3::S3Client> s3_client;

};


#endif //KVASIR_S3COMMUNICATION_H

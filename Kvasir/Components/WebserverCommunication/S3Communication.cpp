#include "S3Communication.h"

Aws::SDKOptions S3Communication::options;
std::shared_ptr<Aws::S3::S3Client> S3Communication::s3_client;

void S3Communication::initAws() {
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = Aws::Region::EU_WEST_1;

    s3_client = Aws::MakeShared<Aws::S3::S3Client>("S3Client", clientConfig);
}

void S3Communication::downloadFile(const std::string& key, const std::string& filePath) {
    Aws::S3::Model::GetObjectRequest getObjectRequest;

    // Link to S3 file
    getObjectRequest.SetBucket("kvasir-storage");
    getObjectRequest.SetKey(key.c_str());

    auto getObjectOutcome = s3_client->GetObject(getObjectRequest);

    std::ofstream local_file(filePath, std::ios::binary);

    local_file << getObjectOutcome.GetResultWithOwnership().GetBody().rdbuf();
    local_file.close();

    std::cout << "Download completed: " << filePath << std::endl;
}

void S3Communication::uploadFile(const std::string& key, const std::string& localFilePath) {
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket("kvasir-storage");
    request.SetKey(key.c_str());

    std::shared_ptr<Aws::FStream> fileStream = Aws::MakeShared<Aws::FStream>("PutObjectAllocationTag", localFilePath.c_str(), std::ios_base::in | std::ios_base::binary);
    request.SetBody(fileStream);
    auto outcome = s3_client->PutObject(request);
}

void S3Communication::shutdownAWS(){
    Aws::ShutdownAPI(options);
}

void S3Communication::uploadVideoSegment(const std::string& fileName) {
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket("kvasir-storage");

    // Extract the base file name and prepend the folder name to it
    std::string baseFileName = fileName.substr(fileName.find_last_of('/') + 1);
    std::string key = "Videos/" + baseFileName;
    request.SetKey(key.c_str());

    // Open the file stream for reading the video file in binary mode
    auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream", fileName.c_str(), std::ios_base::in | std::ios_base::binary);
    request.SetBody(input_data);

    // Commenting out to avoid AWS charges :)
    //auto outcome = s3_client->PutObject(request);
}

std::vector<std::string> S3Communication::getFileNames(const std::string& folderName) {
    Aws::S3::Model::ListObjectsV2Request request;

    request.SetBucket("kvasir-storage");
    request.SetPrefix(folderName.c_str());


    auto outcome = s3_client->ListObjectsV2(request);
    std::vector<std::string> fileNames;

    int fileCount = outcome.GetResult().GetContents().size();

    if (fileCount > 1)
    {
        const auto& contents = outcome.GetResult().GetContents();
        for (size_t i = 1; i < contents.size(); ++i) {
            fileNames.push_back(contents[i].GetKey());
        }

        return fileNames;
    }
    else
    {
        // The folder is empty
        return fileNames;
    }
}

void S3Communication::deleteFile(const std::string &key) {
    Aws::S3::Model::DeleteObjectRequest request;

    request.SetBucket("kvasir-storage");
    request.SetKey(key);

    auto outcome = s3_client->DeleteObject(request);
}

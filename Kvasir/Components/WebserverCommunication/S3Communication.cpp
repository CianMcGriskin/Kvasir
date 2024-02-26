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

void S3Communication::readJsonFile(std::string filePath) {
    Aws::S3::Model::GetObjectRequest getObjectRequest;

    // Link to S3 file
    getObjectRequest.SetBucket("kvasir-storage");
    getObjectRequest.SetKey("PeopleInformation.json");

    auto getObjectOutcome = s3_client->GetObject(getObjectRequest);

    std::ofstream local_file(filePath, std::ios::binary);

    local_file << getObjectOutcome.GetResultWithOwnership().GetBody().rdbuf();
    local_file.close();

    std::cout << "Download completed: " << filePath << std::endl;
}

void S3Communication::uploadJsonFile(const std::string& filePath) {
    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket("kvasir-storage");
    request.SetKey("PeopleInformation.json");

    std::shared_ptr<Aws::FStream> fileStream = Aws::MakeShared<Aws::FStream>("PutObjectAllocationTag", filePath.c_str(), std::ios_base::in | std::ios_base::binary);
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

    // Execute the PutObject request to upload the file to S3

    // Commenting out to avoid AWS charges :)
    //auto outcome = s3_client->PutObject(request);

}

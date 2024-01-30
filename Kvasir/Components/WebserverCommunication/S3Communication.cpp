#include <fstream>
#include <filesystem>
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

void S3Communication::listBuckets()
{
    auto outcome = s3_client->ListBuckets();

    if (outcome.IsSuccess())
    {
        std::cout << "List of S3 Buckets:" << std::endl;
        const Aws::Vector<Aws::S3::Model::Bucket>& bucketList = outcome.GetResult().GetBuckets();

        for (const auto& bucket : bucketList) {
            std::cout << bucket.GetName() << std::endl;
        }
    } else {
        std::cerr << "Error listing buckets: " << outcome.GetError().GetMessage() << std::endl;
    }
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

void S3Communication::writeToJsonFile() {

}

void S3Communication::shutdownAWS(){
    Aws::ShutdownAPI(options);
}

#include <iostream>
#include <filesystem>
#include <iostream>

#include "Components/WebserverCommunication/S3Communication.h"
#include "Components/Model.h"
#include "Components/WebserverCommunication/StreamService.h"
#include "Components/FaceDetection/FaceDetection.h"
#include "Components/Utils/Utils.h"
#include "Components/MotionBasedRecorder/MotionBasedRecorder.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>

void capture();
void processFrameInBackground();
void pollForImages();
void notificationDispatcher(const std::shared_ptr<NotificationQueue>& queue);
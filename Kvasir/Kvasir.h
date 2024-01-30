#include <iostream>
#include <filesystem>

#include "Components/WebserverCommunication/S3Communication.h"
#include "Components/CameraControl.h"
#include "Components/Model.h"
#include "Components/WebserverCommunication/StreamService.h"
#include "Components/FaceDetection/FaceDetection.h"
#include "Components/Utils/Utils.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <iostream>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
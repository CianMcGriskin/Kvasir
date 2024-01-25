import argparse
import subprocess
import os

# Get the source directory
sourceFolder = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

# Function used to clone repos needed for this project
def clone_repository(repo_url, folder_name, sourceFolder, recursive=False):
    clone_path = os.path.join(sourceFolder, "Dependencies", folder_name)
    if not os.path.exists(clone_path):
        try:
            clone_cmd = ["git", "clone"]
            if recursive:
                clone_cmd.append("--recurse-submodules")
            clone_cmd.extend([repo_url, clone_path])
            subprocess.run(clone_cmd)
            print(f"Git repository cloned into '{clone_path}' successfully.")
        except subprocess.CalledProcessError as e:
            print(f"Error cloning the Git repository: {e}")
    else:
        print(f"Folder '{clone_path}' already exists. Skipping cloning.")

# Argument parser setup
parser = argparse.ArgumentParser(description='Clone specific repositories required for your use case.')
parser.add_argument('-t', '--tensorflow', action='store_true', help='Clone TensorFlow')
parser.add_argument('-o', '--opencv', action='store_true', help='Clone OpenCV')
parser.add_argument('-mj', '--mjpeg', action='store_true', help='Clone MJPEG Streamer')
parser.add_argument('-js', '--json', action='store_true', help='Clone JSON')
parser.add_argument('-aws', '--aws-sdk', action='store_true', help='Clone AWS SDK for C++')
parser.add_argument('-a', '--all', action='store_true', help='Clone All Required')
args = parser.parse_args()

# Define repository URLs and folder names
repositories = {
    "OpenCV": "https://github.com/opencv/opencv.git",
    "Tensorflow": "https://github.com/tensorflow/tensorflow.git",
    "MJPEG": "https://github.com/nadjieb/cpp-mjpeg-streamer.git",
    "json": "https://github.com/nlohmann/json.git",
    "aws-sdk-cpp": "https://github.com/aws/aws-sdk-cpp.git"
}

# Clone repositories to specify dependencies to clone
if args.tensorflow or args.all:
    clone_repository(repositories["Tensorflow"], "Tensorflow", sourceFolder)
if args.opencv or args.all:
    clone_repository(repositories["OpenCV"], "OpenCV", sourceFolder)
if args.mjpeg or args.all:
    clone_repository(repositories["MJPEG"], "cpp-mjpeg-streamer", sourceFolder)
if args.json or args.all:
    clone_repository(repositories["json"], "json", sourceFolder)
if args.aws_sdk or args.all:
    clone_repository(repositories["aws-sdk-cpp"], "aws-sdk-cpp", sourceFolder, recursive=True)

# Display available commands if no arguments provided
if not (args.tensorflow or args.opencv or args.mjpeg or args.json or args.aws_sdk or args.all):
    parser.print_help()

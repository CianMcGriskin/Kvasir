import argparse
import subprocess
import os

# Get the source directory
sourceFolder = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

# Function used to clone repos needed for this project
def clone_repository(repo_url, folder_name, sourceFolder):
    clone_path = os.path.join(sourceFolder, "Dependencies", folder_name)
    if not os.path.exists(clone_path):
        try:
            subprocess.run(["git", "clone", repo_url, clone_path])
            print(f"Git repository cloned into '{clone_path}' successfully.")
        except subprocess.CalledProcessError as e:
            print(f"Error cloning the Git repository: {e}")
    else:
        print(f"Folder '{clone_path}' already exists. Skipping cloning.")

# Need to make it possible to build project with seperate dependencies as it is very large altogether and very long to build.
parser = argparse.ArgumentParser(description='Clone specific repositories required for your use case.')
parser.add_argument('-t', '--tensorflow', action='store_true', help='Clone TensorFlow')
parser.add_argument('-o', '--opencv', action='store_true', help='Clone OpenCV') 
args = parser.parse_args()

# Define repository URLs and folder names
repositories = {
    "OpenCV": "https://github.com/opencv/opencv.git",
    "Tensorflow": "https://github.com/tensorflow/tensorflow.git"
}

# Clone repositories to specify dependencies to clone
if args.tensorflow:
    clone_repository(repositories["Tensorflow"], "Tensorflow", sourceFolder)
if args.opencv:
    clone_repository(repositories["OpenCV"], "OpenCV", sourceFolder)

# Display available commands if no arguments provided
if not (args.tensorflow or args.opencv):
    parser.print_help()

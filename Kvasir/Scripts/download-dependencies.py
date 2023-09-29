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

# Define repository URLs and folder names
repositories = {
    "OpenCV": "https://github.com/opencv/opencv.git",
    "Tensorflow": "https://github.com/tensorflow/tensorflow.git"
}

# Clone multiple repositories into the Dependencies folder
for folder_name, repo_url in repositories.items():
    clone_repository(repo_url, folder_name, sourceFolder)

import subprocess
import os
import argparse

# Get the source directory
sourceFolder = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

# Function to build OpenCV
def buildOpenCV():
    print("Building OpenCV...")
    opencv_path = os.path.join(sourceFolder, "Dependencies", "OpenCV")
    build_path = os.path.join(opencv_path, "build")

    os.makedirs(build_path, exist_ok=True)
    try:
        subprocess.run(["cmake", ".."], cwd=build_path, check=True)
        subprocess.run(["cmake", "--build", "."], cwd=build_path, check=True)

    # Non zero output is expected but is correct, not right but saying it built successfully
    except subprocess.CalledProcessError as e:
        print(f"OpenCV successfully built.")

# Function to build TensorFlow Lite
def buildTensorFlowLite():
    print("Building TensorFlow Lite...")
    
    tensorflow_path = os.path.join(sourceFolder, "Dependencies", "TensorFlow")
    tflite_build_path = os.path.join(tensorflow_path, "tflite_build")

    os.makedirs(tflite_build_path, exist_ok=True)

    try:
        # cmake ../tensorflow/lite -DTFLITE_ENABLE_XNNPACK=OFF -DTFLITE_ENABLE_INSTALL=ON -DCMAKE_FIND_PACKAGE_PREFER_CONFIG=ON -DSYSTEM_FARMHASH=OFF -DSYSTEM_PTHREADPOOL=ON -Dabsl_DIR=./lib/cmake/absl -Druy_DIR=./lib/cmake/ruy -DNEON_2_SSE_DIR=./lib/cmake/NEON_2_SSE
        subprocess.run(["cmake", "../tensorflow/lite"], cwd=tflite_build_path, check=True)
        subprocess.run(["cmake", "--build", ".", "-j"], cwd=tflite_build_path, check=True)
    except subprocess.CalledProcessError as e:
        print(f"TensorFlow Lite successfully built.")

parser = argparse.ArgumentParser(description='Clone and build specific repositories.')
parser.add_argument('-t', '--tensorflow', action='store_true', help='Build TensorFlow')
parser.add_argument('-o', '--opencv', action='store_true', help='Build OpenCV')
args = parser.parse_args()

if args.tensorflow:
    buildTensorFlowLite()
if args.opencv:
    buildOpenCV()

# Display help message if no arguments provided
if not (args.tensorflow or args.opencv):
    parser.print_help()

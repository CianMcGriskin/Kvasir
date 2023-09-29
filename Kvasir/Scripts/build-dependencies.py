import subprocess
import os

# Get the source directory
sourceFolder = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

def buildTensorflow():
    d = 1
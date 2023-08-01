import os
import platform
import subprocess

def git_pull_gerrit(output_dir):
    try:
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        # Change the current working directory to the parent directory
        os.chdir(output_dir)

        # Run 'git pull' to fetch and merge changes from the default remote and branch
        subprocess_result = subprocess.run(["git", "pull"], capture_output=True, text=True)
        print(subprocess_result.stdout)
        print(subprocess_result.stderr)
        print("Changes pulled successfully.")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    # Set the output directory to the parent directory of the script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.abspath(os.path.join(script_dir, ".."))

    # Check the platform to set the correct path separator
    if platform.system() == "Windows":
        output_dir = output_dir.replace(os.sep, "\\")

    git_pull_gerrit(output_dir)

import boto3
import json

# Initialize a boto3 client
s3 = boto3.client('s3')

# Specify your bucket name and file details
bucket_name = 'kvasir-storage'
file_path = 'C:\\Users\\Maciej\\Desktop\\ANPR\\Kvasir\\vehicle_data.json'  # Path to your local file
s3_file_path = 'Vehicle-Data/yourfile.json'  # Desired path in S3 bucket

# Load your JSON data
with open(file_path, 'r') as file:
    json_data = json.load(file)

# Convert your data back to a string to upload it as a file
json_string = json.dumps(json_data)

# Upload the file
s3.put_object(Bucket=bucket_name, Key=s3_file_path, Body=json_string)

print(f"File {file_path} uploaded to {bucket_name}/{s3_file_path}")

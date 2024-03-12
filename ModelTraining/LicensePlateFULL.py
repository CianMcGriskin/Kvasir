import pandas as pd
import json
import os
import requests
import boto3

# Assuming API URL and key are set as environment variables for better security
api_url = os.getenv("API_URL", "https://driver-vehicle-licensing.api.gov.uk/vehicle-enquiry/v1/vehicles")
api_key = os.getenv("API_KEY", "lkGLOnnrAD8WD3qMa5eVY241qQzP6adrhqyTsPO8")
headers = {'x-api-key': api_key, 'Content-Type': 'application/json'}

def parse_csv(csv_path):
    df = pd.read_csv(csv_path)
    # Select the record with the highest License_Plate_Conf_Score for each Car_ID
    highest_confidence_records = df.loc[df.groupby('Car_ID')['License_Plate_Conf_Score'].idxmax()]
    car_data = highest_confidence_records[['Car_ID', 'license_number']].to_dict(orient='records')
    return car_data


def filter_json_data(json_data):
    root_keys_to_remove = ["filename", "timestamp", "camera_id", "usage", "processing_time"]
    for key in root_keys_to_remove:
        json_data.pop(key, None)

    for result in json_data.get("results", []):
        keys_to_remove = ["box", "region", "candidates", "dscore"]
        for key in keys_to_remove:
            result.pop(key, None)

        if "model_make" in result:
            result["model_make"] = result["model_make"][0]
        if "color" in result:
            result["color"] = result["color"][0]
        if "vehicle" in result and "box" in result["vehicle"]:
            del result["vehicle"]["box"]
        if "orientation" in result:
            result["orientation"] = result["orientation"][0]

    return json_data

def call_plate_recognition_api(image_path):
    upload_url = "http://localhost:8080/v1/plate-reader/"
    with open(image_path, 'rb') as img_file:
        files = {'upload': img_file}
        response = requests.post(upload_url, files=files)

    if response.status_code == 200:
        print("Image uploaded successfully.")
        json_response = response.json()
        filtered_json = filter_json_data(json_response)
        return filtered_json
    else:
        print(f"Failed to upload image. Status code: {response.status_code}")
        return None

def call_license_plate_data_api(license_plate):
    payload = {'registrationNumber': license_plate}
    response = requests.post(api_url, headers=headers, json=payload)

    if response.status_code == 200:
        content = response.json()
        filtered_response = {
            "registrationNumber": content.get("registrationNumber"),
            "taxStatus": content.get("taxStatus"),
            "taxDueDate": content.get("taxDueDate"),
            "motStatus": content.get("motStatus"),
            "make": content.get("make"),
            "colour": content.get("colour")
        }
        return {"RegistrationNumber": license_plate, "Response": filtered_response}
    else:
        return {"RegistrationNumber": license_plate, "StatusCode": response.status_code, "Response": "Error or rate limit exceeded"}

def save_json_to_file(json_data, file_path):
    with open(file_path, 'w') as file:
        json.dump(json_data, file, indent=4)

def upload_file_to_s3(file_path, bucket_name, s3_file_path):
    # Initialize a boto3 client
    s3 = boto3.client('s3')

    # Load your JSON data
    with open(file_path, 'r') as file:
        json_data = json.load(file)

    # Convert your data back to a string to upload it as a file
    json_string = json.dumps(json_data)

    # Upload the file
    s3.put_object(Bucket=bucket_name, Key=s3_file_path, Body=json_string)
    print(f"File {file_path} uploaded to {bucket_name}/{s3_file_path}")

def main():
    csv_path = "C:\\Users\\User\\Desktop\\Object-Training-main\\Results_interpolated.csv"
    car_data = parse_csv(csv_path)
    images_base_directory = "C:\\Users\\User\\Desktop\\Object-Training-main\\car_images\\"

    combined_data = {}

    for car in car_data:
        # Adjusted to include decimal point in folder and file names
        image_folder_path = f"{images_base_directory}car_{car['Car_ID']}.0\\"  # Add .0 after car['Car_ID']
        image_path = f"{image_folder_path}car_{car['Car_ID']}.0.jpg"  # Add .0 before .jpg
        recognition_data = call_plate_recognition_api(image_path)

        license_plate = car['license_number']
        vehicle_data = call_license_plate_data_api(license_plate)

        combined_data[f"Car_ID_{car['Car_ID']}"] = {
            "RecognitionData": recognition_data,
            "VehicleData": vehicle_data
        }

    # Continue with the rest of the script as before...


    # Save the combined data for all cars to a JSON file
    json_file_path = "combined_vehicle_data.json"
    save_json_to_file(combined_data, json_file_path)
    print(f"Combined vehicle data saved to {json_file_path}")

    # Specify your bucket name and file details
    bucket_name = 'kvasir-storage'
    local_file_path = 'combined_vehicle_data.json'  # This should be the path where your JSON file is saved
    s3_file_path = 'Vehicle-Data/vehicle_data.json'  # Desired path in S3 bucket

    # After saving JSON to file, upload it to S3
    upload_file_to_s3(local_file_path, bucket_name, s3_file_path)

if __name__ == "__main__":
    main()

import requests
import json


# Function to filter the JSON data
def filter_json_data(json_data):
    # Remove specified keys from the root of the JSON data
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


# Assume "car.jpg" is in the specified folder
image_path = 'C:\\Users\\Maciej\\Downloads\\car.jpg'

# Upload the image to the plate recognition server
upload_url = "http://localhost:8080/v1/plate-reader/"
with open(image_path, 'rb') as img_file:  # Use context manager to ensure file is closed
    files = {'upload': img_file}
    response = requests.post(upload_url, files=files)

if response.status_code == 200:
    print("Image uploaded successfully.")
    json_response = response.json()

    # Filter the JSON data
    filtered_json = filter_json_data(json_response)

    # Save the filtered JSON data to a file
    with open('filtered_response.json', 'w') as file:
        json.dump(filtered_json, file, indent=4)

    print("Filtered JSON saved to 'filtered_response.json'.")
else:
    print("Failed to upload image. Status code:", response.status_code)

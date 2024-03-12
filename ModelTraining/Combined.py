import requests
import json

# Function to perform plate recognition
def recognize_plate(filepath, regions, token):
    with open(filepath, 'rb') as fp:
        response = requests.post(
            'https://api.platerecognizer.com/v1/plate-reader/',
            data=dict(regions=regions, detection_mode="vehicle"),
            files=dict(upload=fp),
            headers={'Authorization': f'Token {token}'})
    return response.json()

# Function to get vehicle details
def get_vehicle_details(registration_number, api_key):
    api_url = "https://driver-vehicle-licensing.api.gov.uk/vehicle-enquiry/v1/vehicles"
    headers = {'x-api-key': api_key, 'Content-Type': 'application/json'}
    payload = {'registrationNumber': registration_number}
    response = requests.post(api_url, headers=headers, json=payload)
    return response

# Your tokens and file paths
plate_recognizer_token = '2238414e55e8079e4838ccd97c837f0190cd0f5d'
dvla_api_key = 'lkGLOnnrAD8WD3qMa5eVY241qQzP6adrhqyTsPO8'
image_path = 'C:\\Users\\Maciej\\Downloads\\car.jpg'

regions = ["gb"]  # Change to your regions
registration_numbers = []  # We'll fill this with detected registration numbers

# Step 1: Recognize plate
plate_recognition_result = recognize_plate(image_path, regions, plate_recognizer_token)

# Assuming you extract the plate number(s) correctly from the result
# For simplicity, this is a placeholder for the logic that extracts plate numbers
# Add actual logic to parse and add plate numbers to registration_numbers
for result in plate_recognition_result.get('results', []):
    plate = result.get('plate')
    registration_numbers.append(plate)

# Step 2: Get vehicle details for each recognized plate
vehicle_details = []
for reg_number in registration_numbers:
    response = get_vehicle_details(reg_number, dvla_api_key)
    if response.status_code == 200:
        detail = {
            "RegistrationNumber": reg_number,
            "StatusCode": response.status_code,
            "Response": response.json()
        }
    else:
        detail = {
            "RegistrationNumber": reg_number,
            "StatusCode": response.status_code,
            "Response": "Error or rate limit exceeded"
        }
    vehicle_details.append(detail)

# Step 3: Combine and save the data
combined_data = {
    "PlateRecognition": plate_recognition_result,
    "VehicleDetails": vehicle_details
}

# Specify the file path where you want to save the combined JSON data
combined_file_path = 'combined_vehicle_data.json'

# Save the combined results to a JSON file
with open(combined_file_path, 'w') as file:
    json.dump(combined_data, file, indent=4)

print(f"Combined data saved to {combined_file_path}.")



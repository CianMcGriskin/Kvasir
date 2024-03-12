import requests
import json

# Replace these variables as necessary
api_url = "https://driver-vehicle-licensing.api.gov.uk/vehicle-enquiry/v1/vehicles"
api_key = "lkGLOnnrAD8WD3qMa5eVY241qQzP6adrhqyTsPO8"
registration_numbers = ["kf07lmj", "ERZ3571", "MK62FLB"]  # Example registration numbers
headers = {'x-api-key': api_key, 'Content-Type': 'application/json'}

def send_request(registration_number):
    payload = {'registrationNumber': registration_number}
    response = requests.post(api_url, headers=headers, json=payload)
    return response

results = []
for reg_number in registration_numbers:
    response = send_request(reg_number)
    if response.status_code == 200:
        result = {
            "RegistrationNumber": reg_number,
            "StatusCode": response.status_code,
            "Response": response.json()
        }
    else:
        result = {
            "RegistrationNumber": reg_number,
            "StatusCode": response.status_code,
            "Response": "Error or rate limit exceeded"
        }
    results.append(result)

# Specify the file path where you want to save the JSON data
file_path = 'vehicle_data.json'  # Update this path as needed

# Save the results to a JSON file
with open(file_path, 'w') as file:
    json.dump(results, file, indent=4)

print(f"Data saved to {file_path}.")


#"GL59NKC", "ERZ3571"
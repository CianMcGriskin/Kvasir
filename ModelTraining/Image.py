import cv2
import imutils
import pytesseract
from cloudmersive_image_api_client import RecognizeApi, ApiClient
from cloudmersive_image_api_client.rest import ApiException

# Configure Cloudmersive API Key
configuration = ApiClient().configuration
configuration.api_key['Apikey'] = '2819aaae-abc4-4631-80a9-f442facbb808'  # Replace 'YOUR_API_KEY' with your actual Cloudmersive API key

# Function to preprocess the image for license plate detection
def preprocess_image(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray = cv2.bilateralFilter(gray, 11, 17, 17)
    edged = cv2.Canny(gray, 30, 200)
    return edged

# Function to extract the license plate
def extract_license_plate(image):
    img_copy = image.copy()
    contours = cv2.findContours(preprocess_image(img_copy), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours = imutils.grab_contours(contours)
    contours = sorted(contours, key=cv2.contourArea, reverse=True)[:10]
    plate_region = None

    for cnt in contours:
        peri = cv2.arcLength(cnt, True)
        approx = cv2.approxPolyDP(cnt, 0.02 * peri, True)
        if len(approx) == 4:
            x, y, w, h = cv2.boundingRect(approx)
            plate_region = image[y:y+h, x:x+w]
            break

    if plate_region is not None:
        plate_text = pytesseract.image_to_string(plate_region, config='--psm 7')
        return plate_text.strip()
    else:
        return "License plate not detected"

# Function to get vehicle make and model
def get_vehicle_info(image_path):
    with ApiClient(configuration) as api_client:
        api_instance = RecognizeApi(api_client)
        image_file = open(image_path, 'rb')

        try:
            api_response = api_instance.recognize_vehicle_make_model(image_file)
            image_file.close()  # Ensure the file is closed after use
            if api_response.successful:
                return api_response.best_outcome.make, api_response.best_outcome.model
            else:
                return "Not Detected", "Not Detected"
        except ApiException as e:
            print("Exception when calling RecognizeApi->recognize_vehicle_make_model: %s\n" % e)
            return "API Error", "API Error"

# --- Main Execution ---
image_path = 'C:\\Users\\Maciej\\Downloads\\car.jpg'  # Update to your image path
image = cv2.imread(image_path)

# Extract license plate
license_plate = extract_license_plate(image)
print("License Plate:", license_plate)

# Get vehicle information if license plate was detected
if license_plate != "License plate not detected":
    make, model = get_vehicle_info(image_path)
    print("Vehicle Make:", make)
    print("Vehicle Model:", model)
else:
    print("License plate not detected, skipping vehicle info retrieval.")

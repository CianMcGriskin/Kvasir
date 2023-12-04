import easyocr
import cv2
import numpy as np

# Initialize the OCR reader
reader = easyocr.Reader(['en'], gpu=True)

#TODO WRITE CSV OUTPUT

#TODO format license plate output

def preprocess_image(image):

    #Applying processing efects to make the license plate more readable
    #In progress for more tweaks if needed

    # Convert to grayscale
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Apply Gaussian blur
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)

    # Use adaptive thresholding
    thresh = cv2.adaptiveThreshold(blurred, 255,
                                   cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                   cv2.THRESH_BINARY_INV, 11, 2)

    # Morphological operations
    kernel = np.ones((3, 3), np.uint8)
    morph = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, kernel)
    morph = cv2.morphologyEx(morph, cv2.MORPH_OPEN, kernel)

    return morph


def read_license_plate(license_plate_crop):

    #Read license plate from cropped image
    detections = reader.readtext(license_plate_crop)

    for detection in detections:
        bbox, text, score = detection

        text = text.upper().replace(' ', '')

    #todo ADD license plate formating
    return None, None


##Requires more testing idk if this help or makes things worse xd

def upscale_image(image, scale_factor=2.0):

   #Upsacle image for better clarity
    height, width = image.shape[:2]
    new_height, new_width = int(height * scale_factor), int(width * scale_factor)
    upscaled_image = cv2.resize(image, (new_width, new_height), interpolation=cv2.INTER_LINEAR)
    return upscaled_image


def get_car(license_plate, vehicle_track_ids):

    # extract license plate co-ordinated
    x1, y1, x2, y2, score, class_id = license_plate

    # Initialize a flag to indicate if a matching vehicle is found
    foundIt = False

    # Iterate through each tracked vehicle
    for j in range(len(vehicle_track_ids)):
        # Extract coordinates and id of the  vehicle
        xcar1, ycar1, xcar2, ycar2, car_id = vehicle_track_ids[j]

        # Check if the license plate is within the vehicle's bounding box
        if x1 > xcar1 and y1 > ycar1 and x2 < xcar2 and y2 < ycar2:
            # If the license plate is inside the vehicle's bounding box, mark as found
            car_indx = j
            foundIt = True
            break

    # Return the associated vehicle's information if found
    if foundIt:
        return vehicle_track_ids[car_indx]

    # Return a tuple of -1s if no associated vehicle is found
    return -1, -1, -1, -1, -1

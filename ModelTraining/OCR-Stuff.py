import os
from PIL import Image
import easyocr

# Define the path to the folder containing the images
folder_path = 'C:\\Users\\User\\Desktop\\Object-Training-main\\car_images\\Test'

# Define the actual license plates for comparison
actual_license_plates = ["AP05JEO", "NA13NRU", "MW51VSU", "GX15OGJ", "KH05ZZK",
                         "FJ14ZHY", "LM13VCV", "WR02FKD", "EY61NBG"]


# Function to clean and standardize OCR results
def clean_text(text):
    return ''.join(text.split()).upper()


# Create an EasyOCR reader instance
reader = easyocr.Reader(['en'])

# Dictionary to hold the OCR results
ocr_results = {}

# Iterate over each image in the folder
for file_name in os.listdir(folder_path):
    if file_name.endswith('.jpg'):  # Assuming the images are in JPEG format
        print(f"Processing {file_name}...")
        image_path = os.path.join(folder_path, file_name)

        # Perform OCR using EasyOCR
        results = reader.readtext(image_path)
        # Join the detected texts
        ocr_text = ' '.join([result[1] for result in results])
        ocr_cleaned = clean_text(ocr_text)

        # Save the cleaned OCR result
        ocr_results[file_name] = ocr_cleaned

# Now, compare the OCR results with the actual license plates
for plate in actual_license_plates:
    # Assume the image file names without extension are the same as the actual license plates
    file_name = f"{plate}.jpg"
    ocr_result = ocr_results.get(file_name, "No OCR result")

    # Compare ignoring case and whitespace
    match = clean_text(plate) == ocr_result
    print(f"File: {file_name}, OCR: {ocr_result}, Actual: {plate}, Match: {match}")

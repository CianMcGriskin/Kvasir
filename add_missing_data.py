import csv
import numpy as np
from scipy.interpolate import interp1d
import os

def interpolate_data(data):
    # Prepare data for interpolation
    frame_numbers = np.array([int(row['Frame_Number']) for row in data])
    car_ids = np.array([int(float(row['Car_ID'])) for row in data])
    license_plate_scores = np.array([float(row['License_Plate_Conf_Score']) for row in data])
    license_numbers = [row['license_number'] for row in data]
    license_number_scores = np.array([float(row['license_number_score']) for row in data])

    # Unique identifiers for cars
    unique_car_ids = np.unique(car_ids)

    interpolated_data = []
    for car_id in unique_car_ids:
        # Filter data for each car
        indices = np.where(car_ids == car_id)
        car_frames = frame_numbers[indices]
        car_license_scores = license_plate_scores[indices]
        car_license_numbers = np.array(license_numbers)[indices]
        car_license_number_scores = license_number_scores[indices]

        # Sort by frame number
        sorted_indices = np.argsort(car_frames)
        car_frames = car_frames[sorted_indices]
        car_license_scores = car_license_scores[sorted_indices]
        car_license_numbers = car_license_numbers[sorted_indices]
        car_license_number_scores = car_license_number_scores[sorted_indices]

        # Interpolate for missing frames
        full_frame_range = np.arange(car_frames[0], car_frames[-1] + 1)
        interp_license_scores = np.interp(full_frame_range, car_frames, car_license_scores)
        interp_license_number_scores = np.interp(full_frame_range, car_frames, car_license_number_scores)

        # Create interpolated entries
        for i, frame_number in enumerate(full_frame_range):
            interpolated_data.append({
                'Frame_Number': str(frame_number),
                'Car_ID': str(car_id),
                'License_Plate_Conf_Score': str(interp_license_scores[i]),
                'license_number': car_license_numbers[0],  # Assuming static license number
                'license_number_score': str(interp_license_number_scores[i])
            })

    return interpolated_data

# Load CSV file
csv_path = 'Results.csv'  # Update with your actual file path
with open(csv_path, 'r') as file:
    reader = csv.DictReader(file)
    data = list(reader)

# Interpolate missing data
interpolated_data = interpolate_data(data)

# Write to a new CSV file
output_csv_path = 'Results_interpolated.csv'  # Update with your desired output path
with open(output_csv_path, 'w', newline='') as file:
    writer = csv.DictWriter(file, fieldnames=interpolated_data[0].keys())
    writer.writeheader()
    writer.writerows(interpolated_data)

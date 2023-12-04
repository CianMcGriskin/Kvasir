from ultralytics import YOLO
from sort.sort import *
from Utilities import *

results = {}
mot_tracker = Sort()

# Create a directory to store car and license plate images
output_directory = './car_images'
if not os.path.exists(output_directory):
    os.makedirs(output_directory)

# Load models
coco_model = YOLO('yolov8n.pt')
license_plate_detector = YOLO('./models/best.pt')

# Load video
cap = cv2.VideoCapture('./sample1.mp4')

vehicles = [2, 3, 5, 7]

# Read frames
# Loop through the video frames using OpenCV, processing them one by one:
frame_nmr = -1
ret = True
while ret:
    frame_nmr += 1
    ret, frame = cap.read()
    if ret:
        results[frame_nmr] = {}
        # Detect vehicles
        detections = coco_model(frame)[0]
        detections_ = []
        #detections.save_crop('outputs')
        for detection in detections.boxes.data.tolist():
            x1, y1, x2, y2, score, class_id = detection
            if int(class_id) in vehicles:
                detections_.append([x1, y1, x2, y2, score])
                #print(detections_)

        # Check if there are detections in this frame
        if len(detections_) > 0:
            track_ids = mot_tracker.update(np.asarray(detections_))
            #print(track_ids)
            # Detect license plates and save images
            license_plates = license_plate_detector(frame)[0]
            #license_plates.save_crop('outputs')
            for license_plate in license_plates.boxes.data.tolist():
                x1, y1, x2, y2, score, class_id = license_plate


                # Assign license plate to car
                xcar1, ycar1, xcar2, ycar2, car_id = get_car(license_plate, track_ids)

                if car_id != -1:
                    # Crop license plate
                    license_plate_crop = frame[int(y1):int(y2), int(x1): int(x2), :]

                    # Upscale the license plate crop
                    license_plate_crop = upscale_image(license_plate_crop)
                    #license_plate_crop.save_crop('outputs')
                    # Process license plate
                    license_plate_crop_thresh = preprocess_image(license_plate_crop)

                    # Read license plate number
                    license_plate_text, license_plate_text_score = read_license_plate(license_plate_crop_thresh)

                    if license_plate_text is not None:
                        results[frame_nmr][car_id] = {'car': {'bbox': [xcar1, ycar1, xcar2, ycar2]},
                                                    'license_plate': {'bbox': [x1, y1, x2, y2],
                                                                      'text': license_plate_text,
                                                                      'bbox_score': score,
                                                                      'text_score': license_plate_text_score}}

                        # Create a directory for each car if it doesn't already exist
                        car_directory = os.path.join(output_directory, f'car_{car_id}')
                        if not os.path.exists(car_directory):
                            os.makedirs(car_directory)

                        # Save the license plate image with the car_id as the filename
                        license_plate_image_filename = os.path.join(car_directory, f'license_plate_{car_id}.jpg')
                        cv2.imwrite(license_plate_image_filename, license_plate_crop)

                        # Save the whole car image with the car_id as the filename
                        car_image_filename = os.path.join(car_directory, f'car_{car_id}.jpg')
                        car_image = frame[int(ycar1):int(ycar2), int(xcar1):int(xcar2), :]
                        cv2.imwrite(car_image_filename, car_image)

# Write results
write_csv(results, './test.csv')


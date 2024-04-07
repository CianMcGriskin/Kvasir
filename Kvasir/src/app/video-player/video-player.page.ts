import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { S3Client, GetObjectCommand, PutObjectCommand, PutObjectCommandInput } from "@aws-sdk/client-s3";
import { IonModal, NavParams } from '@ionic/angular';
import { environment } from '../../environments/environment';
import * as faceapi from 'face-api.js';
import { ModalController } from '@ionic/angular';
import { SwiperContainer } from 'swiper/element';

@Component({
  selector: 'app-video-player',
  templateUrl: './video-player.page.html',
  styleUrls: ['./video-player.page.scss'],
})
export class VideoPlayerPage implements OnInit {

  videos: any[];
  s3Client: S3Client;
  selectedVideoUrl: String | undefined;
  imageChangedEvent: any = '';
  detectedFaces: string[] = [];

  //Form variables
  userName: string = '';
  userReason: string = '';
  croppedImage: File | null = null;
  croppedImageUrl: string | null = null;
  tempCroppedImage: Blob | null = null;


  @ViewChild('videoPlayer') videoPlayer: ElementRef | undefined;
  @ViewChild('slides') slides: SwiperContainer | undefined;
  @ViewChild('croppingModal') croppingModal: IonModal | undefined;
  @ViewChild('addingPersonModal') addingPersonModal: IonModal | undefined;


  constructor(private modalController: ModalController, private navParams: NavParams) {
    this.videos = [];

    this.s3Client = new S3Client({
      region: environment.REGION,
      credentials: {
        accessKeyId: environment.AWS_ACCESS_KEY_ID,
        secretAccessKey: environment.AWS_SECRET_ACCESS_KEY
      }
    });
  }
  async ngOnInit() {
    //Get the Modal params
    if (this.navParams.get('selectedVideoUrl')) {
      this.selectedVideoUrl = this.navParams.get('selectedVideoUrl')
    }
  }

  /**
   * Detects faces from the current frame
   */
  async detectFaces() {
    //Reset the values when detecting faces again on same video
    this.croppedImage = null;
    this.detectedFaces = [];

    const videoElement = this.videoPlayer?.nativeElement;
    const canvas = document.createElement('canvas');
    const context = canvas.getContext('2d');

    canvas.width = videoElement.videoWidth;
    canvas.height = videoElement.videoHeight;

    //Draw the current frame of the video onto the canvas
    context?.drawImage(videoElement, 0, 0, canvas.width, canvas.height);
    const croppedImageData = canvas.toDataURL('image/png');

    try {
      //Generate cropped image from the image URL
      const croppedImage = new Image();
      croppedImage.src = croppedImageData;

      const detections = await faceapi.detectAllFaces(croppedImage, new faceapi.SsdMobilenetv1Options()).withFaceLandmarks().withFaceDescriptors();

      //Draw the original image onto the canvas
      context?.drawImage(croppedImage, 0, 0, canvas.width, canvas.height);

      //Draws boxes around faces (Debugging):
      //faceapi.draw.drawDetections(canvas, faceapi.resizeResults(detections, { width: img.width, height: img.height }));

      //display all detected faces
      for (const detection of detections) {
        const faceBox = detection.detection.box;

        //Draw the cropped face region onto the new canvas
        canvas.width = faceBox.width;
        canvas.height = faceBox.height;
        context?.drawImage(
          croppedImage,
          faceBox.x,
          faceBox.y,
          faceBox.width,
          faceBox.height,
          0,
          0,
          faceBox.width,
          faceBox.height
        );

        const croppedFaceImageData = canvas.toDataURL('image/png');
        this.detectedFaces.push(croppedFaceImageData);
      }
      console.log('faces detected, faces detected: ', detections.length)
    } catch (error) {
      console.error('Error in detectFaces:', error);
    }
  }

  /**
   * Captures the current frame of the video and saves it
   */
  captureFrame(): void {
    if (this.videoPlayer) {
      const video: HTMLVideoElement = this.videoPlayer.nativeElement;
      const canvas = document.createElement('canvas');
      canvas.width = video.videoWidth;
      canvas.height = video.videoHeight;
      const ctx = canvas.getContext('2d');
      if (ctx) {
        ctx.drawImage(video, 0, 0, canvas.width, canvas.height);
        this.imageChangedEvent = { target: { files: [this.dataURLtoFile(canvas.toDataURL('image/png'), 'frame.png')] } };
      }
    }
  }

  /**
   * Convert data URL to a File object
   */
  dataURLtoFile(dataurl: any, filename: any) {
    let arr = dataurl.split(','), mime = arr[0].match(/:(.*?);/)[1],
      bstr = atob(arr[1]), n = bstr.length, u8arr = new Uint8Array(n);
    while (n--) {
      u8arr[n] = bstr.charCodeAt(n);
    }
    return new File([u8arr], filename, { type: mime });
  }

  /**
   * Save cropped image after user moves cropping dimensions
   */
  imageCropped(event: any): void {
    this.tempCroppedImage = event.blob;
  }

  /**
   * Confirm the crop of the image and save the image
   */
  confirmCrop() {
    let blob: Blob;
    if (this.tempCroppedImage) {
      blob = this.tempCroppedImage;

      //Create a File object from the Blob
      this.croppedImage = new File([blob], "croppedImage.png", { type: 'image/png' });
      this.croppedImageUrl = URL.createObjectURL(this.croppedImage);


      if (this.croppingModal?.didPresent) {
        this.croppingModal.dismiss();
        if (this.addingPersonModal) {
          this.addingPersonModal.present();
        }
      }
    }


  }
  /**
   * Save a person to the S3 bucket
   */
  async submitUser() {
    let searchParams = { Bucket: 'kvasir-storage', Key: 'PeopleInformation.json', ResponseCacheControl: "no-cache" };
    let searchCommand = new GetObjectCommand(searchParams);

    try {
      let nextIndex: number = 0;
      let existingPeopleObject: any = null;

      await this.s3Client.send(searchCommand).then((value: any) => {
        value.Body?.transformToString().then(async (dataAsString: any) => {
          existingPeopleObject = JSON.parse(dataAsString);

          //Generate an array of existing indices
          let existingIndices = Object.keys(existingPeopleObject).map(index => parseInt(index)).sort((a, b) => a - b);

          nextIndex = existingIndices.length; //Default to the next index if no gaps in index are found
          for (let i = 0; i < existingIndices.length; i++) {// Look for gaps between indices
            if (existingIndices[i] !== i) {
              nextIndex = i;
              break;
            }
          } if (this.croppedImage) {
            //Upload image of the person to the constant images folder to save
            let savedKey = `Images/${new Date().toISOString()}-${this.croppedImage.name}`;
            this.uploadImage(this.croppedImage, savedKey)

            //Upload image of the person to the temporary images folder 
            let tempKey = `TempPersonImage/${nextIndex}-${new Date().toISOString()}-${this.croppedImage.name}`;
            this.uploadImage(this.croppedImage, tempKey)


            //New person turned into object
            let personInfo = {
              Key: [savedKey],
              Name: this.userName,
              Reason: this.userReason,
              faces: [] // Empty array for backend to fill in
            };
            if (existingPeopleObject) {
              existingPeopleObject[nextIndex] = personInfo;

              //Change the updated object back to a JSON string
              let updatedDataAsString = JSON.stringify(existingPeopleObject, null, 2);

              //Add the file back with new user
              let addCommand = new PutObjectCommand({
                Bucket: 'kvasir-storage',
                Key: 'PeopleInformation.json',
                Body: updatedDataAsString,
                ContentType: 'application/json'
              });
              await this.s3Client.send(addCommand);
            }

          }
        })
      });
    } catch (error) {

      console.error('Error uploading image:', error);
    } finally {
      if (this.croppingModal?.didPresent) {
        this.croppingModal.dismiss();
      }
      if (this.addingPersonModal?.didPresent) {
        this.addingPersonModal.dismiss();
      }
    }
  }

  /**
   * Upload an image of a person to the S3 bucket
   */
  async uploadImage(file: File, key: string) {

    let commandInput: PutObjectCommandInput = {
      Bucket: 'kvasir-storage',
      Key: key,
      Body: file,
      ContentType: file.type,
    };
    try {
      let command = new PutObjectCommand(commandInput);
      this.s3Client.send(command);
    } catch {
      console.error("Error sending file")
    }
  }
}

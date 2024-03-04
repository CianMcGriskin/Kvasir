import { Component, OnInit } from '@angular/core';
import { S3Client, ListObjectsV2Command, GetObjectCommand, DeleteObjectCommand, PutObjectCommand, PutObjectCommandInput } from "@aws-sdk/client-s3";
import { LoadingController } from '@ionic/angular';
import { environment } from '../../environments/environment';
import { AlertController } from '@ionic/angular';
import { Filesystem, Directory, Encoding } from '@capacitor/filesystem';
import { FormBuilder, FormGroup, Validators } from '@angular/forms';

@Component({
  selector: 'app-barred-list',
  templateUrl: './barred-list.page.html',
  styleUrls: ['./barred-list.page.scss'],
})
export class BarredListPage implements OnInit {
  images: any[];
  s3Client: S3Client;
  displaySelectedImageUrl: String | undefined;
  userName: string = '';
  userReason: string = '';

  form: FormGroup | null = null;
  addingUserSelectedImage: File | null = null;

  constructor(private loadingController: LoadingController, private fb: FormBuilder) {
    this.images = [];

    this.form = this.fb.group({
      userName: ['', Validators.required],
      userReason: ['', Validators.required],
      userFiles: [null, Validators.required]
    });


    this.s3Client = new S3Client({
      region: environment.REGION,
      credentials: {
        accessKeyId: environment.AWS_ACCESS_KEY_ID,
        secretAccessKey: environment.AWS_SECRET_ACCESS_KEY
      }
    });
  }


  async ngOnInit() {
    this.loadImages();
  }

  async loadImages() {
    const loading = await this.loadingController.create({
      message: 'Loading Images...'
    });
    await loading.present();

    try {
      this.images = await this.listImagesFromS3Folder();
    } catch (error) {
      console.error('Error fetching images:', error);
    } finally {
      loading.dismiss();
    }
  }

  /**
   * Retrieves a list of videos from the S3 bucket inside the 'Images' folder
   * @returns array of images from S3 bucket
   */
  async listImagesFromS3Folder() {

    let params = {
      Bucket: 'kvasir-storage',
      Prefix: 'PersonImage/',
    };

    try {
      //Retrieve images with .jpg ending with the paramaters set in params
      let command = new ListObjectsV2Command(params);
      let data = await this.s3Client.send(command);

      const allowedImageExtensions = ['.jpg', '.jpeg', '.png']; //Image Extensions allowed

      const images = data.Contents?.filter(obj => {
        const key = obj?.Key || '';
        return allowedImageExtensions.some(ext => key.toLowerCase().endsWith(ext));
      }) || [];

      return images;
    }
    catch (error) {
      console.error('Error:', error);
      throw error;
    }
  }

  /**
   * Returns file name without the prefix. e.g. 'Images/image.jpg' will return 'image.jpg'
   * @param fullFileName
   */
  extractFileName(fullFileName: string): string {
    let parts = fullFileName.split('/');
    let fileNameWithoutPrefix = parts[parts.length - 1];
    return fileNameWithoutPrefix;
  }

  async showImage(key: string) {
    //Reset the values when swapping between images
    this.displaySelectedImageUrl = undefined;

    let params = {
      Bucket: 'kvasir-storage',
      Key: key,
    };

    try {
      let command = new GetObjectCommand(params)
      let image = await this.s3Client.send(command);

      //Transform the video to a byte array and store it as a BLOB
      let imageData = await image.Body?.transformToByteArray();
      if (imageData) {
        let videoBlob = new Blob([imageData], { type: 'image/jpg' });

        //Generate a URL to the video so we can display it inside the HTML file
        this.displaySelectedImageUrl = URL.createObjectURL(videoBlob);
      }
      else {
        this.displaySelectedImageUrl = undefined;
        console.error("Invalid video URL");
      }
    } catch (error) {
      console.error('Error:', error);
      throw error;
    }

  }

  async deleteImage(key: string) {
    if (!key) {
      console.error("No image to delete");
      return;
    }

    let params = {
      Bucket: 'kvasir-storage',
      Key: key,
    };

    try {
      let deleteCommand = new DeleteObjectCommand(params);
      await this.s3Client.send(deleteCommand);

      //Reset selected values after deletion in case image is showing up
      this.displaySelectedImageUrl = undefined;
      this.loadImages();
    } catch (error) {
      console.error('Error deleting image:', error);
    }
  }
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

  onFileSelected(event: any): void {
    const file: File = event.target.files[0];
    if (file) {
      this.addingUserSelectedImage = file;

      // Tell the form that the file has been selected
      if (this.form) {
        this.form.patchValue({ userFiles: file });
      } else {
        console.log("Error with form, it does not exist")
      }
    }

  }
  async submitUser() {
    // Retrieve the json of the barred people
    let searchParams = { Bucket: 'kvasir-storage', Key: 'PeopleInformation.json', ResponseCacheControl: "no-cache" };
    let searchCommand = new GetObjectCommand(searchParams);

    try {
      // Retrieve the next free index
      let nextIndex: number = 0;
      let existingPeopleObject: any = null;

      await this.s3Client.send(searchCommand).then((value: any) => {
        value.Body?.transformToString().then(async (dataAsString: any) => {
          existingPeopleObject = JSON.parse(dataAsString);

          // Generate an array of existing indices
          let existingIndices = Object.keys(existingPeopleObject).map(index => parseInt(index)).sort((a, b) => a - b);

          nextIndex = existingIndices.length; // Default to the next index if no gaps in index are found
          for (let i = 0; i < existingIndices.length; i++) { // Look for gaps between indices
            if (existingIndices[i] !== i) {
              nextIndex = i;
              break;
            }
          } if (this.addingUserSelectedImage) {
            // Upload image of the person to the constant images folder to save
            let savedKey = `Images/${new Date().toISOString()}-${this.addingUserSelectedImage.name}`;
            this.uploadImage(this.addingUserSelectedImage, savedKey)

            // Upload image of the person to the temporary images folder 
            let tempKey = `TempPersonImage/${nextIndex}-${new Date().toISOString()}-${this.addingUserSelectedImage.name}`;
            this.uploadImage(this.addingUserSelectedImage, tempKey)

            // New person turned into object
            let personInfo = {
              Key: [savedKey],
              Name: this.userName,
              Reason: this.userReason,
              faces: [] // Empty array for backend to fill in
            };
            if (existingPeopleObject) {
              existingPeopleObject[nextIndex] = personInfo;

              // Change the updated object back to a JSON string
              let updatedDataAsString = JSON.stringify(existingPeopleObject, null, 2);

              // Add the file back with new user
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
      //refresh image display
      this.loadImages();
    }
  }

}

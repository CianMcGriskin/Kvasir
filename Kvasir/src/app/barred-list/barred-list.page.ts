import { Component, OnInit } from '@angular/core';
import { S3Client, ListObjectsV2Command, GetObjectCommand, DeleteObjectCommand, PutObjectCommand, PutObjectCommandInput } from "@aws-sdk/client-s3";
import { LoadingController } from '@ionic/angular';
import { environment } from '../../environments/environment';
import { AlertController } from '@ionic/angular';
import { Filesystem, Directory, Encoding } from '@capacitor/filesystem';

@Component({
  selector: 'app-barred-list',
  templateUrl: './barred-list.page.html',
  styleUrls: ['./barred-list.page.scss'],
})
export class BarredListPage implements OnInit {
  images: any[];
  s3Client: S3Client;
  selectedImageUrl: String | undefined;

  constructor(private loadingController: LoadingController) {
    this.images = [];

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

  async loadImages(){
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
    this.selectedImageUrl = undefined;

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
        this.selectedImageUrl = URL.createObjectURL(videoBlob);
      }
      else {
        this.selectedImageUrl = undefined;
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
      this.selectedImageUrl = undefined;
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
    try{
    let command = new PutObjectCommand(commandInput);
    this.s3Client.send(command);
    }catch{
      console.error("Error sending file")
    }
  }

  onFileSelected(event: any): void {
    const file: File = event.target.files[0];

    if (file) {
      const key = `Images/${new Date().toISOString()}-${file.name}`;
      try{
        
      this.uploadImage(file, key)
      }catch(error){
          
        console.error('Error uploading image:', error);
      }finally{
        //refresh image display
        this.loadImages();
      }
    }
  }
  
}

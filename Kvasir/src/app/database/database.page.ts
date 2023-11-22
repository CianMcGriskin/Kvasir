import { Component, OnInit } from '@angular/core';
import { S3Client, ListObjectsV2Command, GetObjectCommand } from "@aws-sdk/client-s3";
import { LoadingController } from '@ionic/angular';
import { environment } from '../../environments/environment';
import { DomSanitizer, SafeResourceUrl } from '@angular/platform-browser';




@Component({
  selector: 'app-database',
  templateUrl: './database.page.html',
  styleUrls: ['./database.page.scss'],
})
export class DatabasePage implements OnInit {

  videos: any[];
  s3Client: S3Client;
  selectedVideoUrl: String | undefined;

  constructor(private loadingController: LoadingController, private sanitizer: DomSanitizer) { 
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
    const loading = await this.loadingController.create({
      message: 'Loading Videos...'
    });
    await loading.present();

    try {
      this.videos = await this.listVideosFromS3Folder();
    } catch (error) {
      console.error('Error fetching videos:', error);
    } finally {
      loading.dismiss();
    }
  }

  async listVideosFromS3Folder() {
    let params = {
      Bucket: 'kvasir-storage',
      Prefix: 'Videos/',
    };

    try {
      
      const command = new ListObjectsV2Command(params);
      let data = await this.s3Client.send(command);
      const videos = data.Contents?.filter(obj => obj?.Key?.endsWith('.mp4')) || [];

      return videos;

    } 
    catch (error) {
      console.error('Error:', error);
      throw error;
    }
  }

  extractFileName(key: string): string {
    const parts = key.split('/');
    return parts[parts.length - 1];
  }

  async playVideo(key: string) {
    this.selectedVideoUrl = undefined;

    let params = {
      Bucket: 'kvasir-storage',
      Key: key,
    };

    try
    {
      let objectCommand = new GetObjectCommand(params)
    let testVideo = await this.s3Client.send(objectCommand);

    let videoData = await testVideo.Body?.transformToByteArray();
    if(videoData){
      let videoBlob = new Blob([videoData], { type: 'video/mp4' }); // Adjust MIME 
      this.selectedVideoUrl = URL.createObjectURL(videoBlob);
    console.log(this.selectedVideoUrl);
    }
    else{
      this.selectedVideoUrl = undefined;
      console.error("Invalid video URL");
    }
    }catch(error){
      console.error('Error:', error);
      throw error;
    }
    
  }
}
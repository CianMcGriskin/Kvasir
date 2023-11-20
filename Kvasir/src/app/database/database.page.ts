import { Component, OnInit } from '@angular/core';
import { S3Client, ListObjectsV2Command, GetObjectCommand, GetObjectCommandInput  } from "@aws-sdk/client-s3";
import { LoadingController } from '@ionic/angular';
import { environment } from '../../environments/environment';




@Component({
  selector: 'app-database',
  templateUrl: './database.page.html',
  styleUrls: ['./database.page.scss'],
})
export class DatabasePage implements OnInit {

  videos: any[] | undefined;
  s3Client: S3Client;
  constructor(private loadingController: LoadingController) { 
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
    const params = {
      Bucket: 'kvasir-storage',
      Prefix: 'Videos/',
    };

    try {
      
      const command = new ListObjectsV2Command(params);
      let data = await this.s3Client.send(command);
      const videos = data.Contents?.filter(obj => obj?.Key?.endsWith('.mp4')) || [];

      let getObjectParams = {
        Bucket: 'kvasir-storage',
        Prefix: 'Videos/',
        Key: videos[0].Key,
      };

      let objectCommand = new GetObjectCommand(getObjectParams)
      let testVideo = await this.s3Client.send(objectCommand);
      let stuff = testVideo.Body?.transformToString();
      
      return videos;
    } catch (error) {
      console.error('Error:', error);
      throw error;
    }
  }

  extractFileName(key: string): string {
    const parts = key.split('/');
    return parts[parts.length - 1];
  }

  async playVideo(videoUrl: string) {

    //TODO

  }
}

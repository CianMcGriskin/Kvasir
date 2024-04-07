import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { S3Client, ListObjectsV2Command, GetObjectCommand, PutObjectCommand, PutObjectCommandInput } from "@aws-sdk/client-s3";
import { IonModal, LoadingController } from '@ionic/angular';
import { environment } from '../../environments/environment';
import { DomSanitizer } from '@angular/platform-browser';
import * as faceapi from 'face-api.js';
import { SwiperContainer } from 'swiper/element';
import { ModalController } from '@ionic/angular';
import { VideoPlayerPage } from '../video-player/video-player.page';

@Component({
  selector: 'app-database',
  templateUrl: './database.page.html',
  styleUrls: ['./database.page.scss'],
})
export class DatabasePage implements OnInit {

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

  constructor(private loadingController: LoadingController, private sanitizer: DomSanitizer, private modalController: ModalController) {
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
    //Load face-api models from assets
    await faceapi.nets.tinyFaceDetector.loadFromUri('assets/models');
    await faceapi.nets.faceLandmark68Net.loadFromUri('assets/models');
    await faceapi.nets.faceRecognitionNet.loadFromUri('assets/models');
    await faceapi.nets.ssdMobilenetv1.loadFromUri('assets/models');

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

  /**
   * Retrieves a list of videos from the S3 bucket inside the 'Videos' folder
   * @returns array of videos from S3 bucket
   */
  async listVideosFromS3Folder() {

    let params = {
      Bucket: 'kvasir-storage',
      Prefix: 'Videos/',
    };

    try {
      //Retrieve videos with .mp4 ending with the paramaters set in params
      let command = new ListObjectsV2Command(params);
      let data = await this.s3Client.send(command);
      const videos = data.Contents?.filter(obj => obj?.Key?.endsWith('.mp4')) || [];

      return videos;
    }
    catch (error) {
      console.error('Error:', error);
      throw error;
    }
  }

  /**
   * Returns file name without the prefix. e.g. 'Videos/video.mp4' will return 'video.mp4'
   * @param fullFileName
   */
  extractFileName(fullFileName: string): string {
    let parts = fullFileName.split('/');
    let fileNameWithoutPrefix = parts[parts.length - 1];
    return fileNameWithoutPrefix;
  }

  /**
   * Returns a video from the S3 bucket based on the key passed through
   * @param key 
   */
  async playVideo(key: string) {
      let params = {
        Bucket: 'kvasir-storage',
        Key: key,
      };

      try {
        let command = new GetObjectCommand(params)
        let video = await this.s3Client.send(command);

        //Transform video to BLOB
        let videoData = await video.Body?.transformToByteArray();
        if (videoData) {
          let videoBlob = new Blob([videoData], { type: 'video/mp4' });

          //Generate a URL to the video so we can display it inside the view
          this.selectedVideoUrl = URL.createObjectURL(videoBlob);
        }
        else {
          this.selectedVideoUrl = undefined;
          console.error("Invalid video URL");
        }

      } catch (error) {
        console.error('Error:', error);
        throw error;
      }
    

    const modal = await this.modalController.create({
      component: VideoPlayerPage,
      componentProps: {
        'selectedVideoUrl': this.selectedVideoUrl
      }
    });
    return await modal.present();
  }
}
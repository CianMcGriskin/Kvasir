import { Component, OnInit } from '@angular/core';
import { GetObjectCommand, S3, S3Client } from '@aws-sdk/client-s3';
import { NavParams } from '@ionic/angular';
import { PredictFaceExpressionsTaskBase } from 'face-api.js/build/commonjs/globalApi/PredictFaceExpressionsTask';
import { environment } from 'src/environments/environment';

@Component({
  selector: 'app-image-viewer',
  templateUrl: './image-viewer.page.html',
  styleUrls: ['./image-viewer.page.scss'],
})
export class ImageViewerPage implements OnInit {
  imageKey: any[] = [];
  displaySelectedImageUrl: string | undefined;
  personName: any;
  personReason: any;

  s3Client: S3Client
  constructor(private navParams: NavParams) { 
    this.s3Client = new S3Client({
      region: environment.REGION,
      credentials: {
        accessKeyId: environment.AWS_ACCESS_KEY_ID,
        secretAccessKey: environment.AWS_SECRET_ACCESS_KEY
      }
    });
  }

  async ngOnInit() {
     //Retrieve the image key
     if (this.navParams.get('imageKey')) {
       this.imageKey = Object.values(this.navParams.get('imageKey'))
       this.personName = this.navParams.get('personName')
       this.personReason = this.navParams.get('personReason')

       console.log(this.imageKey[0])
    }
    console.log(this.imageKey[0])
    let params = {
      Bucket: 'kvasir-storage',
      Key: this.imageKey[0],
    };

    try {
      let command = new GetObjectCommand(params)
      let image = await this.s3Client.send(command);

      //Transform image to BLOB
      let imageData = await image.Body?.transformToByteArray();
      if (imageData) {
        let imageBlob = new Blob([imageData], { type: 'image/jpg' });

        //Generate a URL to the image so we can display it inside the view
        this.displaySelectedImageUrl = URL.createObjectURL(imageBlob);
      }
      else {
        this.displaySelectedImageUrl = undefined;
        console.error("Invalid image URL");
      }
    } catch (error) {
      console.error('Error:', error);
      throw error;
    }

  }

}

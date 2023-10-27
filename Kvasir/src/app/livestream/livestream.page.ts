import { Component, OnInit } from '@angular/core';
import { DomSanitizer, SafeUrl } from '@angular/platform-browser';
import { HttpClient } from '@angular/common/http';
import { Subscription, interval } from 'rxjs';
import { LoadingController } from '@ionic/angular';

@Component({
  selector: 'app-livestream',
  templateUrl: './livestream.page.html',
  styleUrls: ['./livestream.page.scss'],
})
export class LivestreamPage implements OnInit {

  streamUrl: string = 'http://192.168.1.8:8080';
  isCameraOffline: boolean = false;
  streamImage: any;

  private subscription: Subscription | undefined;

  constructor(private loadingController: LoadingController) {
    this.streamImage = new Image();
  }

  async ngOnInit() {

    const loading = await this.loadingController.create({
      message: 'Loading Footage...'
    });
    await loading.present();

    try {
      //Run checkStreamStatus initally so user doesn't have to wait 500ms to get footage back 
      await this.checkStreamStatus();      
    } catch (error) {
      console.error('Error fetching footage:', error);
    } finally {
      this.subscription = interval(500).subscribe(() => {
        this.checkStreamStatus();
      });
      loading.dismiss();
    }
  }



  async checkStreamStatus() {

    return new Promise<void>((resolve, reject) => {
      this.streamImage.src = this.streamUrl;
  
      this.streamImage.onload = () => {
        console.log(this.streamImage.width + " " + this.streamImage.height)
        if (this.streamImage.width === 0) { //If image is empty, camera is offline
          this.isCameraOffline = true;
          reject(); 
        } else {
          this.isCameraOffline = false;
          resolve(); 
        }
      };
  
      this.streamImage.onerror = () => {
        this.isCameraOffline = true;
        reject(); //Reject the promise if there was an error loading the image
      };
    });
  }
}

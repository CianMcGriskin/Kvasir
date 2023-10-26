import { Component, OnInit} from '@angular/core';
import { DomSanitizer, SafeUrl } from '@angular/platform-browser';
import { HttpClient } from '@angular/common/http';
import { Subscription, interval } from 'rxjs';

@Component({
  selector: 'app-livestream',
  templateUrl: './livestream.page.html',
  styleUrls: ['./livestream.page.scss'],
})
export class LivestreamPage implements OnInit{

  streamUrl: string = 'http://192.168.1.8:8080';
  isCameraOffline: boolean = false;
  streamImage: any;

  private subscription: Subscription | undefined;
 
  constructor(private sanitizer: DomSanitizer, private http: HttpClient) {
    this.streamImage = new Image();
  }

  ngOnInit() {
    // Check the stream status initially
    this.checkStreamStatus();

    this.subscription = interval(500).subscribe(() => {
      this.checkStreamStatus();
    });
  }

  checkStreamStatus() {
    this.streamImage.src = this.streamUrl;

    this.streamImage.onload = () => {
      console.log(this.streamImage.width + " " + this.streamImage.height)
      if (this.streamImage.width === 0) {
        this.isCameraOffline = true;
      } else {
        this.isCameraOffline = false;
      }
    };

    this.streamImage.onerror = () => {
      this.isCameraOffline = true;
    };
  }
}
 
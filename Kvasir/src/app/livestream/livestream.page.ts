import { Component, OnInit} from '@angular/core';
import { DomSanitizer, SafeUrl } from '@angular/platform-browser';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-livestream',
  templateUrl: './livestream.page.html',
  styleUrls: ['./livestream.page.scss'],
})
export class LivestreamPage implements OnInit{
  mjpegUrl: SafeUrl;
  isCameraOffline: boolean = false;
 
  constructor(private sanitizer: DomSanitizer, private http: HttpClient) {
    this.mjpegUrl = this.sanitizer.bypassSecurityTrustUrl('http://192.168.1.8:8080/');
  }
  handleImageError() {
    // This function will be called when the image fails to load
    this.isCameraOffline = true;
  }

  ngOnInit() {
    // Check the stream status initially
    this.checkStreamStatus();

    // Poll the server every 5 seconds (adjust the interval as needed)
    setInterval(() => {
      this.checkStreamStatus();
    }, 5000);
  }

  checkStreamStatus() {
    const img = new Image();
    img.onload = () => {
      // Image loaded successfully, stream is online
      this.isCameraOffline = false;
    };
    img.onerror = () => {
      // Error occurred, camera is offline
      this.isCameraOffline = true;
    };
    img.src = this.mjpegUrl.toString() + '?' + new Date().getTime(); // Append a timestamp to prevent caching
  }
}
 
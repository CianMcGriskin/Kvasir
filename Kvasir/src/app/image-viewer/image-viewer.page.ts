import { Component, OnInit } from '@angular/core';
import { NavParams } from '@ionic/angular';

@Component({
  selector: 'app-image-viewer',
  templateUrl: './image-viewer.page.html',
  styleUrls: ['./image-viewer.page.scss'],
})
export class ImageViewerPage implements OnInit {
  displaySelectedImageUrl = undefined;
  constructor(private navParams: NavParams) { }

  ngOnInit() {
     // Get the Modal params
     if (this.navParams.get('selectedVideoUrl')) {
      this.displaySelectedImageUrl = this.navParams.get('displaySelectedImageUrl')
    }
  }

}

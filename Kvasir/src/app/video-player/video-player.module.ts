import { CUSTOM_ELEMENTS_SCHEMA, NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { VideoPlayerPageRoutingModule } from './video-player-routing.module';

import { VideoPlayerPage } from './video-player.page';
import { ImageCropperModule } from 'ngx-image-cropper';

@NgModule({
  schemas: [CUSTOM_ELEMENTS_SCHEMA],
  imports: [
    ImageCropperModule,
    CommonModule,
    FormsModule,
    IonicModule,
    VideoPlayerPageRoutingModule
  ],
  declarations: [VideoPlayerPage]
})
export class VideoPlayerPageModule {}

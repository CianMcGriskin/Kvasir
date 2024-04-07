import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { ImageViewerPageRoutingModule } from './image-viewer-routing.module';

import { ImageViewerPage } from './image-viewer.page';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    ImageViewerPageRoutingModule
  ],
  declarations: [ImageViewerPage]
})
export class ImageViewerPageModule {}

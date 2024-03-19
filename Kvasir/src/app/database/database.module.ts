import { NgModule, CUSTOM_ELEMENTS_SCHEMA } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { DatabasePageRoutingModule } from './database-routing.module';

import { DatabasePage } from './database.page';
import { ImageCropperModule } from 'ngx-image-cropper';

@NgModule({
  schemas: [CUSTOM_ELEMENTS_SCHEMA],
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    DatabasePageRoutingModule,
    ImageCropperModule
  ],
  declarations: [DatabasePage]
})
export class DatabasePageModule {}

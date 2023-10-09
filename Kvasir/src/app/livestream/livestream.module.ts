import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { LivestreamPageRoutingModule } from './livestream-routing.module';

import { LivestreamPage } from './livestream.page';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    LivestreamPageRoutingModule
  ],
  declarations: [LivestreamPage]
})
export class LivestreamPageModule {}

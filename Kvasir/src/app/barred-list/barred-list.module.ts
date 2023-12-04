import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { BarredListPageRoutingModule } from './barred-list-routing.module';

import { BarredListPage } from './barred-list.page';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    BarredListPageRoutingModule
  ],
  declarations: [BarredListPage]
})
export class BarredListPageModule {}

import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { LivestreamPage } from './livestream.page';

const routes: Routes = [
  {
    path: '',
    component: LivestreamPage
  }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule],
})
export class LivestreamPageRoutingModule {}

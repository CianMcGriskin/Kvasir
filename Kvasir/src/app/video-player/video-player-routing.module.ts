import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { VideoPlayerPage } from './video-player.page';

const routes: Routes = [
  {
    path: '',
    component: VideoPlayerPage
  }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule],
})
export class VideoPlayerPageRoutingModule {}

import { NgModule } from '@angular/core';
import { PreloadAllModules, RouterModule, Routes } from '@angular/router';

const routes: Routes = [
  {
    path: 'home',
    loadChildren: () => import('./home/home.module').then( m => m.HomePageModule)
  },
  {
    path: '',
    redirectTo: 'home',
    pathMatch: 'full'
  },
  {
    path: 'livestream',
    loadChildren: () => import('./livestream/livestream.module').then( m => m.LivestreamPageModule)
  },  {
    path: 'database',
    loadChildren: () => import('./database/database.module').then( m => m.DatabasePageModule)
  },
  {
    path: 'management',
    loadChildren: () => import('./management/management.module').then( m => m.ManagementPageModule)
  },
  {
    path: 'barred-list',
    loadChildren: () => import('./barred-list/barred-list.module').then( m => m.BarredListPageModule)
  },

];

@NgModule({
  imports: [
    RouterModule.forRoot(routes, { preloadingStrategy: PreloadAllModules })
  ],
  exports: [RouterModule]
})
export class AppRoutingModule { }

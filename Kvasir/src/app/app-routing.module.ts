import { NgModule } from '@angular/core';
import { PreloadAllModules, RouterModule, Routes } from '@angular/router';
import { AuthGuard } from './auth.guard'; // Update the path as per your project structure
const routes: Routes = [
  {
    path: 'home',
    loadChildren: () => import('./home/home.module').then( m => m.HomePageModule),
    canActivate: [AuthGuard] 
  },
  {
    path: '',
    redirectTo: 'home',
    pathMatch: 'full'
  },
  {
    path: 'livestream',
    loadChildren: () => import('./livestream/livestream.module').then( m => m.LivestreamPageModule),
    canActivate: [AuthGuard] 
  },
  {
    path: 'database',
    loadChildren: () => import('./database/database.module').then( m => m.DatabasePageModule),
    canActivate: [AuthGuard] 
  },
  {
    path: 'management',
    loadChildren: () => import('./management/management.module').then( m => m.ManagementPageModule),
    canActivate: [AuthGuard] 
  },
  {
    path: 'barred-list',
    loadChildren: () => import('./barred-list/barred-list.module').then( m => m.BarredListPageModule),
    canActivate: [AuthGuard] 
  },
  {
    path: 'login',
    loadChildren: () => import('./login/login.module').then( m => m.LoginPageModule)
  },

];

@NgModule({
  imports: [
    RouterModule.forRoot(routes, { preloadingStrategy: PreloadAllModules })
  ],
  exports: [RouterModule]
})
export class AppRoutingModule { }

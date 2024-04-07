import { Component } from '@angular/core';
import { NavController } from '@ionic/angular';
import { Auth } from '@angular/fire/auth';
import { Router } from '@angular/router';

@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage {

  constructor(private navController: NavController, private auth: Auth, private router: Router) {
    
  }
  /**
   * Redirects to livestream page
   */
  goToLivestreamPage() {
    this.navController.navigateForward('livestream'); 
  }
  goToBarredListPage() {
    this.navController.navigateForward('barred-list'); 
  }
  goToVideosPage() {
    this.navController.navigateForward('database'); 
  }
}
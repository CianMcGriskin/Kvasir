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

  async logout() {
    try {
      await this.auth.signOut();
      this.router.navigateByUrl('/login'); // Redirect to login page after logout
    } catch (error) {
      console.error('Error during logout:', error);
    }
  }

  goToLivestreamPage() {
    this.navController.navigateForward('livestream'); 
  }
}
import { Component } from '@angular/core';
import { NavController } from '@ionic/angular';
import * as AWS from 'aws-sdk';
import { environment } from 'src/environments/environment';

@Component({
  selector: 'app-home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})
export class HomePage {

  constructor(private navController: NavController) {
    
  }

  goToLivestreamPage() {
    this.navController.navigateForward('livestream'); 
  }
}
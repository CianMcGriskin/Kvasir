import { Component, OnInit } from '@angular/core';

import { Auth, signInWithEmailAndPassword} from '@angular/fire/auth';
import { Router } from '@angular/router';

@Component({
  selector: 'app-login',
  templateUrl: './login.page.html',
  styleUrls: ['./login.page.scss'],
})
export class LoginPage implements OnInit {

  email: string = '';
  password: string = '';

  constructor(private auth: Auth, private router: Router) {}

  /**
   * Logs the user into his account and redirects to home page
   */
  async login() {
    try {
      const userCredential = await signInWithEmailAndPassword(this.auth, this.email, this.password);
      console.log('You have been successfully logged in!', userCredential);
      //Navigate to the home page upon successful login
      this.router.navigateByUrl('/home');
    } catch (error) {
      console.error('Login failed:', error);
    }
  }
  ngOnInit() {
  }
}

import { Component, OnInit } from '@angular/core';

import { Auth, signInWithEmailAndPassword } from '@angular/fire/auth';
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

  async login() {
    try {
      const userCredential = await signInWithEmailAndPassword(this.auth, this.email, this.password);
      console.log('You have been successfully logged in!', userCredential);
      // Navigate to the home page (or another page) upon successful login
      this.router.navigateByUrl('/home');
    } catch (error) {
      console.error('Login failed:', error);
    }
  }
  ngOnInit() {
  }
}

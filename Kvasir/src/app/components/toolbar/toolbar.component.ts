import { Component } from '@angular/core';
import { Auth } from '@angular/fire/auth';
import { Router, Event as RouterEvent, NavigationEnd } from '@angular/router';
import { filter } from 'rxjs/operators';

@Component({
  selector: 'app-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss'],
})
export class ToolbarComponent {

  showLogoutButton: Boolean = true;
  constructor(private auth: Auth, private router: Router) {
    this.router.events.pipe(
      filter((event: RouterEvent) => event instanceof NavigationEnd)
    ).subscribe((event: any) => {
      this.showLogoutButton = event.url !== '/login';
    });

   }

  /**
   * Logs the user out of his account and redirects to login page
   */
  async logout() {
    try {
      await this.auth.signOut();
      this.router.navigateByUrl('/login'); //Redirect to login page after logout
    } catch (error) {
      console.error('Error during logout:', error);
    }
  }

}

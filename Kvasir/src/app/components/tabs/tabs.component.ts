import { Component } from '@angular/core';
import { Router, Event as RouterEvent, NavigationEnd } from '@angular/router';
import { filter } from 'rxjs/operators';

@Component({
  selector: 'app-tabs',
  templateUrl: './tabs.component.html',
  styleUrls: ['./tabs.component.scss'],
})
export class TabsComponent {
  showTabs: boolean = true;

  constructor(private router: Router) {
    this.router.events.pipe(
      filter((event: RouterEvent) => event instanceof NavigationEnd)
    ).subscribe((event: any) => {
      this.showTabs = event.url !== '/login';
    });
  }
}

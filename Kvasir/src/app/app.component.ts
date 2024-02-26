import { Component } from '@angular/core';
import { register } from 'swiper/element/bundle';
import { NotificationsService } from './services/notifications.service';
import { NotificationConfiguration } from "@aws-sdk/client-s3";

register();

@Component({
  selector: 'app-root',
  templateUrl: 'app.component.html',
  styleUrls: ['app.component.scss'],
})
export class AppComponent {
  constructor(private notificationService: NotificationsService) {}

  ngOnInit() {
    this.notificationService.startPolling(4000); // Poll every 10 seconds
  }

  ngOnDestroy() {
    this.notificationService.stopPolling();
  }
}

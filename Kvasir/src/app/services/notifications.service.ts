import { Injectable } from '@angular/core';
import { GetObjectCommand, S3Client, PutObjectCommand } from '@aws-sdk/client-s3';
import { interval, Subscription } from 'rxjs';
import { environment } from 'src/environments/environment';
import { AlertController } from '@ionic/angular';

@Injectable({
  providedIn: 'root'
})
export class NotificationsService {

  private pollingSubscription: Subscription | undefined;
  private alertQueue: Function[] = [];
  private alertInProgress = false;
  s3Client: S3Client;

  searchParams = { Bucket: 'kvasir-storage', Key: 'PeopleNotifications.json', ResponseCacheControl: "no-cache" };
  searchCommand = new GetObjectCommand(this.searchParams);

  constructor(private alertController: AlertController) {

    this.s3Client = new S3Client({
      region: environment.REGION,
      credentials: {
        accessKeyId: environment.AWS_ACCESS_KEY_ID,
        secretAccessKey: environment.AWS_SECRET_ACCESS_KEY
      }
    });


  }

  async startPolling(intervalMilliseconds: number) {
    // Check if polling is already active to avoid multiple subscriptions
    if (this.pollingSubscription && !this.pollingSubscription.closed) {
      return;
    }

    let pollingObservable = interval(intervalMilliseconds);
    this.pollingSubscription = pollingObservable.subscribe({
      next: () => this.fetchAndProcessNotifications(),
      error: (error) => console.error('Polling error:', error),
    });
  }

  stopPolling(): void {
    if (this.pollingSubscription) {
      this.pollingSubscription.unsubscribe();
    }
  }

  private async fetchAndProcessNotifications() {
    await this.s3Client.send(this.searchCommand).then((value: any) => {
      value.Body?.transformToString().then((dataAsString: any) => {
        // Check if the file holds any notifications
        this.checkForNotifications(JSON.parse(dataAsString).notifications)
      })
    });
  }

  async clearNotificationsFile() {
    let notificationsObject = { notifications: [] }
    let notificationsContent = JSON.stringify(notificationsObject)
    let fileName = 'PeopleNotifications.json';

    try {
      // Add the file back as empty notification object
      let addCommand = new PutObjectCommand({
        Bucket: 'kvasir-storage',
        Key: fileName,
        Body: notificationsContent,
        ContentType: 'application/json'
      });
      let response = await this.s3Client.send(addCommand);
    } catch (error) {
      console.error('Error uploading notifications file:', error);
    }
  }



  private async checkForNotifications(notifications: any) {
    if (notifications.length > 0) {
      notifications.forEach((notification: any) => {
        this.alertQueue.push(() => this.presentAlert(notification));
      });

      if (!this.alertInProgress) {
        this.processNextAlert();
      }

      // Remove the notifications from the file and push it to the database
      this.clearNotificationsFile();
    }
  }

  private async processNextAlert() {
    if (this.alertQueue.length > 0 && !this.alertInProgress) {
      this.alertInProgress = true;
      let createAndShowAlert = this.alertQueue.shift();
      if (createAndShowAlert) {
        await createAndShowAlert();
        this.alertInProgress = false;
        this.processNextAlert();
      }
    }
  }

  private async presentAlert(notification: any) {
    let alert = await this.alertController.create({
      header: `BARRED PERSON DETECTED`,
      subHeader: 'subheader',
      message: 'message',
      buttons: ['OK'],
    });

    await alert.present();
    return new Promise<void>(resolve => alert.onDidDismiss().then(() => resolve()));
  }
}

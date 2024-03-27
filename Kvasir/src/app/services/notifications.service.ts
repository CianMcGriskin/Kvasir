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

  faceSearchParams = { Bucket: 'kvasir-storage', Key: 'PeopleNotifications.json', ResponseCacheControl: "no-cache" };
  faceSearchCommand = new GetObjectCommand(this.faceSearchParams);

  carSearchParams = { Bucket: 'kvasir-storage', Key: 'Vehicle-Data/yourfile.json', ResponseCacheControl: "no-cache" };
  carSearchCommand = new GetObjectCommand(this.carSearchParams);

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
    //GET NOTIFICATIONS FOR FACES
    await this.s3Client.send(this.faceSearchCommand).then((value: any) => {
      value.Body?.transformToString().then((dataAsString: any) => {
        // Check if the file holds any notifications
        this.checkForFaceNotifications(JSON.parse(dataAsString).notifications)
      })
    });

    //GET NOTIFICATIONS FOR CARS
    await this.s3Client.send(this.carSearchCommand).then((value: any) => {
      value.Body?.transformToString().then((dataAsString: any) => {
        // Check if the file holds any notifications
        this.checkForCarNotifications(JSON.parse(dataAsString))
      })
    });
  }


  private async checkForFaceNotifications(notifications: any) {
    if (notifications.length > 0) {
      notifications.forEach((notification: any) => {
        this.alertQueue.push(() => this.presentFaceAlert(notification));
      });

      if (!this.alertInProgress) {
        this.processNextAlert();
      }

      // Remove the notifications from the file and push it to the database
      this.clearFaceNotificationsFile();
    }
  }

  private async checkForCarNotifications(notifications: any) {
    if (notifications.length > 0) {
      notifications.forEach((notification: any) => {
        this.alertQueue.push(() => this.presentCarAlert(notification));
      });

      if (!this.alertInProgress) {
        this.processNextAlert();
      }

      // Remove the notifications from the file and push it to the database
      this.clearCarNotificationsFile();
    }
  }

  async clearFaceNotificationsFile() {
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

  async clearCarNotificationsFile() {
    let notificationsContent = JSON.stringify([])
    let fileName = 'Vehicle-Data/yourfile.json';

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

  private async presentFaceAlert(notification: any) {
    let alert = await this.alertController.create({
      header: `BARRED PERSON DETECTED`,
      subHeader: 'subheader',
      message: 'message',
      buttons: ['OK'],
    });
    await alert.present();
    return new Promise<void>(resolve => alert.onDidDismiss().then(() => resolve()));
  }

  private async presentCarAlert(notification: any) {

    console.log(notification)
    let alert = await this.alertController.create({
      header: `CAR DETECTED`,
      subHeader: `${notification.Response.registrationNumber}`,
      message: `A ${notification.Response.yearOfManufacture} ${notification.Response.colour} ${notification.Response.make}. Tax is due on ${notification.Response.taxDueDate} and NCT is due on ${notification.Response.motExpiryDate}`,
      buttons: ['OK'],
    });
    await alert.present();
    return new Promise<void>(resolve => alert.onDidDismiss().then(() => resolve()));
  }
}

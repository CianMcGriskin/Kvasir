import { Injectable } from '@angular/core';
import { GetObjectCommand, S3Client, PutObjectCommand } from '@aws-sdk/client-s3';
import { interval, Subscription } from 'rxjs';
import { environment } from 'src/environments/environment';
import { AlertController, IonicSafeString } from '@ionic/angular';

@Injectable({
  providedIn: 'root'
})
export class NotificationsService {

  private pollingSubscription: Subscription | undefined;
  private alertQueue: Function[] = [];
  private alertInProgress = false;
  s3Client: S3Client;
  imageUrl = '';

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
  /**
   * Start polling the notification json file in the S3
   */
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

  /**
   * Stop polling the notification json file in the S3
   */
  stopPolling(): void {
    if (this.pollingSubscription) {
      this.pollingSubscription.unsubscribe();
    }
  }

  /**
   * Process the notification from the S3
   */
  private async fetchAndProcessNotifications() {
    //GET NOTIFICATIONS FOR FACES
    await this.s3Client.send(this.faceSearchCommand).then((value: any) => {
      value.Body?.transformToString().then((dataAsString: any) => {
        this.checkForFaceNotifications(JSON.parse(dataAsString).notifications)
      })
    });

    //GET NOTIFICATIONS FOR CARS
    await this.s3Client.send(this.carSearchCommand).then((value: any) => {
      value.Body?.transformToString().then((dataAsString: any) => {
        this.checkForCarNotifications(JSON.parse(dataAsString))
      })
    });
  }

  /**
   * Checks for notifications on the face-detection end
   */
  private async checkForFaceNotifications(notifications: any) {
    if (notifications.length > 0) {
      notifications.forEach((notification: any) => {
        this.alertQueue.push(() => this.presentFaceAlert(notification));
      });

      if (!this.alertInProgress) {
        this.processNextAlert();
      }

      this.clearFaceNotificationsFile();
    }
  }

  /**
   * Checks for notifications on the registration-plate-detection end
   */
  private async checkForCarNotifications(notifications: any) {
    if (notifications.length > 0) {
      notifications.forEach((notification: any) => {
        this.alertQueue.push(() => this.presentCarAlert(notification));
      });

      if (!this.alertInProgress) {
        this.processNextAlert();
      }

      this.clearCarNotificationsFile();
    }
  }

  /**
   * Clears face notification file
   */
  async clearFaceNotificationsFile() {
    let notificationsObject = { notifications: [] }
    let notificationsContent = JSON.stringify(notificationsObject)
    let fileName = 'PeopleNotifications.json';

    try {
      //Add the file back as empty notification object
      let addCommand = new PutObjectCommand({
        Bucket: 'kvasir-storage',
        Key: fileName,
        Body: notificationsContent,
        ContentType: 'application/json'
      });
      await this.s3Client.send(addCommand);
    } catch (error) {
      console.error('Error uploading notifications file:', error);
    }
  }

   /**
   * Clears registration plate notification file
   */
  async clearCarNotificationsFile() {
    let notificationsContent = JSON.stringify([])
    let fileName = 'Vehicle-Data/yourfile.json';

    try {
      // Add the file back as empty object
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

  /**
   * Processes next alert in the queue after the last one is done displaying
   */
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

   /**
   * Presents alert for face-detection end
   */
  private async presentFaceAlert(notification: any) {
    let searchParams = { Bucket: 'kvasir-storage', Key: 'PeopleInformation.json', ResponseCacheControl: "no-cache" };
    let searchCommand = new GetObjectCommand(searchParams);

    await this.s3Client.send(searchCommand).then((value: any) => {
      value.Body?.transformToString().then(async (dataAsString: any) => {
        //Check if the file holds any notifications
        let peopleInformation = Object.values(JSON.parse(dataAsString))
        let notificationPerson = peopleInformation.filter((person: any) => person.Name === `${notification.name}`) as any[]

        if (notificationPerson.length > 0) {
          let imageKey = notificationPerson[0].Key[0]; 

          //Retrieve the image using the key
          let params = {
            Bucket: 'kvasir-storage',
            Key: imageKey,
          };
      
          try {
            let command = new GetObjectCommand(params)
            await this.s3Client.send(command).then(async (image:any) => {
              await image.Body?.transformToByteArray().then(async (imageData: any) => {
                if (imageData) {
                  let imageBlob = new Blob([imageData], { type: 'image/jpg' });
          
                  //Generate a URL to the image so we can display it inside the HTML file
                  this.imageUrl = await URL.createObjectURL(imageBlob);

                  let alert = await this.alertController.create({
                    header: `Detected ${notification.name}`,
                    subHeader: `Reason for Bar: ${notification.reason}`,
                    message:  new IonicSafeString(`<img src="${this.imageUrl}" alt="No image found for this person" />`),
                    buttons: ['OK'],
                  });
                  await alert.present();
                  return new Promise<void>(resolve => alert.onDidDismiss().then(() => resolve()));
                }
                else{
                  let alert = await this.alertController.create({
                    header: `Detected ${notification.name}`,
                    subHeader: `Reason for Bar: ${notification.reason}`,
                    message:  `No image found for this person`,
                    buttons: ['OK'],
                  });
                  
                  await alert.present();
                  return new Promise<void>(resolve => alert.onDidDismiss().then(() => resolve()));
                }
              
              });
            });
          } catch (error) {
            console.error('Error:', error);
            throw error;
          }
      
        }
      })
    });
  }

   /**
   * Presents alert for registration-plate-detection end
   */
  private async presentCarAlert(notification: any) {
    let alert = await this.alertController.create({
      header: `CAR DETECTED`,
      subHeader: `${notification.Response.registrationNumber}`,
      message: `A ${notification.Response.yearOfManufacture} ${notification.Response.colour} ${notification.Response.make}. Tax is due on ${notification.Response.taxDueDate} and NCT is due on ${notification.Response.motExpiryDate}`,
      buttons: ['OK'],
    });
    await alert.present();
    return new Promise<void>(resolve => alert.onDidDismiss().then(() => resolve()));
  }

  private delay(ms: number) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
}

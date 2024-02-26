import { Injectable } from '@angular/core';
import { GetObjectCommand, S3Client } from '@aws-sdk/client-s3';
import { interval, Subscription } from 'rxjs';
import { environment } from 'src/environments/environment';

@Injectable({
  providedIn: 'root'
})
export class NotificationsService {

  private pollingSubscription: Subscription | undefined;

  s3Client: S3Client;
  searchParams = { Bucket: 'kvasir-storage', Key:'PeopleNotifications.json'};
  command = new GetObjectCommand(this.searchParams);
  constructor() {

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

    const pollingObservable = interval(intervalMilliseconds);

    this.pollingSubscription = pollingObservable.subscribe({
      next: async () => {
        
      await this.s3Client.send(this.command).then((value: any) => {
        value.Body?.transformToString().then((dataAsString: any) => {
          this.checkForNotifications(JSON.parse(dataAsString).notifications)
        })
      });
      
      },
      error: (error) => {
        console.error('Polling error:', error);
      }
    });
  }

  stopPolling(): void {
    if (this.pollingSubscription) {
      this.pollingSubscription.unsubscribe();
    }
  }

  checkForNotifications(notifications: any): void{
    if(notifications[0] != null){
      console.log(notifications)
    }
  }
}

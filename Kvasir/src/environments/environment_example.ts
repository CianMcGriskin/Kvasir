//This file will show you how to setup your AWS and FIREBASE keys for this application.
//Firstly create a file called environment.ts and paste in the following code:

//Replace all the empty spaces with your api keys
export const environment = {
  production: false,
  AWS_ACCESS_KEY_ID: '',
  AWS_SECRET_ACCESS_KEY: '',
  REGION: '',

  firebaseConfig: {
    apiKey: "",
    authDomain: "",
    projectId: "",
    storageBucket: "",
    messagingSenderId: "",
    appId: "",
    measurementId: ""
  }
};

//Then create a file called environment.prod.ts and fill it in like so:
export const environment = {
  production: true, //Make sure this is set to true
  AWS_ACCESS_KEY_ID: '',
  AWS_SECRET_ACCESS_KEY: '',
  REGION: '',

  firebaseConfig: {
    apiKey: "",
    authDomain: "",
    projectId: "",
    storageBucket: "",
    messagingSenderId: "",
    appId: "",
    measurementId: ""
  }
};


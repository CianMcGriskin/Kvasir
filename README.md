# Kvasir Mobile

## Overview
Kvasir Mobile is a mobile application designed to enhance security and surveillance through real-time alerting and monitoring capabilities. As a crucial part of a broader security system that includes Kvasir Backend and Kvasir ANPR, this application lets users actively monitor and respond to security alerts directly from their mobile devices.

## Prerequisites
These are the versions of software used during development of the app:
- node.js - 20.11.0
- npm - 10.2.4
- ionic - 7.2.0
## API Keys
In order to run this application you will need specific keys from an AWS (Amazon Web Services) account and implement them inside the environmental files as seen in `Kvasir/src/environments/environment_example.ts`

This file walks you through step by step on how to implement your API keys. Firebase doesn't require any keys as they have been left public for this project but if you want to use your own Firebase application, you can change the Firebase API keys to your own.

## Installation Guide
Step 1: Cloning the repository
```
git clone -b Mobile https://github.com/CianMcGriskin/Kvasir.git
```
Step 2: CD into the repository
```
cd Kvasir/Kvasir
```
Step 3: Install node dependancies
```
npm install
```
Step 4: Following the instructions in the file `src/environments/environment_example.ts`, create two environmental files that store API keys needed to run the application.

Step 5: Run the application
```
ionic serve
```

## Recorded Demo
The demonstration is saved as Demo.mp4 in this repository. 


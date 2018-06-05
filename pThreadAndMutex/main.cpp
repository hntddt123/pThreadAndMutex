//
//  main.cpp
//  pThreadAndMutex
//
//  Created by Nientai Ho on 2018/4/1.
//  Copyright © 2018年 HNT Studio. All rights reserved.
//
//  The one way Whittier Tunnel problem
//  To Whittier
//  To Bear Valley
//  up to n car in tunnel
//  Tunnel thread and car thread

#include <iostream>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string>

using namespace std;

struct Car {
    int carNumber;
    int passingTime;
    string direction;
};

static pthread_cond_t ok;
static pthread_mutex_t trafficLock;
static string tunnelStatus = "";
static int maxCarsInTunnel = 0;
static int carsInTunnel = 0;
static int carCountWB = 0;
static int carCountBB = 0;
static int waitingCars = 0;
static int done = 0;

void arriveTunnel(int carNo, string direction) {
    cout << "Car #" <<  carNo << " going to " << direction << " arrives at the tunnel" << endl;
    if (direction == "Whittier") {
        carCountWB++;
    } else {
        carCountBB++;
    }
}

void enterTunnel(int carNo, string direction) {
    carsInTunnel++;
    cout << "Car #" <<  carNo << " going to " << direction <<  " enters the tunnel" << endl;
}

void exitTunnel(int carNo) {
    pthread_mutex_lock(&trafficLock);
    carsInTunnel--;
    pthread_cond_broadcast(&ok);
    cout << "Car #" <<  carNo << " exits the tunnel" << endl;
    pthread_mutex_unlock(&trafficLock);
}

void* carThread(void* car) {
    Car newCar = *((Car*) car);
    
    pthread_mutex_lock(&trafficLock);
    //Print a arrive message
    arriveTunnel(newCar.carNumber, newCar.direction);
    
    //Wait Until it can enter the tunnel
    if (carsInTunnel >= maxCarsInTunnel && tunnelStatus == newCar.direction) {
        waitingCars++;
    }
    while (!(tunnelStatus == newCar.direction && carsInTunnel < maxCarsInTunnel)) {
        pthread_cond_wait(&ok, &trafficLock);
    }
    //Print enter the tunnel message
    enterTunnel(newCar.carNumber, newCar.direction);
    pthread_mutex_unlock(&trafficLock);
    
    //sleep for cross tunnel time
    sleep(newCar.passingTime);
    
    //Exit the tunnel
    exitTunnel(newCar.carNumber);
    
    //Terminate pthread
    pthread_exit(0);
}

void* tunnelThread(void* data) {
    while (done == 0) {
        pthread_mutex_lock(&trafficLock);
        tunnelStatus = "Whittier";
        cout << "The tunnel is now open for Whittier-bound traffic." << endl;
        pthread_cond_broadcast(&ok);
        pthread_mutex_unlock(&trafficLock);
        sleep(5);
        pthread_mutex_lock(&trafficLock);
        tunnelStatus = "NO";
        cout << "The tunnel is now closed to ALL traffic." << endl;
        pthread_mutex_unlock(&trafficLock);
        sleep(5);
        pthread_mutex_lock(&trafficLock);
        tunnelStatus = "Big bear";
        cout << "The tunnel is now open for Bear Valley-bound traffic." << endl;
        pthread_cond_broadcast(&ok);
        pthread_mutex_unlock(&trafficLock);
        sleep(5);
        pthread_mutex_lock(&trafficLock);
        tunnelStatus = "NO";
        cout << "The tunnel is now closed to ALL traffic." << endl;
        pthread_mutex_unlock(&trafficLock);
        sleep(5);
    }
    pthread_exit(0);
}

int main(int argc, const char * argv[]) {
    //Let Xcode I/O redirection comment out when using shell
    freopen("input3a.txt","r",stdin);

    int arriveTime = 0;
    int passingTime = 0;
    int maxCars = 128;
    string direction = "";
    
    cin >> maxCarsInTunnel;
    cout << "Maximum cars in tunnel:"  << maxCarsInTunnel << endl;
    
    //Thread part
    pthread_t tid;
    pthread_t cartid[maxCars];
    pthread_mutex_init(&trafficLock, NULL);
    pthread_cond_init(&ok, NULL);
    
    //Create Tunnel thread
    pthread_create(&tid, NULL, tunnelThread, &done);
    
    int count = 1;
    Car cars[128];
    //read input text
    while (cin >> arriveTime >> direction >> passingTime) {
        cars[count].carNumber = count;
        cars[count].direction = direction;
        (cars[count].direction == "WB") ? (cars[count].direction = "Whittier") : (cars[count].direction = "Big bear");
        cars[count].passingTime = passingTime;
        sleep(arriveTime);
        //Create car threads
        pthread_create(&cartid[count], NULL, carThread, &cars[count]);
        count++;
    }
    //Wait for other car threads
    for (int i=1; i<count; i++) {
        pthread_join(cartid[i], NULL);
    }
    done = 1;
    cout << carCountWB << " car(s) going to Whittier arrived at the tunnel" << endl;
    cout << carCountBB << " car(s) going to Bear Valley arrived at the tunnel" << endl;
    cout << waitingCars << " car(s) were delayed." << endl;
    
    return 0;
}


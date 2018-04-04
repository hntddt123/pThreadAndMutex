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
#include <unistd.h>
#include <pthread.h>
#include <string>

using namespace std;

static int tunnelStatus = 0;
static int sequenceNumber = 0;
static string direction;

void* carThread(void* data) {
    static pthread_mutex_t permit;
    static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;
    pthread_cond_wait(&ok, &permit);
    pthread_mutex_init(&permit, NULL);
    pthread_mutex_lock(&permit);

    cout << "Car #" << sequenceNumber << endl;
    if (tunnelStatus == 0) {
        cout << "going to Whittier arrives at the tunnel." << endl;
    } else {
        cout << "going to Big bear arrives at the tunnel." << endl;
    }
    sequenceNumber++;
    pthread_cond_signal(&ok);
    pthread_mutex_unlock(&permit);
    
    pthread_exit(0);
}

void* tunnelThread(void* data) {
    while (0) {
        tunnelStatus = 1;
        cout << "Bound for Big Bear" << endl;
        sleep(5);
        tunnelStatus = 0;
        cout << "Bound for Whittier" << endl;
        sleep(5);
    }
    pthread_exit(0);
}


int main(int argc, const char * argv[]) {
    
    //Let Xcode I/O redirection comment out when using shell
    freopen("input30.txt","r",stdin);
    int maxCars = 0;
    int startTime = 0;
    int arriveTime = 0;
    int passingTime = 0;
    
    cin >> maxCars;
    cout << "Maximum cars:"  << maxCars << endl;
    while (cin >> arriveTime >> direction >> passingTime) {
//        cout << "Car arrive at: " << arriveTime << endl;
//        if (direction == "WB") {
//            cout << "Car going to Whittier arrives at the tunnel." << endl;
//        }else {
//            cout << "Car going to Big Bear arrives at the tunnel." << endl;
//
//        }
//        cout << "It takes " << passingTime << " sec to pass" << endl;
    }

    //Thread part
    pthread_t tid;
    pthread_t carTid[maxCars];
    //Create Tunnel thread
    pthread_create(&tid, NULL, tunnelThread, &tunnelStatus);
    //create car threads
    for (int i=0; i<maxCars; i++) {
        pthread_create(&carTid[i], NULL, carThread, &sequenceNumber);
    }
    //Wait for other car threads
    for (int i=0; i<maxCars; i++) {
        pthread_join(carTid[i], NULL);
    }
    //wait for tunnel thread
    pthread_join(tid, NULL);
    
    return 0;
}

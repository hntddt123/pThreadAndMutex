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
#include <vector>

using namespace std;

time_t start = time(0);
static int tunnelStatus = 1;
static int maxTunnelCars = 0;
static int whittierBound = 0;
static int bigBearBound = 0;
static int NCars = 0;

struct CarNode {
    int totalCars;
    int carNumber;
    int arriveTime;
    int passingTime;
    string direction;
};

void* carThread(void* carVector) {
    vector<CarNode>* carsPtr = (vector<CarNode>*)carVector;
    vector<CarNode> cars = carsPtr[0];
    static pthread_mutex_t permit;
    static pthread_cond_t ok = PTHREAD_COND_INITIALIZER;
    int secondsPassed;
   
    pthread_mutex_init(&permit, NULL);
    //Critital Section
    pthread_mutex_lock(&permit);
    while (NCars > maxTunnelCars) {
        pthread_cond_wait(&ok, &permit);
    }
    secondsPassed = difftime(time(0), start);
    cout << "Time:" << secondsPassed << endl;
    if (tunnelStatus == 1 && cars[NCars].direction == "WB") {
        cout << "Car #" <<  cars[NCars].carNumber << " going to Whittier enters the tunnel " << endl;
        sleep(cars[NCars].passingTime);
        cout << "Car #" <<  cars[NCars].carNumber << " exits the tunnel " << endl;
        NCars++;
    }else if (tunnelStatus == 0 && cars[NCars].direction == "BB") {
        cout << "Car #" <<  cars[NCars].carNumber << " going to Big bear enters the tunnel " << endl;
        sleep(cars[NCars].passingTime);
        cout << "Car #" <<  cars[NCars].carNumber << " exits the tunnel " << endl;
        NCars++;
    }
    pthread_cond_signal(&ok);
    pthread_mutex_unlock(&permit);
    //End of critital section
    pthread_exit(0);
}

void* tunnelThread(void* data) {
    int* totalCars = (int*) data;
        while (NCars <= *totalCars) {
        tunnelStatus = 1;
        cout << "---Bound for Whittier---" << endl;
        sleep(5);
        tunnelStatus = -1;
        cout << "---No traffic allowed---" << endl;
        sleep(5);
        tunnelStatus = 0;
        cout << "---Bound for Big Bear---" << endl;
        sleep(5);
        tunnelStatus = -1;
        cout << "---No traffic allowed---" << endl;
        sleep(5);
    }
    pthread_exit(0);
}

int main(int argc, const char * argv[]) {
    freopen("input30.txt","r" , stdin);
    
    int totalCars = 0;
    int arriveTime = 0;
    int passingTime = 0;
    string direction;
    vector<CarNode> carVector;
    CarNode car;
    cin >> maxTunnelCars;
    cout << "Maximum cars in tunnel:"  << maxTunnelCars << endl;

    while (!cin.eof()) {
        totalCars++;
        cin >> arriveTime >> direction >> passingTime;
        if (direction == "WB") {
            car.carNumber = totalCars;
            car.arriveTime = arriveTime;
            car.passingTime = passingTime;
            car.direction = direction;
            carVector.push_back(car);
//            cout << "Car #" << totalCars << " going to Whittier arrives at the tunnel at time:" << arriveTime << endl;
//            cout << "It takes " << passingTime << " sec to pass" << endl;
        }else if (direction == "BB"){
            car.carNumber = totalCars;
            car.arriveTime = arriveTime;
            car.passingTime = passingTime;
            car.direction = direction;
            carVector.push_back(car);
//            cout << "Car #" << totalCars << " going to Big Bear arrives at the tunnel at time:" << arriveTime << endl;
//            cout << "It takes " << passingTime << " sec to pass" << endl;
        }
    }
    cout << "Total Cars:" << totalCars << endl;
    for (int i=0; i<totalCars; i++) {
        carVector[i].totalCars = totalCars;
        cout << "Car#" << carVector[i].carNumber << " " << carVector[i].arriveTime << " " << carVector[i].passingTime << " " << carVector[i].direction << endl;
    }
    //Thread part
    pthread_t tid;
    pthread_t carTid[totalCars];
    //Create Tunnel thread
    pthread_create(&tid, NULL, tunnelThread, &totalCars);
    //create car threads
    for (int i=0; i<totalCars; i++) {
        sleep(carVector[i].arriveTime);
        if (carVector[i].direction == "WB") {
            cout << "Car #" <<  carVector[i].carNumber << " going to Whittier arrives at the tunnel " << endl;
        }else if (carVector[i].direction == "BB") {
            cout << "Car #" <<  carVector[i].carNumber << " going to Big bear arrives at the tunnel " << endl;
        }
        pthread_create(&carTid[i], NULL, carThread, &carVector);
    }
    //Wait for other car threads
    for (int i=0; i<totalCars; i++) {
        pthread_join(carTid[i], NULL);
    }
    //wait for tunnel thread
    pthread_join(tid, NULL);
    
    return 0;
}


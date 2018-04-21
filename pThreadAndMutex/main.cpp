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

struct CarNode {
    int carNumber;
    int arriveTime;
    int passingTime;
    string direction;
};

static time_t start = time(0);
static pthread_cond_t ok;
static pthread_cond_t wb;
static pthread_cond_t bb;

static pthread_mutex_t permit;
static vector<CarNode> carVector;
static string tunnelStatus = "";
static int maxTunnelCars = 0;
static int whittierBound = 0;
static int bigBearBound = 0;
static int waitCars = 0;

void* carThread(void* carId) {
    int id = *((int *) carId);
    static int secondsPassed;
    static int carsInTunnel = 0;
    //Print a arrive message
    if (carVector[id].direction == "WB") {
        secondsPassed = difftime(time(0), start);
        cout << "Car #" <<  carVector[id].carNumber << " going to Whittier arrives at the tunnel at time:" << secondsPassed << endl;
        whittierBound++;
    }else if (carVector[id].direction == "BB") {
        secondsPassed = difftime(time(0), start);
        cout << "Car #" <<  carVector[id].carNumber << " going to Big bear arrives at the tunnel at time:" << secondsPassed << endl;
        bigBearBound++;
    }
    //Critital Section
    pthread_mutex_lock(&permit);
    //Wait Until it can enter the tunnel
    while (maxTunnelCars < carsInTunnel) {
        pthread_cond_wait(&ok, &permit);
        waitCars++;
    }
    while ((carVector[id].direction == "WB" && carVector[id].direction != tunnelStatus)) {
        //cout << "Car #" << carVector[id].carNumber << " going to WB have to wait" << endl;
        pthread_cond_wait(&wb, &permit);
    }
    while ((carVector[id].direction == "BB" && carVector[id].direction != tunnelStatus)) {
        //cout << "Car #" << carVector[id].carNumber << " going to BB have to wait" << endl;
        pthread_cond_wait(&bb, &permit);
    }
    //Print enter the tunnel message
    if (carVector[id].direction == "WB") {
        secondsPassed = difftime(time(0), start);
        cout << "Car #" <<  carVector[id].carNumber << " going to Whittier enters the tunnel at time:" << secondsPassed << endl;
        carsInTunnel++;
        //sleep for cross tunnel time
        sleep(carVector[id].passingTime);
    }else if (carVector[id].direction == "BB") {
        secondsPassed = difftime(time(0), start);
        cout << "Car #" <<  carVector[id].carNumber << " going to Big bear enters the tunnel at time:" << secondsPassed << endl;
        carsInTunnel++;
        //sleep for cross tunnel time
        sleep(carVector[id].passingTime);
    }
    //Print exits the tunnel message
    secondsPassed = difftime(time(0), start);
    cout << "Car #" <<  carVector[id].carNumber << " exits the tunnel at time:" << secondsPassed << endl;
    carsInTunnel--;
    pthread_mutex_unlock(&permit);
    pthread_cond_signal(&ok);
    //End of critital section
    //Terminate pthread
    pthread_exit((void*)0);
}

void* tunnelThread(void* data) {
    while (1) {
        tunnelStatus = "WB";
        cout << "---Bound for Whittier---" << endl;
        pthread_cond_broadcast(&wb);
        sleep(5);
        tunnelStatus = "NO";
        cout << "---No traffic allowed---" << endl;
        sleep(5);
        tunnelStatus = "BB";
        cout << "---Bound for Big Bear---" << endl;
        pthread_cond_broadcast(&bb);
        sleep(5);
        tunnelStatus = "NO";
        cout << "---No traffic allowed---" << endl;
        sleep(5);
    }
}

int main(int argc, const char * argv[]) {
    //Let Xcode I/O redirection comment out when using shell
    freopen("input3b.txt","r",stdin);
    
    int arriveTime = 0;
    int passingTime = 0;
    int totalCars = 1;
    string direction;
    CarNode car;
    
    cin >> maxTunnelCars;
    cout << "Maximum cars in tunnel:"  << maxTunnelCars << endl;
    
    //read input text
    while (cin >> arriveTime >> direction >> passingTime) {
        car.carNumber = totalCars;
        car.arriveTime = arriveTime;
        car.passingTime = passingTime;
        car.direction = direction;
        carVector.push_back(car);
        totalCars++;
    }
    totalCars--;
    cout << "Total Cars:" << totalCars << endl;
    for (int i=0; i<totalCars; i++) {
        cout << "Car#" << carVector[i].carNumber << " " << carVector[i].arriveTime << " " << carVector[i].passingTime << " " << carVector[i].direction << endl;
    }
    //Thread part
    pthread_t tid;
    pthread_t carTid[totalCars];
    pthread_mutex_init(&permit, NULL);
    pthread_cond_init(&ok, NULL);
    pthread_cond_init(&wb, NULL);
    pthread_cond_init(&bb, NULL);

    //Create Tunnel thread
    pthread_create(&tid, NULL, tunnelThread, &totalCars);
    //Create car threads
    for (int i=0; i<totalCars; i++) {
        //cout << "Thread:" << carTid[i] << endl;
        sleep(carVector[i].arriveTime);
        int id = i;
        pthread_create(&carTid[i], NULL, carThread, &id);
    }
    //Wait for other car threads
    for (int i=0; i<totalCars; i++) {
        pthread_join(carTid[i], NULL);
    }
    cout << whittierBound << " car(s) going to Whittier arrived at the tunnel" << endl;
    cout << bigBearBound << " car(s) going to Bear Valley arrived at the tunnel" << endl;
    cout << waitCars << " car(s) had to wait because the tunnel was full" << endl;
    
    return 0;
}


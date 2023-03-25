//
// Created by Ryan Moro on 2020-03-01.
//
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include "timetime.h"

using namespace std;
fstream log;

// create log file
void createLog(string logfile) {
    log.open(logfile, fstream::out);
    if(!log){
        cout << "Error in creating file" << endl;
    }
    log.close();
}

// TODO , add host for example ug11.20295
void addClientLogStart(int port, string address, string logfile){
    log.open(logfile, fstream::app);
    if(!log) {
        cout << "\nLOGFILE: |" << logfile << "|" <<endl;
        cout << "Error in opening file!!!" << endl;
    }
    log << "Using port " << port << "\n";
    log << "Using server address " << address << "\n";
    log << "Host " << logfile << "\n"; 
    log.close();
}

void addClientLogEnd(int numtrans, string logfile){
    log.open(logfile, fstream::app);
    if(!log) {
        cout << "\nLOGFILE: |" << logfile << "|" <<endl;
        cout << "Error in opening file!!!" << endl;
    }
    log << "Sent " << numtrans << " transactions\n";
    log.close();
}

void addClientLogSleep(int sleeptime, string logfile){
    log.open(logfile, fstream::app);
    if(!log) {
        cout << "\nLOGFILE: |" << logfile << "|" <<endl;
        cout << "Error in opening file!!!" << endl;
    }
    log << "Sleep " << sleeptime << " units\n";
    log.close();
}

//add log entry to specified file
// event is Send or Recv
void addClientLog(string event = "", int trans_num = 0, string logfile = ""){
    log.open(logfile, fstream::app);
    if(!log) {
        cout << "\nLOGFILE: |" << logfile << "|" <<endl;
        cout << "Error in opening file!!!" << endl;
    }
    string space = "  ";
    string strans_num = to_string(trans_num);
    int snl = strans_num.length();
    for(int i = 0; i < snl-1; i++){
        if(space != ""){
            space.pop_back();
        }
    }
    string trans = "";
    if(event == "Send"){
        trans = "T";
    }
    else {
        trans = "D";
    }
    log << getTime() <<": ";
    log << event << " (";
    log << trans << space << trans_num << ")\n";
    log.close();
}
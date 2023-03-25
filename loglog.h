//
// Created by Ryan Moro on 2020-03-01.
//

#ifndef UNTITLED1_PRODCONLOG_H
#define UNTITLED1_PRODCONLOG_H

#endif //UNTITLED1_PRODCONLOG_H

#include <string>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

//display contents of log
void displayLog(string logfile);

//create log file
void createLog(string logfile);

void addClientLogStart(int port, string address, string logfile);

void addClientLogEnd(int numtrans, string logfile);

void addClientLogSleep(int sleeptime, string logfile);

//add log entry to specified file
// event is Send or Recv
void addClientLog(string event = "", int trans = 0, string logfile = "");

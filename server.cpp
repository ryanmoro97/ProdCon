#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h> 
#include <arpa/inet.h>     
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h>  
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include "tands.h"
#include "loglog.h"
#include "timetime.h"

using namespace std;

// time struct for socket timeout  
struct timeval to;

int main(int argc , char *argv[]){   
    int addrlen, new_socket, client_socket[10],  
        max_clients = 10, activity, i, valread , sd,
        trans_num = 0, port = atoi(argv[1]), max_sd;
    struct sockaddr_in serv_addr;   
    char buffer[1025];  
    vector<string> hostnames;
    // array to count num of trans from each client
    int host_trans_nums[max_clients];
    for(int i = 0; i < max_clients; i++){
        host_trans_nums[i] = 0;
    }
    // last transaction time
    string start, end;
    // set of socket descriptors for select
    fd_set readfds;     
    // initialize all client_sockets
    for(i = 0; i < max_clients; i++){   
        client_socket[i] = 0;   
    }   
    int listenfd = 0;
    // create a master socket  
    listenfd = socket(AF_INET , SOCK_STREAM , 0);
    // create socket  
    serv_addr.sin_family = AF_INET;   
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);   
    serv_addr.sin_port = htons(port);   
         
    // bind the socket to port  
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));      
    // max 10 pending socket connections  
    listen(listenfd, 10);
    // display server details
    printf("Using port : %d  \n" , ntohs (serv_addr.sin_port));
         
    addrlen = sizeof(serv_addr);   
    // handle the incoming connection  
    while(1){   
        //clear the socket set  
        FD_ZERO(&readfds);   
        //add master socket to set  
        FD_SET(listenfd, &readfds);   
        max_sd = listenfd;   
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++){   
            //socket descriptor  
            sd = client_socket[i];   
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET(sd ,&readfds);   
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
        
        //wait for an activity on one of the sockets , timeout is 60 sec
        to.tv_sec = 60;
        to.tv_usec = 0;
        activity = select(max_sd + 1 ,&readfds ,NULL ,NULL ,&to);   
        if((activity < 0) && (errno!=EINTR)){  
            continue;   
        }
        if(activity == 0){
            printf("No activity for 60 sec, exiting\n");
            break;
        }   

        // detect incoming transaction
        if(FD_ISSET(listenfd, &readfds)){   
            if((new_socket = accept(listenfd,  
              (struct sockaddr *)&serv_addr, (socklen_t*)&addrlen))<0){   
                cout << "Accept error" << endl;  
            }   
               
            // add new socket to array of sockets  
            for (i = 0; i < max_clients; i++){   
                // if position is empty  
                if( client_socket[i] == 0 ){   
                    client_socket[i] = new_socket;   
                    break;   
                }
                else if(client_socket[i] == -1){
                   continue;
                } 
            }   
        }   
             
        // if socket is already connected - read & execute the transaction 
        for(i = 0; i < max_clients; i++){   
            sd = client_socket[i];  
            if(FD_ISSET(sd ,&readfds)){   
                // read message
                // if nothing is read then close the socket
                if((valread = read(sd, buffer, 1024)) == 0){   
                    // cout << "closing" << endl;
                    // Close the socket and mark as -1 so that the same machine can run another client with a new log file  
                    FD_CLR(sd, &readfds);
                    close(sd);   
                    client_socket[i] = -1;   
                    shutdown(sd, SHUT_RDWR);
                } 
                // execute trans and notify client when done 
                else{   
                    buffer[valread] = '\0';   
                    if(buffer[0] == 'T'){
                        // first trans, get start time
                        if(trans_num == 0){
                            start = getTime();
                        }
                        // count total transactions
                        trans_num++;
                        // count each hosts transactions
                        host_trans_nums[i]++;
                        // get full trans execution value 
                        string sn = string(&buffer[1], &buffer[strlen(buffer)]);
                        // string formatting
                        string space = "  ";
                        int snl = sn.length();
                        if(sn.at(sn.length()-1) == ' '){
                            sn.pop_back();
                            snl --;
                        }
                        for(int i = 0; i < snl-1; i++){
                            if(space != ""){
                                space.pop_back();
                            }
                        }
                        // display server work
                        cout << getTime() << ": # " << trans_num << " (T" << space << sn << ") from " << hostnames.at(i) << endl;
                        // execute transaction
                        int n = stoi(sn);
                        Trans(n);
                        // display server done work
                        end = getTime();
                        cout << end << ": # " << trans_num << " (Done) from " << hostnames.at(i) << endl; 
                        // convert trans # to cstr to send over socket (Done)                   
                        string tnum = "D" + to_string(trans_num);
                        char cstr[tnum.size()+1];
                        strcpy(cstr, tnum.c_str());    
                        send(sd, cstr, strlen(cstr), 0);   
                    }
                    else{
                        // initial connection message, add name to vector
                        // will correspond to i value in array and #trans in host_trans_nums
                        hostnames.push_back(buffer);
                    }
                }   
            }   
        }   
    }

    cout << "\nSUMMARY\n";
    string space;
    string host_trans_num;
    // shutdown all connections and display info from each one
    for(i = 0; i < hostnames.size(); i++){ 
        space = "   ";
        host_trans_num = to_string(host_trans_nums[i]);
        for(int i = 0; i < host_trans_num.length()-1; i++){
            if(space != ""){
                space.pop_back();
            }
        }
        cout << space << host_trans_nums[i] << " transactions from " << hostnames.at(i) << endl;
        sd = client_socket[i]; 
        close(sd);
        shutdown(sd, SHUT_RDWR);
    }
    if(trans_num != 0){
        double elapsed = stod(end) - stod(start);
        double tpersec = trans_num/elapsed;
        cout << setprecision (1) << fixed << tpersec << " transactions/sec ";
        cout << "(" << trans_num << "/" << setprecision (2) << fixed << elapsed << ")" << endl;
    }
    else{
        cout << "No transactions" << endl;
    }  

    return 0;   
}   
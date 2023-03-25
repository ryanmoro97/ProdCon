#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <string.h> 
#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>
#include "loglog.h"
#include "tands.h"
#include "timetime.h"

using namespace std;

// read instructions from cmd line or input file
// T<n> -send to server to execute
        // client is idle until server completes trans
// Record trans calls in log

// S<n> - client sleeps
// record sleep 

// Each client maintains a log file that has the name machinename.pid

int main(int argc, char *argv[]){
    int sockfd = 0;
    int port = atoi(argv[1]);
    string address = argv[2];
    char recvBuff[1024];
    char buffer[1025];
    char hostbuf[256];
    int pid = getpid();
    char apid[6];
    sprintf(apid, "%d", pid);
    string spid(apid);

    // filename for log
    // string logfile;
    struct sockaddr_in serv_addr; 
    // string hostname;
    gethostname(hostbuf, sizeof(hostbuf)); 
    string shostbuf(hostbuf);
    string logfile = shostbuf + "." + spid;
    cout << "hostname: " << logfile << endl;
    // create log file with hostname
    createLog(logfile);
    addClientLogStart(port, address, logfile);

    memset(recvBuff, '0',sizeof(recvBuff));
    // create socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 
    // set address family 
    serv_addr.sin_family = AF_INET;
    // assign port no
    serv_addr.sin_port = htons(port); 

    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0){
        printf("\n inet_pton error occured\n");
        return 1;
    } 
    // connect to the socket
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    // send connection indicator to server of clients hostname for logging
    char msg[logfile.size()+1];
    strcpy(msg, logfile.c_str());
    send(sockfd, msg, strlen(msg), 0 );

    // if there is no input file then read arguments from keyboard
    // EOF from keyboard is ctrl-D
    vector <string> inputs;
    string input;
    int trans_num = 0;
    // collect commands from file or cmd line until EOF/ CTRL-D
    while(getline(cin, input)){
        cout <<"input: " << input << endl;
        int n = stoi(string(&input[1], &input[input.length()]));
        if(input[0] == 'S'){
            addClientLogSleep(n, logfile);
            Sleep(n);
        }
        else if(input[0] == 'T'){
            trans_num++;
            // get full transaction 'n' value and send it to server to execute
            char cstr[input.size()+1];
            strcpy(cstr, input.c_str());
            addClientLog("Send", n, logfile);
            send(sockfd , cstr , strlen(cstr) , 0 ); 
            // wait for servers acknowledgement of work done (once a value is read)
            int valread = read(sockfd, buffer, 1024); 
            addClientLog("Recv", n, logfile);
            buffer[valread] = '\0'; 
            // printf("t#: %s\n",buffer );
        }
        else{
            printf("Invalid input\n");
        }
    }
    // notify server that client is done

    // send to log file
    addClientLogEnd(trans_num, logfile);
    // close socket
    close(sockfd);
    shutdown(sockfd, SHUT_RDWR);
    return 0;
}

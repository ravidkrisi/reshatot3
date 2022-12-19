/*
    TCP/IP-server
*/

#include <stdio.h>
#include <sys/time.h>

// Linux and other UNIXes
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 5060  // The port that the server listens
#define BUFFER_SIZE 1024

void write_file(int clientsocket)
{
    char buffer[BUFFER_SIZE];
    char end[BUFFER_SIZE]={'\0'};
    int n;
      while (1)
       {
        n = recv(clientsocket, buffer, BUFFER_SIZE, 0);
        if(strcmp(end, buffer)==0){
            break;
        } 
             if (n <= 0)
             {
                printf("%d",n);
                 break;
                 return;
             }
         printf("%s", buffer);
        bzero(buffer, BUFFER_SIZE);
    }
  return;
  }

int main() {
    // signal(SIGPIPE, SIG_IGN);  // on linux to prevent crash on closing socket

    // Open the listening (server) socket
    int listeningSocket = -1;
    listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 0 means default protocol for stream sockets (Equivalently, IPPROTO_TCP)
    if (listeningSocket == -1) {
        printf("Could not create listening socket : %d\n", errno);
        return 1;
    }

    // Reuse the address if the server socket on was closed
    // and remains for 45 seconds in TIME-WAIT state till the final removal.
    //
    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0) {
        printf("setsockopt() failed with error code : %d\n", errno);
        return 1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; // any IP at this port (Address to accept any incoming messages)
    serverAddress.sin_port = htons(SERVER_PORT);  // network order (makes byte order consistent)

    // Bind the socket to the port with any IP at this port
    int bindResult = bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1) {
        printf("Bind failed with error code : %d\n", errno);
        // close the socket
        close(listeningSocket);
        return -1;
    }

    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    // 500 is a Maximum size of queue connection requests
    // number of concurrent connections
    int listenResult = listen(listeningSocket, 3);
    if (listenResult == -1) {
        printf("listen() failed with error code : %d", errno);
        // close the socket
        close(listeningSocket);
        return -1;
    }

    // Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");
    struct sockaddr_in clientAddress;  //
    socklen_t clientAddressLen = sizeof(clientAddress);

    while (1) {
        memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (clientSocket == -1) {
            printf("listen failed with error code : %d", errno);
            // close the sockets
            close(listeningSocket);
            return -1;
        }

        printf("A new client connection accepted\n");

        // Receive a message from client
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        //set var to start and stop time. record time for start time
        struct timeval stop, start;
        gettimeofday(&start, NULL);
        int n;

        //receiving the first part of the file 
        write_file(clientSocket);
        //record time for stop time 
        gettimeofday(&stop, NULL);

        //print message of the time it took to receive the first file
        printf("before sending part 2");
        // printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 10000 + stop.tv_usec - start.tv_usec);

        //set authentication in a string 
        char *message = "110100000001100111100110\n";
        int messageLen = strlen(message) + 1;

        // //send the sender the authentication 
        // int bytesSent = send(clientSocket, message, messageLen, 0);
        // if (bytesSent == -1) {
        //     printf("send() failed with error code : %d", errno);
        //     close(listeningSocket);
        //     close(clientSocket);
        //     return -1;
        // } else if (bytesSent == 0) {
        //     printf("peer has closed the TCP connection prior to send().\n");
        // } else if (bytesSent < messageLen) {
        //     printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
        // } else {
        //     printf("message was successfully sent.\n");
        // }

        //record time of start time for sencond part of the file 
        gettimeofday(&start, NULL);
        printf("before sending part 2");
        //receiving the sencond part of the file 
        // write_file(clientSocket);


        //record stop time of the sencond part of the file 
        gettimeofday(&stop, NULL);

        //print the time it took to receive the second part of the file
        // printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 10000 + stop.tv_usec - start.tv_usec);
    }

    close(listeningSocket);

    return 0;
}

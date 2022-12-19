#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/tcp.h>

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"

#define BUFFER_SIZE 1024

void send_file(FILE *fp, int sockfd){
    int n=0;
    char data[BUFFER_SIZE] = {0};

    while(fgets(data, BUFFER_SIZE, fp) != NULL) 
    {
        if (send(sockfd, data, sizeof(data), 0) == -1) 
        {
        perror("[-]Error in sending file.");
        exit(1);
        }
        bzero(data, BUFFER_SIZE);

    }
    printf("i got here");
    char end[BUFFER_SIZE]={'\0'};;
    send(sockfd, end, sizeof(end), 0);
}
int main() {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == -1) {
        printf("Could not create socket : %d", errno);
        return -1;
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);                                              // (5001 = 0x89 0x13) little endian => (0x13 0x89) network endian (big endian)
    int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress.sin_addr);  // convert IPv4 and IPv6 addresses from text to binary form
    // e.g. 127.0.0.1 => 0x7f000001 => 01111111.00000000.00000000.00000001 => 2130706433
    if (rval <= 0) {
        printf("inet_pton() failed");
        return -1;
    }

    // Make a connection to the server with socket SendingSocket.
    int connectResult = connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1) {
        printf("connect() failed with error code : %d", errno);
        // cleanup the socket;
        close(sock);
        return -1;
    }

    printf("connected to server\n");

    // open the file and split the file for 2 equal strings
    FILE *file;
    file = fopen("ex3", "r");
    if(file == NULL)
    {
        printf("couldnt load the file");
    }
     
    fseek(file,0,SEEK_END);
    int length = ftell(file);
    fseek(file,0,SEEK_SET);

    int length1 = length/2;
    int length2 = length-length1;

    FILE *part1, *part2;
    part1 = fopen("part1.txt", "w+");
    part2 = fopen("part2.txt", "w+");

    char c;
    int i =0;
    while((c=fgetc(file))!=EOF)
    {
        if(i<=length1)
        {
        fprintf(part1,"%c", c);
        }
        else
        {
        fprintf(part2,"%c", c);
        }
        i++;
    }
   
   fseek(part1,0,SEEK_SET);
   fseek(part2,0,SEEK_SET);

    //set the cc algorithm to CUBIC 
    // if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, "cubic", 5) < 0)
    // {
    //   printf("set socket error from client\n");
    // }

    //send the first part of the file to the receiver 

    send_file(part1, sock);
    
    // Receive authentication from receiver
    // char bufferReply[BUFFER_SIZE] = {'\0'};
    // int bytesReceived = recv(sock, bufferReply, BUFFER_SIZE, 0);
    // if (bytesReceived == -1) {
    //     printf("recv() failed with error code : %d", errno);
    // } else if (bytesReceived == 0) {
    //     printf("peer has closed the TCP connection prior to recv().\n");
    // } else {
    //     printf("authentication: %s", bufferReply);
    // }

    // set the cc algorithm to RENO 
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, "reno", 4) < 0)
        {
            printf("set socket error from client\n");
        }

    // send second part of the file to the receiver 
    // send_file(part2, sock);

    close(sock);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define client_port 25422
//cite the code from http://www.bogotobogo.com/cplusplus/sockets_server_client.php

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    printf("The client is up and running.\n");
    char receive[256];
    char function[256], input[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    while(1){
    portno = client_port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please search or prefix word:");
    memset(input,0,sizeof(input));
    scanf("%s %s", function,input);
    n = write(sockfd, input, strlen(input));
    printf("the client sent<%s>and<%s>to AWS\n",input,function);
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(receive,256);
    n = read(sockfd, receive, 256);
    if (n < 0) 
         error("ERROR reading from socket");
    if(strcmp(receive,"")!=0)
       printf("found a match for <%s> : %s\n",input,receive);
    else 
       printf("found no match for <%s>\n",input);
	}
    close(sockfd);
    return 0;
}
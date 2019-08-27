#define _BSD_SOURCE
#define client_port 25422
#define aws_port 24422
#define serverA_port  21422
#define serverB_port 22422
#define serverC_port 23422
#define monitor_port "26438" 
#define BACKLOG 10 

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>

//cite the code from http://beej-zhcn.netdpi.net/06-client-server-background/ and http://www.bogotobogo.com/cplusplus/sockets_server_client.php


void error(const char *msg)
{
    perror(msg);
    exit(1);
}
void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(int argc, char *argv[])
{
     printf("The AWS is up and running.\n");
     int sockfd, clientfd, portno;
     socklen_t clilen;
     char input[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

    int monitorfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd =  socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno =  atoi(argv[1]);

     serv_addr.sin_family = AF_INET;  

     serv_addr.sin_addr.s_addr = INADDR_ANY;  

     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

    if ((rv = getaddrinfo(NULL,monitor_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
     }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((monitorfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(monitorfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(monitorfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(monitorfd);
            perror("server: bind");
            continue;
        }

        break;
    }

     freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    while(1){ 
    listen(sockfd,5);

   
    if (listen(monitorfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    //printf("server: waiting for connections...\n");

     clilen = sizeof(cli_addr);
     clientfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, &clilen);
     if (clientfd < 0) 
          error("ERROR on accept");

      //printf("server: got connection from %s port %d\n",
      //      inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

     bzero(input,sizeof(input));
     n = read(clientfd,input,sizeof(input));
     if (n < 0) error("ERROR reading from socket");
     printf("The AWS received input=<%s>from the client using TCP over port %d\n",input,ntohs(cli_addr.sin_port));

     char result[256];
     bzero(result,sizeof(result));
     int serverA =socket(AF_INET,SOCK_DGRAM,0);
     if(serverA<0){
     perror("wrong socket");
     return 1;
     }
     struct sockaddr_in reAddrA;
     int lenA;
     int addr_lenA=sizeof(struct sockaddr_in);
     char receiveA[256]={""};
     memset(&reAddrA,0,sizeof(reAddrA));
     reAddrA.sin_family=AF_INET;
     reAddrA.sin_port=htons(serverA_port);
     reAddrA.sin_addr.s_addr=inet_addr("127.0.0.1");
     sendto(serverA,input,sizeof(input),0,(struct sockaddr*)&reAddrA,addr_lenA);
     printf("The AWS send<%s>to backend-server A\n", input);
     bzero(receiveA,sizeof(receiveA));
     lenA =recvfrom(serverA,receiveA,sizeof(receiveA),0,(struct sockaddr*)&reAddrA,(socklen_t*)&addr_lenA);
     printf("The AWS received:<%s>frome backend-serverA using UDP over port\n",receiveA);
     if(strcmp(receiveA,"")!=0)
     {bzero(result,sizeof(result));
     strcpy(result,receiveA);}
     close(serverA);

     int serverB =socket(AF_INET,SOCK_DGRAM,0);
     if(serverB<0){
     perror("wrong socket");
     return 1;
     }
     struct sockaddr_in reAddrB;
     int lenB;
     int addr_lenB=sizeof(struct sockaddr_in);
     char receiveB[256];
     memset(&reAddrB,0,sizeof(reAddrB));
     reAddrB.sin_family=AF_INET;
     reAddrB.sin_port=htons(serverB_port);
     reAddrB.sin_addr.s_addr=inet_addr("127.0.0.1");
     sendto(serverB,input,sizeof(input),0,(struct sockaddr*)&reAddrB,addr_lenB);
     printf("The AWS send<%s>to backend-server B\n", input);
     bzero(receiveB,sizeof(receiveB));
     lenB=recvfrom(serverB,receiveB,sizeof(receiveB),0,(struct sockaddr*)&reAddrB,(socklen_t*)&addr_lenB);
     printf("The AWS received:<%s>frome backend-serverB using UDP over port\n",receiveB);
     if(strcmp(receiveB,"")!=0)
     {bzero(result,sizeof(result));
      strcpy(result,receiveB);}
     close(serverB);

     int serverC =socket(AF_INET,SOCK_DGRAM,0);
     if(serverC<0){
     perror("wrong socket");
     return 1;
     }
     struct sockaddr_in reAddrC;
     int lenC;
     int addr_lenC=sizeof(struct sockaddr_in);
     char receiveC[256];
     memset(&reAddrC,0,sizeof(reAddrC));
     reAddrC.sin_family=AF_INET;
     reAddrC.sin_port=htons(serverC_port);
     reAddrC.sin_addr.s_addr=inet_addr("127.0.0.1");
     sendto(serverC,input,sizeof(input),0,(struct sockaddr*)&reAddrC,addr_lenC);
     printf("The AWS send<%s>to backend-server C\n", input);
     bzero(receiveC,sizeof(receiveC));
     lenC =recvfrom(serverC,receiveC,sizeof(receiveC),0,(struct sockaddr*)&reAddrC,(socklen_t*)&addr_lenC);
     printf("The AWS received:<%s>frome backend-serverC using UDP over port\n",receiveC);
     if(strcmp(receiveC,"")!=0)
     {bzero(result,sizeof(result));
      strcpy(result,receiveC);}
     close(serverC);


     send(clientfd,result,sizeof(result),0);
     printf("The AWS sent matches to client\n");
     sin_size = sizeof their_addr;
        new_fd = accept(monitorfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        //printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(monitorfd); // child doesn't need the listener
            if (send(new_fd,input,sizeof(input),0) == -1)
                perror("send");
            printf("The AWS sent <%s> to the monitor via TCP port 26438\n",input);
            exit(0);           
        }
      }
        close(new_fd);
        close(clientfd);
        close(sockfd);
     return 0; 
     
}

#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  
#include <ctype.h>
#define serverC_port 23422
//cite the code from http://lobert.iteye.com/blog/1769618

int main(int argc, char *argv[]){  
    int sockfd,len;  
    struct sockaddr_in addr;  
    int addr_len = sizeof(struct sockaddr_in);  
    char buffer[256];  
    char line[256];
    char defn[256]={""};
    int m,n;
    if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){  
        perror ("socket");  
        exit(1);  
    }   
    bzero ( &addr, sizeof(addr) );  
    addr.sin_family=AF_INET;  
    addr.sin_port=htons(serverC_port);  
    addr.sin_addr.s_addr=htonl(INADDR_ANY) ;  
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))<0){  
        perror("connect");  
        exit(1);  
    } 
    printf("The server C is up and running using UDP on port 23422 \n");
    while(1){  
        bzero(buffer,sizeof(buffer)); 
        bzero(defn,sizeof(defn)); 
        len = recvfrom(sockfd,buffer,sizeof(buffer), 0 , (struct sockaddr *)&addr ,(socklen_t*)&addr_len);   
        //printf("receive from %s\n" , inet_ntoa(addr.sin_addr));  
        printf("The serverC received input<%s>\n",buffer);
        FILE *fp = fopen("backendC.txt","r");
        if((fp = fopen("backendC.txt","r")) == NULL) //
        { 
        printf("error!"); 
         return -1; 
        } 
        //printf("%s\n",buffer);
        while (!feof(fp)) 
        { 
          fgets(line,256,fp);  //
          if(strncasecmp(line,buffer,strlen(buffer))==0)
          {
            //printf("%s\n", line); //
            strcpy(defn,line+4+strlen(buffer));
            //printf("%s\n",defn);
            m=1;
            break;
          }
          else 
            m=0;
        } 
        fclose(fp); 
        printf("The serverC has found <%d> match\n", m);
        sendto(sockfd,defn,len,0,(struct sockaddr *)&addr,addr_len);  
        printf("The serverC finished sending the output to AWS\n");
    }  
    close(sockfd);
    return 0;
} 
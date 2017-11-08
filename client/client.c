#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1025];
    char fileName[1025] ;
    char fileValue[1025] ;
    struct sockaddr_in serv_addr;
    if(argc != 2)
    {
        printf("\n Usage: %s <username:password@serverip> \n",argv[0]);
        return 1;
    }
    char *user_pass,*serverip;
    user_pass = strtok(argv[1], "@");
    serverip = strtok( NULL, "@");
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5200);
    serv_addr.sin_addr.s_addr = inet_addr(serverip);

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    send( sockfd , user_pass , strlen(user_pass) ,0);

    char Auth[2];
    int au = recv( sockfd, Auth , sizeof(Auth)-1, 0 );
    if( au < 0)
        printf("\n Read error r\n");

    n = recv(sockfd, recvBuff, sizeof(recvBuff)-1,0);
    recvBuff[n] = 0;
    printf( "%s" , recvBuff );
    if(n < 0)
    {
        printf("\n Read error \n");
    }

    if( strcmp(Auth, "0" ) == 0 )
    {
        close(sockfd);
        return 0;
    }
    printf( "\nEnter the name of file to fetch from server \n" );
    scanf("%s" , fileName );
    send( sockfd, fileName , sizeof(fileName) -1 ,0);

    char fileFound[2];
    int r = recv( sockfd, fileFound , sizeof(fileFound)-1 , 0);
    fileFound[r] = 0;
    if( r < 0)
        printf("\n Read error r\n");

    int  m = recv(sockfd, fileValue , sizeof(fileValue)-1,0);
    fileValue[m] = 0;
    if(m < 0)
    {
        printf("\n Read error m\n");
    }

    if( strcmp(fileFound, "1" ) == 0 )
    {
        FILE *fp;
        fp=fopen( fileName ,"w");
        fprintf( fp ,"%s", fileValue );
        printf("the file was received successfully");
    }
    else
    {
        printf("%s" , fileValue); 
    }
    close( sockfd);
    return 0;
}


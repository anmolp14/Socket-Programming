#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h> 

int main(int argc, char *argv[])
{
   int listenfd = 0, connfd = 0;
   struct sockaddr_in serv_addr;

   listenfd = socket(AF_INET, SOCK_STREAM, 0);

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_addr.sin_port = htons(5200);

   bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
   listen(listenfd, 10);
   while(1)
   {
      connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
      char sendBuff[1025] = "No such username found!!" ;
      char recvBuff[1025] ;
      char fileName[1025];

      int  n = recv( connfd, recvBuff, sizeof(recvBuff)-1,0);
      recvBuff[n] = 0;
      if(n < 0)
      {
         printf("\n Read error \n");
         return 0;
      }
      /*printf( "%s" , recvBuff);*/
      char *user, *pass;
      user = strtok(recvBuff , ":");
      pass = strtok(NULL , ":");
      FILE* userf  = fopen("./users.txt" , "r" );
      if (userf == NULL)
      {
         printf("no such file.");
         return 0;
      }
      int flag = 0;
      char fuser[513]; char fpass[513];
      while( fscanf(userf, "%s %s", fuser, fpass) > 0   )
      {
         if( strcmp( fuser , user)  == 0 )
         {
            if( strcmp( fpass ,  pass) == 0  )
            {
               send( connfd, "1", strlen("1") , 0 );
               char mesge[] = "HELLO " ;
               strcat( mesge , fuser );
               send(connfd, mesge , strlen(mesge), 0);
               int  m = recv( connfd, fileName, sizeof(fileName)-1,0);
               fileName[m] = 0;
               if(m < 0)
               {
                  printf("\n Read error \n");
                  return 0;
               }
               DIR           *d;
               struct dirent *dir;
               d = opendir(".");
               int fl=0;
               while ((dir = readdir(d)) != NULL)
               {
                  if( strcmp( fileName , dir->d_name) == 0 )
                  {
                     send( connfd, "1", strlen("1") , 0 );
                     fl  = 1;
                     char buffer[1025];
                     FILE *f;
                     f=fopen(fileName,"r");
                     fseek(f, 0, SEEK_SET);
                     fread(buffer, sizeof(buffer), 1, f);
                     // printf("%s\n", buffer);
                     send(connfd, buffer , strlen(buffer), 0);
                     memset( buffer , 0, sizeof(buffer) );

                     fclose(f);
                     printf("the file %s was sent successfully\n", fileName);
                     break;
                  }
               }
               if( fl == 0 )
               {
                  send( connfd, "0", strlen("0") , 0 );
                  send(connfd, "File Not Found" , strlen("File Not Found"), 0);
               }

               closedir(d);

            }
            else
            {
               send( connfd, "0", strlen("0") , 0 );
               send(connfd, "Authentication Failure!!!" , strlen( "Authentication Failure!!!" ),0);
            }
            flag = 1;
            break;
         }
      }
      if( !flag )
      {
         send( connfd, "0", strlen("0") , 0 );
         send(connfd, sendBuff, strlen(sendBuff),0);
      }
      fclose(userf);
      close(connfd);
   }
   close(listenfd);
}

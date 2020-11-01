#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#include <sys/types.h>  //sendto
#include <sys/socket.h> //sendto

#define DEFAULT_BUFLEN      512
#define DEFAULT_CLIENT_PORT     27016
#define DEFAULT_SERVER_PORT     27015
#define NUM_OF_PAR      7

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server, client;
    char message[DEFAULT_BUFLEN];

        //Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            printf("Could not create socket");
        }
        puts("Socket created");

        server.sin_addr.s_addr = inet_addr("ip_address");
        server.sin_family = AF_INET;
        server.sin_port = htons(DEFAULT_SERVER_PORT);

        //Connect to remote server
        if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            perror("connect failed. Error");
            return 1;
        }

        puts("Connected\n");

    int array[] = {1, 12, 3, 3, 300, 5, 3};

    printf("Sending array\n");
    for(int i = 0; i < NUM_OF_PAR; i++){
        printf("%d  ", array[i]);       
    }

    if(send(sock, array, NUM_OF_PAR, 0) < 0)    
    {
        printf("\nArray is not successfully sent\n");
    }
    else
    {
        printf("\nArray is sent\n");
    }

    printf("\n");

        close(sock);

        return 0;
}
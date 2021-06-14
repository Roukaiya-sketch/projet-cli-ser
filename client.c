#include <netdb.h> 
#include <stdio.h>   
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX 100
#define PORT 8080 
#define SA struct sockaddr
#define FIFO_FILE_1  "/tmp/client_to_server_fifo"
#define FIFO_FILE_2  "/tmp/server_to_client_fifo"



void recvFile(int sockfd) 
{ 
 	char buff[MAX];  
 
 	FILE *fp;
 	fp=fopen("Received.txt","w"); 
 
 	if( fp == NULL )
	{
  		printf("Error IN Opening File ");
  		return ;
 	}
 
 	while( read(sockfd,buff,MAX) > 0 )
  	fprintf(fp,"%s",buff);
 
 	printf("Fichier reçu avec succès! \n");
 	printf("le nouveau fichier est received.txt! \n");

}

void list(int sockfd)
{	
	char buff[MAX];
	while( read(sockfd,buff,MAX) > 0 )
  	printf("%s \n",buff);
}


int main() 
{ 

 	int sockfd, connfd; 
 	struct sockaddr_in servaddr, cli; 
 	sockfd = socket(AF_INET, SOCK_STREAM, 0); 

 	if (sockfd == -1) 
	{ 
  		printf("socket creation failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("Socket successfully created..\n"); 
 
 	bzero(&servaddr, sizeof(servaddr)); 
 	servaddr.sin_family = AF_INET; 
 	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
 	servaddr.sin_port = htons(PORT);  
 	
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
	{ 
  		printf("connection with the server failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("connected to the server..\n");
 
	system("clear");
    	int client_to_server;
    	int server_to_client;

    	char str[240];
	
	
	printf(" \n");
	printf("Choisisez un numero,svp : \n");
	printf(" \n");
	printf("1) Obtenir un document disponible sur le serveur \n");
	printf("2) Obtenir la liste des documents disponibles sur le serveur \n");
	printf("3) Supprimer de la liste un document du serveur \n");
	printf(" \n");
	printf("Votre numero,svp: ");
	fgets(str,sizeof(str),stdin);
    	str[strlen(str)-1] = '\0';
	
	
    	/* write str to the FIFO */
    	client_to_server = open(FIFO_FILE_1, O_WRONLY);
    	server_to_client = open(FIFO_FILE_2, O_RDONLY);

	write(client_to_server, str, sizeof(str));
	
	char str2[240];

	int temp;
	temp = *str; 

	if (temp == 49)
	{
		printf("\n");
		
		printf("le nom du document vouz voulez recuperer : ");
		fgets(str2,sizeof(str2),stdin);
    		str2[strlen(str2)-1] = '\0';
	
		write(client_to_server, str2, sizeof(str2));
		
		recvFile(sockfd); 
 		close(sockfd); 
		
	}
	
	else if (temp == 50)
	{
                printf("List of files on Server");
		list(sockfd); // Calling the function to display the files
	}

	else if (temp == 51) //Delete file option
	{
		printf("\n");
		
		printf("le nom de document vouz souhaitez supprimer : ");
		fgets(str2,sizeof(str2),stdin);
    		str2[strlen(str2)-1] = '\0';
	
		write(client_to_server, str2, sizeof(str2));
		
		char bufff[1000];
		read(client_to_server, bufff, BUFSIZ);
		if (*bufff == 51)
			printf("Failed to delete file\n");
		else 
			printf("Supprimé avec succès\n");
	}

	else
	{
		printf("\n");		
		printf("\033[1;31m");
		printf("Vous avez entré une valeur invalide! \n");
		printf("\033[0m");
	} 	
	
	close(client_to_server);
    	close(server_to_client);

	
}

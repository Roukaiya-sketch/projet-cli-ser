#include <stdlib.h> // for basic memmory allocation and deallocation
#include <stdio.h> // for file read and write
#include <netdb.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#define MAX 100
#define PORT 8080 
#define SA struct sockaddr
#define FIFO_FILE_1  "/tmp/client_to_server_fifo"
#define FIFO_FILE_2  "/tmp/server_to_client_fifo"

void sentFile(int sockfd, char *mystring)
{ 
	char buff[MAX]; 

	FILE *fp;
 	fp=fopen(mystring,"r");  
	if( fp == NULL )
	{
 		printf("Error IN Opening File .. \n");
  		return ;
 	}
 
 	while ( fgets(buff,MAX,fp) != NULL ) 
  	write(sockfd,buff,sizeof(buff));  

 	fclose (fp);       
 
 	printf("Fichier envoyé avec succès!! \n"); 
}

void list(int sockfd)
{
	struct dirent *de;  
	

	char cwd[PATH_MAX];
  	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		
   		DIR *dr = opendir(cwd); 
  
    		if (dr == NULL)  
    		{ 
        		printf("Could not open current directory" ); 
    		}

		char buff1[MAX]; 

    		while ((de = readdir(dr)) != NULL)
		{
            		write(sockfd,de->d_name,sizeof(buff1));
  		}
    		closedir(dr);

	} 
	else 
	{
       		perror("getcwd() error");
   	}
   
}

void deletefile(int sockfd, char *mystring1) 
{
   	if (remove(mystring1) == 0)
	{
		char output[1000]= "1"; 
		printf("Deleted successfully \n");
		write(sockfd,output,sizeof(output));
	} 
   	else
	{
		char output[1000]= "2"; 
      		printf("Unable to delete the file \n");
		write(sockfd,output,sizeof(output));
	}
} 


int main() 
{
 
	int client_to_server;
    	int server_to_client;   	
	char buf[BUFSIZ];
	char buf1[1000];
	char buf2[1000];
	
    	
    	mkfifo(FIFO_FILE_1, 0666);
    	mkfifo(FIFO_FILE_2, 0666);

	int sockfd, connfd, len;    
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
 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
 	servaddr.sin_port = htons(PORT);   
 	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) 
	{ 
		printf("socket bind failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("Socket successfully binded...\n");
 	
	if ((listen(sockfd, 5)) != 0) 
	{ 
  		printf("Listen failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("Server listening...\n"); 
 
 	len = sizeof(cli);  
 	connfd = accept(sockfd, (SA*)&cli, &len);  
 
 	if (connfd < 0) 
	{ 
  		printf("server acccept failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("server acccept the client...\n"); 
	
	
	client_to_server = open(FIFO_FILE_1, O_RDONLY);
       	server_to_client = open(FIFO_FILE_2, O_WRONLY);

        read(client_to_server, buf, BUFSIZ);

	int temp;
	temp = *buf; 
	printf("\n");

	if (temp == 49)
	{
		read(client_to_server, buf1, 1000);
		printf("File to be transfered : %s \n", buf1);
		
		sentFile(connfd,buf1);
	
	}
	
	
	if (temp == 50) 
	{

		list(connfd);
	}


	if (temp == 51)
	{
		read(client_to_server, buf2, 1000);
		printf("File to be deleted : %s \n", buf2);

		deletefile(connfd, buf2); 
		
	}
	

	memset(buf, 0, sizeof(buf));

	close(client_to_server);
    	close(server_to_client);

	unlink(FIFO_FILE_1);
   	unlink(FIFO_FILE_2);

	
 
}

#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "string.h"
#include <fcntl.h>


#define bufS 500




int main (int argc, char **argv)
{
	if (argc<3)
	{
		char* msgEr="Erreur : Veuillez rentrer un nom de serveur et le nom du fichier \n";
		write(STDOUT_FILENO,msgEr,strlen(msgEr));
		exit(EXIT_SUCCESS);
	}
	

	char* servName=argv[1];
	char* fileName=argv[2];
	char* port=argv[3];				
		
	printf("\n Nom du serveur : %s",servName);	
	printf("\n Nom du fichier : %s \n",fileName);
	printf("\n numéro de port : %s \n",port);
	
// Open the file for reading
    int filefd = open(fileName, O_RDONLY);
    if (filefd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
//Configuration of an addrinfo variable according to the structure

	struct addrinfo hints;
	memset(&hints,0,sizeof (struct addrinfo));

	struct addrinfo *result,*rp;
	//Setting of the communication : IPV4, UDP, ...
	hints.ai_family=AF_INET;//attributs of hints collected
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=IPPROTO_UDP;
	
	int adr_srv=getaddrinfo(servName,port,&hints,&result);//the first argument is the name of the server to contact, or "host", i.e. the one entered by the user; the second is the port of the service to contact (variable named service 
	rp=result;
	
	int sfd;

	//create socket with fields to get IPV4(AF_INET) from res returned by getaddrinfo 
	sfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);


    //a) WRQ : [01"fileName"octet]
	//buf will contain the wrq request
	char buf[512];
	buf[0]=0; //opcode for WRQ :start by two bytes "02" 
	buf[1]=2; 
		
	strcpy(&buf[2],fileName);// concat fileName to 10

	
	strcpy(&buf[2+strlen(fileName)+1],"octet");
	
	
	
	//int packetLength= (int) (2+strlen(fileName));
	int packetLength= (int) (2+strlen(fileName)+1+5+1);
	//sending the request WRQ
	int sizerecu=sendto(sfd,buf,packetLength,0,rp->ai_addr,rp->ai_addrlen);
	
	if (sizerecu==-1)
    {
	printf("error sendto");	
	}		
	
	printf(" nombre caractères envoyés pour la WRQ: %d \n",sizerecu);


/*	
    int bytesRead;
	int blockNumber = 1; // Starting block number
// reception of ACK from the WRQ

  	struct sockaddr src_addr;
	socklen_t addrlen;
	int tailleRcv=1000;
	char bufReceiv[tailleRcv];

	int recvNb=recvfrom(sfd,bufReceiv,tailleRcv,0,&src_addr,&addrlen);

	char ackOct[4];
	ackOct[0]=0;
	ackOct[1]=4;
	ackOct[2]=bufReceiv[2];
	ackOct[3]=bufReceiv[3];
	
	
	int packetLengthAck=4;
	
	int sizerecuAck=sendto(sfd,ackOct,4,0,&src_addr,addrlen);



//Sending the data 
 // Prepare and send the data packet
	buf[0] = 3; // Opcode for DATA
	buf[1] = 0;
	buf[2] = bufReceiv[2]; // block number start
	buf[3] = bufReceiv[3];//block number end
	packetLength = bytesRead + 2;

	sizerecu = sendto(sfd, buf, packetLength, 0, rp->ai_addr, rp->ai_addrlen);

	if (sizerecu == -1) {
             perror("Error sending data packet");
             exit(EXIT_FAILURE);
         }

	

	
	

	if (sizerecuAck==-1)
		{
		printf("error sendto ACK");	
		}		
	
	printf(" [ACK] : nombre caractères envoyés: %d \n \n",sizerecuAck);
*/

shutdown(sfd,SHUT_RDWR);
close(sfd);







	return 0;
}
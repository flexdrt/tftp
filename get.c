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

    //getnameinfo(result->ai_addr,result->ai_addr_len,bufferHostName,128,bufferServiceName,128,);
    
    int sfd;
	//create socket with fields to get IPV4(AF_INET) from res returned by getaddrinfo 
	sfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);
	
	int fd;

	fd=open(fileName,O_CREAT | O_WRONLY | O_TRUNC,S_IRWXG | S_IRWXU |S_IRWXO);
	if(fd==0)
	{
		printf("error sendto ACK");	
 		exit (EXIT_FAILURE) ;
	}
	else{
		printf(" this file(%d) is correctly opened.\n",fd);
	}

    //a) RRQ : [01"fileName"octet]
	//buf will contain the rrq request
	char buf[512];
	buf[0]=0; //start by a 01
	buf[1]=1; 
		
	strcpy(&buf[2],fileName);// concat fileName to 01

	
	strcpy(&buf[2+strlen(fileName)+1],"octet");
	
	
	
	//int packetLength= (int) (2+strlen(fileName));
	int packetLength= (int) (2+strlen(fileName)+1+5+1);
	int sizerecu=sendto(sfd,buf,packetLength,0,rp->ai_addr,rp->ai_addrlen);
	
	if (sizerecu==-1)
    {
	printf("error sendto");	
	}		
	
	printf(" nombre caractères envoyés: %d \n",sizerecu);
	

// b) packet data et ack 
// code en contruction 
    struct sockaddr src_addr;
	socklen_t addrlen;
	int tailleRcv=1000;
	char bufReceiv[tailleRcv];
	
	int blockNumberReceived;
	int blockNumber=1;
	int recvNb;

	do {
	recvNb=recvfrom(sfd,bufReceiv,tailleRcv,0,&src_addr,&addrlen);
	blockNumberReceived=bufReceiv[2]*256+bufReceiv[3];

	printf("nombre recu recvfrom %d, blockNumber =%d \n",recvNb, blockNumberReceived);

	if(blockNumberReceived==blockNumber){
		write(fd,bufReceiv+4,recvNb-4);
		blockNumber++;
	}
	
	
	
	//char bufSend[512];
	
	
	char ackOct[4];
	ackOct[0]=0;
	ackOct[1]=4;	
	
	
	//strcpy(&bufSend[2],ackOct);
	
	//strcpy(&bufSend[2+strlen(ack)+1],"octet");
	ackOct[2]=bufReceiv[2];
	ackOct[3]=bufReceiv[3];
	
	
	int packetLengthAck=4;
	
	int sizerecuAck=sendto(sfd,ackOct,4,0,&src_addr,addrlen);
	
	

	if (sizerecuAck==-1)
		{
		printf("error sendto ACK");	
		}		
	
	printf(" nombre caractères envoyés: %d \n \n",sizerecuAck);

	

	}
	
	while(recvNb-4==512);

	//A la fin 
	//shutdown(sfd,SHUT_RDWR);
	//close(sfd);



	return 0;
}
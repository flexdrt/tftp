#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <time.h>

#include <sys/socket.h>
#include <netdb.h>
#define bufS 500




int main (int argc, char **argv)
{
	if (argc<3)
	{
		char* msgEr="Veuillez rentrer un nom de serveur et le nom du fichier \n";
		write(STDOUT_FILENO,msgEr,strlen(msgEr));
		exit(EXIT_SUCCESS);
	}
	

	char* servName=argv[1];
	char* fileName=argv[2];
				
		
	printf("\n Nom du serveur : %s",servName);	
		
	printf("\n Nom du fichier : %s \n",fileName);
		
				
	
	int sfd;
	
	struct addrinfo hints;
	memset(&hints,0,sizeof (struct addrinfo));
	struct addrinfo *res,*rp;
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=IPPROTO_UDP;
	
	int adr_srv=getaddrinfo(servName,"1069",&hints,&res);//le premier argument est le nom du serveur à contacter, ou "host" donc celui renseigné par l'utilisateur; le second est le port du servie à contactr(variable nommée service 
	
	rp=res;
	
	sfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);//création du socket avec les champs pr avoir de l'IPV4(AF_INET) de res retourné par getaddrinfo 
	
	
	
	
	char buf[512];
	buf[0]=0;
	buf[1]=1;
		
	strcpy(&buf[2],fileName);
	//char msgRab[7]="0octet0";
	strcpy(&buf[2+strlen(fileName)+1],"octet");
	
	
	
	int packetLength= (int) (2+strlen(fileName)+1+5+1);
	
	int sizerecu=sendto(sfd,buf,packetLength,0,rp->ai_addr,rp->ai_addrlen);
	
	if (sizerecu==-1){
	printf("error sendto");	
	}		
	
	printf(" nombre caractères envoyés: %d \n",sizerecu);
	
	//**********************  b) packet data et ack 
	struct sockaddr *src_addr;
	socklen_t *addrlen;
	int tailleRcv=1000;
	char bufReceiv[tailleRcv];
	
	
	int recvNb=recvfrom(sfd,bufReceiv,tailleRcv,0,src_addr,addrlen);
	
	printf("nombre recu recvfrom %d",recvNb);
	
	
	//A la fin 
	//shutdown(sfd,SHUT_RDWR);
	//close(sfd);
	
	return 0;
}





//****************************************************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <time.h>

#include <sys/socket.h>
#include <netdb.h>
#define bufS 500




int main (int argc, char **argv)
{
	if (argc<3)
	{
		char* msgEr="Veuillez rentrer un nom de serveur et le nom du fichier \n";
		write(STDOUT_FILENO,msgEr,strlen(msgEr));
		exit(EXIT_SUCCESS);
	}
	

	char* servName=argv[1];
	char* fileName=argv[2];
				
		
	printf("\n Nom du serveur : %s",servName);	
		
	printf("\n Nom du fichier : %s \n",fileName);
		
				
	
	int sfd;
	
	struct addrinfo hints;
	memset(&hints,0,sizeof (struct addrinfo));
	struct addrinfo *res,*rp;
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=IPPROTO_UDP;
	
	int adr_srv=getaddrinfo(servName,"1069",&hints,&res);//le premier argument est le nom du serveur à contacter, ou "host" donc celui renseigné par l'utilisateur; le second est le port du servie à contactr(variable nommée service 
	
	rp=res;
	
	sfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);//création du socket avec les champs pr avoir de l'IPV4(AF_INET) de res retourné par getaddrinfo 
	
	
	
	
	char buf[512];
	buf[0]=1;
	buf[1]=0;
		
	strcpy(&buf[2],fileName);
	
	strcpy(&buf[2+strlen(fileName)+1],"octet");
	
	
	
	int packetLength= (int) (2+strlen(fileName)+1+5+1);
	
	int sizerecu=sendto(sfd,buf,packetLength,0,rp->ai_addr,rp->ai_addrlen);
	
	if (sizerecu==-1){
	printf("error sendto");	
	}		
	
	printf(" nombre caractères envoyés: %d \n",sizerecu);
	
	//**********************  b) packet data et ack 
	struct sockaddr *src_addr;
	socklen_t *addrlen;
	int tailleRcv=1000;
	char bufReceiv[tailleRcv];
	
	
	int recvNb=recvfrom(sfd,bufReceiv,tailleRcv,0,src_addr,addrlen);
	
	printf("nombre recu recvfrom %d",recvNb);
	
	
	//A la fin 
	//shutdown(sfd,SHUT_RDWR);
	//close(sfd);
	
	return 0;
}
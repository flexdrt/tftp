# Rapport d'étonnement TP2 : Client TFTP GET & PUT

## Synthèse :

***Dans ce TP, j'ai appris à utiliser le sockets IP afin de se connecter à une machine distante. J'ai découvert la norme TFTP qui me permet de lire ou d'écrire des fichiers afin de les télécharger ou de les téléverser. Il m'a fallu utiliser la notion de fichier sous linux pour manipuler les téléchargements.*** 

Les questions 1/2/3 sont incluses dans les questions 4 et 5 

### 4) Get



Ce programme prend en entrée le nom du serveur, le nom du fichier, et le numéro de port depuis la ligne de commande. 

```c
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
```

Après il configure les paramètres réseau

```c
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
```

Il créé une socket

```c
      int sfd;
	//create socket with fields to get IPV4(AF_INET) from res returned by getaddrinfo 
	sfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);
```

 et ouvre le fichier en écriture

```c
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
```



Le programme envoie une demande de lecture (RRQ) TFTP au serveur

La requête RRQ est formée de la sorte :

```c
/a) RRQ : [01"fileName"octet]
	//buf will contain the rrq request
	char buf[512];
	buf[0]=0; //start by a 01
	buf[1]=1; 
		
	strcpy(&buf[2],fileName);// concat fileName to 01

	strcpy(&buf[2+strlen(fileName)+1],"octet");
```

Puis il reçoit les blocs de données du serveur, les écrit dans le fichier, et envoie des accusés de réception (ACK). Ce processus se répète jusqu'à ce que la taille des données reçues soit inférieure à la taille maximale autorisée.

On s'attend à plusieurs blocks de 512, avec les ACKs en réponse à chaque.

```c
do {
	recvNb=recvfrom(sfd,bufReceiv,tailleRcv,0,&src_addr,&addrlen);
	blockNumberReceived=bufReceiv[2]*256+bufReceiv[3];

	printf("nombre recu recvfrom %d, blockNumber =%d \n",recvNb, blockNumberReceived);

	if(blockNumberReceived==blockNumber){
		write(fd,bufReceiv+4,recvNb-4);
		blockNumber++;
	}
	
	
	char ackOct[4];
	ackOct[0]=0;
	ackOct[1]=4;	
	
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
```

Test avec l'image png :

Je télécharge l'image ensea.png (que j'ai renommée logo par la suite pour voir la différencier lorsque je l'upload avec put sur le serveur) avec la commande ./get localhost ensea.png 1069 .

![](tftp/ensea_dansrep_screen.png)

![capture_requete_image_ensea](tftp/capture_requete_image_ensea.png)











### 5) Put 

L'objectif est de lire le contenu du fichier stocké localement et de pouvoir le transmettre au serveur.

J'ai écrit le code pour ouvrir le fichier et récupérer son contenu. 

```c
// Open the file for reading
    int filefd = open(fileName, O_RDONLY);
    if (filefd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
```

Je transmet au serveur ma requête d'écriture WRQ. 

Il l'a reçoit bien d'après la capture wireshark
![](tftp/wireshark wrq recue.png)

Lorsque je transmet la requête WRQ, il ne contient pas les données du fichier à upload.

En effet, je n'envoie pas le contenu du fichier. 

Je dois récupérer le numéro de block de l'acquittement, envoyé par le serveur, pour écrire dans un nouvelle trame DAT les données dans un buffer.

Enfin il faut envoyé cette trame avec sendto(). 

Théoriquement, dans une boucle tant que, il faudrait écrire :

Il faut que ce buffer commence par l'opcode 03 plus le numéro_de_block suivi des données du fichier à envoyer.

La boucle se termine lorsque le fichier est lu entièrement : c-a-d que l'on a un caractère différent de 0.

```c
// Sending the data
    while ((bytesRead = read(filefd, buf + 4, sizeof(buf) - 4)) > 0) {
        // Prepare and send the data packet
        buf[0] = 3; // Opcode for DATA
        buf[1] = 0;
        buf[2] = bufReceiv[2]; // block number start
		buf[3] = bufReceiv[3];//block number end

        int packetLength = bytesRead + 4;
        ssize_t sizerecu = sendto(sfd, buf, packetLength, 0, rp->ai_addr, rp->ai_addrlen);

        if (sizerecu == -1) {
            perror("Error sending data packet");
            exit(EXIT_FAILURE);
        }
```

Avant l'envoie de la trame DAT, j'ai écrit le code pour récupérer l'ACK et numéro de block à l'aide de la fonction recvfrom().

```c
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

```

Seulement, la récupération du code de l'ACK ne fonctionne pas. Ainsi, il m'est impossible d'envoyer les données du fichier ( trame DAT) et remplir le fichier que j'ai demandé de créer au serveur.

C'est pourquoi dans le dossier du serveur, il se trouve qu'il y a le fichier t_put mais il est vide (0 octet).

![image-20231227022207275](tftp/t_put_vide_screen.png)

#### Conclusion 

#### GET 

Le client get est complet, il permet de télécharger des fichiers qui sont situés sur le serveur localhost que ce soit alt256, ones1024 ou ensea.png.

#### PUT

Le client put est bien avancé, il me permet de créer les fichiers sur le serveur mais il me reste à envoyer le contenu de ces fichiers. 

Reste à compléter :

- Implémentation fonctionnelle des ACKs pour pouvoir upload le fichier avec une seule trame DAT
- Implémentation fonctionnelle des ACKs pour pouvoir upload le fichier sur plusieurs trames DAT afin d'envoyer des fichiers volumineux

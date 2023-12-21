#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "string.h"


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
				
		
	printf("\n Nom du serveur : %s",servName);	
	printf("\n Nom du fichier : %s \n",fileName);
	
	
	return 0;
}
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define bufS 500

int main(int argc, char **argv) {
    if (argc < 3) {
        char *msgEr = "Erreur : Veuillez rentrer un nom de serveur et le nom du fichier \n";
        write(STDOUT_FILENO, msgEr, strlen(msgEr));
        exit(EXIT_SUCCESS);
    }

    char *servName = argv[1];
    char *fileName = argv[2];
    char *port = argv[3];

    printf("\n Nom du serveur : %s", servName);
    printf("\n Nom du fichier : %s \n", fileName);
    printf("\n numéro de port : %s \n", port);

    // Open the file for reading
    int filefd = open(fileName, O_RDONLY);
    if (filefd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Configuration of an addrinfo variable according to the structure
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    struct addrinfo *result, *rp;
    // Setting of the communication: IPV4, UDP, ...
    hints.ai_family = AF_INET;          // attributs of hints collected
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int adr_srv = getaddrinfo(servName, port, &hints, &result);
    rp = result;

    int sfd;

    // create socket with fields to get IPV4(AF_INET) from res returned by getaddrinfo
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    // WRQ: [01"fileName"octet]
    char buf[512];
    buf[0] = 0; // opcode for WRQ
    buf[1] = 2;

    strcpy(&buf[2], fileName); // concatenate fileName
    strcpy(&buf[2 + strlen(fileName)], "octet");

    int packetLength = (int)(2 + strlen(fileName) + 1 + 5 + 1);
    // sending the request WRQ
    int sizerecu = sendto(sfd, buf, packetLength, 0, rp->ai_addr, rp->ai_addrlen);

    if (sizerecu == -1) {
        perror("Error sending WRQ");
        exit(EXIT_FAILURE);
    }

    printf("Nombre de caractères envoyés pour la WRQ : %d \n", sizerecu);

    int bytesRead;
    int blockNumber = 1; // Starting block number

    // reception of ACK
    struct sockaddr_storage src_addr;
    socklen_t addrlen = sizeof(src_addr);
    char bufReceiv[bufS];

    int recvNb = recvfrom(sfd, bufReceiv, bufS, 0, (struct sockaddr*)&src_addr, &addrlen);

    char ackOct[4];
    ackOct[0] = 0;
    ackOct[1] = 4;
    ackOct[2] = bufReceiv[2];
    ackOct[3] = bufReceiv[3];

    int packetLengthAck = 4;

    int sizerecuAck = sendto(sfd, ackOct, packetLengthAck, 0, (struct sockaddr*)&src_addr, addrlen);

    if (sizerecuAck == -1) {
        perror("Error sending ACK");
        exit(EXIT_FAILURE);
    }

    printf("Nombre de caractères envoyés pour l'ACK : %d \n", sizerecuAck);

    // Sending the data
    while ((bytesRead = read(filefd, buf + 4, sizeof(buf) - 4)) > 0) {
        // Prepare and send the data packet
        buf[0] = 3; // Opcode for DATA
        buf[1] = 0;
        buf[2] = (blockNumber >> 8) & 0xFF; // block number high byte
        buf[3] = blockNumber & 0xFF;        // block number low byte

        int packetLength = bytesRead + 4;
        ssize_t sizerecu = sendto(sfd, buf, packetLength, 0, rp->ai_addr, rp->ai_addrlen);

        if (sizerecu == -1) {
            perror("Error sending data packet");
            exit(EXIT_FAILURE);
        }

        // Wait for ACK
        recvNb = recvfrom(sfd, bufReceiv, bufS, 0, (struct sockaddr*)&src_addr, &addrlen);

        if (recvNb == -1) {
            perror("Error receiving ACK");
            exit(EXIT_FAILURE);
        }

        // Check if the received packet is an ACK with the expected block number
        if (bufReceiv[0] != 4 ||
            (bufReceiv[2] != buf[2] && bufReceiv[3] != buf[3])) {
            printf("Unexpected response or incorrect block number in ACK\n");
            exit(EXIT_FAILURE);
        }

        // Update variables for the next iteration
        blockNumber++;
    }

    // Close the file
    close(filefd);

    // Close the socket and free resources
    shutdown(sfd, SHUT_RDWR);
    close(sfd);

    return 0;
}

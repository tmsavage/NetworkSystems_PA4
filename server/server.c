/*
 * udpserver.c - A simple UDP echo server
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define BUFSIZE 1024

struct Message {
  char *cmd;
  char filename[20];
  char data[1002];
};



/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
	int sockfd; /* socket */
	int portno; /* port to listen on */
	int clientlen; /* byte size of client's address */
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	struct hostent *hostp; /* client host info */
	char buf[BUFSIZE]; /* message buf */
	char *hostaddrp; /* dotted decimal host addr string */
	int optval; /* flag value for setsockopt */
	int n; /* message byte size */

	/*
	* check command line arguments
	*/
	if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(1);
	}
	portno = atoi(argv[1]);


	/*
	* socket: create the parent socket
	*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	error("ERROR opening socket");



	/* setsockopt: Handy debugging trick that lets
	* us rerun the server immediately after we kill it;
	* otherwise we have to wait about 20 secs.
	* Eliminates "ERROR on binding: Address already in use" error.
	*/
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
	     (const void *)&optval , sizeof(int));



	/*
	* build the server's Internet address
	*/
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);



	/*
	* bind: associate the parent socket with a port
	*/
	if (bind(sockfd, (struct sockaddr *) &serveraddr,
	   sizeof(serveraddr)) < 0)
	error("ERROR on binding");



	/*
	* main loop: wait for a datagram, then echo it
	*/
	clientlen = sizeof(clientaddr);
	while (1) {

		/*
		* recvfrom: receive a UDP datagram from a client
		*/
		bzero(buf, BUFSIZE);
		n = recvfrom(sockfd, buf, BUFSIZE, 0,
			 (struct sockaddr *) &clientaddr, &clientlen);
		if (n < 0)
		error("ERROR in recvfrom");


		/*
		* gethostbyaddr: determine who sent the datagram
		*/
		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
				  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		if (hostp == NULL)
		error("ERROR on gethostbyaddr");
		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		if (hostaddrp == NULL)
		error("ERROR on inet_ntoa\n");
		printf("server received datagram from %s (%s)\n\n",
		   hostp->h_name, hostaddrp);
		printf("server received %d/%d bytes: %s\n\n", strlen(buf), n, buf);



    /*
    * save values from the packet
    */
    char* option = strtok(buf, " ");  // command option from user
    char* fileName = strtok(NULL, " "); // file name from user
    char* firstPart = strtok(NULL, " ");
    char* secondPart = strtok(NULL, " ");
    option = strtok(option, "\n");
    char* newFileName = strtok(fileName, "\n");
    firstPart = strtok(firstPart, "\n");
    secondPart = strtok(secondPart, "\n");


    printf("First Part ID: %s\n\n", firstPart);
    printf("Second Part ID: %s\n\n", secondPart);
    /*
     *  'put' command
     */
    if(strcmp("put", option) == 0){
      int fileNameLength = strlen(newFileName);
      char updatedFileName1[fileNameLength + 1];
      memset(updatedFileName1, 0, fileNameLength + 1);
      strcat(updatedFileName1, firstPart);
      printf("File Name: %s \n", strcat(updatedFileName1, newFileName));

      int receivedChecker = 0;
      char fileBuf[BUFSIZE];
      memset(fileBuf, 0, BUFSIZE);

      FILE *thisFile = fopen(updatedFileName1, "wb");
      if(thisFile == NULL){
        printf("Failed to open file :( \n");
      }
      else {
        receivedChecker = BUFSIZE;
        while(receivedChecker == BUFSIZE){
          receivedChecker = recvfrom(sockfd, fileBuf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
          //printf("This is the fileBuf: %s\n", fileBuf);
          if(receivedChecker < 0){
            printf("Error receiving file! \n \n");
          }
          else {
            printf("File packet successfully received WOOHOO!! \n \n");
	    //printf("FILE CONTENTS: %s\n\n", fileBuf);
            printf("File Buf: %s \n", fileBuf);
            fwrite(fileBuf, sizeof(char), receivedChecker, thisFile);
          }

        }

      }
      fclose(thisFile);
     // free(fileBuf);

      printf("Ready to receive 2nd file contents! \n\n");
      char updatedFileName2[fileNameLength + 1];
      memset(updatedFileName2, 0, fileNameLength+1);
      strcat(updatedFileName2, secondPart);
      printf("File Name: %s \n", strcat(updatedFileName2, newFileName));

      receivedChecker = 0;
      char fileBuf2[BUFSIZE];
      memset(fileBuf2, 0, BUFSIZE);

      FILE *nextFile = fopen(updatedFileName2, "wb");
      if(nextFile == NULL){
        printf("Failed to open file :( \n");
      }
      else {
        receivedChecker = BUFSIZE;
        while(receivedChecker == BUFSIZE){
          receivedChecker = recvfrom(sockfd, fileBuf2, BUFSIZE, 0, (struct sockaddr *) &clientaddr, &clientlen);
          //printf("This is the filebuf2: %s\n", fileBuf2);
          if(receivedChecker < 0){
            printf("Error receiving file! \n \n");
          }
          else {
            printf("File packet successfully received WOOHOO!! \n \n");
	    //printf("File contents: %s\n\n", fileBuf2);
            printf("File buf: %s \n", fileBuf2);
            fwrite(fileBuf2, sizeof(char), receivedChecker, nextFile);
          }

        }

      }
      fclose(nextFile);

    }

    /*
     *  'get' command
     */
    if(strcmp("get", option) == 0){
      char indicatorBuf[BUFSIZE]; /* indicator buf */

      while(1){
        bzero(indicatorBuf, BUFSIZE);
        n = recvfrom(sockfd, indicatorBuf, BUFSIZE, 0,
           (struct sockaddr *) &clientaddr, &clientlen);
        if (n < 0){
          error("ERROR in recvfrom");
        }
        if(strcmp(indicatorBuf, "1")){
          int fileNameLength = strlen(newFileName);
          char updatedFileName1[fileNameLength+1];
          memset(updatedFileName1, 0, fileNameLength+1);
          strcat(updatedFileName1, firstPart);
          printf("Get File Name: %s \n", strcat(updatedFileName1, newFileName));

          //printf("%s \n", newFileName);
          int sentChecker = 0;
          int byteSize = 0;
          char fileBuf[BUFSIZE];
          int whileFlag = 1;

          memset(fileBuf, 0, BUFSIZE);

          FILE *thisFile = fopen(updatedFileName1, "rb");
          if(thisFile == NULL){
              printf("Failed to read file :( \n");
          }
          else {

            while(whileFlag == 1){
              byteSize = fread(fileBuf, sizeof(char), BUFSIZE, thisFile);
              printf("ByteSize: %d \n", byteSize);

              printf("File Buf: %s \n", fileBuf);
              sentChecker = sendto(sockfd, fileBuf, byteSize, 0, (struct sockaddr *) &clientaddr, clientlen);

              if(sentChecker < 0) {
                printf("ERROR sending file! ABORT! \n \n");
                exit(1);
              }
              else {
                printf("File packet successfully sent! WOOHOO!! \n \n");
                //exit(1);  // remove this
              }

              if(byteSize != BUFSIZE){
                whileFlag = 0;
              }
            }
          }
          fclose(thisFile);



          char updatedFileName2[fileNameLength+1];
          memset(updatedFileName2, 0, fileNameLength+1);
          strcat(updatedFileName2, firstPart);
          printf("Get File Name: %s \n", strcat(updatedFileName2, newFileName));

          //printf("%s \n", newFileName);
          sentChecker = 0;
          byteSize = 0;
          char fileBuf2[BUFSIZE];
          whileFlag = 1;

          memset(fileBuf2, 0, BUFSIZE);

          FILE *nextFile = fopen(updatedFileName2, "rb");
          if(nextFile == NULL){
              printf("Failed to read file :( \n");
          }
          else {

            while(whileFlag == 1){
              byteSize = fread(fileBuf2, sizeof(char), BUFSIZE, nextFile);
              printf("ByteSize: %d \n", byteSize);

              printf("File Buf: %s \n", fileBuf2);
              sentChecker = sendto(sockfd, fileBuf2, byteSize, 0, (struct sockaddr *) &clientaddr, clientlen);

              if(sentChecker < 0) {
                printf("ERROR sending file! ABORT! \n \n");
                exit(1);
              }
              else {
                printf("File packet successfully sent! WOOHOO!! \n \n");
                //exit(1);  // remove this
              }

              if(byteSize != BUFSIZE){
                whileFlag = 0;
              }
            }
          }
          fclose(nextFile);
          break;
        }

      } // End While Loop

    }

  //   FILE *myFile;
  //   char filename[50] = "example.txt"
  //   char data[50];
  //   myFile = fopen(filename, "r");
  //   if(fp == NULL){
  //     printf("Could not open the file\n");
  //   }
  //   fgets(data, 50, myFile);
  //   n = sendto(sockfd, data, strlen(data), 0,
  //      (struct sockaddr *) &clientaddr, clientlen);
  // if (n < 0)
  //     error("ERROR in sendto");


    /*
     *  'delete' command
     */
    if(strcmp("delete", option) == 0){
      int ret;
      FILE *myFile;
      int m = 0;
      ret = remove(newFileName);
      if(ret == 0){
        m = sendto(sockfd, "File deleted successfully", sizeof("File deleted successfully"), 0 , (struct sockaddr *) &clientaddr, clientlen);
        if(m < 0){
          printf("Delete Failed! \n");
        }
        else{
          printf("Delete Successful! \n");
        }
      }
      else {
        printf("Error: unable to delete file \n");
      }
    }

  /*
   *  'ls' command
   */
    if(strcmp("ls", option) == 0) {
      DIR *d;
      int m = 0;
      char lsBuf[BUFSIZE];
      struct dirent *dir;
      d = opendir(".");
      if (d) {
        while ((dir = readdir(d)) != NULL) {
          printf("%s\n", dir->d_name);
          bzero(lsBuf, BUFSIZE);
          strcpy(lsBuf, dir->d_name);
          m = sendto(sockfd, lsBuf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, clientlen);

          // if(m < 0){
          //   printf("FAIL IN LS \n");
          // }
          // else{
          //   printf("TADA IN LS \n");
          // }
        }

        closedir(d);
        sendto(sockfd, "Completed file list", sizeof("Completed file list"), 0, (struct sockaddr *) &clientaddr, clientlen);
  	  }
    }
    //printf("HERE 1 \n");
    /*
     *  'exit' command
     */
    if(strcmp("exit", option) == 0)  {
      exit(0);
    }

  //printf("HERE 2 \n");
		/*
		* sendto: echo the input back to the client
		*/
    // sleep(1);
		// n = sendto(sockfd, buf, strlen(buf), 0,
		//        (struct sockaddr *) &clientaddr, clientlen);
    // //printf("N THING: %d \n", n);
		// if (n < 0)
		// error("ERROR in sendto");
		}
}
/* if(strcmp("get", option) == 0){
      printf("Before Buffer Declaration!\n");
      //char indicatorBuf[3]; // indicator buf
      //indicatorBuf = malloc(sizeof(char) * BUFSIZE);
      //printf("Now We're In GET!\n");
      while(1){
        printf("In While Loop \n");
        bzero(buf, BUFSIZE);

        n = recvfrom(sockfd, buf, 3, 0,
           (struct sockaddr *) &clientaddr, &clientlen);
        if (n < 0){
          error("ERROR in recvfrom");
        }
        printf("Here i guess?\n");
        printf("Indicator Buf: %s\n", buf);
        //printf("After REceiving Data\n\n");
        char* indicator = strtok(buf, " ");
        char* part = strtok(NULL, " ");
        part = strtok(part, "\n");


        if(strcmp(indicator, "1")){
          int fileNameLength = strlen(newFileName);
          char updatedFileName1[fileNameLength+1];
          memset(updatedFileName1, 0, fileNameLength+1);
          strcat(updatedFileName1, part);
          printf("Get File Name: %s \n", strcat(updatedFileName1, newFileName));

          //printf("%s \n", newFileName);
          int sentChecker = 0;
          int byteSize = 0;
          char fileBuf[BUFSIZE];
          int whileFlag = 1;

          memset(fileBuf, 0, BUFSIZE);

          FILE *thisFile = fopen(updatedFileName1, "rb");
          if(thisFile == NULL){
              printf("Failed to read file :( \n");
          }
          else {

            while(whileFlag == 1){
              byteSize = fread(fileBuf, sizeof(char), BUFSIZE, thisFile);
              printf("ByteSize: %d \n", byteSize);

              printf("File Buf: %s \n", fileBuf);
              sentChecker = sendto(sockfd, fileBuf, byteSize, 0, (struct sockaddr *) &clientaddr, clientlen);

              if(sentChecker < 0) {
                printf("ERROR sending file! ABORT! \n \n");
                exit(1);
              }
              else {
                printf("File packet successfully sent! WOOHOO!! \n \n");
                //exit(1);  // remove this
              }

              if(byteSize != BUFSIZE){
                whileFlag = 0;
              }
            }
          }
          fclose(thisFile);
          bzero(updatedFileName1, fileNameLength+1);
        }
*/

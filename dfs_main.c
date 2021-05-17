#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*--------https://stackoverflow.com/questions/7627723/how-to-create-a-md5-hash-of-a-string-in-c-------*/
// struct MD5Context {
//     uint32_t buf[4];
//     uint32_t bits[2];
//     unsigned char in[64];
// };


#define BUFSIZE 1024
#define SIZE 4
#define HTSIZE 20

int hashArray[HTSIZE];

int hashCode(int key) {
   return abs(key) % SIZE;
}

int htHash(int key){
   return abs(key) % HTSIZE;
}

int search(int key) {
   //get the hash
   int hashIndex = hashCode(key);
   printf("The hash index is: %d\n", hashIndex);

   //move in array until an empty
   while(hashArray[hashIndex] != -1) {

      if(hashArray[hashIndex] == key)
         return hashArray[hashIndex];

      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= HTSIZE;
   }

   return -1;
}

void insert(int key, int data) {

   //get the hash
   int hashIndex = htHash(key);

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != -1 && hashIndex != HTSIZE) {
      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= HTSIZE;
   }

   hashArray[hashIndex] = data;
}

void display() {
   int i = 0;

   for(i = 0; i<HTSIZE; i++) {

      if(hashArray[i] != -1)
         printf(" (%d,%d)", i ,hashArray[i]);
      else
         printf(" ~~ ");
   }

   printf("\n");
}

int hashStringToMD5(char *myFile)
{
   unsigned char digest[16];
   //    //const char* string = "012345678";
   FILE *inFile = fopen(myFile, "rb");
   MD5_CTX context;
   int bytes;
   unsigned char data[1024];
   if (inFile == NULL)
   {
      printf("%s cannot be opened", myFile);
   }

   MD5_Init(&context);
   while ((bytes = fread(data, 1, 1024, inFile)) != 0)
   {
      MD5_Update(&context, data, bytes);
   }
   MD5_Final(digest, &context);

   char md5string[33];
   for (int i = 0; i < 16; ++i)
   {
      sprintf(&md5string[i * 2], "%02x", (unsigned int)digest[i]);
   }
   fclose(inFile);
   uint32_t num;
   memcpy(&num, digest, 4);
   printf("The md5 string is %s, and the int is %d\n", md5string, num);
   return num;
}

int hashNameToMD5(char * myString){
      unsigned char digest[16];
      MD5_CTX context;
      MD5_Init(&context);
      MD5_Update(&context, myString, strlen(myString));
      MD5_Final(digest, &context);

      char md5string[33];
      for(int i = 0; i < 16; ++i){
         sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);

      }
      uint32_t num;
      memcpy(&num, digest, 4);
      return num;
}

//find the size of the file, calculate how many bytes are going to each server
void splitFile()
{

   char newCmdLineBuf[1024];
   char cmdLineBuf[1024];
   while (1)
   {
      /* get a message from the user */
      bzero(cmdLineBuf, 1024);
      bzero(newCmdLineBuf, 1024);
      printf("Please enter any of the following commands: \n");
      printf("\"get <filename>\" downloads file from DFS \n");
      printf("\"put <filename>\" puts uploads file to DFS \n");
      printf("\"list <filename>\" shows which files can and cannot be downloaded \n");
      printf("\"exit\" quits the program \n");
      printf("\n");
      printf("Command: ");
      fgets(cmdLineBuf, 1024, stdin);

      //memcpy(newCmdLineBuf, cmdLineBuf, 1024);

      //parse out command and filename
      char *option = strtok(cmdLineBuf, " "); // command option from user
      char *fileName = strtok(NULL, " ");     // file name from user
      option = strtok(option, "\n");
      char *newFileName = strtok(fileName, "\n"); //eliminates '\n' character at end of fileName string

      printf("Command: %s\n", option);

      if(strcmp("exit", option) == 0){
          exit(0);
      }
            //printf("Buf1 looks like %s\n\nBuf2 looks like %s\n\nBuf3 looks like %s\n\nBuf4 looks like %s\n\n", buf1, buf2, buf3, buf4);
      //
      // printf("Here are the MD5 values for each bufferrrrrr\n");

      if (strcmp("get", option) == 0)
      {
         //do get stuff
	 int htIndex = hashNameToMD5(newFileName) % HTSIZE;
	 //if it's not in the hash table then throw some error
	 int hashVal = search(htIndex);

         int sockfd1 = -1, sockfd2 = -1, sockfd3 = -1, sockfd4 = -1, n = -1;
         int serverlen1, serverlen2, serverlen3, serverlen4;
         struct sockaddr_in serveraddr1, serveraddr2, serveraddr3, serveraddr4;
         struct hostent *server1, *server2, *server3, *server4;

         printf("Right before declaring \n");
         int sockets[] = {
             sockfd1,
             sockfd2,
             sockfd3,
             sockfd4};
         int serverlens[] = {
             serverlen1,
             serverlen2,
             serverlen3,
             serverlen4};
         struct sockaddr_in serveraddrs[] = {
             serveraddr1,
             serveraddr2,
             serveraddr3,
             serveraddr4};
         struct hostent *servers[] = {
             server1,
             server2,
             server3,
             server4};

         char *hostname;
         char buf[BUFSIZE];
         int flag = 1;
         char c;
         int b;

         //compute hash value of the file

         hostname = "localhost";
         // strcat(hostname, "localhost");

         int portno1 = 10001;
         int portno2 = 10002;
         int portno3 = 10003;
         int portno4 = 10004;
         int portnos[] = {
             portno1,
             portno2,
             portno3,
             portno4};

         int buf1Locations[2];
         int buf2Locations[2];
         int buf3Locations[2];
         int buf4Locations[2];

         if (hashVal == 0)
         {
            buf1Locations[0] = 0;
            buf1Locations[1] = 1;
            buf2Locations[0] = 1;
            buf2Locations[1] = 2;
            buf3Locations[0] = 2;
            buf3Locations[1] = 3;
            buf4Locations[0] = 3;
            buf4Locations[1] = 0;
         }
         else if (hashVal == 1)
         {
            buf1Locations[0] = 3;
            buf1Locations[1] = 0;
            buf2Locations[0] = 0;
            buf2Locations[1] = 1;
            buf3Locations[0] = 1;
            buf3Locations[1] = 2;
            buf4Locations[0] = 2;
            buf4Locations[1] = 3;
         }
         else if (hashVal == 2)
         {
            buf1Locations[0] = 2;
            buf1Locations[1] = 3;
            buf2Locations[0] = 3;
            buf2Locations[1] = 0;
            buf3Locations[0] = 0;
            buf3Locations[1] = 1;
            buf4Locations[0] = 1;
            buf4Locations[1] = 2;
         }
         else if (hashVal == 3)
         {
            buf1Locations[0] = 1;
            buf1Locations[1] = 2;
            buf2Locations[0] = 2;
            buf2Locations[1] = 3;
            buf3Locations[0] = 3;
            buf3Locations[1] = 0;
            buf4Locations[0] = 0;
            buf4Locations[1] = 1;
         }
         int *bufLocations[4] = {
             buf1Locations,
             buf2Locations,
             buf3Locations,
             buf4Locations};

         for (int i = 0; i < 4; i++)
         {
            sockets[i] = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockets[i] < 0)
            {
               perror("ERROR opening socket 4!");
            }
            servers[i] = gethostbyname(hostname);
            if (servers[i] == NULL)
            {
               fprintf(stderr, "ERROR, no such host as %s\n", hostname);
            }

            bzero((char *)&serveraddrs[i], sizeof(serveraddrs[i]));
            serveraddrs[i].sin_family = AF_INET;
            bcopy((char *)servers[i]->h_addr,
                  (char *)&serveraddrs[i].sin_addr.s_addr, servers[i]->h_length);
            serveraddrs[i].sin_port = htons(portnos[i]);

            serverlens[i] = sizeof(serveraddrs[i]);
            int *bufVal = bufLocations[i];
            int loc1 = *(bufVal++);
            int loc2 = *(bufVal++);
            sprintf(newCmdLineBuf, "%s %s %d %d", option, newFileName, loc1, loc2);

            printf("The following is being sent from the newCmdLineBuf: %s\n", newCmdLineBuf);
            n = sendto(sockets[i], newCmdLineBuf, strlen(newCmdLineBuf), 0, &serveraddrs[i], serverlens[i]);
            if (n < 0)
            {
               printf("send failed!\n");
            }
         }




      }
      else if (strcmp("put", option) == 0)
      {
         //md5 for where the content is going
         int md5FileContentVal = hashStringToMD5(newFileName);
         int hashVal = hashCode(md5FileContentVal);

         //md5 for storing filename hash to md5 value
         int htIndex = hashNameToMD5(newFileName) % HTSIZE;
	 insert(htIndex, hashVal);
	 display();

         printf("The first value is %d\n", hashVal);

         //******** SEND CONTENTS TO SERVER ********
         int sockfd1 = -1, sockfd2 = -1, sockfd3 = -1, sockfd4 = -1, n = -1;
         int serverlen1, serverlen2, serverlen3, serverlen4;
         struct sockaddr_in serveraddr1, serveraddr2, serveraddr3, serveraddr4;
         struct hostent *server1, *server2, *server3, *server4;

         printf("Right before declaring \n");
         int sockets[] = {
             sockfd1,
             sockfd2,
             sockfd3,
             sockfd4};
         int serverlens[] = {
             serverlen1,
             serverlen2,
             serverlen3,
             serverlen4};
         struct sockaddr_in serveraddrs[] = {
             serveraddr1,
             serveraddr2,
             serveraddr3,
             serveraddr4};
         struct hostent *servers[] = {
             server1,
             server2,
             server3,
             server4};

         char *hostname;
         char buf[BUFSIZE];
         int flag = 1;
         char c;
         int b;

         //compute hash value of the file

         hostname = "localhost";
         // strcat(hostname, "localhost");

         int portno1 = 10001;
         int portno2 = 10002;
         int portno3 = 10003;
         int portno4 = 10004;
         int portnos[] = {
             portno1,
             portno2,
             portno3,
             portno4};

         int buf1Locations[2];
         int buf2Locations[2];
         int buf3Locations[2];
         int buf4Locations[2];

         if (hashVal == 0)
         {
            buf1Locations[0] = 0;
            buf1Locations[1] = 1;
            buf2Locations[0] = 1;
            buf2Locations[1] = 2;
            buf3Locations[0] = 2;
            buf3Locations[1] = 3;
            buf4Locations[0] = 3;
            buf4Locations[1] = 0;
         }
         else if (hashVal == 1)
         {
            buf1Locations[0] = 3;
            buf1Locations[1] = 0;
            buf2Locations[0] = 0;
            buf2Locations[1] = 1;
            buf3Locations[0] = 1;
            buf3Locations[1] = 2;
            buf4Locations[0] = 2;
            buf4Locations[1] = 3;
         }
         else if (hashVal == 2)
         {
            buf1Locations[0] = 2;
            buf1Locations[1] = 3;
            buf2Locations[0] = 3;
            buf2Locations[1] = 0;
            buf3Locations[0] = 0;
            buf3Locations[1] = 1;
            buf4Locations[0] = 1;
            buf4Locations[1] = 2;
         }
         else if (hashVal == 3)
         {
            buf1Locations[0] = 1;
            buf1Locations[1] = 2;
            buf2Locations[0] = 2;
            buf2Locations[1] = 3;
            buf3Locations[0] = 3;
            buf3Locations[1] = 0;
            buf4Locations[0] = 0;
            buf4Locations[1] = 1;
         }
         int *bufLocations[4] = {
             buf1Locations,
             buf2Locations,
             buf3Locations,
             buf4Locations};

         for (int i = 0; i < 4; i++)
         {
            sockets[i] = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockets[i] < 0)
            {
               perror("ERROR opening socket 4!");
            }
            servers[i] = gethostbyname(hostname);
            if (servers[i] == NULL)
            {
               fprintf(stderr, "ERROR, no such host as %s\n", hostname);
            }

            bzero((char *)&serveraddrs[i], sizeof(serveraddrs[i]));
            serveraddrs[i].sin_family = AF_INET;
            bcopy((char *)servers[i]->h_addr,
                  (char *)&serveraddrs[i].sin_addr.s_addr, servers[i]->h_length);
            serveraddrs[i].sin_port = htons(portnos[i]);

            serverlens[i] = sizeof(serveraddrs[i]);
            int *bufVal = bufLocations[i];
            int loc1 = *(bufVal++);
            int loc2 = *(bufVal++);
            sprintf(newCmdLineBuf, "%s %s %d %d", option, newFileName, loc1, loc2);

            printf("The following is being sent from the newCmdLineBuf: %s\n", newCmdLineBuf);
            n = sendto(sockets[i], newCmdLineBuf, strlen(newCmdLineBuf), 0, &serveraddrs[i], serverlens[i]);
            if (n < 0)
            {
               printf("send failed!\n");
            }
         }// callCommand(option, newFileName, );


	 //do put stuff
         //printf("Inside put else if\n");
         FILE *thisFile = fopen(newFileName, "rb");
         if (thisFile == NULL)
         {
             printf("Failed to read file\n");
         }
         fseek(thisFile, 0, SEEK_END);
         off_t size = ftell(thisFile);

         //reset the file to the beginning
         fseek(thisFile, 0, SEEK_SET);
         printf("The size of the file is %ld\n", size);

         off_t filePartSize = size / 4;

         printf("The size of the file part is %ld\n", filePartSize);

         off_t lastFilePartSize = filePartSize + (size % 4);

         char entireFile[size];
         char buf1[filePartSize];
         char buf2[filePartSize];
         char buf3[filePartSize];
         char buf4[lastFilePartSize];

         char *bufs[4] = {
             buf1,
             buf2,
             buf3,
             buf4};
         fread(entireFile, 1, size, thisFile);

         fseek(thisFile, 0, SEEK_SET);
         fread(buf1, 1, filePartSize, thisFile);
         fread(buf2, 1, filePartSize, thisFile);
         fread(buf3, 1, filePartSize, thisFile);
         fread(buf4, 1, lastFilePartSize, thisFile);


	 for (int i = 0; i < 4; i++)
         {
            int sentChecker1 = 0;

            // Send buffers to servers
            char *currBuf = bufs[i];
            int correctFileSize = 0;
            if (i < 3)
            {
               correctFileSize = filePartSize;
            }
            else
            {
               correctFileSize = lastFilePartSize;
            }

            printf("THIS IS THE BUFFER:  %s \n\n", currBuf);
            //check each buffer's prospective location and send the content there
            int *correctBufLocations = bufLocations[i];
            for (int j = 0; j < 2; j++)
            {
               int index = *correctBufLocations;
               printf("The index is %d\n", index);
               sentChecker1 = sendto(sockets[index], currBuf, correctFileSize, 0, (struct sockaddr *)&serveraddrs[index], serverlens[index]);
               if (sentChecker1 < 0)
               {
                  printf("ERROR sending file to Server 1! ABORT! \n \n");
                  exit(1);
               }
               correctBufLocations++;
            }
         }
      	 fclose(thisFile);
	 //free(newCmdLineBuf);
      }
      else if (strcmp("list", option) == 0)
      {
         //do list stuff
      }
      else if (strcmp("exit", option) == 0)
      {
         //exittttt
	 printf("Inside exit function\n");
         exit(0);
      }
      else
      {
         printf("Invalid command, try again\n");
      }
   }

   printf("File packet successfully sent! WOOHOO!! \n \n");

}

int main()
{
   //given example ips and html files, hash them into the array and retrieve them
   for(int i = 0; i < HTSIZE; i++){
   	hashArray[i] = -1;
   }


   splitFile();
}

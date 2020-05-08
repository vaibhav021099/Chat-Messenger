/* 
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "common.h"

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
int sockfd, portno, n;
struct sockaddr_in serveraddr;
struct hostent *server;
char *hostname;
char buf[DATA_SIZE];
char rbuf[DATA_SIZE];

PACKET rpacket;
PACKET wpacket;
char username[NAME_SIZE];
char password[NAME_SIZE];
ACCOUNT account;
pthread_t list_thread_id, read_thread_id, write_thread_id; 
char thread_status = 1;

void error(char *msg) {
    	perror(msg);
    	exit(0);
}

int login(int sckfd)
{
	int retval = 0;
	wpacket.startbyte = START_BYTE;
	wpacket.command   = LOGIN;
	wpacket.size 	  = 0;	
	strcpy(wpacket.username,username);	
	strcpy(wpacket.password,password);	
	wpacket.endbyte   = END_BYTE;
	
	/* send the message line to the server */
    	n = write(sockfd, &wpacket, sizeof (wpacket));
    	if (n < 0) 
	{
		printf ("login message send failure\n");
		return -1;
	}

	n = read(sockfd, &rpacket, sizeof (rpacket));
    	if (n < 0) 
	{
      		printf("login verification read failure\n");
		return -1;	
	}
	switch (rpacket.command)
	{
		case LOGIN_SUCCESS:
      			printf("login verification success\n");
			retval = 0;
		break;
		case LOGIN_FAILURE:
      			printf("login verification failed\n");
			retval = -1;
		break;
		default:
			retval = -1;
		break;		
	}
	return retval;
}

void *getuserlist(void *vargp)
{
	int count = 0;
        int i, retval = 0;
        wpacket.startbyte = START_BYTE;
        wpacket.command   = USERLIST;
        wpacket.size      = 0;  
        strcpy(wpacket.username,username);
        strcpy(wpacket.password,password);
        wpacket.endbyte   = END_BYTE;

	while (thread_status == 1)
	{
	        /* send the message line to the server */
        	n = write(sockfd, &wpacket, sizeof (wpacket));
	        if (n < 0)
	        {
        	        printf ("getuserlist message send failure\n");
                	return -1;
	        }

        	n = read(sockfd, &rpacket, sizeof (rpacket));
	        if (n < 0)
        	{
                	printf("getuserlist message read failure\n");
	                return -1;
        	}
	        switch (rpacket.command)
        	{
                	case USERLIST:
                        	printf("Got user list verification success\n");
				memcpy (&account, &rpacket.data[0], sizeof(account));
				for (i=0;i<account.totaluser;i++)
				{
					if (account.user[i].onlinestatus == 1)
					{
						printf("\t%s\n", account.user[i].username);
					}	
				}
	                        retval = 0;
        	        break;
                	default:
                        	retval = -1;
	                break;
        	}
		if (++count == 5)
		{
			thread_status = 0;
		}
		sleep(5);
	}
}

void chat()
{

}

int main(int argc, char **argv) {

    	/* check command line arguments */
    	if (argc != 5) {
       		fprintf(stderr,"usage: %s <hostname> <port> <username>\n", argv[0]);
       		exit(0);
    	}
    	hostname = argv[1];
    	portno = atoi(argv[2]);
	strcpy(username, argv[3]);
	strcpy(password, argv[4]);


    	/* socket: create the socket */
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) 
        	error("ERROR opening socket");
		

    	/* gethostbyname: get the server's DNS entry */
    	server = gethostbyname(hostname);
    	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        	exit(0);
    	}

    	/* build the server's Internet address */
    	bzero((char *) &serveraddr, sizeof(serveraddr));
    	serveraddr.sin_family = AF_INET;
    	bcopy((char *)server->h_addr, 
		(char *)&serveraddr.sin_addr.s_addr, server->h_length);
    		 serveraddr.sin_port = htons(portno);

    	/* connect: create a connection with the server */
    	if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
      		error("ERROR connecting");

	if (login(sockfd) == -1)
	{
		printf("Exit right here\n");
		error("ERROR login");
	}

	pthread_create(&list_thread_id, NULL, getuserlist, NULL); 
    	pthread_join(list_thread_id, NULL); 

    	//strcpy(buf, "quit0");
	//while (strcmp(buf, "quit") != 0)
    	//{
	//	printf ("Calling getuserlist()\n");
	//	getuserlist();
	//	sleep(15);
		
		//chat();
	//}
	//while (1);

	//logout();




#if 0
    strcpy(buf, "quit0");

    while (strcmp(buf, "quit") != 0)
    {
      /* get message line from the user */
      printf("Please enter msg: ");
      bzero(buf, BUFSIZE);
    //fgets(buf, BUFSIZE, stdin);
	scanf("%s", buf);

    /* send the message line to the server */
    n = write(sockfd, buf, strlen(buf));
    if (n < 0) 
      error("ERROR writing to socket");
    /* print the server's reply */
    bzero(rbuf, BUFSIZE);
    n = read(sockfd, rbuf, BUFSIZE);
    if (n < 0) 
      error("ERROR reading from socket");
    printf("Echo from server: %s\n", rbuf);
    printf("buf %s\n", buf);
#endif
    close(sockfd);
    return 0;
}

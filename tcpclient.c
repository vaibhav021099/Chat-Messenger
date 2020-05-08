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
char wbuf[DATA_SIZE];
char rbuf[DATA_SIZE];

char username[NAME_SIZE];
char password[NAME_SIZE];
ACCOUNT account;
pthread_t list_thread_id, read_thread_id, write_thread_id; 
char thread_status = 1;
pthread_t thread_id, readthread_id, writethread_id;
pthread_t initiaterthread_id, responderthread_id;
char selecteduser = 0;


void error(char *msg) {
    	perror(msg);
    	exit(0);
}

int login(int sckfd)
{
	int l;
	int retval = 0;
	PACKET rlpacket;
	PACKET wlpacket;
	wlpacket.startbyte = START_BYTE;
	wlpacket.command   = LOGIN;
	wlpacket.size 	  = 0;	
	strcpy(wlpacket.username,username);	
	strcpy(wlpacket.password,password);	
	wlpacket.endbyte   = END_BYTE;
	
	/* send the message line to the server */
    	l = write(sockfd, &wlpacket, sizeof (wlpacket));
    	if (l < 0) 
	{
		printf ("login message send failure\n");
		return -1;
	}

	l = read(sockfd, &rlpacket, sizeof (rlpacket));
    	if (l < 0) 
	{
      		printf("login verification read failure\n");
		return -1;	
	}
	switch (rlpacket.command)
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

void selectuser()
{
       	int w;
        PACKET rwpacket;
        PACKET wwpacket;
        int count = 0;
        int i, retval = 0;

	printf ("Select the user for the list : ");
	scanf ("%d", &selecteduser);

        wwpacket.startbyte = START_BYTE;
        wwpacket.command   = SELECTUSER;
        wwpacket.size      = 0;
        strcpy(wwpacket.username,username);
        strcpy(wwpacket.password,password);
	wwpacket.selecteduser = selecteduser;
        wwpacket.endbyte   = END_BYTE;



        wwpacket.selecteduser = selecteduser;
        memcpy (&wwpacket.data[0], &wbuf, strlen(wbuf));
        wwpacket.size      = strlen(wbuf);
        /* send the message line to the server */
        w = write(sockfd, &wwpacket, sizeof (wwpacket));
        if (w < 0)
        {
        	printf ("writemessage message send failure\n");
                return -1;
        }
}

void getuserlists()
{
	int g;
        PACKET rgpacket;
        PACKET wgpacket;
	int count = 0;
        int i, retval = 0;
        wgpacket.startbyte = START_BYTE;
        wgpacket.command   = USERLIST;
        wgpacket.size      = 0;  
        strcpy(wgpacket.username,username);
        strcpy(wgpacket.password,password);
        wgpacket.endbyte   = END_BYTE;

	/* send the message line to the server */
        g = write(sockfd, &wgpacket, sizeof (wgpacket));
	if (g < 0)
	{
                printf ("getuserlist message send failure\n");
               	return -1;
	}

        g = read(sockfd, &rgpacket, sizeof (rgpacket));
	if (g < 0)
        {
               	printf("getuserlist message read failure\n");
	        return -1;
        }
	switch (rgpacket.command)
        {
               	case USERLIST:
                       	printf("Got user list success\n");
			memcpy (&account, &rgpacket.data[0], sizeof(account));
			for (i=1;i<account.totaluser;i++)
			{
				if (account.user[i].onlinestatus == 1)
				{
					printf("\t %d - %s\n", account.user[i].socketfd, account.user[i].username);
				}	
			}
	                retval = 0;
                break;
               	default:
                       	retval = -1;
	        break;
        }
	return NULL;
}

void *getuserlist(void *vargp)
{
	int g;
        PACKET rgpacket;
        PACKET wgpacket;
	int count = 0;
        int i, retval = 0;
        wgpacket.startbyte = START_BYTE;
        wgpacket.command   = USERLIST;
        wgpacket.size      = 0;  
        strcpy(wgpacket.username,username);
        strcpy(wgpacket.password,password);
        wgpacket.endbyte   = END_BYTE;

	while (thread_status == 1)
	{
	        /* send the message line to the server */
        	g = write(sockfd, &wgpacket, sizeof (wgpacket));
	        if (g < 0)
	        {
        	        printf ("getuserlist message send failure\n");
                	return -1;
	        }

        	g = read(sockfd, &rgpacket, sizeof (rgpacket));
	        if (g < 0)
        	{
                	printf("getuserlist message read failure\n");
	                return -1;
        	}
	        switch (rgpacket.command)
        	{
                	case USERLIST:
                        	printf("Got user list success\n");
				memcpy (&account, &rgpacket.data[0], sizeof(account));
				for (i=1;i<account.totaluser;i++)
				{
					if (account.user[i].onlinestatus == 1)
					{
						printf("\t %d - %s\n", account.user[i].socketfd, account.user[i].username);
					}	
				}
	                        retval = 0;
        	        break;
                	default:
                        	retval = -1;
	                break;
        	}
//		if (++count == 5)
//		{
//			thread_status = 0;
//		}
		sleep(5);
	}
	return NULL;
}

void *writemessage(void *vargp)
{
	int w;
        PACKET rwpacket;
        PACKET wwpacket;
        int count = 0;
        int i, retval = 0;
        wwpacket.startbyte = START_BYTE;
        wwpacket.command   = CHAT;
        wwpacket.size      = 0;
        strcpy(wwpacket.username,username);
        strcpy(wwpacket.password,password);
        wwpacket.endbyte   = END_BYTE;

        while (thread_status == 1)
        {
		printf ("Write : ");
		//scanf("%s", wbuf);
		bzero(wbuf, DATA_SIZE);
		memset(wbuf,0, DATA_SIZE);
    		fgets(wbuf, DATA_SIZE, stdin);
		//scanf("%s", wbuf);

		
		wwpacket.selecteduser = selecteduser;
		memcpy (&wwpacket.data[0], &wbuf, strlen(wbuf));
        	wwpacket.size      = strlen(wbuf);
                /* send the message line to the server */
                w = write(sockfd, &wwpacket, sizeof (wwpacket));
                if (w < 0)
                {
                        printf ("writemessage message send failure\n");
                        return -1;
                }
		strcpy(wbuf,wbuf);
		if (strcmp (wbuf, "logout")  == 0)
		{
			thread_status = 0;
		}

        }
        return NULL;
}


void *readmessage(void *vargp)
{
	int r;
        PACKET rrpacket;
        PACKET wrpacket;
	while (thread_status == 1)
	{
        	r = read(sockfd, &rrpacket, sizeof (rrpacket));
	        if (r < 0)
        	{
                	printf("getuserlist message read failure\n");
	                return -1;
        	}
	        if  (rrpacket.command == CHAT)
        	{
                       	printf("Got chat message success of size %d\n", rrpacket.size);
			strncpy (rbuf, &rrpacket.data[0], rrpacket.size);
			//strcat (rbuf, '\0');
			printf (rbuf);
        	}
		strcpy(rbuf,rbuf);
                if (strcmp (rbuf, "logout")  == 0)
                {
                        thread_status = 0;
                }

        }
        return NULL;
}

void *initiater(void *vargp)
{
        int w; 
        PACKET rrpacket;
        PACKET wwpacket;
	char slctuser;

	printf ("Select the user : ");
	scanf ("%d", &slctuser);

	if (selecteduser == 0)
	{
		selecteduser = slctuser;

	        wwpacket.startbyte = START_BYTE;
        	wwpacket.command   = SELECTUSER;
        	wwpacket.size      = 0;
        	strcpy(wwpacket.username,username);
        	strcpy(wwpacket.password,password);
        	wwpacket.selecteduser = selecteduser;
       	 	wwpacket.endbyte   = END_BYTE;



        	/* send the message line to the server */
        	w = write(sockfd, &wwpacket, sizeof (wwpacket));
        	if (w < 0)
        	{	
                	printf ("writemessage message send failure\n");
                	return -1;
        	}

	}
	printf("initiater exit\n");
	pthread_cancel(responderthread_id); 
	pthread_exit(NULL);
        return NULL;
}

void *responder(void *vargp)
{
        int r; 
        PACKET rrpacket;
        PACKET wrpacket;
	char slctuser = 0;
	printf ("responder thread \n");
        while (slctuser == 0)
        {
                r = read(sockfd, &rrpacket, sizeof (rrpacket));
                if (r < 0)
                {
                        printf("selecteduser message read failure\n");
                        return -1;
                }
		printf ("responder received\n");
                if  (rrpacket.command == SELECTUSER)
                {
                        printf("Got user selection %d\n", rrpacket.selecteduser);
			selecteduser = rrpacket.selecteduser;
			slctuser = selecteduser;
                }
        }
	printf("responder exit\n");
	pthread_cancel(initiaterthread_id); 
	pthread_exit(NULL);
        return NULL;
}

void whoyouare()
{
	printf("Inside whoyouare\n");
        pthread_create(&initiaterthread_id, NULL, initiater, NULL);
        pthread_create(&responderthread_id, NULL, responder, NULL);
}

void chat()
{
	printf("Inside chat\n");
        pthread_create(&writethread_id, NULL, writemessage, NULL);
        pthread_create(&readthread_id, NULL, readmessage, NULL);

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

	//pthread_create(&thread_id, NULL, getuserlist, NULL); 
	printf("Press Enter\n");
	fgetc(stdin);
	getuserlists();
	//getc();

	whoyouare();
        //pthread_join(initiaterthread_id, NULL);
        //pthread_join(responderthread_id, NULL);
	while (selecteduser == 0);
	printf("I am here\n");
	chat();
	while (thread_status == 1);
#if 0
    	pthread_join(thread_id, NULL); 
        pthread_join(writethread_id, NULL);
        pthread_join(readthread_id, NULL);
#endif



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


#include <stdio.h>

#include <stdlib.h>

#include <stddef.h>

#include <string.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include "common.h"

/* port we're listening on */

#define PORT 2020

PACKET rpacket;
PACKET wpacket;
ACCOUNT account;

char username[NAME_SIZE];
char password[NAME_SIZE];

int getaccount();
int verifyaccount(int sfd, char *uname, char *pword);
void writetothefile();

int main(int argc, char *argv[])

{

	/* master file descriptor list */

	fd_set master;

	/* temp file descriptor list for select() */

	fd_set read_fds;

	/* server address */

	struct sockaddr_in serveraddr;

	/* client address */

	struct sockaddr_in clientaddr;

	/* maximum file descriptor number */

	int fdmax;

	/* listening socket descriptor */

	int listener;

	/* newly accept()ed socket descriptor */

	int newfd;

	/* buffer for client data */

	char buf[1024];

	int nbytes;

	/* for setsockopt() SO_REUSEADDR, below */

	int yes = 1;

	int addrlen;

	
	int i, j, k;

	/* clear the master and temp sets */

	FD_ZERO(&master);

	FD_ZERO(&read_fds);

	if (getaccount() == -1)
	{
		printf ("Account file not valid\n");
		exit (0);
	}


	/* get the listener */

	if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)

	{

		perror("Server-socket() error lol!");

		/*just exit lol!*/

		exit(1);

	}

	printf("Server-socket() is OK...\n");

	/*"address already in use" error message */

	if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)

	{

		perror("Server-setsockopt() error lol!");

		exit(1);

	}

	printf("Server-setsockopt() is OK...\n");



	/* bind */

	serveraddr.sin_family = AF_INET;

	serveraddr.sin_addr.s_addr = INADDR_ANY;

	serveraddr.sin_port = htons(PORT);

	memset(&(serveraddr.sin_zero), '\0', 8);



	if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)

	{

		perror("Server-bind() error lol!");

		exit(1);

	}

	printf("Server-bind() is OK...\n");



	/* listen */

	if(listen(listener, 10) == -1)

	{

		perror("Server-listen() error lol!");

		exit(1);

	}

	printf("Server-listen() is OK...\n");



	/* add the listener to the master set */

	FD_SET(listener, &master);

	/* keep track of the biggest file descriptor */

	fdmax = listener; /* so far, it's this one*/



	/* loop */

	for(;;)

	{

		/* copy it */

		read_fds = master;



		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)

		{

			perror("Server-select() error lol!");

			exit(1);

		}

		printf("Server-select() is OK...\n");



		/*run through the existing connections looking for data to be read*/

		for(i = 0; i <= fdmax; i++)

		{

			if(FD_ISSET(i, &read_fds))

			{ /* we got one... */

				if(i == listener)

				{

					/* handle new connections */

					addrlen = sizeof(clientaddr);

					if((newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1)

					{

						perror("Server-accept() error lol!");

					}

					else

					{

						printf("Server-accept() is OK...\n");



						FD_SET(newfd, &master); /* add to master set */

						if(newfd > fdmax)

						{ /* keep track of the maximum */

							fdmax = newfd;

						}

						printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr), newfd);

					}

				}

				else

				{
					//printf ("I am here 1 \n");

					/* handle data from a client */

					//if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
					if((nbytes = recv(i, &rpacket, sizeof(rpacket), 0)) <= 0)

					{
						//printf ("I am here 2 \n");

						/* got error or connection closed by client */

						if(nbytes == 0)
						{

							/* connection closed */

							printf("%s: socket %d hung up\n", argv[0], i);

						        for (k=1;k<account.totaluser;k++)
       							{
					                	if ( account.user[k].socketfd == i)
               							{
                       							account.user[k].onlinestatus = 0;
								 	account.user[k].chating = 0;
								        account.user[k].selecteduser = 0;
									writetothefile();
						                        break;
						                }
						        }


						}



						else

							perror("recv() error lol!");



						/* close it... */

						close(i);

						/* remove from master set */

						FD_CLR(i, &master);

					}

					else

					{
						//printf ("I am here 3 fdmax = %d\n", fdmax);

						/* we got some data from a client*/

						for(j = 0; j <= fdmax; j++)

						{
						//printf ("I am here 4 j= %d\n", j);

							/* send to everyone! */

							if(FD_ISSET(j, &master))

							{
								//printf ("I am here 5 i = %d listener = %d j= %d\n", i, listener, j);

								/* except the listener and ourselves */

								//if(j != listener && j != i)
								//if(j != listener )
								if(j == i )

								{
									//printf ("I am here 6 i = %d listener = %d j= %d\n", i, listener, j);
									switch (rpacket.command)
        								{
                								case LOGIN:
											if (verifyaccount(i,rpacket.username, rpacket.password) \
												!= -1)
											{
												wpacket.startbyte = START_BYTE;
        											wpacket.command   = LOGIN_SUCCESS;
        											wpacket.size      = 0;  
        											wpacket.endbyte   = END_BYTE;
											}
											else
											{
												wpacket.startbyte = START_BYTE;
        											wpacket.command   = LOGIN_FAILURE;
        											wpacket.size      = 0;  
        											wpacket.endbyte   = END_BYTE;
											}
                								break;
                                                                                case USERLIST:
                                                                                	wpacket.startbyte = START_BYTE;
                                                                                        wpacket.command   = USERLIST;
                                                                                        wpacket.size      = 0;
											memcpy (&wpacket.data[0], &account, sizeof(account));
                                                                                        wpacket.endbyte   = END_BYTE;
                                                                                break;
										case CHAT:
											printf("Received chat message %d\n", \
												rpacket.selecteduser);
                                                                                        wpacket.startbyte = START_BYTE;
                                                                                        wpacket.command   = CHAT;
                                                                                        wpacket.size      = rpacket.size;
                                                                                        memcpy (&wpacket.data[0], &rpacket.data[0], \
												rpacket.size);
                                                                                        wpacket.endbyte   = END_BYTE;
											if(send(rpacket.selecteduser, \
												&rpacket, sizeof(rpacket), 0) == -1)
												perror("send() error lol!");
                                                                                break;
                                                                                case SELECTUSER:
                                                                                        printf("Received user selection message %d\n", \
                                                                                                rpacket.selecteduser);
                                                                                        wpacket.startbyte = START_BYTE;
                                                                                        wpacket.command   = SELECTUSER;
											wpacket.selecteduser = j;
                                                                                        wpacket.endbyte   = END_BYTE;
											j = rpacket.selecteduser;
                                                                                break;

        								}


									//if(send(j, buf, nbytes, 0) == -1)
									if(send(j, &wpacket, sizeof(wpacket), 0) == -1)

										perror("send() error lol!");

								}

							}

						}

					}

				}

			}

		}

	}

	return 0;

}


int getaccount()
{
        int i;
        FILE *infile;

        // open file for reading 
        infile = fopen ("account.dat", "r");
        if (infile == NULL)
        {
                fprintf(stderr, "\nError opend file\n");
                return -1;
        }

        // read struct to file 
        fread (&account, sizeof(ACCOUNT), 1, infile);

        if (account.totaluser == 0)
        {

                fprintf(stderr, "\nadmin account not present in the opend file\n");
                fclose(infile);
                return -1;
        }

        if (account.totaluser == 20)
        {

                fprintf(stderr, "\nMaximum user already present in the opend file\n");
                fclose(infile);
                return -1;
        }
        fclose(infile);
	return 0;
}

int verifyaccount(int sfd, char *uname, char *pword)
{
	int i,j;
	int retval = -1;
	printf ("Inside verifyaccoutn %d total user %d \n",sfd, account.totaluser);
	getaccount();

	for (i=1;i<account.totaluser;i++)
        {
		if ( (strcmp(uname, account.user[i].username) == 0) && \
			(strcmp(pword, account.user[i].password) == 0) )
		{
			account.user[i].onlinestatus = 1;
			account.user[i].socketfd = sfd;
			writetothefile();
			retval = i;
			break;
		}
        }

        for (j=1;j<account.totaluser;j++)
        {
                if ( account.user[j].onlinestatus == 1)
                {
			printf ("%d online\n",j);
                }
        }
	
	return retval;
}


void writetothefile()
{
	FILE *infile;

	printf ("Inside writetothefile\n");
        // open file for reading 
        infile = fopen ("account.dat", "w");
        if (infile == NULL)
        {
                fprintf(stderr, "\nError opend file\n");
                return;
        }


        // write struct to file 
        fwrite (&account, sizeof(ACCOUNT), 1, infile);

        if(fwrite != 0)
                printf("contents to file written successfully !\n");
        else
                printf("error writing file !\n");

        // close file 
        fclose (infile);
}

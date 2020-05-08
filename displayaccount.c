// C program for writing  
// struct to file 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "common.h"
  
// a struct to read and write 
#define TOTAL_USER 21  
int main () 
{ 
	int i;
    	FILE *infile; 
	ACCOUNT account;
      
    	// open file for reading 
    	infile = fopen ("account.dat", "r"); 
    	if (infile == NULL) 
    	{	 
        	fprintf(stderr, "\nError opend file\n"); 
        	exit (1); 
    	} 

    	// read struct to file 
    	fread (&account, sizeof(ACCOUNT), 1, infile); 

	for (i=0;i<account.totaluser;i++)
	{
		printf ("online status of %d is  %d\n", i, account.user[i].onlinestatus);
        	printf ("user socketfd of %d is %d\n", 	i, account.user[i].socketfd);
        	printf ("user name of %d is %s\n", 	i, account.user[i].username);
		printf ("password of %d is %s\n", 	i, account.user[i].password);
	}
	
    	fclose (infile); 
  
    	return 0; 
} 

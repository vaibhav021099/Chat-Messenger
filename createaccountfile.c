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
      
    	// open file for writing 
    	infile = fopen ("account.dat", "w"); 
    	if (infile == NULL) 
    	{	 
        	fprintf(stderr, "\nError opend file\n"); 
        	exit (1); 
    	} 

	account.totaluser = 1;
	account.user[0].onlinestatus = 1;
	account.user[0].socketfd = 3;
	strcpy (account.user[0].username, "admin");
	strcpy (account.user[0].password, "admin");

	for (i=1;i<TOTAL_USER;i++)
	{
		account.user[i].onlinestatus = 0;
		account.user[i].socketfd = 0;
		strcpy (account.user[i].username, "");
		strcpy (account.user[i].password, "");
		account.user[i].chating = 0;
		account.user[i].selecteduser = 0;
	}
  
    	// write struct to file 
    	fwrite (&account, sizeof(ACCOUNT), 1, infile); 
      
    	if(fwrite != 0)  
        	printf("contents to file written successfully !\n"); 
    	else 
        	printf("error writing file !\n"); 
  
    	// close file 
    	fclose (infile); 
  
    	return 0; 
} 

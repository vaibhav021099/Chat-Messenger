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
	
	if (account.totaluser == 0)
	{
	
        	fprintf(stderr, "\nadmin account not present in the opend file\n"); 
		fclose(infile);
		exit(0);
	}
	
	if (account.totaluser == 20)
	{
	
        	fprintf(stderr, "\nMaximum user already present in the opend file\n"); 
		fclose(infile);
		exit(0);
	}
	fclose(infile);


        // open file for reading 
        infile = fopen ("account.dat", "w");
        if (infile == NULL)
        {        
                fprintf(stderr, "\nError opend file\n");
                exit (1);
        }

	printf ("Enter User Name : ");
	scanf ("%s", &account.user[account.totaluser].username);

	printf ("Enter Password Name : ");
	scanf ("%s", &account.user[account.totaluser].password);

	account.totaluser++;

  
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

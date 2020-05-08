#ifndef _COMMON_H
#define _COMMON_H

#define NAME_SIZE 25
#define TOTAL_USER 21
#define DATA_SIZE 1024
#define START_BYTE 0xFE
#define END_BYTE 0xFD

typedef enum cmd
{
	LOGIN,
	LOGIN_SUCCESS,
	LOGIN_FAILURE,
	USERLIST,
	SELECTUSER,
	CHAT,
	LOGOUT,
	MAX_COMMAND
}COMMAND;	

typedef struct
{
	char startbyte;
	char command;
	short size;
	char user;
	char username[NAME_SIZE];
	char password[NAME_SIZE];
	char selecteduser;
	char selectedusername[NAME_SIZE];
	char data[DATA_SIZE];
	char endbyte;
}PACKET;


typedef struct
{
	char onlinestatus;
	int  socketfd;
	char username[NAME_SIZE];
	char password[NAME_SIZE];
	char chating;
	char selecteduser;
}USER;

typedef struct
{
	char totaluser;
	USER user[TOTAL_USER];
	//char password[NAME_SIZE];
}ACCOUNT;

#endif

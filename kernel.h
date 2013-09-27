#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define TERMTYPE "VT100"

enum{
	ECHO =   1,
	SGA  =   3,
	ST   =   5,
	TM   =   6,
	TT   =  24,
	WS   =  31,
	TS   =  32,
	RFC  =  33,
	LM   =  34,
	XDL  =  35,
	EV   =  36,
	NEO  =  39,

	SE   = 240,
	NOP  = 241,
	DM   = 242,
	BRK  = 243,
	IP   = 244,
	AO   = 245,
	AYT  = 246,
	EC   = 247,
	EL   = 248,
	GA   = 249,
	SB   = 250,
	WILL = 251,
	WONT = 252,
	DO   = 253,
	DONT = 254,
	IAC  = 255
};

/* connect server return the sock fd */
int do_connect(char *hostname, int port);

/* return buf len */
int read_byte(int sockfd);
int read_bytes(int sockfd, char *buf, int size);

int write_byte(int sockfd, char buf);
int write_bytes(int sockfd, char *buf, int size);

int send_command(int sockfd, int comm, int opt);

int proc_sb(int sockfd);
int proc_will(int sockfd);
int proc_wont(int sockfd);
int proc_do(int sockfd);
int proc_dont(int sockfd);
int proc_iac(int sockfd);

int send_sb(int sockfd, char opt1, char opt2, char *opt3, int size);


#endif
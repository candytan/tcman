#include "kernel.h"


/* 
if succeed, return sockfd
else return -1
*/
int do_connect(char *hostname, int port)
{
	int sockfd;
	struct sockaddr_in addr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket failed");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(hostname);
	bzero(&(addr.sin_zero), 8);

	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0){
		perror("connect failed");
		return -1;
	}

	return sockfd;
}

int read_byte(int sockfd)
{
	char buf;
	recv(sockfd, &buf, 1, 0);
	return buf;
}

int read_bytes(int sockfd, char *buf, int size)
{
	return recv(sockfd, buf, size, 0);
}

int write_byte(int sockfd, char buf)
{
	return send(sockfd, &buf, 1, 0);
}

int write_bytes(int sockfd, char *buf, int size)
{
	return send(sockfd, buf, size, 0);
}

int send_command(int sockfd, int comm, int opt)
{
	unsigned char buf[3];
	buf[0] = (unsigned char)IAC;
	buf[1] = (unsigned char)comm;
	buf[2] = (unsigned char)opt;

	return write_bytes(sockfd, buf, 3);
}

int proc_sb(int sockfd)
{
	char req, buf1, buf2;
	req = read_byte(sockfd);
	buf2 = read_byte(sockfd);

	while(1){
		buf1 = buf2;
		buf2 = read_byte(sockfd);
		if(buf1 == IAC && buf2 == SE)
			break;
	}

	switch(req){
		case TT:
			send_sb(sockfd, req, 0, TERMTYPE, strlen(TERMTYPE));
			break;

		case TS:
			send_sb(sockfd, req, 0, "38400,38400", 11);
			break;

		default:
			break;
	}
}

int proc_will(int sockfd)
{
	char req;
	req = read_byte(sockfd);

	if(req == ECHO || req == SGA)
		return send_command(sockfd, DO, req);
	else
		return send_command(sockfd, DONT, req);
}

int proc_wont(int sockfd)
{
	char req;
	req = read_byte(sockfd);
	return send_command(sockfd, DONT, req);
}

int proc_do(int sockfd)
{
	char req;
	req = read_byte(sockfd);

	if(req == WS){
		send_command(sockfd, WILL, req);
		// Jserv says "FIXME magic number (80x24)"
		char ws[] = {0x50, 0x00, 0x18};
		send_sb(sockfd, req, 0, ws, 3);
	}
	else if(req == TT || req == TS){
		send_command(sockfd, WILL, req);
	}
	else if(req == ECHO){
		send_command(sockfd, WONT, req);
	}
	else {
		send_command(sockfd, WONT, req);
	}

	return 0;
}

int proc_dont(int sockfd)
{
	char req;
	req = read_byte(sockfd);

	switch(req){
		case SB:
			proc_sb(sockfd);
			break;
		case WILL:
			proc_will(sockfd);
			break;
		case WONT:
			proc_wont(sockfd);
			break;
		case DO:
			proc_do(sockfd);
			break;
		case DONT:
			proc_dont(sockfd);
			break;
		default:
			break;
	}

	return 0;
}

int proc_iac(int sockfd)
{
	char req;
	req = read_byte(sockfd);

	switch(req){
		case SB:
			proc_sb(sockfd);
			break;
		case WILL:
			proc_will(sockfd);
			break;
		case WONT:
			proc_wont(sockfd);
			break;
		case DO:
			proc_do(sockfd);
			break;
		case DONT:
			proc_dont(sockfd);
			break;
		default:
			break;
	}
}

int send_sb(int sockfd, char opt1, char opt2, char *opt3, int size)
{
	int i;
	char *buf = malloc(sizeof(char) * (size + 6));

	buf[0] = IAC;
	buf[1] = SB;
	buf[2] = opt1;
	buf[3] = opt2;

	for(i = 0; i < size; i++){
		buf[4 + i] = opt3[i];
	}

	buf[4 + size] = IAC;
	buf[5 + size] = SE;

	return write_bytes(sockfd, buf, size);
}
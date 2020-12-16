#include <stdio.h>
#include <stdlib.h>


// types and socket stuff
#include <sys/types.h>
#include <sys/socket.h>

#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

// stuff to do because I can

// IP and protocol stuff
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 *	Sami Alameddine, 2020
 *
 * 	This is another web server implementation by me
 *
 *
 * 	<executioncommand><binaryname> <portno> <file>
 * */

// defines
#define SAYHI //printf("\033[0;32mhello\033[0m\n");fflush(stdout);
#define SA struct sockaddr
#define DEFAULT_PORT 6567
#define MAXLINE 4096
#define HTTP_HDR_RP_SZ 21

// configuration defines

#define ALS_DEBUG_TARGET
#define PORTNO_SAFETY
#define NSFF
#define INTERRUPT_DIE
// Debug boilerplate

static FILE *FN;
void handler(int handlerNum);
#ifdef ALS_DEBUG_TARGET
char *bin2hex(const unsigned char *input, size_t len){
	char *db_res;
	char hexits[]="0123456789ABCDEF";
	if(input==NULL||len<=0){
		return NULL;
	}
	int res_db_len=(len+3)+1;

	db_res=(char*)malloc(res_db_len);
	bzero(db_res,res_db_len);

	for(int db_i=0;db_i<len;db_i++){
		db_res[db_i*3]	=hexits[input[db_i]>>4];
		db_res[(db_i*3)+1]	=hexits[input[db_i]&0x0F];
		db_res[(db_i*3)+2]	=' ';
	}
	return db_res;
}

#endif

int main(int argc, char** argv) {
	SAYHI
	int PORT_NO;
	signal(SIGINT, handler);
	FILE  *CONF;
	fprintf(stdout,"Loading Config...\n");
	fflush(stdout);
	CONF=fopen(".wsconf","rb");
	if(CONF==NULL){
		fprintf(stderr,"\033[1;31mFailed to load config, reverting to defaults set at compile time\033[0m\n");
		PORT_NO=80;
	}
	fseek(CONF,0,SEEK_END);
    long config_fsize=ftell(CONF);
	fseek(CONF,0,SEEK_SET);
    char* conf_buffer=calloc(1, sizeof(char)*(config_fsize+1));
    	fread(conf_buffer, config_fsize,1, CONF);
	if(sscanf(conf_buffer,"CONF_TYPE ALSAMI_WEB\nPORTNO %d\nMPG index.html", &PORT_NO)==EOF){fprintf(stderr,"\033[1;31mERORR: CANNOT READ FROM CONFIGURATION FILE! Reverting to defaults set at compile time\033[0m\n");}
    free(conf_buffer);
#ifdef PORTNO_SAFETY
	if(PORT_NO<10){
		fprintf(stderr,"You put a port number below 10!\nDue to safety features determined at compile time, the port number will revert to the default selected at compile time\n");
		PORT_NO=80;
	}
#endif
	fprintf(stdout,"CONFIG DETAILS:\n\tPort Number: %d\n", PORT_NO);
	fclose(CONF);
	//int PORT_NO=argv[1];
	//char filenm=argv[2];
	SAYHI
	char filenm[]="index.html";
	// declare stuff
	int  listenfd,connfd;
	struct sockaddr_in servaddr;
	//uint8_t buff[MAXLINE+1];
	uint8_t recvline[MAXLINE+1];
	// allocates resources for an internet socket (a TCP socket)
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0){
		fprintf(stderr,"\033[1;31mError While trying to create socket\033[0m\n");
		exit(1);
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=		AF_INET;
	// just says that it would respond to anything
	servaddr.sin_addr.s_addr=	htonl(INADDR_ANY);
	servaddr.sin_port=		htons(PORT_NO);
	SAYHI
	// listen and bind
	if((bind(listenfd,(SA *)&servaddr,sizeof(servaddr)))<0){
		fprintf(stderr,"\033[1;31mError on bind\033[0m\n");
		exit(1);
	}
	if((listen(listenfd,10))<0){
		fprintf(stderr, "Error on listen");
		exit(1);
	}

	FN=fopen("log","w+");

	if(FN=NULL){
		fprintf(stderr,"\033[1;31mCannot Open Log File!\033[0m\n");exit(1);
	}

	FILE *FILEN;
	FILEN=fopen(filenm, "r");
	if(FILEN==NULL){fprintf(stderr,"File Error\n");fflush(stderr);}
	fseek(FILEN,0,SEEK_END);
	long fsize=ftell(FILEN);
	fseek(FILEN,0,SEEK_SET);
	SAYHI
	uint8_t buff[fsize+(HTTP_HDR_RP_SZ+1)];

	//char *webpage=malloc(fsize+1);
	char *webpage[fsize+1];
	fread(webpage,1,fsize,FILEN);

	// sprintfs the thing

	//char webpage[]="HTTP/1.1 200 OK\r\n\r\nHello";

	fclose(FILEN);

	int n;
	// infinite loop for handling requests
	for(;;){
		struct sockaddr_in addr;
		socklen_t addr_len;

		// accept blocks until an incoming connection arrives
		// it returns a "file descriptor" to the connection
		fprintf(stdout,"Waiting for a connection on port %d\n", PORT_NO);
		fflush(stdout);
		connfd=accept(listenfd, (SA *) NULL, NULL);
		SAYHI
		memset(recvline,0,MAXLINE);
		// read the client's message
		n=read(connfd,recvline,MAXLINE-1);
		char* getnm=malloc(sizeof(char)*40);

		if(n<0){fprintf(stderr,"cannot read what the client said! did the client send anything?");}
		if(sscanf(recvline, "GET /%s HTTP/1.1", getnm)==EOF){fprintf(stderr,"Cannot read req");}

        	printf("[alsws]: size of getnm: %zu\n", strlen(getnm));
		if((strcmp("/", getnm))!=0){
			fprintf(stdout,"Special Request Recognized\n");
			FILE *GETF;
			GETF=fopen(getnm, "r");
			if(GETF==NULL){fprintf(stderr,"Cannot read file!\n");}else{
        	        	fseek(GETF,0,SEEK_END);
      		        	long getsz=ftell(GETF);
                		fseek(GETF,0,SEEK_SET);		// same as rewind(f)
               			char *getpg= (char*) calloc(1,(sizeof(char))*getsz+1);
                		fread(getpg, 1, getsz, GETF);
               			fclose(GETF);
                		uint8_t gbuff[getsz+(HTTP_HDR_RP_SZ+1)];
                		snprintf((char*)gbuff, sizeof(gbuff), "HTTP/1.1 200 OK\r\n\r\n%s\r\n",getpg);

	               	 	if(write(connfd,&gbuff,(getsz+(21-1)))==-1){fprintf(stderr,"\033[1;31mUnable to write to socket!\033[0m\n");}
				free(getpg);
			}
		}else if((strcmp(getnm, "/"))==0){

			snprintf((char*)buff, sizeof(buff),"HTTP/1.1 200 OK\r\n\r\n%s\r\n",*webpage);

			//write(connfd,(char*)buff, strlen((char*)buff));
			if(write(connfd,&buff,(fsize+(21-1)))==-1){fprintf(stderr,"\033[1;31mUnable to write to socket!\033[0m\n");}
		}//fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline,(n+1)), recvline);
		//fprintf(FN, "\n%s\n\n%s", bin2hex(recvline,(n+1)), recvline);
		close(connfd);
		SAYHI
		// clean stuff up
		free(getnm);
	}
	fclose(FN);
}

void handler(int handlerNum) {
#ifdef INTERRUPT_DIE
    fprintf(stderr, "[alsws]: Interruption. Closing components and quitting.\n");
    fclose(FN);
    exit(2);
#endif // INTERRUPT_DIE

}

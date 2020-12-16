#ifndef ALSWS_TEMPLATE__WS_H
#define ALSWS_TEMPLATE__WS_H

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

// types and socket stuff
#include <sys/types.h>
#include <sys/socket.h>

#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

// IP and protocol stuff
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

// debug stuff
#include "debug.h"

// cflags:
// -lpthread

#define MMNN 4096

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 80
#endif

#define SA struct sockaddr

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_ACCEPTED 202
#define HTTP_NOCONTENT 204
#define HTTP_RESETCONTENT 205
#define HTTP_MOVEDPERM 301
#define HTTP_FOUND 302
#define HTTP_SEEOTHER 303
#define HTTP_USEPROXY 305
#define HTTP_TEMPREDIRECT 307
#define HTTP_PERMREDIRECT 308
#define HTTP_BADREQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_FORBIDDEN 403
#define HTTP_NOTFOUND 404

#ifndef HTTP_BUFNM
#define HTTP_BUFNM alsws_http_pgbuf
#endif

size_t str2null(char* __str){
	size_t len;
	while(*__str++){
		len++;
		if(__str[len]=='\0')
			break;
	}
	return len;
}

typedef struct ALSWS_TCP_STRUCT{
	char* response;
	char* read;
}ws_tcp_T;



typedef struct ALSWS_STRUCT{
	int PORT_NO;
} alsws_T;

alsws_T* alsws_init(int PORTNO){
	alsws_T* alsws;
	alsws=(alsws_T*)calloc(1, sizeof(alsws_T));	

	return alsws;
}

void alsws_destroy(alsws_T* alsws){
	free(alsws);
}

class TcpServer{
public:
	int listenfd, connfd, n;
	int PORT_NO;
	struct sockaddr_in servaddr;
	void killCommunications();
	void listenAndBind();

	uint8_t recvline[MMNN+1];

	TcpServer(alsws_T* ws_opt){

		// allocate and initalize the server components
		if(ws_opt->PORT_NO==NULL){
			fprintf(stderr,"[ALSWS] Error\nValue NULL passed into the TcpServer constructor\nPort Defaulting to %d\n", DEFAULT_PORT);
			PORT_NO=DEFAULT_PORT;
		} else {
			PORT_NO=ws_opt->PORT_NO;
		}

		// creates and error checks a socket
		if((listenfd=socket(AF_INET, SOCK_STREAM,0))>0){
			fprintf(stderr,"[ALSWS]: ERROR\nError while trying to create socket\n");
			exit(33);
		}bzero(&servaddr,sizeof(servaddr));

		servaddr.sin_family=		AF_INET;
		servaddr.sin_addr.s_addr=	htonl(INADDR_ANY);
		servaddr.sin_port=		htons(PORT_NO);

	}
	~TcpServer(){
		// free up memory
		close(connfd);
	}
};

class HttpServer{
public:
	int listenfd, connfd, n;
	int PORT_NO;
	FILE *webpage;
	void killCommunications();
	void listenAndBind();
	struct sockaddr_in servaddr;
	void MainThreadLoop();
	const char* root_file;

	uint8_t recvline[MMNN+1];

	HttpServer(alsws_T* ws_opt,const char* rootfile){
		
		root_file=rootfile;

		if(ws_opt->PORT_NO==NULL){
			fprintf(stderr,"[ALSWS] Error\nValue NULL passed into the HttpServer constructor\nPort Defaulting to %d\n", DEFAULT_PORT);
			PORT_NO=DEFAULT_PORT;
		} else {
			PORT_NO=ws_opt->PORT_NO;
		}
		// allocate memory and initalize threads
		if((listenfd=socket(AF_INET, SOCK_STREAM,0))<0){
			fprintf(stderr,"[ALSWS]: ERROR\nError while trying to create socket\n");
			exit(33);
		}
		bzero(&servaddr, sizeof(servaddr));

		servaddr.sin_family=		AF_INET;
		servaddr.sin_addr.s_addr=	htonl(INADDR_ANY);
		servaddr.sin_port=		htons(PORT_NO);

	}

	char* getpg(const char* filenm);

	~HttpServer(){
		// free memory
		close(connfd);
	}

private:
	long wp_sz;

};

void HttpServer::killCommunications(){

}

void TcpServer::listenAndBind(){
	if((bind(listenfd,(SA *)&servaddr,sizeof(servaddr)))<0){
		fprintf(stderr,"[ALSWS]: ERROR\nError on bind\nCheck if the port that you are using is already in use\n");
		exit(33);
	}

	if((listen(listenfd,10))<0){
		fprintf(stderr,"[ALSWS]: ERROR\nError on listen.\n");
		exit(33);
	}
}

void HttpServer::listenAndBind(){
	// bind with error checking
	if((bind(listenfd,(SA *)&servaddr,sizeof(servaddr)))<0){
		fprintf(stderr,"[ALSWS]: ERROR\nError on bind\nCheck if the port that you are using is already in use\n");
		exit(33);
	}

	if((listen(listenfd,10))<0){
		fprintf(stderr,"[ALSWS]: ERROR\nError on listen.\n");
		exit(33);
	}
}

// note: the root file name is index.html
char * HttpServer::getpg(const char* filenm){
    char* wp_ret;
    webpage=fopen(filenm,"r");
    if(webpage){
    fseek(webpage, 0, SEEK_END);
    this->wp_sz=ftell(webpage);
    fseek(webpage, 0, SEEK_SET);     // same as rewind(f)
    wp_ret= (char*)calloc(wp_sz, wp_sz);
    if(wp_ret)
    	fread(wp_ret, 1, wp_sz,webpage);

    // close and dinit
    fclose(webpage);
    }
    return wp_ret;
}

void TcpServer::killCommunications(){

}

void HttpServer::MainThreadLoop(){
	// please spin this off into another thread if you can
	char* webr=getpg(root_file);
    	unsigned char buff[wp_sz+21];
	memset(buff, 0, sizeof(buff));
	for(;;) {
		struct sockaddr_in addr;
		socklen_t addr_len;
	
#ifdef CONLOGS
	 printf("\033[0;32mAlsWS: Waiting for a connection on port %d\033[0m\n", this->PORT_NO);
#endif
	 	fflush(stdout);
		
	 	connfd=accept(listenfd, (SA*)NULL,NULL);

		memset(recvline, 0, MMNN);
		n=read(connfd, recvline, MMNN-1);
		// do whatever with recvline here (recvline is what the client sent)
		//\r\ncontent-type: text/html
		snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200 OK\r\n\r\n%s", webr);
		
		puts((char*)buff);fflush(stdout);
		printf("%zu", sizeof(buff));

		if(write(connfd,&buff,(wp_sz+(21)))==-1){fprintf(stderr,"\033[1;31mUnable to write to socket!\033[0m\n");}

		close(connfd);

	}
}

#endif

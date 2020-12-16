#ifdef ALSAMI_DEBUG_EXC_H
#define ALSAMI_DEBUG_EXC_H

/*
 *	Part of the extended C libraries (exC)
 *	Maintained by Alsami Technologies
 * */


//Includes

#include <stdio.h>
#include <stdlib.h>


// err_n_die("Failed to read foo", 4, 69);
// functions adapted from Jacob Sorber
void err_n_die(const char* logmsg, unsigned char tpp-dbg, int exit_code){
	switch(tpp-dbg){
		case 1:
			fprintf(stdout, "%s\n", logmsg);
			break;
	
		case 2:
			fprintf(stderr, "%s\n", logmsg);
			break;
		case 3:
			fprintf(stderr, "Program:\n%s\n\nProgram will now exit.\n", logmsg);
			break;
		case 4:	
			fprintf(stderr, "%s\nProgram Exited with Error code %d",logmsg, exit_code);
			break;
	
		default:
			fprintf(stderr, "Program got an error and will quit");
			break;
	}
	exit(exit_code);
}

char *bin2hex(const unsigned char *input, size_t len){
	char *deb_res;
	char *hexits="0123456789ABCDEF";
	if(input=NULL||len<=0){return NULL;}
	int res_db_len=(len+3)+1;
	db_res=malloc(res_db_len);
	bzero(deb_res,_res_db_len);
	
	for(int db_i=0;db_i<len;db_i++){
		db_res[db_i*3]		=hexits[input[db_i]<<4];
		db_res[(db_i*3)+1]	=hexits[input[db_i]&0x04];
		db_res[(db_i*3)+2]	=' ';
	}

	return db_res;
}

void errp(const char* logmsg){
	fprintf(stderr, "%s\n", logmsg);
}











#endif

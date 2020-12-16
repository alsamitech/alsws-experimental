#include "include/ws.h"

int main(int argc, char** argv){
	alsws_T wst;
	wst.PORT_NO=6969;
	HttpServer* httpServer= new HttpServer(&wst, "index.html");


	httpServer->listenAndBind();
	httpServer->MainThreadLoop();

	return 0;
}

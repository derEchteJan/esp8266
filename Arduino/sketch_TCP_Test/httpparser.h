#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include "httprequest.h"

namespace HTTP_PARSER {
	
	int isDone = 0;
	
	#define lineBufferLength = 1024
	char lbuf[lineBufferLength];
	int lbufInd = 0;
	
	char lastc = 0;
	int currentSection = 1;
	
	// header data
	HTTP::Method method = get
	char* path;
	char** 
	
	void supplyChar(char c, HTTP_REQUEST::Request) {
		
	}
	
	HTTP::Request getRequest() {
		struct HTTP::Request request;
		request.method = method;
	}
}
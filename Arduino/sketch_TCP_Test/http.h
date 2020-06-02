#ifndef HTTP_H
#define HTTP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "httpparser.h"

#pragma once
namespace HTTP
{
	
	// MARK: REQUESTS
	
	enum Method {get = 1, post = 2};
	
	struct Request {
		
		Method method;
		char* path;
		char** params;
		
		int hasPath(char* suffix) {
			char l, r;
			for(int i = 0; l != '\0'; i++) {
				l = path[i]; r = suffix[i];
				if(l == r) continue;
				else return 0;
			}
			return 1;
		}
	};
	
	
	unsigned long currentTime = 0;
	unsigned long previousTime = 0;
	
	void readRequest(WiFiClient client, unsigned long timeout, void (*onRecievedRequest)(WiFiClient, Request)) {
		currentTime = millis();
		previousTime = currentTime;
		while(client.connected() && currentTime - previousTime <= timeout) {
			while (client.available()) {
				char c = client.read();
				Serial.write(c);
				HTTP_PARSER::supplyChar(c);
				if(HTTP_PARSER::isDone) {
					
				}
			}
			currentTime = millis();
		}
		if(currentTime - previousTime > timeout) {
			client.stop();
			Serial.println("Request timed out");
		}
	}
	
	
	// MARK: RESPONSES
	
	struct Response {
		int status;
		char* message;
		//char** headers;
		char* body;
	};
	
	void writeResponse(WiFiClient client, char* status, char* body) {
		client.write("HTTP/1.1 ");
		client.write(status);
		client.write("\nConnection: close\n");
		if(body != NULL) {
			client.write("Content-Type: text/html\n");
			client.write("Access-Control-Allow-Origin: *\n");
			client.write("Content-Length: ");
			int bodyLen = strlen(body);
			int skipLeadingZeroes = true;
			for(int lenDivider = 100; lenDivider > 0; lenDivider /= 10) {
				char digit = (bodyLen / lenDivider % 10) + '0';
				if(digit == '0' && skipLeadingZeroes) {
					continue; 
				} else {
					skipLeadingZeroes = false;
					client.write(digit);
				}
			}
			client.write("\n\n");
			char c = body[0];
			for(int i = 0; c != '\0'; c = body[++i]) client.write(c);
		}
		client.write("\n");
		client.stop();
	}

	
	
}

#endif
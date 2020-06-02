#ifndef WEBFILES_H
#define WEBFILES_H

#pragma once
namespace WEBFILES {
	
	char* buildIndexHtml() {
		char* html = 
		"<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<title>ESP8266</title>"
		"</head>"
		"<body>"
		"<h3>Hello World</h3>"
		"<p id=\"demo\">ESP8266 Web Page</p>"
		"<button type=\"button\" onclick=\"myFunction()\">Try it</button><br>"
		"<button type=\"button\" onclick=\"setStaticColor(255, 255, 255)\">Weiss</button><br>"
		"<button type=\"button\" onclick=\"setStaticColor(255, 0, 0)\">Rot</button><br>"
		"<button type=\"button\" onclick=\"setStaticColor(0, 255, 0)\">Gruen</button><br>"
		"<button type=\"button\" onclick=\"setStaticColor(0, 0, 255)\">Blau</button><br>"
		"<script src=\"/script.js\"></script>"
		"</body>"
		"</html>";
		return html;
	}
	
	char* buildScriptJs() {
		char* script = 
		"function myFunction() {"
			"document.getElementById(\"demo\").innerHTML=\"Paragraph changed.\";"
		"}"
		"function setStaticColor(r, g, b) {"
			"httpAsync(\"POST\", \"/setcolor?r=\"+r+\"&g=\"+g+\"&b=\"+b, null);"
		"}"
		"function httpAsync(method, url, body)"
		"{"
			"var xmlHttp = new XMLHttpRequest();"
			"xmlHttp.onreadystatechange = function() {"
				"if (xmlHttp.readyState == 4) {"
					"if (xmlHttp.status == 200) {"
						"console.log(\"request successfull\");"
					"} else {"
						"if (xmlHttp.status > 0) {" 
							"alert(\"HTTP Error: status \" + xmlHttp.status);"
						"} else {"
							"alert(\"Internal Error, see console\");"
						"}"
					"}"
				"}"
			"}"
			"xmlHttp.open(method, url, true);"
			"xmlHttp.send(body);"
		"}";
		return script;
	}
}

#endif
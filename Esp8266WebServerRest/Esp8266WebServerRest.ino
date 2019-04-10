#include <ESP8266HTTPClient.h>
#include <stdio.h>

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal

#include <ESP8266WebServer.h>

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic      

#include <ArduinoJson.h>

#include "index.h"

#define HTTP_REST_PORT 12859

#define WIFI_RETRY_DELAY 500

#define MAX_WIFI_INIT_RETRY 50

WiFiManager wifiManager;


//
//const char* wifi_ssid = "HUAWEI Mate s_2GEXT";
//
//const char* wifi_passwd = "xxxxxxxxxxx";

struct Led {

	byte id;

	byte gpio;

	byte status;

} led_resource;

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

void init_led_resource()

{

	led_resource.id = 0;

	led_resource.gpio = 0;

	led_resource.status = LOW;

}

int init_wifi() {
	int retries = 0;

	//Serial.println("Connecting to WiFi AP..........");

	wifiManager.autoConnect("LGS-HOMECONTROLLER", "password");

	//WiFi.mode(WIFI_STA);

	//

	////WiFi.begin(wifi_ssid, wifi_passwd);

	//// check the status of WiFi connection to be WL_CONNECTED

	//while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {

	//	retries++;

	//	delay(WIFI_RETRY_DELAY);

	//	Serial.print("#");

	//}

	//return WiFi.status(); // return the WiFi connection status

}

void get_leds() {

	StaticJsonBuffer<200> jsonBuffer;

	JsonObject& jsonObj = jsonBuffer.createObject();

	char JSONmessageBuffer[200];

	if (led_resource.id == 0)

		http_rest_server.send(204);

	else {

		jsonObj["id"] = led_resource.id;

		jsonObj["gpio"] = led_resource.gpio;

		jsonObj["status"] = led_resource.status;

		jsonObj.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

		http_rest_server.send(200, "application/json", JSONmessageBuffer);

	}

}

void json_to_resource(JsonObject& jsonBody) {

	int id, gpio, status;

	id = jsonBody["id"];

	gpio = jsonBody["gpio"];

	status = jsonBody["status"];

	Serial.println(id);

	Serial.println(gpio);

	Serial.println(status);

	led_resource.id = id;

	led_resource.gpio = gpio;

	led_resource.status = status;

}

void post_put_leds() {

	StaticJsonBuffer<500> jsonBuffer;

	String post_body = http_rest_server.arg("plain");

	Serial.println(post_body);

	JsonObject& jsonBody = jsonBuffer.parseObject(http_rest_server.arg("plain"));

	Serial.print("HTTP Method: ");

	Serial.println(http_rest_server.method());

	if (!jsonBody.success()) {

		Serial.println("error in parsin json body");

		http_rest_server.send(400);

	}

	else {

		if (http_rest_server.method() == HTTP_POST) {

			if ((jsonBody["id"] != 0) && (jsonBody["id"] != led_resource.id)) {

				json_to_resource(jsonBody);

				http_rest_server.sendHeader("Location", "/leds/" + String(led_resource.id));

				http_rest_server.send(201);

				pinMode(led_resource.gpio, OUTPUT);

			}

			else if (jsonBody["id"] == 0)

				http_rest_server.send(404);

			else if (jsonBody["id"] == led_resource.id)

				http_rest_server.send(409);

		}

		else if (http_rest_server.method() == HTTP_PUT) {

			if (jsonBody["id"] == led_resource.id) {

				json_to_resource(jsonBody);

				http_rest_server.sendHeader("Location", "/leds/" + String(led_resource.id));

				http_rest_server.send(200);

				digitalWrite(led_resource.gpio, led_resource.status);

			}

			else

				http_rest_server.send(404);

		}

	}

}

void config_rest_server_routing() {

	http_rest_server.on("/1", HTTP_GET, []() {

		String s = MAIN_page; //Read HTML contents

		http_rest_server.send(200, "text/html", s);
		Serial.write("Dati da inviare al microcontrollore");
		//for (int i = 0; i < 5; i++)
		//{
			//Serial.println("Apro il portone");
			digitalWrite(2, HIGH);
			delay(1000);
			digitalWrite(2, LOW);
			delay(1000);
		//}
	});

	http_rest_server.on("/2", HTTP_GET, []() {

		http_rest_server.send(200, "text/html",

			"Procedura 2 avviata");

		Serial.write("Dati da inviare al microcontrollore 2");

		//digitalWrite(2, LOW);

	});

	http_rest_server.on("/leds", HTTP_GET, get_leds);

	http_rest_server.on("/leds", HTTP_POST, post_put_leds);

	http_rest_server.on("/leds", HTTP_PUT, post_put_leds);

}

void setup(void) {

	Serial.begin(115200);

	Serial.print("Start");

	init_led_resource();

	if (init_wifi() == WL_CONNECTED) {

		Serial.print("Connetted to ");

		//Serial.print(wifi_ssid);

		Serial.print("--- IP: ");

		Serial.println(WiFi.localIP());

	}

	else {

		Serial.print("Error connecting to: ");

		//Serial.println(wifi_ssid);

	}

	config_rest_server_routing();

	http_rest_server.begin();

	Serial.println("HTTP REST Server Started");

	pinMode(2, OUTPUT);
}

void loop(void) {
	HTTPClient http;    //Declare object of class HTTPClient
	http.begin("http://192.168.43.197:901/WcfServiceEsp8266/WcfServiceBase.svc/GetDataC");      //Specify request destination
	http.addHeader("Content-Type" , "application/json");  //Specify content-type header
	int httpCode = http.POST("{\"email\" : \"tin@it\",\"name\" : \"luigis\",\"username\" : \"hp9000\"}");   //Send the request
	String payload = http.getString();                  //Get the response payload
	Serial.println(httpCode);   //Print HTTP return code
	Serial.println(payload);    //Print request response payload
	http.end();  //Close connection




	////Parte che chiama un servizio rest service

	//HTTPClient http; //Object of class HTTPClient
	////http.begin("http://jsonplaceholder.typicode.com/users/1");
	//
	//http.begin("http://192.168.43.197:901/WcfServiceEsp8266/WcfServiceBase.svc/GetData");

	////delay(2000);

	//int httpCode = http.GET();

	////Serial.println("Client : "); Serial.println(httpCode);

	//if (httpCode > 0)
	//{
	//	const size_t bufferSize = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(2);// +JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
	//	DynamicJsonBuffer jsonBuffer(bufferSize);
	//	JsonObject& root = jsonBuffer.parseObject(http.getString());

	//	int id = root["id"];
	//	const char* name = root["name"];
	//	const char* username = root["username"];
	//	const char* email = root["email"];

	//	Serial.print("Name:");
	//	Serial.println(name);
	//	Serial.print("Username:");
	//	Serial.println(username);
	//	Serial.print("Email:");
	//	Serial.println(email);
	//}
	//http.end(); //Close connection


	delay(10000);

	http_rest_server.handleClient();
}


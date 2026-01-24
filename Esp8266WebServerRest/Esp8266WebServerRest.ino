// filename: Esp8266WebServerRest.ino
#define FIXERASECONFIG
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>      // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>      // ArduinoJson 7.x
#include <stdio.h>
#include "index.h"
#include "index2.h"
#define HTTP_REST_PORT 80
WiFiManager wifiManager;
struct Led {
	byte id;
	byte gpio;
	byte status;
} led_resource;
ESP8266WebServer http_rest_server(HTTP_REST_PORT);
void init_led_resource() {
	led_resource.id = 0;
	led_resource.gpio = 0;
	led_resource.status = LOW;
}
bool init_wifi() {
	WiFi.hostname("LsgOpener");
	// true se connesso
	return wifiManager.autoConnect("LGS-HOMECONTROLLER", "password");
}
void get_leds() {
	if (led_resource.id == 0) {
		http_rest_server.send(204);
		return;
	}
	JsonDocument doc;
	doc["id"] = led_resource.id;
	doc["gpio"] = led_resource.gpio;
	doc["status"] = led_resource.status;

	char out[200];
	size_t n = serializeJsonPretty(doc, out, sizeof(out));
	if (n == 0) {
		http_rest_server.send(500, "text/plain", "JSON serialize error");
		return;
	}

	http_rest_server.send(200, "application/json", out);
}
void json_to_resource(JsonObject obj) {
	int id = obj["id"] | 0;
	int gpio = obj["gpio"] | 0;
	int status = obj["status"] | 0;

	Serial.println(id);
	Serial.println(gpio);
	Serial.println(status);

	led_resource.id = (byte)id;
	led_resource.gpio = (byte)gpio;
	led_resource.status = (byte)status;
}
void post_put_leds() {
	String post_body = http_rest_server.arg("plain");
	Serial.println(post_body);
	JsonDocument doc;
	DeserializationError err = deserializeJson(doc, post_body);
	if (err) {
		Serial.print("JSON parse error: ");
		Serial.println(err.c_str());
		http_rest_server.send(400, "text/plain", "Bad JSON");
		return;
	}
	JsonObject jsonBody = doc.as<JsonObject>();
	Serial.print("HTTP Method: ");
	Serial.println(http_rest_server.method());
	if (http_rest_server.method() == HTTP_POST) {
		int id = jsonBody["id"] | 0;
		if (id != 0 && (byte)id != led_resource.id) {
			json_to_resource(jsonBody);
			http_rest_server.sendHeader("Location", "/leds/" + String(led_resource.id));
			http_rest_server.send(201);
			pinMode(led_resource.gpio, OUTPUT);
		}
		else if (id == 0) {
			http_rest_server.send(404);
		}
		else {
			http_rest_server.send(409);
		}
	}
	else if (http_rest_server.method() == HTTP_PUT) {
		int id = jsonBody["id"] | 0;
		if ((byte)id == led_resource.id) {
			json_to_resource(jsonBody);
			http_rest_server.sendHeader("Location", "/leds/" + String(led_resource.id));
			http_rest_server.send(200);
			digitalWrite(led_resource.gpio, led_resource.status);
		}
		else {
			http_rest_server.send(404);
		}
	}
}
void resetwifi() {
	http_rest_server.send(200, "text/html", "System reset");
	delay(2000);

	wifiManager.resetSettings();

	// Se vuoi riavvio dopo reset:
	// ESP.restart();
}
void config_rest_server_routing() {
	http_rest_server.on("/menu", HTTP_GET, []() {
		String s = MAIN_page2;
		http_rest_server.send(200, "text/html", s);
		Serial.write("Dati da inviare al microcontrollore");
		});

	http_rest_server.on("/reset", HTTP_GET, []() {
		resetwifi();
		});

	http_rest_server.on("/leds", HTTP_GET, get_leds);
	http_rest_server.on("/leds", HTTP_POST, post_put_leds);
	http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
}
void setup() {
	Serial.begin(9600);
	Serial.println();
	Serial.println("Start");
	init_led_resource();
	if (init_wifi()) {
		Serial.print("Connected --- IP: ");
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("Error connecting (autoConnect failed)");
	}

	config_rest_server_routing();
	http_rest_server.begin();
	Serial.println("HTTP REST Server Started");

	pinMode(2, OUTPUT);
}
void loop() {
	http_rest_server.handleClient();
}

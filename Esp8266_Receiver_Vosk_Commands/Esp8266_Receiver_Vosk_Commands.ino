// filename: Esp8266WebServerRest_min.ino
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#define HTTP_REST_PORT 80
WiFiManager wifiManager;
ESP8266WebServer server(HTTP_REST_PORT);
static bool init_wifi() {
	WiFi.mode(WIFI_STA);
	WiFi.hostname("LsgOpener");
	// Se non trova credenziali valide -> AP "LGS-HOMECONTROLLER" con portal
	// (password opzionale, puoi anche toglierla)
	return wifiManager.autoConnect("LGS-HOMECONTROLLER", "password");
}
static void handle_set() {
	// Esempio chiamata: /set?v=1   oppure /set?v=A   oppure /set?v=abc123
	if (!server.hasArg("v")) {
		server.send(400, "text/plain", "Missing query param: v\nExample: /set?v=1");
		return;
	}
	String v = server.arg("v");     // può essere numerico o alfanumerico
	Serial.print("Ricevuto v = ");
	Serial.println(v);
	server.send(200, "text/plain", "OK v=" + v);
}
static void handle_reset() {
	server.send(200, "text/plain", "Reset WiFi settings, rebooting...");
	delay(300);
	wifiManager.resetSettings();
	ESP.restart();
}
static void setup_routes() {
	server.on("/", HTTP_GET, []() {
		server.send(200, "text/plain",
			"ESP8266 OK\n"
			"Use:\n"
			"  /set?v=1\n"
			"  /set?v=A\n"
			"  /set?v=abc123\n"
			"  /reset\n");
		});
	server.on("/set", HTTP_GET, handle_set);
	server.on("/reset", HTTP_GET, handle_reset);
	server.onNotFound([]() {
		server.send(404, "text/plain", "Not found");
		});
}
void setup() {
	Serial.begin(115200);
	Serial.println();
	Serial.println("Boot");
	if (init_wifi()) {
		Serial.print("Connected. IP: ");
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("WiFiManager autoConnect failed (timeout/abort)");
	}
	setup_routes();
	server.begin();
	Serial.println("HTTP server started");
}
void loop() {
	server.handleClient();
}

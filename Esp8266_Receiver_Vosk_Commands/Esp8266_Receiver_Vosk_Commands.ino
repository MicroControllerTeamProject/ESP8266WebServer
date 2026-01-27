// filename: Esp8266WebServerRest_mode_switch.ino
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#define HTTP_REST_PORT 80

// =========================
// SCEGLI QUI LA MODALITÀ
// =========================
enum BootMode { MODE_STA, MODE_AP };
// <-- cambia in MODE_AP quando vuoi solo AP
static const BootMode BOOT_MODE = BootMode::MODE_STA;   
// =========================
// PARAMETRI
// =========================
static const char* HOSTNAME = "LsgOpener";

// In MODE_STA: SSID/PASS dell’AP di configurazione WiFiManager
static const char* WM_AP_SSID = "LGS-HOMECONTROLLER";
static const char* WM_AP_PASS = "password";   // puoi mettere "" per open

// In MODE_AP: SSID/PASS dell’AP operativo
static const char* AP_SSID = "LGS-HOMECONTROLLER";
static const char* AP_PASS = "password";      // puoi mettere "" per open

WiFiManager wifiManager;
ESP8266WebServer server(HTTP_REST_PORT);

// =========================
// ROUTES
// =========================
static void handle_root() {
    String s;
    s += "ESP8266 OK\n";
    s += "Mode: ";
    s += (BOOT_MODE == MODE_STA) ? "STA (WiFiManager)\n" : "AP\n";
    s += "STA IP: " + WiFi.localIP().toString() + "\n";
    s += "AP  IP: " + WiFi.softAPIP().toString() + "\n";
    s += "Use:\n";
    s += "  /set?mcu_id=FF\n";
    s += "  /reset\n";
    server.send(200, "text/plain", s);
}

static void handle_set() {
    Serial.println("chiamata /set");
    if (!server.hasArg("mcu_id")) {
        server.send(400, "text/plain", "Missing query param: mcu_id\nExample: /set?mcu_id=FF\n");
        return;
    }
    String mcu_id = server.arg("mcu_id");
    Serial.print("Ricevuto mcu_id = ");
    Serial.println(mcu_id);
    server.send(200, "text/plain", "OK mcu_id=" + mcu_id + "\n");
}

static void handle_reset() {
    server.send(200, "text/plain", "Reset WiFi settings, rebooting...\n");
    delay(300);
    wifiManager.resetSettings();   // utile soprattutto in MODE_STA
    ESP.restart();
}

static void setup_routes() {
    server.on("/", HTTP_GET, handle_root);
    server.on("/set", HTTP_GET, handle_set);
    server.on("/reset", HTTP_GET, handle_reset);
    server.onNotFound([]() { server.send(404, "text/plain", "Not found\n"); });
}

// =========================
// NET INIT
// =========================
static void start_sta_wifimanager() {
    WiFi.mode(WIFI_STA);
    WiFi.hostname(HOSTNAME);

    // autoConnect:
    // - se trova credenziali valide -> si connette
    // - altrimenti apre AP+portal (WM_AP_SSID) finché configuri
    bool ok = wifiManager.autoConnect(WM_AP_SSID, WM_AP_PASS);

    if (ok) {
        Serial.print("STA connected. IP: ");
        Serial.println(WiFi.localIP());
    }
    else {
        // In pratica qui ci arrivi raramente: autoConnect di solito resta in portal finché configuri.
        Serial.println("WiFiManager autoConnect failed (timeout/abort).");
    }
}

static void start_ap_only() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.print("AP started. IP: ");
    Serial.println(WiFi.softAPIP());  // di default 192.168.4.1
}

// =========================
// ARDUINO
// =========================
void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Boot");

    if (BOOT_MODE == MODE_STA) start_sta_wifimanager();
    else start_ap_only();

    setup_routes();
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

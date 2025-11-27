// Required Libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- Configuration Constants ---
// **1. WiFi Credentials**
const char* ssid = "your_ssid";
const char* password = "magicwords";

// **2. Relay Pins**
// Relay 1 (Exhaust Fan) - Usually GPIO 0 (D3) on ESP-01
const int RELAY1_PIN = 0; 
// Relay 2 (Internal Fan) - Usually GPIO 2 (D4) on ESP-01 / Built-in LED
const int RELAY2_PIN = 2; 

// **3. Relay Logic**
// Active-LOW means writing LOW (0V) turns the relay ON.
// Change to HIGH if your relay activates on 3.3V/5V.
const int ACTIVE_LEVEL = HIGH; 

// **4. Web Server**
ESP8266WebServer server(80);

// --- State Variables ---
String state_exhaust = "OFF"; 
String state_internal = "OFF";

// --- Function Prototypes ---
void handleRoot();
void handleExhaustControl();
void handleInternalControl();
void handleNotFound();
void setRelayState(int pin, bool turn_on, String &state_var);
void setupRelays();

void setup() {
  Serial.begin(115200);
  delay(10);

  // Initialize Pins
  setupRelays();
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- Setup Web Server Endpoints ---
  
  // 1. Dashboard (Human Interface)
  server.on("/", HTTP_GET, handleRoot); 

  // 2. API Endpoints (Machine/Moonraker Interface)
  // Endpoint for Exhaust Fan (Relay 1)
  server.on("/exhaust", HTTP_GET, handleExhaustControl); 
  // Endpoint for Internal Fan (Relay 2)
  server.on("/internal", HTTP_GET, handleInternalControl); 

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

// --- Relay Setup Function ---
void setupRelays() {
    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);
    
    // Initialize both to OFF (false)
    // We manually write the INACTIVE level to ensure they start off
    int inactive_level = (ACTIVE_LEVEL == HIGH) ? LOW : HIGH;
    digitalWrite(RELAY1_PIN, inactive_level);
    digitalWrite(RELAY2_PIN, inactive_level);
}

// --- Generic Control Function ---
// Controls a specific pin and updates the specific state variable
void setRelayState(int pin, bool turn_on, String &state_var) {
    if (turn_on) {
        digitalWrite(pin, ACTIVE_LEVEL); 
        state_var = "ON";
    } else {
        int inactive_level = (ACTIVE_LEVEL == HIGH) ? LOW : HIGH;
        digitalWrite(pin, inactive_level); 
        state_var = "OFF";
    }
}

// --- Web Server: Dashboard (Manual Control) ---
void handleRoot() {
  // Check for EXHAUST button press
  if (server.hasArg("exhaust")) {
    String arg = server.arg("exhaust");
    arg.toUpperCase();
    if (arg == "ON") setRelayState(RELAY1_PIN, true, state_exhaust);
    if (arg == "OFF") setRelayState(RELAY1_PIN, false, state_exhaust);
  }

  // Check for INTERNAL button press
  if (server.hasArg("internal")) {
    String arg = server.arg("internal");
    arg.toUpperCase();
    if (arg == "ON") setRelayState(RELAY2_PIN, true, state_internal);
    if (arg == "OFF") setRelayState(RELAY2_PIN, false, state_internal);
  }

  // --- HTML Generation ---
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Dual Fan Control</title>";
  html += "<style>";
  html += "body{text-align: center; font-family: sans-serif; background-color: #f4f4f4;}";
  html += ".card{background: white; max-width: 400px; margin: 20px auto; padding: 20px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1);}";
  html += ".btn{border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; width: 80%; border-radius: 4px;}";
  html += ".on{background-color: #2ecc71;}";
  html += ".off{background-color: #e74c3c;}";
  html += "h2{color: #333;}";
  html += ".status{font-weight: bold; font-size: 1.2em;}";
  html += ".status-on{color: #2ecc71;} .status-off{color: #e74c3c;}";
  html += "</style></head><body>";
  
  html += "<h1>AD5x Dual Fan Control</h1>";
  
  // -- Card 1: Exhaust Fan --
  html += "<div class='card'>";
  html += "<h2>Exhaust Fan (Relay 1)</h2>";
  String c1 = (state_exhaust == "ON") ? "status-on" : "status-off";
  html += "<p>Status: <span class='status " + c1 + "'>" + state_exhaust + "</span></p>";
  html += "<a href='/?exhaust=on' class='btn on'>TURN ON</a>";
  html += "<a href='/?exhaust=off' class='btn off'>TURN OFF</a>";
  html += "</div>";

  // -- Card 2: Internal Fan --
  html += "<div class='card'>";
  html += "<h2>Internal Fan (Relay 2)</h2>";
  String c2 = (state_internal == "ON") ? "status-on" : "status-off";
  html += "<p>Status: <span class='status " + c2 + "'>" + state_internal + "</span></p>";
  html += "<a href='/?internal=on' class='btn on'>TURN ON</a>";
  html += "<a href='/?internal=off' class='btn off'>TURN OFF</a>";
  html += "</div>";
  
  html += "<p><small>API Endpoints: /exhaust and /internal</small></p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// --- Web Server: Exhaust API ---
void handleExhaustControl() {
  if (server.hasArg("state")) {
    String arg = server.arg("state");
    arg.toUpperCase();
    if (arg == "ON") setRelayState(RELAY1_PIN, true, state_exhaust);
    else if (arg == "OFF") setRelayState(RELAY1_PIN, false, state_exhaust);
  }
  
  // Return JSON Status
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\": \"" + state_exhaust + "\"}");
}

// --- Web Server: Internal API ---
void handleInternalControl() {
  if (server.hasArg("state")) {
    String arg = server.arg("state");
    arg.toUpperCase();
    if (arg == "ON") setRelayState(RELAY2_PIN, true, state_internal);
    else if (arg == "OFF") setRelayState(RELAY2_PIN, false, state_internal);
  }
  
  // Return JSON Status
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\": \"" + state_internal + "\"}");
}

void handleNotFound(){
  server.send(404, "text/plain", "Not Found");
}

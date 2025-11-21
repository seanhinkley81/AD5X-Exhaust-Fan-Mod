// Required Libraries (Install via Library Manager in Arduino IDE)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- Configuration Constants ---
// **1. WiFi Credentials**
const char* ssid = "your_ssid";
const char* password = "magicwords";
// **2. Relay Pin & Logic**
// GPIO0 is D3 on the ESP-01. Common for ESP-01S relay boards.
const int RELAY_PIN = 0; 
// Active-LOW means writing LOW (0V) turns the relay ON.
const int ACTIVE_LEVEL = LOW; 
// const int ACTIVE_LEVEL = HIGH; // Use this if your relay is Active-HIGH

// **3. Web Server**
// Define the HTTP port (80 is standard)
ESP8266WebServer server(80);

// --- State Variables ---
String current_state = "OFF"; 

// --- Function Prototypes ---
void handleRoot();
void handlePowerControl();
void handleNotFound();
void setFanState(bool turn_on);
void setupRelay();
String state_to_text(bool is_on);


void setup() {
  Serial.begin(115200);
  delay(10);

  setupRelay();
  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup Web Server Endpoints
  server.on("/", HTTP_GET, handleRoot); // New: Index page handler
  server.on("/power", HTTP_GET, handlePowerControl); // Moonraker API handler
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

// --- Relay Setup Function ---
void setupRelay() {
    // Configure the relay pin as an output
    pinMode(RELAY_PIN, OUTPUT);
    
    // Set the relay to the initial OFF state
    setFanState(false);
}

// --- Core Control Function ---
// Sets the physical relay state and updates the status variable
void setFanState(bool turn_on) {
    if (turn_on) {
        // To turn ON, write the ACTIVE_LEVEL (e.g., LOW for Active-LOW)
        digitalWrite(RELAY_PIN, ACTIVE_LEVEL); 
        current_state = "ON";
        Serial.println("Fan set to ON");
    } else {
        // To turn OFF, write the opposite of the ACTIVE_LEVEL
        int inactive_level = (ACTIVE_LEVEL == HIGH) ? LOW : HIGH;
        digitalWrite(RELAY_PIN, inactive_level); 
        current_state = "OFF";
        Serial.println("Fan set to OFF");
    }
}

// --- Helper function to convert state for display ---
String state_to_text(bool is_on) {
  return is_on ? "ON" : "OFF";
}

// --- Web Server Handler for Index Page (Manual Control) ---
void handleRoot() {
  // Check if we have button presses from the GET request
  if (server.hasArg("state")) {
    String state_arg = server.arg("state");
    state_arg.toUpperCase();
    if (state_arg == "ON") {
        setFanState(true);
    } else if (state_arg == "OFF") {
        setFanState(false);
    }
  }

  // --- HTML Content Generation ---
  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>AD5x Fan Control</title>";
  html += "<style>";
  html += "body{text-align: center; font-family: sans-serif;}";
  html += ".button{background-color: #1abc9c; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer;}";
  html += ".state-on{background-color: #2ecc71;}";
  html += ".state-off{background-color: #e74c3c;}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  
  html += "<h1>Welcome to Exhaust Fan Control for AD5x</h1>";
  
  // Display Current State
  String state_class = (current_state == "ON") ? "state-on" : "state-off";
  html += "<h2>Current State: <span class='" + state_class + "'>" + current_state + "</span></h2>";
  
  // ON/OFF Buttons
  // These buttons simply call the root URL with the 'state' argument
  html += "<p>";
  html += "<a href='/?state=on' class='button state-on'>TURN FAN ON</a>";
  html += "<a href='/?state=off' class='button state-off'>TURN FAN OFF</a>";
  html += "</p>";
  
  html += "<p><em>This page is for manual control. Moonraker uses the /power API endpoint.</em></p>";
  
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}


// --- Web Server Handler for Power Control (Moonraker API) ---
// This function remains the same as it handles the JSON response Moonraker expects
void handlePowerControl() {
  // Check if the 'state' argument is present
  if (server.hasArg("state")) {
    String state_arg = server.arg("state");
    state_arg.toUpperCase();

    // Check if the state is 'ON' or 'OFF'
    if (state_arg == "ON") {
      setFanState(true);
    } else if (state_arg == "OFF") {
      setFanState(false);
    } 
    
    // Return a JSON response for Moonraker/client
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\": \"" + current_state + "\"}");
  } else if (server.hasArg("status")) {
    // Handle status check endpoint (e.g., http://[IP_ADDRESS]/power?status=true)
     server.sendHeader("Access-Control-Allow-Origin", "*");
     server.send(200, "application/json", "{\"status\": \"" + current_state + "\"}");
  }
  else {
    // If no state or status argument is provided, return a 400 Bad Request
    server.send(400, "text/plain", "Error: Missing 'state' argument (use ?state=on or ?state=off)");
  }
}

// --- Web Server Handler for 404 Not Found ---
void handleNotFound(){
  server.send(404, "text/plain", "Not Found");
}

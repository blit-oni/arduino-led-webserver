#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>

#define DATA_PIN 5  // Pin for the LED's (WS2811)
#define NUM_LEDS 167  // Number of LEDs in the strip
Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

bool stripEnabled = true;

const char* ssid = "PALCEHOLDER"; // Change this to ur SSID
const char* password = "PLACEHOLDER"; // CHange this to your Password
const char* hostname = "myled";

ESP8266WebServer server(80);

int redPin = D1;    // Pin connected to the red component of the LED
int greenPin = D2;  // Pin connected to the green component of the LED
int bluePin = D3;   // Pin connected to the blue component of the LED

int redValue = 0;   // Current value of the red component (0-255)
int greenValue = 0; // Current value of the green component (0-255)
int blueValue = 0;  // Current value of the blue component (0-255)
int brightValue = 255; 

IPAddress staticIP(192, 168, 201, 139);  // Set your desired static IP address
IPAddress gateway(192, 168, 201, 1);     // Set your network gateway IP address
IPAddress subnet(255, 255, 255, 0); 

void handleRoot() {
  String html = "<html>";
  html += "<head>";
  html += "<style>";
  html += "body {";
  html += "  background-color: #222;";
  html += "  color: #fff;";
  html += "  font-family: Arial, sans-serif;";
  html += "}";
  html += "h1 {";
  html += "  text-align: center;";
  html += "}";
  html += ".slider {";
  html += "  width: 80%;";
  html += "  margin: 10px auto;";
  html += "}";
  html += ".slider input[type='range'] {";
  html += "  width: 100%;";
  html += "  height: 20px;";
  html += "  -webkit-appearance: none;";
  html += "  background-color: #444;";
  html += "  border-radius: 10px;";
  html += "  outline: none;";
  html += "  opacity: 0.7;";
  html += "  transition: opacity 0.2s;";
  html += "}";
  html += ".slider input[type='range']:hover {";
  html += "  opacity: 1;";
  html += "}";
  html += ".slider input[type='range']::-webkit-slider-thumb {";
  html += "  -webkit-appearance: none;";
  html += "  width: 25px;";
  html += "  height: 25px;";
  html += "  background-color: #fff;";
  html += "  border-radius: 50%;";
  html += "  cursor: pointer;";
  html += "}";
  html += ".input-field {";
  html += "  display: flex;";
  html += "  align-items: center;";
  html += "  margin-bottom: 10px;";
  html += "}";
  html += ".input-field input[type='number'] {";
  html += "  width: 50px;";
  html += "  margin-left: 10px;";
  html += "}";
  html += ".toggle-btn {";
  html += "  display: block;";
  html += "  width: 100px;";
  html += "  margin: 20px auto;";
  html += "  padding: 10px;";
  html += "  border-radius: 10px;";
  html += "  background-color: #444;";
  html += "  color: #fff;";
  html += "  text-align: center;";
  html += "  cursor: pointer;";
  html += "  transition: background-color 0.2s;";
  html += "}";
  html += ".toggle-btn:hover {";
  html += "  background-color: #666;";
  html += "}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<h1>RGB LED Control</h1>";
  html += "<div class='slider'>";
  html += "<label for='redSlider'>R</label>";
  html += "<input type='range' min='0' max='255' value='0' id='redSlider' oninput='updateColor()'>";
  html += "<div class='input-field'>";
  html += "<input type='number' min='0' max='255' id='redInput' onchange='updateSlider(this, \"redSlider\")'>";
  html += "</div>";
  html += "</div>";
  html += "<div class='slider'>";
  html += "<label for='greenSlider'>G</label>";
  html += "<input type='range' min='0' max='255' value='0' id='greenSlider' oninput='updateColor()'>";
  html += "<div class='input-field'>";
  html += "<input type='number' min='0' max='255' id='greenInput' onchange='updateSlider(this, \"greenSlider\")'>";
  html += "</div>";
  html += "</div>";
  html += "<div class='slider'>";
  html += "<label for='blueSlider'>B</label>";
  html += "<input type='range' min='0' max='255' value='0' id='blueSlider' oninput='updateColor()'>";
  html += "<div class='input-field'>";
  html += "<input type='number' min='0' max='255' id='blueInput' onchange='updateSlider(this, \"blueSlider\")'>";
  html += "</div>";
  html += "</div>";
  html += "<div class='slider'>";
  html += "<label for='brightnessSlider'>Brightness</label>";
  html += "<input type='range' min='0' max='255' value='" + String(brightValue) + "' id='brightnessSlider' oninput='updateBrightness()'>";
  html += "<div class='input-field'>";
  html += "<input type='number' min='0' max='255' id='brightnessInput' onchange='updateSlider(this, \"brightnessSlider\")'>";
  html += "</div>";
  html += "</div>";
  html += "<div class='toggle-btn' id='toggleBtn' onclick='toggleStrip()'>Toggle Strip</div>";
  html += "<script>";
  html += "var redSlider = document.getElementById('redSlider');";
  html += "var greenSlider = document.getElementById('greenSlider');";
  html += "var blueSlider = document.getElementById('blueSlider');";
  html += "var redInput = document.getElementById('redInput');";
  html += "var greenInput = document.getElementById('greenInput');";
  html += "var blueInput = document.getElementById('blueInput');";
  html += "var brightnessSlider = document.getElementById('brightnessSlider');";
  html += "var brightnessInput = document.getElementById('brightnessInput');";
  html += "var toggleBtn = document.getElementById('toggleBtn');";
  html += "function updateColor() {";
  html += "  var red = redSlider.value;";
  html += "  var green = greenSlider.value;";
  html += "  var blue = blueSlider.value;";
  html += "  redInput.value = red;";
  html += "  greenInput.value = green;";
  html += "  blueInput.value = blue;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/color?r=' + red + '&g=' + green + '&b=' + blue, true);";
  html += "  xhttp.send();";
  html += "}";
  html += "function updateBrightness() {";
  html += "  var brightness = brightnessSlider.value;";
  html += "  brightnessInput.value = brightness;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/brightness?b=' + brightness, true);";
  html += "  xhttp.send();";
  html += "}";
  html += "function updateSlider(input, sliderId) {";
  html += "  var value = input.value;";
  html += "  var slider = document.getElementById(sliderId);";
  html += "  slider.value = value;";
  html += "  if (sliderId === 'brightnessSlider') {";
  html += "    updateBrightness();";
  html += "  } else {";
  html += "    updateColor();";
  html += "  }";
  html += "}";
  html += "function toggleStrip() {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('GET', '/toggle', true);";
  html += "  xhttp.send();";
  html += "}";
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "  redSlider.value = " + String(redValue) + ";";
  html += "  greenSlider.value = " + String(greenValue) + ";";
  html += "  blueSlider.value = " + String(blueValue) + ";";
  html += "  redInput.value = " + String(redValue) + ";";
  html += "  greenInput.value = " + String(greenValue) + ";";
  html += "  blueInput.value = " + String(blueValue) + ";";
  html += "  brightnessSlider.value = " + String(brightValue) + ";";
  html += "  brightnessInput.value = " + String(brightValue) + ";";
  html += "});";
  html += "</script>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}



void handleColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    redValue = server.arg("r").toInt();
    greenValue = server.arg("g").toInt();
    blueValue = server.arg("b").toInt();
    if(stripEnabled){
    strip.fill(strip.Color(redValue, greenValue, blueValue));
    strip.show();}
    /*
    Serial.print("R:");
    Serial.print(redValue);
    Serial.print(" G:");
    Serial.print(greenValue);
    Serial.print(" B:");
    Serial.println(blueValue);
    */
    server.send(200, "text/plain", "Color Updated");
  }
  else {
    server.send(400, "text/plain", "Invalid Request");
  }
}

void handleBrightness() {
  if (server.hasArg("b")) {
    brightValue = server.arg("b").toInt();
    if(stripEnabled){
    strip.setBrightness(brightValue);
    if (brightValue > 0) {
      strip.fill(strip.Color(redValue, greenValue, blueValue));}
    strip.show();}
    server.send(200, "text/plain", "Brightness Updated");
  }
  else {
    server.send(400, "text/plain", "Invalid Request");
  }
}

void handleToggle() {
  stripEnabled = !stripEnabled;

  if (stripEnabled) {
    strip.setBrightness(brightValue);
    strip.fill(strip.Color(redValue, greenValue, blueValue));
    strip.show();  // Turn on the LED strip
  } else {
    strip.clear();  // Turn off the LED strip
    strip.show();
  }

  server.send(200, "text/plain", stripEnabled ? "Strip Enabled" : "Strip Disabled");
}

void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.show(); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  WiFi.config(staticIP, gateway, subnet);

  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

  server.on("/", handleRoot);
  server.on("/color", handleColor);
  server.on("/toggle", handleToggle);
  server.on("/brightness", handleBrightness);


  server.begin();
  Serial.println("HTTP server started");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Access the website at http://");
  Serial.print(hostname);
  Serial.println(".local/");
}

void loop() {
  server.handleClient();

}

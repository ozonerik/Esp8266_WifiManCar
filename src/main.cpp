#define ENA   0          // Enable/speed motors Right        GPIO14(D5) GPIO0(D3)
#define ENB   2          // Enable/speed motors Left         GPIO12(D6) GPIO2(D4)
#define IN_1  14          // L298N in1 motors Right           GPIO15(D8)
#define IN_2  12          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  13           // L298N in3 motors Left            GPIO2(D4) GPIO14(D5)
#define IN_4  15           // L298N in4 motors Left            GPIO0(D3) GPIO12(D6)

#define LED_R  5           // L298N in4 motors Left            GPIO5(D1)
#define LED_L  4           // L298N in4 motors Left            GPIO4(D2)
//#define LED_FL  0           // L298N in4 motors Left            GPIO0(D3)
//#define LED_BL  2           // L298N in3 motors Left            GPIO2(D4)

#define B_RESET  16           // L298N in4 motors Left            GPIO16(D0)

#include <ESP8266WiFi.h> 
#include <WiFiClient.h> 
//#include <ESP8266WebServer.h>
//#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include "LittleFS.h"

String command;             //String to store app command state.
int speedA = 255;  // 0 to 255
int speedB = 255;
int speedCar = 255; //255+255
void speed(int a, int b  ){
  analogWrite(ENA, a);
  analogWrite(ENB, b);
}
void setSpeed(int s ){
    speedCar = s;
}

String pesan;
bool Tblreset=true; //true jika ada tombol reset fisik

//const char* ssid = "NodeMCU Car";
//const char* pswd = "12345678";
//ESP8266WebServer server(80);
AsyncWebServer server(80);
AsyncWebSocket ws("/wscar/ws");
//WebSocketsServer ws = WebSocketsServer(8080);

//websocket fungsi
void notifyClients() {
  ws.textAll(String(pesan));
  //Serial.println(String(pesan));
}

String processor(const String& var) {
  if(var == "MYIP") {
    if(WiFi.status() != WL_CONNECTED) {
      return WiFi.softAPIP().toString();
    } 
    return WiFi.localIP().toString();
  }
  return String();
}

void lampuled(String arah){
  if(arah=="maju"){
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_L, HIGH);
  }else if(arah=="mundur"){
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_L, HIGH);
  }else if(arah=="kanan"){
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_L, LOW);
  }else if(arah=="kiri"){
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_L, HIGH);
  }else if(arah=="stop"){
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_L, LOW);
  }else if(arah=="loading"){
    for (int i = 0; i <= 3; i++)
    {
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_L, LOW);
      delay(500);
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_L, HIGH);
      delay(500);
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_L, LOW);
      delay(500);
    }
  }else if(arah=="success"){
    for (int i = 0; i <= 3; i++)
    {
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_L, LOW);
      delay(500);
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_L, HIGH);
      delay(500);
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_L, LOW);
      delay(500);
    }
  }

}

void forword() {  
  speed(speedCar,speedCar);           //forword
  digitalWrite(IN_1, HIGH);  //Right Motor forword Pin
  digitalWrite(IN_2, LOW);   //Right Motor backword Pin
  digitalWrite(IN_3, LOW);   //Left Motor backword Pin
  digitalWrite(IN_4, HIGH);  //Left Motor forword Pin
  lampuled("maju");
}

void rightforword() {  
  speedB=speedCar+(speedCar*(0.5));
  speedA=510 - speedB;
  speed(speedA,speedB); 
  digitalWrite(IN_1, HIGH);  //Right Motor forword Pin
  digitalWrite(IN_2, LOW);   //Right Motor backword Pin
  digitalWrite(IN_3, LOW);   //Left Motor backword Pin
  digitalWrite(IN_4, HIGH);  //Left Motor forword Pin
  lampuled("kanan");
}

void leftforword() {  
  speedA=speedCar+(speedCar*(0.5));
  speedB=510 - speedA;
  speed(speedA,speedB); 
  digitalWrite(IN_1, HIGH);  //Right Motor forword Pin
  digitalWrite(IN_2, LOW);   //Right Motor backword Pin
  digitalWrite(IN_3, LOW);   //Left Motor backword Pin
  digitalWrite(IN_4, HIGH);  //Left Motor forword Pin
  lampuled("kiri");
}

void backword() {            //backword
  speed(speedCar,speedCar); 
  digitalWrite(IN_1, LOW);   //Right Motor forword Pin
  digitalWrite(IN_2, HIGH);  //Right Motor backword Pin
  digitalWrite(IN_3, HIGH);  //Left Motor backword Pin
  digitalWrite(IN_4, LOW);   //Left Motor forword Pin
  lampuled("mundur");
}

void leftbackword() {            //backword
  speedB=speedCar+(speedCar*(0.5));
  speedA=510 - speedB;
  speed(speedA,speedB); 
  digitalWrite(IN_1, LOW);   //Right Motor forword Pin
  digitalWrite(IN_2, HIGH);  //Right Motor backword Pin
  digitalWrite(IN_3, HIGH);  //Left Motor backword Pin
  digitalWrite(IN_4, LOW);   //Left Motor forword Pin
  lampuled("kiri");
}

void rightbackword() {            //backword
  speedA=speedCar+(speedCar*(0.5));
  speedB=510 - speedA;
  speed(speedA,speedB); 
  digitalWrite(IN_1, LOW);   //Right Motor forword Pin
  digitalWrite(IN_2, HIGH);  //Right Motor backword Pin
  digitalWrite(IN_3, HIGH);  //Left Motor backword Pin
  digitalWrite(IN_4, LOW);   //Left Motor forword Pin
  lampuled("kanan");
}

void turnRight() {           //turnRight
  speed(speedCar,speedCar); 
  digitalWrite(IN_1, LOW);   //Right Motor forword Pin
  digitalWrite(IN_2, HIGH);  //Right Motor backword Pin
  digitalWrite(IN_3, LOW);   //Left Motor backword Pin
  digitalWrite(IN_4, HIGH);  //Left Motor forword Pin
  lampuled("kanan");
}

void turnLeft() {            //turnLeft
  speed(speedCar,speedCar); 
  digitalWrite(IN_1, HIGH);  //Right Motor forword Pin
  digitalWrite(IN_2, LOW);   //Right Motor backword Pin
  digitalWrite(IN_3, HIGH);  //Left Motor backword Pin
  digitalWrite(IN_4, LOW);   //Left Motor forword Pin
  lampuled("kiri");
}

void Stop() {              //stop
  speed(0,0);
  digitalWrite(IN_1, LOW);  //Right Motor forword Pin
  digitalWrite(IN_2, LOW);  //Right Motor backword Pin
  digitalWrite(IN_3, LOW);  //Left Motor backword Pin
  digitalWrite(IN_4, LOW);  //Left Motor forword Pin
  lampuled("stop");
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "F") == 0) {
      pesan = "F";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      forword();
      notifyClients();
    }else if (strcmp((char*)data, "B") == 0) {
      pesan = "B";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      backword();
      notifyClients();
    }else if (strcmp((char*)data, "L") == 0) {
      pesan = "L";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      turnLeft();
      notifyClients();
    }else if (strcmp((char*)data, "R") == 0) {
      pesan = "R";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      turnRight();
      notifyClients();
    }else if (strcmp((char*)data, "RF") == 0) {
      pesan = "RF";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      rightforword();
      notifyClients();
    }else if (strcmp((char*)data, "RB") == 0) {
      pesan = "RB";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      rightbackword();
      notifyClients();
    }else if (strcmp((char*)data, "LF") == 0) {
      pesan = "LF";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      leftforword();
      notifyClients();
    }else if (strcmp((char*)data, "LB") == 0) {
      pesan = "LB";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      leftbackword();
      notifyClients();
    }else{
      pesan = "S";
      command=pesan;
      Serial.print("Command: ");
      Serial.println(command);
      Stop();
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
//end websocket

//****Start Wifi Manager
// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";


//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;
String IPLocal;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
//IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

boolean restart = false;

//****End Wifi Manager

// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
    Serial.println("LittleFS mounted successfully");
  }
}

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
  file.close();
}

// Initialize WiFi
bool initWiFi() {

  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());

  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    return false;
  }

  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.println("Connecting to WiFi...");
  delay(5000);
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect.");
    //reset AP
    ssid = "";
    Serial.print("SSID set to: ");
    Serial.println(ssid);
    // Write file to save value
    writeFile(LittleFS, ssidPath, ssid.c_str());

    pass = "";
    Serial.print("Password set to: ");
    Serial.println(pass);
    // Write file to save value
    writeFile(LittleFS, passPath, pass.c_str());

    ip = "";
    Serial.print("IP Address set to: ");
    Serial.println(ip);
    // Write file to save value
    writeFile(LittleFS, ipPath, ip.c_str());

    gateway = "";
    Serial.print("Gateway set to: ");
    Serial.println(gateway);
    // Write file to save value
    writeFile(LittleFS, gatewayPath, gateway.c_str());
    restart = true;
    Serial.print("Reset Done. ESP will restart, connect to your router and go to IP address: 192.168.4.1");
    return false;
    //end reset
  }
    Serial.print("Successfully connected to ");
    Serial.println(ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void resetAP(){
    ssid = "";
    Serial.print("SSID set to: ");
    Serial.println(ssid);
    // Write file to save value
    writeFile(LittleFS, ssidPath, ssid.c_str());

    pass = "";
    Serial.print("Password set to: ");
    Serial.println(pass);
    // Write file to save value
    writeFile(LittleFS, passPath, pass.c_str());

    ip = "";
    Serial.print("IP Address set to: ");
    Serial.println(ip);
    // Write file to save value
    writeFile(LittleFS, ipPath, ip.c_str());

    gateway = "";
    Serial.print("Gateway set to: ");
    Serial.println(gateway);
    // Write file to save value
    writeFile(LittleFS, gatewayPath, gateway.c_str());

    Serial.println("RESET WIFI");
    lampuled("loading");
    restart = true;
    Serial.print("Reset Done. ESP will restart, connect to your router and go to IP address: 192.168.4.1");
}

void setup() {

  Serial.begin(115200);

  initFS();
  initWebSocket();
  setSpeed(225);

  pinMode(ENA, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(B_RESET, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_L, OUTPUT);

  // Load values saved in LittleFS
  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  ip = readFile(LittleFS, ipPath);
  gateway = readFile (LittleFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);

  //delay(1000);
  //Serial.println(">> Setup");
  //WiFi.mode(WIFI_AP);           //Only Access point
  //WiFi.softAP(ssid, pswd);  //Start HOTspot removing password will disable security

 //IPAddress myIP = WiFi.softAPIP();  //Get IP address
  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
 // Serial.println(myIP);

  server.on("/bootstrap.min.css", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(LittleFS, "/bootstrap.min.css", "text/css");
  });

  server.on("/bootstrap.bundle.min.js", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(LittleFS, "/bootstrap.bundle.min.js", "text/js");
  });

  server.on("/favicon.png", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(LittleFS, "/favicon.png", "image/png");
  });
  server.on("/wscar", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(LittleFS, "/wscar.html", "text/html", false, processor);
  });

/*   server.on("/ws", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(LittleFS, "/wscar.html", "text/html",false,processor);
  }); */

//***start
if(initWiFi()) {
    // Route for root / web page
    server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/?State=
    if (request->hasParam("State")) {
      command = request->getParam("State")->value();
      }
      Serial.print("Command: ");
      Serial.println(command);
      //request->send(200, "text/plain", command);
      //request->send(LittleFS, "/index.html", "text/html",false,processor);
      request->send(LittleFS, "/index.html", "text/html",false,processor);
    });
    server.serveStatic("/", LittleFS, "/");
    server.begin();
  }
  else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("Arfa Car", "12345678");

    IPAddress IP = WiFi.softAPIP();
    IPLocal=IP.toString();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    server.on("/", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/?State=
    if (request->hasParam("State")) {
      command = request->getParam("State")->value();
      }
      Serial.print("Command: ");
      Serial.println(command);
      //request->send(200, "text/plain", command);
      request->send(LittleFS, "/index.html", "text/html",false,processor);
    });
    server.serveStatic("/", LittleFS, "/");
    server.begin();

    // Web Server Root URL
    server.on("/wifimanager", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifimanager.html", "text/html");
    });
    server.serveStatic("/", LittleFS, "/");
    
    server.on("/wifimanager", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(LittleFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(LittleFS, gatewayPath, gateway.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      restart = true;
      request->send(200, "text/html", "<h1 style='text-align: center'>Done. ESP will restart, connect to your router and go to IP address:</h1><br><br><div style='text-align: center; color:red; font-size: 60px;'>" + ip +"</div>");
    });
  server.begin();
  }
  //***end
  // RESET
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    resetAP();
    request->send(200, "text/html", "<h1 style='text-align: center'>Reset Done. ESP will restart, connect to your router and go to IP address:</h1><br><br><div style='text-align: center; color:red; font-size: 60px;'>192.168.4.1</div><br><br><a href='http://192.168.4.1' style='text-align: center; color:blue; font-size: 60px;'>BACK</a>");
  });
  server.begin();
}

void loop(){

  ws.cleanupClients();

  if (restart){
    delay(5000);
    ESP.restart();
  }

  //Jika konek tombol RESET FISIK;
  if(Tblreset){
    if (digitalRead(B_RESET) == LOW){
        resetAP();
      }
  }

  //server.handleClient();
  //analogWrite(ENA, speedA);
  //analogWrite(ENB, speedB);
  //Serial.print("Command: ");
  //Serial.println(command);
  //Serial.println("Speed= "+String(speedCar));

  //command = server.arg("State");
  if (command == "F") forword();
  else if (command == "B") backword();
  else if (command == "L") turnLeft();
  else if (command == "R") turnRight();
  else if (command == "I") rightforword();
  else if (command == "G") leftforword();
  else if (command == "J") leftbackword();
  else if (command == "H") rightbackword();
  else if (command == "9") setSpeed(150); //12
  else if (command == "8") setSpeed(160); //39
  else if (command == "7") setSpeed(170); //66
  else if (command == "6") setSpeed(180); //93
  else if (command == "5") setSpeed(190); //120
  else if (command == "4") setSpeed(200); //147
  else if (command == "3") setSpeed(210); //175
  else if (command == "2") setSpeed(220); //201
  else if (command == "1") setSpeed(230); //228
  else if (command == "0") setSpeed(250); //255
  else if (command == "S") Stop();
}
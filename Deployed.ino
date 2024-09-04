/*
  Make sure your Firebase project's '.read' and '.write' rules are set to 'true'. 
  Ignoring this will prevent the MCU from communicating with the database. 
  For more details- https://github.com/Rupakpoddar/ESP8266Firebase 

  Download the Android app from- https://play.google.com/store/apps/details?id=com.rupak.firebaseremote
  Online remote (Works with any web browser)- https://rupakpoddar.github.io/Firebase-automation-web-interface/
  Use Python to control devices- https://github.com/Rupakpoddar/Firebase-with-python
*/

#include <ArduinoJson.h>            // https://github.com/bblanchon/ArduinoJson 
#include <ESP8266Firebase.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define _SSID "ARJUN"          // Your WiFi SSID 
#define _PASSWORD "04464426"      // Your WiFi Password 
#define REFERENCE_URL "https://hostelroom1k-default-rtdb.firebaseio.com/"  // Your Firebase project reference url 
#define ONE_WIRE_BUS D5

#define device1 5   // D1
#define device2 4   // D2
#define device3 0   // D3
#define device4 14  // D5
int device_list[4] = {device1, device2, device3, device4};

Firebase firebase(REFERENCE_URL);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  pinMode(device1, OUTPUT);
  pinMode(device2, OUTPUT);
  pinMode(device3, OUTPUT);
  pinMode(device4, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED_BUILTIN, HIGH);

  firebase.json(true);  // Make sure to add this line. 
  sensors.begin();
}

void loop() {
  String data = firebase.getString("Switches");  // Get data from database.

  // Deserialize the data.
  // Consider using the Arduino Json assistant- https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_OBJECT_SIZE(5) + 60;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, data);

  String Device1 = doc["Relay1"];
  String Device2 = doc["Relay2"];
  String Device3 = doc["Relay3"];
  String Device4 = doc["Relay4"];

  // Print data
  Serial.println("Device 1: "+Device1);
  Serial.println("Device 2: "+Device2);
  Serial.println("Device 3: "+Device3);
  Serial.println("Device 4: "+Device4);
  Serial.println("");

  String status_list[4] = {Device1, Device2, Device3, Device4};

  for (int i=0;i<4;i++) {
    if (status_list[i] == "ON"){
      digitalWrite(device_list[i], HIGH);
    }
    else{
      digitalWrite(device_list[i], LOW);
    }
  }

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
    if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
    firebase.setFloat("Sensors/Temperature", tempC);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
}

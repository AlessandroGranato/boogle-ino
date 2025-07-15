#include <ESP8266WiFi.h>        // Required for WiFi connection on ESP8266
#include <WiFiManager.h>        // WiFiManager for easy WiFi configuration
#include <OneWire.h>            // For reading temperature sensor data
#include <DallasTemperature.h>  // For using temperature sensor data
#include <ESP8266HTTPClient.h>  // For HTTP(S) requests
#include <WiFiClientSecure.h>   // For secure (HTTPS) communication
#include <ArduinoJson.h>        // To handle JSON data

//Things to do:
// Error handling: create a class that contains code (number value, but String format), description (string) and use it when need to return error


// Data wire is plugged into port digital port 2 (GPIO4) on the NODEMCU ESP8266
#define ONE_WIRE_BUS 4

//Device name
#define DEVICE_ID "DVC-001"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature waterTempSensor(&oneWire);

// define https client object
WiFiClientSecure client;

// define http client object
HTTPClient http;

void setup() {
  startSerial(9600);
  wifiConnectionSetup();
  waterTempSensorSetup();
  httpSetup();
}

void loop() {
    float temperature = retrieveWaterTemp();  
    sendWaterTemp(temperature);
}

void startSerial(int baudRate) {
  Serial.begin(baudRate);
  Serial.println("START");
}

void wifiConnectionSetup() {
    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("Wifi connected successfully");
    }
}

void waterTempSensorSetup() {
  // Start up the library
  waterTempSensor.begin();
  delay(2000);
  if (waterTempSensor.getDeviceCount() == 0) {
      Serial.println("No temperature sensors found!");
  } else {
      Serial.println("Temperature sensors initialized successfully.");
  }  
}

void httpSetup() {
  client.setInsecure();  // To ignore SSL certificate verification (similar to `-k` in cURL)
}

float retrieveWaterTemp()
{ 
  // call waterTempSensor.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  waterTempSensor.requestTemperatures(); // Send the command to get temperatures
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = waterTempSensor.getTempCByIndex(0);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature is: ");
    Serial.println(tempC);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  delay(2000);
  return tempC;
}

void sendWaterTemp(float temperature)
{ 
  Serial.print("--- START sendWaterTemp with temperature: ");
  Serial.print(temperature);
  
  if (!isWiFiConnected()) {
      // Return or exit if not connected to WiFi
      Serial.println("Error: Cannot send POST request, not connected to WiFi.");
      return;
  }

  // Specify destination URL
  http.begin(client, "https://boogle-be.duckdns.org/bds/api/temperatures");
  
  // Specify content-type header
  http.addHeader("Content-Type", "application/json");
  
  // Create the JSON payload
  String jsonData = "{\"device\": {\"deviceIdentifier\": \"" + String(DEVICE_ID) + "\"}, \"value\": " + temperature + ", \"creationDate\": \"2023-10-02T10:15:00\"}";
  
  // Send POST request
  int httpResponseCode = http.POST(jsonData);
  
  // Check HTTP response code and print the response from the server
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response Code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }
  
  // Close the connection
  http.end();
  delay(5000);
  Serial.print("--- END sendWaterTemp");
}

// Function to check WiFi connection
bool isWiFiConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error: Not connected to WiFi");
        return false;
    }
    return true;
}

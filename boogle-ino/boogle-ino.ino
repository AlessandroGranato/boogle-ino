#include <WiFiManager.h> 
#include <OneWire.h> // used to read water sensor temperature
#include <DallasTemperature.h> // used to use water sensor temperature data in a user-friendly way

// Data wire is plugged into port digital port 2 (GPIO4) on the NODEMCU ESP8266
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature waterTempSensor(&oneWire);

void setup() {
  startSerial(9600);
  wifiConnectionSetup();
  waterTempSensorSetup();  
}

void loop() {
    waterTempSensorLoop();  
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

void waterTempSensorLoop()
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
}

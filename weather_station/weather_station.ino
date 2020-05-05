// Include libraries
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "config.h"

// Define variables
WiFiClient client;
String result;
int counter = 60;                                      
String weatherDescription = "";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
float FeelsLike;


void setup() { // Connect to WiFi and init light
    pinMode(LED_BUILTIN, OUTPUT);
    
    Serial.begin(115200);
    Serial.print("\nConnecting to wifi");
    WiFi.begin(WIFISSID, WIFIPASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println(" Connected");
    delay(1000);
}

void loop() {
    if (counter == 60) { // Get new data every 10 minutes
        counter = 0;
        delay(1000);
        getWeatherData();
    } else {
        counter++;
        displayWeather(weatherLocation, weatherDescription);
        displayConditions(Temperature, Humidity, Pressure, FeelsLike);
        delay(10000);
    }
}

void getWeatherData() { // Client function to send/receive GET request data.
    Serial.print("Fetching data: ");
    digitalWrite(LED_BUILTIN, LOW);
    
    if (client.connect("api.openweathermap.org", 80)) { // Starts client connection, checks for connection
        client.println("GET /data/2.5/weather?id="+String(CITYID)+"&units=metric&APPID="+String(APIKEY));
        client.println("Host: api.openweathermap.org");
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");
        client.println();
    } 
    else { // Wasn't able to connect
        Serial.println("Connection failed");
        Serial.println();
    }
  
    while(client.connected() && !client.available()) 
    delay(1);                                          // Waits for data
    while (client.connected() || client.available())    
         {                                             // Connected or data available
           char c = client.read();                     // Gets byte from ethernet buffer
           result = result+c;
         }

    digitalWrite(LED_BUILTIN, HIGH);
    client.stop(); // Stop client
    result.replace('[', ' ');
    result.replace(']', ' ');
    Serial.println(result);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, result);

    String location = doc["name"];
    String country = doc["sys"]["country"];
    float temperature = doc["main"]["temp"];
    float feelslike = doc["main"]["feels_like"];
    float humidity = doc["main"]["humidity"];
    String weather = doc["weather"]["main"];
    String description = doc["weather"]["description"];
    float pressure = doc["main"]["pressure"];
    weatherDescription = description;
    weatherLocation = location;
    Country = country;
    Temperature = temperature;
    Humidity = humidity;
    Pressure = pressure;
    FeelsLike = feelslike;

}

void displayWeather(String location,String description) {
    Serial.println("Weather in "+location+", "+Country+": "+description);
}

void displayConditions(float Temperature, float Humidity, float Pressure, float FeelsLike) {
    // Temperature
    Serial.print(" T: ");
    Serial.print(Temperature,1);
    Serial.print("°C. Feels like ");
    Serial.print(FeelsLike,1);
    Serial.println("°C");
    
    // Humidity
    Serial.print(" H: ");
    Serial.print(Humidity,0);
    Serial.println("% ");
    
    // Pressure
    Serial.print(" P: ");
    Serial.print(Pressure,1);
    Serial.println(" hPa");
}

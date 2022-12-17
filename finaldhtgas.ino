 #include <WiFi.h>
#include<DHT.h>
#include<ThingSpeak.h>
#include <HTTPClient.h>
#include <time.h>

#define DHTPIN 15
#define DHTTYPE DHT11
String apiKey = "L255YSLRO016C4CX"; // Enter your Write API key from ThingSpeak
const char *ssid = "Hai"; // replace with your wifi ssid and wpa2 key
const char *pass = "rinnususan.";
const char* server = "api.thingspeak.com";
DHT dht(DHTPIN,DHTTYPE);

WiFiClient client;
#define MAIN_SSID "Hai"
#define MAIN_PASS "rinnususan."
#define CSE_IP "192.168.43.28"
#define CSE_PORT 5089
#define OM2M_ORGIN "admin:admin"
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "Gas_Level" 
#define OM2M_DATA_CONT "Node-1/Data"
#define INTERVAL 15000L
const char * ntpServer = "pool.ntp.org";

HTTPClient http;

long randNumber;
long int prev_millis = 0;
unsigned long epochTime;
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime( & timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time( & now);
  return now;
}



void setup()
{  
Serial.begin(115200);
delay(10);
  dht.begin();
Serial.println("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
}
void loop()
{
  float t=dht.readTemperature();
float h = analogRead(35);


if (isnan(h))
{
Serial.println("Failed to read from MQ-5 sensor!");
return;
}
 
if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
   
  ThingSpeak.setField(2,t);
String postStr = apiKey;
postStr += "&field1=";
postStr += String((h/1023)*100);
postStr += "r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
Serial.print("Gas Level: ");
Serial.println((h/1023)*100);
Serial.println("Data Send to Thingspeak");
 Serial.print("Temperature: ");
  Serial.print(t);
}
delay(500);
client.stop();

 
// thingspeak needs minimum 15 sec delay between updates.
delay(1500);



 if (millis() - prev_millis >= INTERVAL) {
    epochTime = getTime();
    String data;
    String server = "http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String() + OM2M_MN;

    http.begin(server + String() + OM2M_AE + "/" + OM2M_DATA_CONT + "/");

    http.addHeader("X-M2M-Origin", OM2M_ORGIN);
    http.addHeader("Content-Type", "application/json;ty=4");
    http.addHeader("Content-Length", "100");

    data = "[" + String(epochTime) + ", " + String(h) + ", " + String(t) +   + "]"; 
    String req_data = String() + "{\"m2m:cin\": {"

      +
      "\"con\": \"" + data + "\","

      +
      "\"lbl\": \"" + "V1.0.0" + "\","

      //+ "\"rn\": \"" + "cin_"+String(i++) + "\","

      +
      "\"cnf\": \"text\""

      +
      "}}";
    int code = http.POST(req_data);
    http.end();
    Serial.println(code);
    prev_millis = millis();
  }
  delay(500);
}

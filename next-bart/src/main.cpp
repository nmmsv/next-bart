#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define WIFI_SSID "NeemsWiFi2.4"
#define WIFI_PASSWORD "HolaSoyDora420"

std::vector<int> extract_etd_time(String payload, String direction);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(921600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting..");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected. Local IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // put your main code here, to run repeatedly:
  String api_path = "https://api.bart.gov/api/etd.aspx?cmd=etd&orig=ROCK&key=MW9S-E7SL-26DU-VV8V&json=y";
  HTTPClient http;

  http.begin(api_path.c_str());
  int response = http.GET();
  if (response > 0)
  {
    String payload = http.getString();

    std::vector<int> etds = extract_etd_time(payload, "South");
    Serial.print("South: ");
    for (int k = 0; k < etds.size(); k++)
    {
      Serial.print(etds[k]);
      Serial.print(", ");
    }

    Serial.println("");
  }
  delay(10000);
}

std::vector<int> extract_etd_time(String payload, String direction)
{
  JSONVar myObject = JSON.parse(payload);
  std::vector<int> etds;
  for (int i = 0; i < myObject["root"]["station"][0]["etd"].length(); i++)
  {
    for (int j = 0; j < myObject["root"]["station"][0]["etd"][i]["estimate"].length(); j++)
    {
      String min = myObject["root"]["station"][0]["etd"][i]["estimate"][j]["minutes"];
      String dir = myObject["root"]["station"][0]["etd"][i]["estimate"][j]["direction"];
      if (dir == direction)
      {
        etds.push_back(min.toInt());
      }
    }
  }

  return etds;
}
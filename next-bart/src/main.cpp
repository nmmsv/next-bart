#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <LiquidCrystal.h>
#include <sstream>

#define WIFI_SSID "NeemsWiFi2.4"
#define WIFI_PASSWORD "HolaSoyDora420"
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

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

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Rockridge (S):");
}

void loop()
{ // put your main code here, to run repeatedly:
  String api_path = "https://api.bart.gov/api/etd.aspx?cmd=etd&orig=ROCK&key=MW9S-E7SL-26DU-VV8V&json=y";
  HTTPClient http;
  if (WiFi.status() != WL_CONNECTED)
  {
    lcd.setCursor(0, 1);
    lcd.print("DISCONNECTED!");
    Serial.println("WIFI DISCONNECTED!");
    delay(30000);
  }
  else
  {
    http.begin(api_path.c_str());
    int response = http.GET();
    if (response > 0)
    {
      String payload = http.getString();

      // extract southbound estimated time of departure
      std::vector<int> etds = extract_etd_time(payload, "South");
      std::ostringstream os;

      Serial.print("South: ");
      for (int k = 0; k < etds.size(); k++)
      {
        if (etds[k] < 100 and etds[k] > 0)
        {
          Serial.print(etds[k]);
          os << etds[k];
          if (k != etds.size() - 1)
          {
            Serial.print(", ");
            os << ",";
          }
        }
      }
      std::string tmp = os.str();
      tmp.insert(tmp.begin(), 16 - tmp.size(), ' ');
      const char *cstr = tmp.c_str();

      // set the cursor to column 0, line 1
      // (note: line 1 is the second row, since counting begins with 0):
      lcd.setCursor(0, 1);
      lcd.print(cstr);

      Serial.println("");
      delay(10000);
    }
    else
    {
      lcd.setCursor(0, 1);
      lcd.print("HTML Code " + (String)response);

      Serial.println("HTML Code " + (String)response);
      delay(10000);
    }
  }
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
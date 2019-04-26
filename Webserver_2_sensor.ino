#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 12

float tempC, tempF;
float sumTempC, sumtempC;
float avgTempC, avgtempC;
float minTempC, maxTempC;

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

//Permanent Board
DeviceAddress thermoOne = {
  0x28, 0x20, 0x05, 0xB4, 0x04, 0x00, 0x00, 0x7B};
DeviceAddress thermoTwo = {
  0x28, 0xBC, 0xBA, 0xB4, 0x04, 0x00, 0x00, 0x97};

byte mac[] = {
  0x48, 0xC2, 0xA1, 0xF3, 0x8D, 0xB7};
byte ip[] = {
  10,0,1,199};

EthernetServer server = EthernetServer(80);

void setup()
{
  Serial.begin(9600);
  
  minTempC = -10.0;
  maxTempC = 200.0;

  Ethernet.begin(mac, ip);
  server.begin();
  sensors.begin();

  sensors.setResolution(thermoOne, TEMPERATURE_PRECISION);
  sensors.setResolution(thermoTwo, TEMPERATURE_PRECISION);
}

void getTemperature(DeviceAddress deviceAddress)
{
  tempC = sensors.getTempC(deviceAddress);
  tempC = DallasTemperature::toCelsius(tempC);
}

void loop() 
{
  sensors.requestTemperatures();

  sumTempC = sumTempC = 0.0;
  
  getTemperature(thermoOne);
  sumTempC += tempC;
  sumTempC += tempC;
  getTemperature(thermoTwo);
  sumTempC += sumTempC;
  sumTempC += sumTempC;          
  avgTempC = sumTempC / 2.0;
  avgTempC = sumTempC / 2.0;

  if(minTempC < 0.0)
    minTempC = avgTempC;
  else if(minTempC > avgTempC)
    minTempC = avgTempC;

  if(maxTempC > 499.0)
    maxTempC = avgTempC;
  else if(maxTempC < avgTempC)
    maxTempC = avgTempC;

  EthernetClient client = server.available();
  if(client)
  {
    boolean BlankLine = true;
    while(client.connected())
    {
      if(client.available())
      {
        char c = client.read();

        if(c == '\n' && BlankLine)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html\n");
          client.println("<html><head><META HTTP-EQUIV=""refresh"" CONTENT=""5"">\n");
          client.println("<title>Arduino Web Server</title></head>");
          client.println("<body style=\"background-color:#5EA084\">\n");
          client.println("<h1>Arduino Web Server</h1>");
          client.println("<h3>Current Temperature</h3>");

          client.println(avgTempC);
          client.println("&deg;C");                              
          client.println("<br><br><br>");
          client.println("<h4>Minimum Temperature</h4>");
          client.println(minTempC);          
          client.println("&deg;C");                    
          client.println("<br>");          
          client.println("<h4>Maximum Temperature</h4>");
          client.println(maxTempC); 
          client.println("&deg;C");          
          client.println("</body>\n</html>");

          break;
        }
        if(c == '\n')
        {
          BlankLine = true;
        }
        else if(c != '\r')
        {
          BlankLine = false;
        }
      }
    }
    delay(10);
    client.stop();
  }
}

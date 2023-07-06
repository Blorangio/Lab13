#include "oled-wing-adafruit.h"
#include "mqtt.h"
#include "env.h"
#include "blynk.h"

OledWingAdafruit display;

SYSTEM_THREAD(ENABLED);

void callback(char *topic, byte *payload, unsigned int length);

MQTT client("lab.thewcl.com", 1883, callback);

volatile int recieving = 0;

void updatePositionFunc();

Timer updatePosition(10000, updatePositionFunc, false);

volatile float positions[2];

bool once = 0;

void setup()
{
  Serial.begin(9600);
  display.setup();
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  client.subscribe("Iot2023");
  updatePosition.start();
}
void loop()
{
  // Blynk.run();
  display.loop();
  display.clearDisplay();
  display.setCursor(0, 0);

  if (client.isConnected())
  {
    client.loop();
  }
  else
  {
    client.connect(System.deviceID());
    client.subscribe("Iot2023");
  }

  display.printf("Latitude: %.4f\nLongitude: %.4f", positions[0], positions[1]);

  if (once)
  {
    if (recieving % 2 == 0)
    {
      client.publish("Iot2023", "0");
    }
    else
    {
      client.publish("Iot2023", "1");
    }
    once = false;
  }

  display.display();
}

void updatePositionFunc()
{
  once = true;
}

void callback(char *topic, byte *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.printf("%s", p);
  Serial.println();

  // To parse an int
  float receivedFloat = atof(p);
  if (receivedFloat != 0.0 && receivedFloat != 1.0)
  {
    positions[recieving % 2] = receivedFloat;
    recieving++;
  }
}
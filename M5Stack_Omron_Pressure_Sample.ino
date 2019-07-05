#include <driver/dac.h>
#include <M5Stack.h>
#include "Omron2SMPB02E.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include "Ambient.h"
#include "config.h"

WiFiClient client;
Ambient ambient;
Omron2SMPB02E prs;
// Omron2SMPB02E prs(0); // in case of SDO=0 configuration

#define PERIOD 5 * 60
#define JST 3600* 9
#define DELAY_CONNECTION 100

void setupWiFi(void)
{
    int ret, i;
    while ((ret = WiFi.status()) != WL_CONNECTED) {
        Serial.printf("> stat: %02x", ret);
        ret = WiFi.begin(ssid, password);  //  Wi-Fi APに接続
        i = 0;
        while ((ret = WiFi.status()) != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
            delay(DELAY_CONNECTION);
            if ((++i % (1000 / DELAY_CONNECTION)) == 0) {
                Serial.printf(" >stat: %02x", ret);
            }
            if (i > 10 * 1000 / DELAY_CONNECTION) { // 10秒経過してもDISCONNECTEDのままなら、再度begin()
                break;
            }
        }
    }
    Serial.println("WiFi connected\r\nIP address: ");
    Serial.println(WiFi.localIP());
}

void setupAmbient(void)
{
  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
}

void setupPrs(void)
{
  prs.begin();
  prs.set_mode(MODE_NORMAL);
}

void setup()
{
  M5.begin();
  dac_output_disable(DAC_CHANNEL_1);
  Serial.begin(115200);
  setupPrs();
  setupWiFi();
  setupAmbient();
  delay(300);
}

void loop()
{
  delay(2000);
  float temp = prs.read_temp();
  float pressure = prs.read_pressure();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Temperature:\r\n%f[degC]\r\n\r\n", temp);
  M5.Lcd.printf("Pressure:\r\n%f[Pa]\r\n", pressure);

  ambient.set(1, String(temp).c_str());
  ambient.set(2, String(pressure).c_str());
  ambient.send();
}

#include <Wire.h>
#include "Arduino.h"
#include "WiFiMulti.h"
#include "Audio.h"
// #include "TCA9534.h"

#define I2S_DOUT 4
#define I2S_BCLK 5
#define I2S_LRC 6

Audio audio;
WiFiMulti wifiMulti;

// TCA9534 ioex;

String ssid = "yanfa1";
String password = "1223334444yanfa";

bool i2cScanForAddress(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

// Wrapper function for sending I2C commands
void sendI2CCommand(uint8_t command) {
  uint8_t error;
  // Start sending commands to the specified address
  Wire.beginTransmission(0x30);
  // Send command
  Wire.write(command);
  //  End transmission and return status
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("command 0x");
    Serial.print(command, HEX);
    Serial.println(" Sent successfully");
  } else {
    Serial.print("Command sent error, error code:");
    Serial.println(error);
  }
}

void setup() {
  Serial.begin(115200);

  Wire.begin(15, 16);
  delay(50);
  // ioex.attach(Wire);
  // ioex.setDeviceAddress(0x18);
  // ioex.config(3, TCA9534::Config::OUT);
  // ioex.config(4, TCA9534::Config::OUT);
  // ioex.output(3, TCA9534::Level::L);
  // ioex.output(4, TCA9534::Level::H);
  while (1) {
    if (i2cScanForAddress(0x30) && i2cScanForAddress(0x5D)) {
      Serial.print("The microcontroller is detected: address 0x");
      Serial.println(0x30, HEX);
      Serial.print("The microcontroller is detected: address 0x");
      Serial.println(0x5D, HEX);


      break;
    } else {
      Serial.print("No microcontroller was detected: address 0x");
      Serial.println(0x30, HEX);
      Serial.print("No microcontroller was detected: address 0x");
      Serial.println(0x5D, HEX);
      //Prevent the microcontroller did not start to adjust the bright screen
      sendI2CCommand(0x19);
      pinMode(1, OUTPUT);
      digitalWrite(1, LOW);
      //ioex.output(2, TCA9534::Level::L);
      //ioex.output(2, TCA9534::Level::H);
      delay(120);
      pinMode(1, INPUT);

      delay(100);
    }
  }
  sendI2CCommand(0x17);
  Serial.printf("[LINE--%d]\n", __LINE__);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid.c_str(), password.c_str());
  wifiMulti.run();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect(true);
    wifiMulti.run();
  }
  Serial.printf("[LINE--%d]\n", __LINE__);
  Serial.println("--- WIFI_CONNECTED ---");
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(20);  // 0...21
  //ioex.output(4, TCA9534::Level::L);
  //  audio.connecttoFS(SD, "/320k_test.mp3");
  //  audio.connecttoFS(SD, "test.wav");

  // Choose the URL of the music you want to play
  
  // This is Taylor Swift singing 'Last Christmas'

  audio.connecttohost("http://music.163.com/song/media/outer/url?id=1391891631.mp3"); // Empire state of mine.mp3
  Serial.printf("[LINE--%d]\t ready to play!!\n", __LINE__);
}

void loop() {
  audio.loop();
  if (Serial.available()) {  // put streamURL in serial monitor
    audio.stopSong();
    String r = Serial.readString();
    r.trim();
    if (r.length() > 5) audio.connecttohost(r.c_str());
    log_i("free heap=%i", ESP.getFreeHeap());
  }
}

// optional
void audio_info(const char *info) {
  Serial.print("info        ");
  Serial.println(info);
}
void audio_id3data(const char *info) {  //id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
}
void audio_eof_mp3(const char *info) {  //end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
}
void audio_showstation(const char *info) {
  Serial.print("station     ");
  Serial.println(info);
}
void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle ");
  Serial.println(info);
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     ");
  Serial.println(info);
}
void audio_commercial(const char *info) {  //duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}
void audio_icyurl(const char *info) {  //homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}
void audio_lasthost(const char *info) {  //stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}

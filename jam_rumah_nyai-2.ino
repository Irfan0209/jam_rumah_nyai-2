

//SETUP DMD
#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 2

#if defined(ESP8266) || defined(ESP32)  // Jika menggunakan ESP8266 atau ESP32
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  
  #include <DMDESP.h>
  #include <ESP_EEPROM.h>

  DMDESP  Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)
  
  // Pengaturan hotspot WiFi dari ESP8266
  char ssid[20]     = "JAM_PANEL_MUSHOLLAH";
  char password[20] = "00000000";
  const char* host = "OTA-PANEL";

  ESP8266WebServer server(80);

#else
  #include <SPI.h>
  #include <DMD3asis.h>
  #include <avr/pgmspace.h>
  #include <MemoryFree.h>
  #include <EEPROM.h>
  
  DMD3 Disp(DISPLAYS_WIDE,DISPLAYS_HIGH);
  
#endif



#include <Wire.h>
#include <RtcDS3231.h>


#include "PrayerTimes.h"


#include <C:\Users\irfan\Documents\Project\project-jam-jws(pro)\fonts/SystemFont5x7.h>
#include <C:\Users\irfan\Documents\Project\project-jam-jws(pro)\fonts/Font4x6.h>
#include <C:\Users\irfan\Documents\Project\project-jam-jws(pro)\fonts/System4x7.h>
#include <C:\Users\irfan\Documents\Project\project-jam-jws(pro)\fonts/SmallCap4x6.h>
#include <C:\Users\irfan\Documents\Project\project-jam-jws(pro)\fonts/EMSans6x16.h>


#define BUZZ  4//D4 // PIN BUZZER

#define Font0 SystemFont5x7
#define Font1 Font4x6
#define Font2 System4x7 
#define Font3 SmallCap4x6
#define Font4 EMSans6x16

// Ukuran EEPROM (pastikan cukup untuk semua data)
#define EEPROM_SIZE 50

//create object
RtcDS3231<TwoWire> Rtc(Wire);
RtcDateTime now;
double times[sizeof(TimeName)/sizeof(char*)];

uint8_t ihtiSholat[]    = {0,0,0,0,0};
uint8_t iqomah[]        = {5,1,5,5,5,2,5};
uint8_t displayBlink[]  = {5,0,5,5,5,5,5};
uint8_t dataIhty[]      = {3,0,3,3,0,3,2};

struct Config {
  uint8_t chijir;
  uint8_t durasiadzan;
  uint8_t ihti;
  float latitude = -7.364057;
  float longitude = 112.646222;
  uint8_t zonawaktu = 7;
};


struct TanggalDanWaktu
{
  uint8_t detik;
  uint8_t menit;
  uint8_t jam;
  uint8_t hari;
  uint8_t tanggal;
  uint8_t bulan;
  uint8_t tahun;
};

struct Tanggal
{
  uint8_t tanggal;
  uint8_t bulan;
  uint16_t tahun;
};

struct TanggalJawa
{
  uint8_t pasaran;
  uint8_t wuku;
  uint8_t tanggal;
  uint8_t bulan;
  uint16_t tahun;
};

struct JamDanMenit
{
  uint8_t jam;
  uint8_t menit;
};

TanggalDanWaktu tanggalMasehi;
Tanggal tanggalHijriah;
TanggalJawa tanggalJawa;
JamDanMenit waktuMagrib;
Config config;



// Variabel untuk waktu, tanggal, teks berjalan, tampilan ,dan kecerahan
String setJam        = "00:00:00";
String setTanggal    = "01-01-2024";
String setText       = "Selamat Datang!";
char info1[100];
char info2[100];
uint16_t    brightness    = 50;
char   text[200] ;
bool   adzan         = 0;
bool   stateBuzzer   = 1;
uint8_t    DWidth        = Disp.width();
uint8_t    DHeight       = Disp.height();
uint8_t    sholatNow     = -1;
bool       reset_x       = 0; 

/*======library tambahan=======*/
//byte   tampilan      = 1;
//byte   mode          = 1;
uint8_t   list          = 0; 
//bool   flag1         = 1;//variabel untuk menyimpan status animasi running text  
uint8_t    speedDate      = 40; // Kecepatan default date
uint8_t    speedText1     = 40; // Kecepatan default text  
uint8_t     speedText2    = 40;
float dataFloat[10];
int   dataInteger[10];
uint8_t indexText;

/*============== end ================*/

enum Show{
  ANIM_INFO,
  ANIM_JAM,
  ANIM_TEXT,
  ANIM_SHOLAT,
  ANIM_ADZAN,
  ANIM_IQOMAH,
  ANIM_BLINK,
  //ANIM_ZONK
};

Show show = ANIM_JAM;


#if defined(ESP8266) || defined(ESP32)
  //----------------------------------------------------------------------
  // HJS589 P10 FUNGSI TAMBAHAN UNTUK NODEMCU ESP8266
  
  void ICACHE_RAM_ATTR refresh() {
    Disp.refresh();
    timer0_write(ESP.getCycleCount() + 80000);
  }
  
  void Disp_init_esp() {
    EEPROM.begin(EEPROM_SIZE);
    Disp.start();
    Disp.clear();
    Disp.setBrightness(brightness);
    Serial.println("Setup dmd selesai");
  
    noInterrupts();
    timer0_isr_init();
    timer0_attachInterrupt(refresh);
    timer0_write(ESP.getCycleCount() + 80000);
    interrupts();
  }
  
  IPAddress local_IP(192, 168, 2, 1);      // IP Address untuk AP
  IPAddress gateway(192, 168, 2, 1);       // Gateway
  IPAddress subnet(255, 255, 255, 0);      // Subnet mask
  
  void AP_init() {
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ssid);
    WiFi.setSleepMode(WIFI_NONE_SLEEP); // Pastikan WiFi tidak sleep
  
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    
    ArduinoOTA.setHostname(host);
     ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_FS
        type = "filesystem";
      }
  
      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });
    ArduinoOTA.begin();
    
    Serial.println("Server dimulai.");  
  }
#else
  // =========================================
  // DMD3 P10 utility Function================
  // =========================================
  void Disp_init_arduino() 
    { 
      Disp.setDoubleBuffer(true);
      Timer1.initialize(1500);
      Timer1.attachInterrupt(scan);
      setBrightness(200);
      fType(1);  
      Disp.clear();
      Disp.swapBuffers();
    }

  void setBrightness(int bright)
    { Timer1.pwm(9,bright);}

  void scan()
    { Disp.refresh();}
  
#endif

void setup() {
  Serial.begin(115200);
  pinMode(BUZZ, OUTPUT); 

  int rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
    if (rtn != 0) {
      Serial.println(F("I2C bus error. Could not clear"));
      if (rtn == 1) {
        Serial.println(F("SCL clock line held low"));
      } else if (rtn == 2) {
        Serial.println(F("SCL clock line held low by slave clock stretch"));
      } else if (rtn == 3) {
        Serial.println(F("SDA data line held low"));
      }
    } 
    else { // bus clear, re-enable Wire, now can start Wire Arduino master
      Wire.begin();
    }
  
  Rtc.Begin();
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

#if defined(ESP8266) || defined(ESP32)
  Disp_init_esp();
  AP_init();
#else
  Disp_init_arduino();
#endif

JadwalSholat();

for(int i = 0; i < 4; i++)
 {
      Buzzer(1);
      delay(80);
      Buzzer(0);
      delay(80);
 }

}

void loop() {
  getData();

}

void getData(){
  if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        int eq = input.indexOf('=');
        if (eq != -1) {
          String key = input.substring(0, eq);
          String value = input.substring(eq + 1);
          
          if (key == "Tm") {
            String setJam = value;
            RtcDateTime now = Rtc.GetDateTime();
            uint8_t colon = value.indexOf(':');
            uint8_t dash1 = value.indexOf('-');
            uint8_t dash2 = value.indexOf('-', dash1 + 1);
            uint8_t dash3 = value.indexOf('-', dash2 + 1);
            
            if (colon != -1 && dash1 != -1 && dash2 != -1 && dash3 != -1) {
              uint8_t jam = value.substring(0, colon).toInt();
              uint8_t menit = value.substring(colon + 1, dash1).toInt();
              uint8_t tanggal = value.substring(dash1 + 1, dash2).toInt();
              uint8_t bulan = value.substring(dash2 + 1, dash3).toInt();
              uint16_t tahun = value.substring(dash3 + 1).toInt();
              Rtc.SetDateTime(RtcDateTime(tahun, bulan, tanggal, jam, menit, now.Second()));
            }
            
          }
          
          else if (key == "text") {
            value.toCharArray(text,value.length()+1);
          }

          else if (key == "Br") {
            brightness = map(value.toInt(),0,100,10,255);
          }

          else if (key == "Sptx") {
            speedText1 =  map(value.toInt(),0,100,10,80);
          }

          else if (key == "Spdt") {
            speedDate =  map(value.toInt(),0,100,10,80);
          }

          else if (key == "Lk") {
            parsingData(value);
          }

          else if (key == "Iq") {
            // Mencari posisi tanda "-"
            int separatorIndex = value.indexOf('-');
          
            // Memisahkan angka pertama
            int indexSholat = value.substring(0, separatorIndex).toInt();
          
            // Memisahkan angka kedua
            int indexKoreksi = value.substring(separatorIndex + 1).toInt();  
            iqomah[indexSholat]=indexKoreksi;
          }

          else if (key == "Dy") {
            // Mencari posisi tanda "-"
            int separatorIndex = value.indexOf('-');
          
            // Memisahkan angka pertama
            int indexSholat = value.substring(0, separatorIndex).toInt();
          
            // Memisahkan angka kedua
            int indexKoreksi = value.substring(separatorIndex + 1).toInt();  
            displayBlink[indexSholat]=indexKoreksi;
          }

          else if (key == "Kr") {
            // Mencari posisi tanda "-"
            int separatorIndex = value.indexOf('-');
          
            // Memisahkan angka pertama
            int indexSholat = value.substring(0, separatorIndex).toInt();
          
            // Memisahkan angka kedua
            int indexKoreksi = value.substring(separatorIndex + 1).toInt();  
            dataIhty[indexSholat]=indexKoreksi;
          }

          else if (key == "Bzr") {
            stateBuzzer = value.toInt();
          }

          else if (key == "newPassword"){
            if(value.length()==8){
              //Serial.println(String()+"newPassword:"+newPassword);
              value.toCharArray(password, value.length() + 1); // Set password baru
              //saveStringToEEPROM(56, password); // Simpan password AP
              server.send(200, "text/plain", "Password WiFi diupdate");
            }
          }
        }
    }
}

 //----------------------------------------------------------------------
// I2C_ClearBus menghindari gagal baca RTC (nilai 00 atau 165)

int I2C_ClearBus() {
  
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}

void Buzzer(uint8_t state)
  {
    if(!stateBuzzer) return;
    
    switch(state){
      case 0 :
        digitalWrite(BUZZ,HIGH);
      break;
      case 1 :
        digitalWrite(BUZZ,LOW);
      break;
      case 2 :
        for(int i = 0; i < 2; i++){
          digitalWrite(BUZZ,LOW);
          delay(80);
          digitalWrite(BUZZ,HIGH);
          delay(80);
        }
      break;
    };
  }

  void parsingData(String data){
  // Data string
  //String data = "0.1234-111.2345-7";
  
  char charData[20]; 
  data.toCharArray(charData, sizeof(charData));

  // Buffer untuk strtok_r()
  char *token;
  char *savePtr;

  // Array penyimpanan angka
  float angkaFloat[10];
  int angkaInt[10];
  int indexFloat = 0, indexInt = 0;

  // Mulai parsing pertama
  token = strtok_r(charData, "-", &savePtr);

  while (token != NULL) { // Menggunakan while karena hanya dijalankan sekali
    Serial.print("Nilai ditemukan: ");
    Serial.println(token);

    // Cek apakah token mengandung titik (float) atau tidak (int)
    if (strchr(token, '.') != NULL) {
      dataFloat[indexFloat] = atof(token);
//      Serial.print("Disimpan sebagai float: ");
//      Serial.println(dataFloat[indexFloat], 5);
      config.latitude = dataFloat[0];
      config.longitude = dataFloat[1];
      indexFloat++;
    } else {
      dataInteger[indexInt] = atoi(token);
//      Serial.print("Disimpan sebagai int: ");
//      Serial.println(dataInteger[indexInt]);
      config.zonawaktu = dataInteger[0];
      indexInt++;
    }

    // Ambil nilai berikutnya
    token = strtok_r(NULL, "-", &savePtr);
  }

  //Serial.println("\nParsing selesai di proses()");
}

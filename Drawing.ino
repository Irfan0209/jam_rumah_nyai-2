5//const char msg[] PROGMEM = "MUSHOLLAH HIDAYATULLAH RT19/RW03,DODOKAN,TANJUNGSARI";
const char * const pasar[] PROGMEM = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
const char * const Hari[] PROGMEM = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
const char * const bulanMasehi[] PROGMEM = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
const char msg1[] PROGMEM ="SAIPUL,SANIYAH,MUSY'IROH,ACHMAD";
const char * const namaBulanHijriah[] PROGMEM = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
};
const char jadwal[][8] PROGMEM = {
    "SUBUH ", "TERBIT ", "DZUHUR ", "ASHAR ", 
    "TRBNM ", "MAGRIB ", "ISYA' "
  };


//================= tampilan animasi ==================//

void runAnimasiJam(){
  
  RtcDateTime now = Rtc.GetDateTime();
  static int    y=0;
  static bool    s; // 0=in, 1=out              
  static uint32_t   lsRn;
  uint32_t          Tmr = millis();
  uint8_t dot    = now.Second();
  char buff_jam[20];
  
  if(dot & 1){sprintf(buff_jam,"%02d:%02d",now.Hour(),now.Minute());}
  else{sprintf(buff_jam,"%02d %02d",now.Hour(),now.Minute());}
  
  if((Tmr-lsRn)>75) 
      { 
        if(s==0 and y<9 ){lsRn=Tmr;y++; }
        if(s==1 and y>0){lsRn=Tmr;y--; if(y == 1){ Disp.drawText(0,0, "          "); }}
      }
  
   if(y ==9 and flagAnim == true) {s=1;}

   if (y == 0 and s==1) {y=0; s=0; flagAnim = false; show = ANIM_SHOLAT;}
  
  fType(0); 
  dwCtr(0,y-9, buff_jam); 
  //Serial.println("dot:" + String(dot));
}

void drawDate(){
  static uint16_t x;
  static uint16_t fullScroll = 0;
  RtcDateTime now = Rtc.GetDateTime();
  static uint32_t   lsRn;
  uint32_t          Tmr = millis();
   
  uint8_t Speed = speedDate;
  uint8_t daynow   = now.DayOfWeek();    // load day Number
  
  char buff_date[100]; // Pastikan ukuran buffer cukup besar
    snprintf(buff_date,sizeof(buff_date), "%s %s %02d %s %04d %02d %s %04dH",
    Hari[daynow], pasar[jumlahhari() % 5], now.Day(), bulanMasehi[now.Month()-1], now.Year(),
    tanggalHijriah.tanggal, namaBulanHijriah[tanggalHijriah.bulan - 1], tanggalHijriah.tahun);

  
  if (fullScroll == 0) { // Hitung hanya sekali
    fullScroll = Disp.textWidth(buff_date) + Disp.width() + 50;
  }

 if (Tmr - lsRn > Speed) { 
  lsRn = Tmr;
  fType(0); 
  Disp.drawText(Disp.width() - (x++), 9, buff_date);
  
  if (x >= fullScroll) {
    x = 0;
    show = ANIM_TEXT;
  }
}

}

void runningTextInfo() {
  static uint16_t x = 0;
  static uint32_t lsRn;
  uint32_t Tmr = millis();
  uint8_t Speed = speedText1;
  
  char msg_buffer[50]; // Pastikan cukup besar untuk teks
  strcpy_P(msg_buffer, msg1); // Ambil teks dari Flash

  // Hitung panjang teks hanya sekali
  static uint16_t fullScroll = 0;
  if (fullScroll == 0) { 
    fullScroll = Disp.textWidth(msg_buffer) + Disp.width() + 250;
  }

  // Jalankan animasi scrolling berdasarkan millis()
  if (Tmr - lsRn > Speed && flagAnim == false) { 
    lsRn = Tmr;
    fType(0);
    
    int posX = Disp.width() - x;
    if (posX < -Disp.textWidth(msg_buffer)) { // Cegah teks keluar layar
      x = 0;
      flagAnim = true;
      fullScroll=0;
      Disp.clear();
      return;
    }

    Disp.drawText(posX, 9, msg_buffer);
    x++; // Geser teks ke kiri
  }
}


void runAnimasiSholat(){
 
  //if(adzan) return;
  RtcDateTime now = Rtc.GetDateTime();
  static int        y=0;
  static int        x=0;
  static uint8_t       s=0; // 0=in, 1=out   
  static uint8_t       s1=0;
  static uint8_t list,lastList;

  if(list != lastList){s=0; s1=0; x=0; y=0;lastList = list; }

  static uint32_t   lsRn;
  uint32_t          Tmr = millis(); 
  
  const char *jadwal[] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "TRBNM", "MAGRIB", "ISYA'"};
  int hours, minutes;
  uint16_t tahun = now.Year();
  uint8_t bulan = now.Month();
  uint8_t tanggal = now.Day();
  char buff_jam[10];

  if((Tmr-lsRn)>55) 
  { 
    if(s1==0 and y<9){lsRn=Tmr; y++; }
    if(s==1 and x<33){lsRn=Tmr; x++; }
  }

  if((Tmr-lsRn)>4000 and y == 9) {s1=1; s=1;}

  if (x == 33 and s==1 and s1 == 1) { 
    s=0;
    s1=0;
    x=0;
    y=0;
     
    list++; 
    if(list==4){list=5;} 
    
    if(list==7){list=0; Disp.clear(); show=ANIM_JAM; }//else{list=list;}
  }

  String sholat = jadwal[list];

  get_float_time_parts(times[list], hours, minutes);

    //minutes = minutes + config.ihti;
    minutes += dataIhty[list];
  if(minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
  }

  sprintf(buff_jam,"%02d:%02d",hours,minutes);

  if(s1==0){
    fType(3);
    dwCtr(0,y-9, sholat);
    fType(0);
    dwCtr(0,18-y, buff_jam);
  }
  else{
    Disp.drawLine((list<6)?x-1:x,-1,(list<6)?x-1:x,16,1);
    Disp.drawLine((list<6)?x-2:x-1,-1,(list<6)?x-2:x-1,16,0);
  }
}















//======================= end ==========================//

//==================== tampilkan jadwal sholat ====================//
void jadwalSholat(){
 
//if (adzan) return;

  RtcDateTime now = Rtc.GetDateTime();
  static int x = 0;
  static byte s = 0; // 0 = masuk, 1 = keluar
  static uint8_t    list;
  static uint32_t   lsRn;
  uint32_t          Tmr = millis(); 

  //int hours, minutes;
  char buff_jam[6]; // Format HH:MM hanya butuh 6 karakter
  char sholat[8];   // Buffer untuk menyimpan nama sholat dari PROGMEM

  // Ambil nama sholat dari Flash
  strcpy_P(sholat, jadwal[list]);
  
  int hours, minutes;
  uint16_t tahun = now.Year();
  uint8_t bulan = now.Month();
  uint8_t tanggal = now.Day();

    if((Tmr-lsRn)>55) 
      { 
        if(s==0 and x<33){lsRn=Tmr; x++; }
        if(s==1 and x>0){lsRn=Tmr;x--; }
      }

   if((Tmr-lsRn)>1500 and x ==33) {s=1;}

   if (x == 0 && s == 1) { 
    s = 0;
    list++; 
    if (list == 4) list = 5;  
    if (list == 7) list = 0;
    if(list == 0){ JadwalSholat(); }
  }

  // Ambil nama sholat dari Flash
  strcpy_P(sholat, jadwal[list]);

  get_float_time_parts(times[list], hours, minutes);

  minutes = minutes + dataIhty[list];

  if (minutes >= 60) { minutes -= 60; hours++; }

  // Format HH:MM
  sprintf(buff_jam, "%02d:%02d", hours, minutes);

  // Tampilkan teks dengan animasi
  fType(3);
  Disp.drawText(-33 + x, 17, sholat);
  Disp.drawRect(-33 + x + 29, 17, -33 + x + 29, 23, 0);

  fType(0);
  Disp.drawText(67 - x, 17, buff_jam);
  Disp.drawRect(67 - x - 1, 17, 67 - x - 1, 23, 0);
}
//=========================================================================//
  
/*======================= animasi memasuki waktu sholat ====================================*/
void drawAzzan()
{
    static const char *jadwal[] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "TRBNM", "MAGRIB", "ISYA'"};
    const char *sholat = jadwal[sholatNow]; 
    static uint8_t ct = 0;
    static uint32_t lsRn = 0;
    uint32_t Tmr = millis();
    const uint8_t limit = config.durasiadzan;

    if (Tmr - lsRn > 500 && ct <= limit)
    {
        lsRn = Tmr;
        if (!(ct & 1))  // Lebih cepat dibandingkan ct % 2 == 0
        {
            fType(0);
            Disp.drawText(1, 0, "ADZAN");
            fType(3);
            Disp.drawText(1, 9, sholat);
            Buzzer(1);
        }
        else
        {
            Buzzer(0);
            Disp.clear();
        }
        ct++;
    }
    
    if ((Tmr - lsRn) > 1500 && (ct > limit))
    {
        show = ANIM_JAM;
        Disp.clear();
        ct = 0;
        Buzzer(0);
    }
}
/*
void drawIqomah()  // Countdown Iqomah (9 menit)
{  
    static uint32_t lsRn = 0;
    static int ct = 0;  // Mulai dari 0 untuk menghindari error
    static int mnt, scd;
    char locBuff[10];  
    uint32_t now = millis();  // Simpan millis() di awal
    
    int cn_l = (iqomah[sholatNow] * 60);
    
    mnt = (cn_l - ct) / 60;
    scd = (cn_l - ct) % 60;
    sprintf(locBuff, "-%02d:%02d", mnt, scd);

    if ((ct & 1) == 0) {  // Gunakan bitwise untuk optimasi modulo 2
        fType(0);
        dwCtr(0, 8, "IQOMAH");
    }

    fType(0);
    dwCtr(0, 16, locBuff);

    if (now - lsRn > 1000) 
    {   
        lsRn = now;
        ct++;

        Serial.println(F("test run"));  // Gunakan F() untuk hemat RAM

        if (ct > (cn_l - 5)) {
            Buzzer(ct & 1);  // Gunakan bitwise untuk menggantikan modulo 2
        }
    }

    if (ct >= cn_l)  // Pakai >= untuk memastikan countdown selesai dengan benar
    {
        ct = 0;
        Buzzer(0);
        Disp.clear();
        show = ANIM_BLINK;
    }    
}

void blinkBlock()
{
    static uint32_t lsRn;
    static int ct = 0;
    const int ct_l = displayBlink[sholatNow] * 60;  // Durasi countdown
    uint32_t now = millis();  // Simpan millis()

    // Ambil waktu dari RTC
    RtcDateTime rtcNow = Rtc.GetDateTime();

    // Hitung countdown
    int mnt = (ct_l - ct) / 60;
    int scd = (ct_l - ct) % 60;

//    char locBuff[6];
//    sprintf(locBuff, " %d:%02d ", mnt, scd);

//    fType(2);
//    Disp.drawText(10, 8, locBuff);

    // Tampilkan jam besar
    char timeBuff[9];
    sprintf(timeBuff, "%02d:%02d:%02d", rtcNow.Hour(), rtcNow.Minute(),rtcNow.Second());
    
    fType(3);
    dwCtr(0, 16, timeBuff);

    // Update countdown setiap detik
    if (now - lsRn > 1000)
    {
        lsRn = now;
        Serial.print(F("ct:"));
        Serial.println(ct);
        ct++;
    }

    // Reset jika countdown selesai & kembali ke animasi utama
    if (ct > ct_l)
    {
        sholatNow = -1;
        adzan = false;
        ct = 0;
        Disp.clear();
        show = ANIM_JAM;
    }
}*/
//===================================== end =================================//

//=========================== setingan untuk tampilan text=================//
void fType(int x)
  {
    if(x==0) Disp.setFont(Font0);
    else if(x==1) Disp.setFont(Font1); 
    else if(x==2) Disp.setFont(Font2);
    else if(x==3) Disp.setFont(Font3);
    else if(x==4) Disp.setFont(Font4);
  
  }

  void dwCtr(int x, int y, String Msg){
   uint16_t   tw = Disp.textWidth(Msg);
   uint16_t   c = int((DWidth-x-tw)/2);
   Disp.drawText(x+c,y,Msg);
}
//====================== end ==========================//

const char msg[] PROGMEM = "MUSHOLLAH HIDAYATULLAH RT19/RW03,DODOKAN,TANJUNGSARI";
const char * const pasar[] PROGMEM = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
const char * const Hari[] PROGMEM = {"AHAD","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
const char * const bulanMasehi[] PROGMEM = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
const char msg1[] PROGMEM ="LURUSKAN DAN RAPATKAN SHAFF SHOLAT";
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






















//======================= end ==========================//

//==================== tampilkan jadwal sholat ====================//
void jadwalSholat(){
 
if (adzan) return;

  RtcDateTime now = Rtc.GetDateTime();
  static int x = 0;
  static byte s = 0; // 0 = masuk, 1 = keluar
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
    const uint8_t limit = 40; // config.durasiadzan;

    if (Tmr - lsRn > 500 && ct <= limit)
    {
        lsRn = Tmr;
        if (!(ct & 1))  // Lebih cepat dibandingkan ct % 2 == 0
        {
            fType(0);
            dwCtr(0, 8, "ADZAN");
            fType(0);
            dwCtr(0, 16, sholat);
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
        show = ANIM_IQOMAH;
        Disp.clear();
        ct = 0;
        Buzzer(0);
    }
}

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
}
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

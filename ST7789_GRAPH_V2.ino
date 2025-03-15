
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define TFT_DC D1
#define TFT_RST D2
#define TFT_CS D8
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int ekran_x = 120;  //ekran x çözünürlüğü     screen x resulatiom
int ekran_y = 120;  //ekran y çözünürlüğü     screen y resulatiom

int prev_x1;
int prev_y1;

int current_x1;
int current_y1;

void setup() {
  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);
  tft.setTextColor(ST77XX_ORANGE);
  tft.setTextSize(1);
  ESP.wdtDisable();                           //yazılımsal wdt'yi devredışı bırakma      disable software wdt
  *((volatile uint32_t*)0x60000900) &= ~(1);  //donanım wdt'yi devredışı bırakma      disable hardware wdt
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);  //ekranı temizle     clear screen
  ekran_x = 120;
  ekran_y = 120;
  grafik(5, 1023, 0, 5);  //ölçüm süresi, max değer, min değer,eksen bölüm sayısı       graph period,max value,min value,division coefficient

  tft.fillScreen(ST77XX_BLACK);
  ekran_x = 160;
  ekran_y = 90;
  grafik(5, 1023, 0, 5);

  tft.fillScreen(ST77XX_BLACK);
  ekran_x = 120;
  ekran_y = 240;
  grafik(5, 1023, 0, 5);

  tft.fillScreen(ST77XX_BLACK);
  ekran_x = 240;
  ekran_y = 240;
  grafik(5, 1023, 0, 10);
}

void grafik(int zaman, int max, int min, float aralik) {
  long basla = millis();
  tft.drawFastHLine(2, ekran_y - 2, ekran_x - 10, ST77XX_WHITE);  //x ekseni    x axis
  tft.drawFastVLine(2, ekran_y - 2, -ekran_y, ST77XX_WHITE);      //y ekseni    y axis

  for (int x = 0; x <= aralik; x++) {                                      //zaman ekseni   time axis
    tft.drawFastVLine(x * (ekran_x / aralik), ekran_y, -ekran_y, 0x39e7);  //dikey bölüm çizigleri,0x39e7 bir renk kodu           vertical division lines,0x39e7 is a color code: https://rgbcolorpicker.com/565
    tft.setCursor(x * (ekran_x / aralik) - 3, ekran_y - 10);
    tft.println(String(zaman / aralik * x, 1));  //saniye   time labels in seconds
  }
  for (int y = aralik; y > 0; y--) {                                //ölçek ekseni    scale axis
    tft.drawFastHLine(2, y * (ekran_y / aralik), ekran_x, 0x39e7);  //yatay bölüm çizgileri     horizontal division lines
    tft.setCursor(10, y * (ekran_y / aralik) - 5);
    tft.println(String(max - ((max - min) / aralik * y), 1));  //ölçek    scale labels
  }
  while (zaman * 1000 > (millis() - basla)) {  //grafik ve okuma
    current_x1 = map((millis() - basla), 0, zaman * 1000, 0, ekran_x);
    current_y1 = map(analogRead(A0), min, max, 0, ekran_y);//analogRead(A0)'ı grafiği çizilecek değişken/fonksiyonla değiştirin       replace analogRead(A0) with variable/function that you want to plot
    tft.drawLine(prev_x1, prev_y1, current_x1, current_y1, 0xf9c3); 
    prev_x1 = current_x1;
    prev_y1 = current_y1;
    //sensor okuma işlemleri (varsa)      read sensors here(if required)
  }

  tft.invertDisplay(true);  //tamamlanma bildirimi      graph complation notification
  delay(500);
  tft.invertDisplay(false);
  delay(500);
  tft.invertDisplay(true);

  delay(3000);
}
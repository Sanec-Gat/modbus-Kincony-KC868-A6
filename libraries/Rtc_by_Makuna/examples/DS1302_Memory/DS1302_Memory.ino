// ПОДКЛЮЧЕНИЕ:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
#include <ThreeWire.h>
#include <RtcDS1302.h>

#define RTC_RST_PIN 2
#define RTC_DAT_PIN 4
#define RTC_CLK_PIN 5

ThreeWire myWire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))

const char data[] = "который час";

void setup() {
  Serial.begin(57600);

  Serial.print("скомпилировано: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC потерял доверие к DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC был защищен от записи, включаем запись сейчас");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC не работал активно, запускаем сейчас");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC старше времени компиляции! (Обновляем DateTime)");
    Rtc.SetDateTime(compiled);
  }


  /* закомментируйте при втором запуске, чтобы убедиться, что информация сохраняется надолго */
  // Сохраняем что-то в памяти RTC
  uint8_t count = sizeof(data);
  uint8_t written = Rtc.SetMemory((const uint8_t*)data, count);  // включает нулевой терминатор для строки
  if (written != count) {
    Serial.print("что-то не совпало, count = ");
    Serial.print(count, DEC);
    Serial.print(", written = ");
    Serial.print(written, DEC);
    Serial.println();
  }
  /* конец закомментированного раздела */
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now);
  Serial.println(" +");

  if (!now.IsValid()) {
    // Распространенные причины:
    //    1) батарейка на устройстве разряжена или отсутствует, и линия питания была отключена
    Serial.println("RTC потерял доверие к DateTime!");
  }

  delay(5000);

  // читаем данные
  uint8_t buff[20];
  const uint8_t count = sizeof(buff);
  // получаем наши данные
  uint8_t gotten = Rtc.GetMemory(buff, count);

  if (gotten != count) {
    Serial.print("что-то не совпало, count = ");
    Serial.print(count, DEC);
    Serial.print(", gotten = ");
    Serial.print(gotten, DEC);
    Serial.println();
  }

  Serial.print("данные прочитаны (");
  Serial.print(gotten);
  Serial.print(") = \"");
  // печатаем строку, но завершаем, если встречаем null
  for (uint8_t ch = 0; ch < gotten && buff[ch]; ch++) {
    Serial.print((char)buff[ch]);
  }
  Serial.println("\"");

  delay(5000);
}



void printDateTime(const RtcDateTime& dt) {
  char datestring[26];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}
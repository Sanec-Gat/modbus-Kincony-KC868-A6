//#include "Arduino.h"
//#include "Wire.h"

/*KC868-A6 DS1307 CODE*/
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"


RTC_DS1307 rtc;

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(115200);
	Wire.begin(4, 15);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop () {
    DateTime now = rtc.now();

    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(" (");
    //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.printf("%02d", now.hour());
    Serial.print(':');
    Serial.printf("%02d", now.minute());
    Serial.print(':');
    Serial.printf("%02d", now.second());
    Serial.println();

    Serial.print("UNIX time (1/1/1970) = ");
    Serial.print(now.unixtime());
    Serial.print(" sec = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println(" day");

    // calculate a date which is 7 days, 12 hours, 30 minutes, and 6 seconds into the future
    DateTime dw (F(__DATE__), F(__TIME__));

    Serial.print("Download sketch data/time: ");
  	Serial.print(dw.day(), DEC);
    Serial.print('/');
    Serial.print(dw.month(), DEC);
    Serial.print('/');
    Serial.print(dw.year(), DEC);
    Serial.print("  ");
    Serial.println(F(__TIME__));
    Serial.println();
    delay(1000);
}


#include "Arduino.h"
#include "PCF8574.h"

// Set i2c address
PCF8574 pcf8574(0x22,4,15);
PCF8574 pcf8574_re(0x24);
unsigned long timeElapsed;

void setup()
{
Serial.begin(115200);
delay(1000);

pcf8574.pinMode(P0, INPUT);
pcf8574.pinMode(P1, INPUT);
pcf8574.pinMode(P2, INPUT);
pcf8574.pinMode(P3, INPUT);
pcf8574.pinMode(P4, INPUT);
pcf8574.pinMode(P5, INPUT);

  pcf8574_re.pinMode(P0, OUTPUT);
  pcf8574_re.pinMode(P1, OUTPUT);
  pcf8574_re.pinMode(P2, OUTPUT);
  pcf8574_re.pinMode(P3, OUTPUT);
  pcf8574_re.pinMode(P4, OUTPUT);
  pcf8574_re.pinMode(P5, OUTPUT);

    Serial.print("Init pcf8574...");
    if (pcf8574.begin()){
        Serial.println("OK");
    }else{
        Serial.println("KO");
    }
		Serial.print("Init pcf8574_v2...");
    if (pcf8574_re.begin()){
        Serial.println("OK_2");
    }else{
        Serial.println("KO_2");
    }

	/*
	pcf8574_re.digitalWrite(P0, LOW);
  pcf8574_re.digitalWrite(P1, LOW);
  pcf8574_re.digitalWrite(P2, LOW);
  pcf8574_re.digitalWrite(P3, LOW);
  pcf8574_re.digitalWrite(P4, LOW);
  pcf8574_re.digitalWrite(P5, LOW);
*/

delay(1000);
}

void loop()
{
uint8_t val1 = pcf8574.digitalRead(P0);
uint8_t val2 = pcf8574.digitalRead(P1);
uint8_t val3 = pcf8574.digitalRead(P2);
uint8_t val4 = pcf8574.digitalRead(P3);
uint8_t val5 = pcf8574.digitalRead(P4);
uint8_t val6 = pcf8574.digitalRead(P5);



if (val1==LOW) {Serial.println("KEY1 _ PRESSED");
	pcf8574_re.digitalWrite(P0, LOW);
	}else{pcf8574_re.digitalWrite(P0, HIGH);}

if (val2==LOW) Serial.println("KEY2 PRESSED");
if (val3==LOW) Serial.println("KEY3 PRESSED");
if (val4==LOW) Serial.println("KEY4 PRESSED");
if (val5==LOW) Serial.println("KEY5 PRESSED");
if (val6==LOW) Serial.println("KEY6 PRESSED");

    delay(300);
}
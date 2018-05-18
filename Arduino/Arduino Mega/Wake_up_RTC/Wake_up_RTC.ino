#include <avr/sleep.h>
#include <avr/power.h>
#include <DS3232RTC.h>

//Set up the RTC variables
tmElements_t tm;
const int interruptionPin = 2;
int periodo = 1;

void setSleepTime(void);
void startSleep(void);
void digitalClockDisplay(void);
void pinInterrupt(void);

void setup()
{
  Serial.begin(9600);
  // Initialize the RTC functions
  pinMode(interruptionPin, INPUT_PULLUP);
  RTC.set(now());
  RTC.squareWave(SQWAVE_NONE);
  RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0);
  RTC.alarmInterrupt(ALARM_2, true);
  Serial.println(String("System date = ") + day() + "/" + month() + "/" + year() + " " + hour() + ":" + minute() + ":" + second() + "\n");
}


void loop()
{
  setSleepTime();
  startSleep();
  digitalClockDisplay();

}


void setSleepTime(void) {
  RTC.read(tm);
  RTC.setAlarm(ALM2_MATCH_MINUTES, (byte)(tm.Minute + periodo) % 60, 0, 0);

  //RTC.setAlarm(ALM2_EVERY_MINUTE,0,0,0);
}
void pinInterrupt(void) {
  detachInterrupt(0);
  
  Serial.println("Awaaaaaake!!");
}


//PRINT TIME FROM RDS3231
void digitalClockDisplay(void) {
  // digital clock display of the time
  RTC.read(tm);
  Serial.print(tm.Day, DEC);
  Serial.print('/');
  Serial.print(tm.Month, DEC);
  Serial.print('/');
  Serial.print(tm.Year + 1970, DEC);
  Serial.print("   ");
  Serial.print(tm.Hour, DEC);
  Serial.print(':');
  Serial.print(tm.Minute, DEC);
  Serial.print(':');
  Serial.println(tm.Second, DEC);
}

void startSleep(void) {
  Serial.println("Entering sleep");
  digitalClockDisplay();
  delay(200);

  //Setup pin as an interrupt and attach handler.
  attachInterrupt(digitalPinToInterrupt(interruptionPin), pinInterrupt, CHANGE);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_enable();

  sleep_mode(); //can be awakened by interruption

  sleep_disable();
}


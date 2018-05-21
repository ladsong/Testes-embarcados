#include "TinyGPS++.h"
//#include "SoftwareSerial.h"

//SoftwareSerial serial_connection(10, 11); //RX=pin 10, TX= pin 11
TinyGPSPlus gps;

HardwareSerial Serial1(1);

void setup()
{
	Serial.begin(9600);
  Serial1.begin(9600);
	//serial_connection.begin(9600);
	Serial.println("GPS Start");
}

void loop()
{
	while(Serial1.available())
	{
		if (gps.encode(Serial1.read())) {
		  Serial.println("Quantidade de Satelites:");
      Serial.println(gps.satellites.value());
      Serial.println("Latitude:");
      Serial.println(gps.location.lat(), 6);
      Serial.println("Longitude:");
      Serial.println(gps.location.lng(), 6);
      Serial.println("Velocidade MPH:");
      Serial.println(gps.speed.mph());
      Serial.println("Altitude:");
      Serial.println(gps.altitude.feet());
      Serial.println();
		}
   //Serial.println("Primeiro while");
	}
	/*if(gps.location.isUpdated())
	{
		Serial.println("Quantidade de Satelites:");
		Serial.println(gps.satellites.value());
		Serial.println("Latitude:");
		Serial.println(gps.location.lat(), 6);
		Serial.println("Longitude:");
		Serial.println(gps.location.lng(), 6);
		Serial.println("Velocidade MPH:");
		Serial.println(gps.speed.mph());
		Serial.println("Altitude:");
		Serial.println(gps.altitude.feet());
		Serial.println();
		Serial.println();
		Serial.println();
	} else Serial.println("Location not updated");*/
}

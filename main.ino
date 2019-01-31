#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <SD.h>

File myFile;

SoftwareSerial mySerial(6, 7);
TinyGPS gps;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

void setup()
{
	Serial.begin(9600);
	Serial.print("Initializing SD card...");
  
	if (!SD.begin(4)) {
		Serial.println("initialization failed!");
		return;
	}
	Serial.println("initialization done.");
	mySerial.begin(9600);
	delay(1000);
	Serial.print("Sizeof(gpsobject) = ");
	Serial.println(sizeof(TinyGPS));
	Serial.println();
}

void loop()
{
	bool newdata = false;
	unsigned long start = millis();
  
	// Every 5 seconds print an update
	while (millis() - start < 5000)
	{
		if (mySerial.available())

		{
			char c = mySerial.read();
			//Serial.print(c);  // uncomment to see raw GPS data
			if (gps.encode(c))
			{
				newdata = true;
				break;  // uncomment to print new data immediately!
			}
		}
	}

	if (newdata)
	{
		Serial.println("Acquired Data");
		Serial.println("-------------");
		gpsdump(gps);
		Serial.println("-------------");
	}

}

void gpsdump(TinyGPS &gps)
{

	float flat, flon;
	unsigned long age, date, time, chars;
	int year;
	byte month, day, hour, minute, second, hundredths;
	unsigned short sentences, failed;

	gps.f_get_position(&flat, &flon, &age);
	Serial.print("Lat/Long(float): "); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);
	Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

	gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
	Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
	Serial.print("  Time: "); Serial.print(static_cast<int>(hour + 1));  Serial.print(":"); Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
	Serial.print("  Fix age: ");  Serial.print(age); Serial.println("ms.");

	Serial.print("Alt(float): "); printFloat(gps.f_altitude()); Serial.println();
	Serial.print("Speed(kph): "); printFloat(gps.f_speed_kmph()); Serial.println();
	Serial.print("Course(float): "); printFloat(gps.f_course()); Serial.println();

	gps.stats(&chars, &sentences, &failed);
	Serial.print("Stats: characters: "); Serial.print(chars); Serial.print(" sentences: ");
	Serial.print(sentences); Serial.print(" failed checksum: "); Serial.println(failed);

	myFile = SD.open("test2.txt", FILE_WRITE);
	if (myFile) {

		gps.f_get_position(&flat, &flon, &age);
		myFile.print("Lat/Long(float): "); myFileprintFloat(flat, 5); myFile.print(", "); myFileprintFloat(flon, 5);
		myFile.print(" Fix age: "); myFile.print(age); myFile.println("ms.");

		gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
		myFile.print("Date: "); myFile.print(static_cast<int>(day)); myFile.print("/"); myFile.print(static_cast<int>(month)); myFile.print("/"); myFile.print(year);
		myFile.print("  Time: "); myFile.print(static_cast<int>(hour + 1));  myFile.print(":"); myFile.print(static_cast<int>(minute)); myFile.print(":"); myFile.print(static_cast<int>(second));
		myFile.print("  Fix age: ");  myFile.print(age); myFile.println("ms.");

		myFile.print("Alt(float): "); myFile.print(gps.f_altitude()); myFile.println();
		myFile.print("Speed(kph): "); myFile.print(gps.f_speed_kmph()); myFile.println();
		myFile.print("Course(float): "); myFile.print(gps.f_course()); myFile.println();

		gps.stats(&chars, &sentences, &failed);
		myFile.print("Stats: characters: "); myFile.print(chars); myFile.print(" sentences: ");
		myFile.print(sentences); myFile.print(" failed checksum: "); myFile.println(failed);
		myFile.println();
		myFile.close();
	}
	else {
		// if the file didn't open, print an error:
		Serial.println("error opening test.txt");
	}
}

//funkcja odpowiedzialna za zaookrąglenia oraz ujemne wartości
void printFloat(double number, int digits)
{
	if (number < 0.0)
	{
		Serial.print('-');
		// myFile.print('-');
		number = -number;
	}

	// zaookreglenie w góre i w dół 0.9 = 1 ; 0.2 = 0
	double rounding = 0.5;
	for (uint8_t i = 0; i<digits; ++i)
		rounding /= 10.0;

	number += rounding;

	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	Serial.print(int_part);
	//myFile.print(int_part);

	if (digits > 0)
		Serial.print(".");

	while (digits-- > 0)
	{
		remainder *= 10.0;
		int toPrint = int(remainder);
		Serial.print(toPrint);
		remainder -= toPrint;
	}
}

void myFileprintFloat(double number, int digits)
{
	// Handle negative numbers
	if (number < 0.0)
	{
		myFile.print('-');
		// myFile.print('-');
		number = -number;
	}

	// zaookreglenie w góre i w dół 0.9 = 1 ; 0.2 = 0
	double rounding = 0.5;
	for (uint8_t i = 0; i<digits; ++i)
		rounding /= 10.0;

	number += rounding;

	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	myFile.print(int_part);
	//myFile.print(int_part);

	if (digits > 0)
		myFile.print(".");

	while (digits-- > 0)
	{
		remainder *= 10.0;
		int toPrint = int(remainder);
		myFile.print(toPrint);
		remainder -= toPrint;
	}
}

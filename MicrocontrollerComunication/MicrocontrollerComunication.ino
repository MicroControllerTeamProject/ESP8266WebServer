/*
 Name:		ExperimentProject3.ino
 Created:	2/3/2019 5:33:21 PM
 Author:	luigi.santagada
*/

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (Serial.available() > 0)
	{
		Serial.println(Serial.readString());
	}
}
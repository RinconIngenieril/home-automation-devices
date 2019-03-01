/******************************************

Design by Enrique Gómez for:
https://github.com/RinconIngenieril/Home-Automation-Devices

AC Relay board will send '1' when the button is pressed
AC Relay board will send '0' when the button is not pressed
******************************************/

void setup() {
  pinMode(D2, INPUT);
  Serial.begin(115200);
  Serial.println("I'm alive");

}

void loop() {
  Serial.println(digitalRead(D2));
  delay(200);
}

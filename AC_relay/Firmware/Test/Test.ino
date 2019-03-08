/******************************************

Design by Enrique Gómez for:
https://github.com/RinconIngenieril/Home-Automation-Devices

AC Relay board will send '1' when the button is pressed
AC Relay board will send '0' when the button is not pressed

The Relay will change its state when the button is pressed, every 400 milliseconds 
******************************************/

void setup() {
  pinMode(D2, INPUT);
  pinMode(D0, OUTPUT);
  Serial.begin(115200);
  Serial.println("I'm alive");

}

void loop() {
  Serial.println(digitalRead(D2));
  delay(200);
  if(digitalRead(D2) == HIGH){
    digitalWrite(D0, !digitalRead(D0));
    delay(200);
  }
}

/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
int counter = 0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  
  for(int j = 20; j > 10; j--){
    for(int i = 0; i < 100; i++){
      digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(j);                       // wait for a second
      digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
      delay(5);                // wait for a second
    }}
}
// the loop function runs over and over again forever
void loop() {
  /*if(counter % 1000){
     digitalWrite(10, !digitalRead(10));
  }
  digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                // wait for a second
  
  counter++;*/
}
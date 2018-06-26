#define interruptPin 3
int IRread = 0;
int lastIRread = 0;

void setup() {
  // Initialize the serial port and display
  Serial.begin(2000000);

  // Enable the pullup resistor and attach the interrupt
  pinMode(interruptPin, INPUT_PULLUP);
}

void waitTime(){
  while (IRread != HIGH && IRread == lastIRread){   
    lastIRread = IRread;  
    delay(10);
    IRread = digitalRead(interruptPin);
  }
  Serial.print("New cycle begin at ");
  Serial.println(millis());
}

void loop() {
  waitTime();
}


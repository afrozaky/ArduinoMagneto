/** LINEAR STEPPER MOTOR ACCELERATION
    Authors       : Zaky Hussein, Priyanka Deo
    Last Modified : June 18th, 2018
    The following program linearly accelerates a stepper motor to bring it to a maximum frequency, runs it at the frequency for a set time,
    then decelerates it back to a minimum frequency. Then it changes the direction of rotation and repeats the sequence a given number of times.
    Please follow the folllowing instructions for running the code:
    1) Hit the upload button
    2) Turn the motor driver ON
    3) Select Tools -> Serial Monitor
    4) Follow instructions on the serial monitor
    Please DO NOT touch the following script, thank you.    
**/

// INPUTS
double strokeLength = 0;                                                            // [mm] stroke length to cover in a half cycle
double totalTime = 0;                                                               // [s] time for one half cycle/full stroke length
double cycles = 0;

// DECLARING CONSTANTS
double peakTime;
const double startFreq = 250;                                                       // [Hz] min value of 62 Hz by precision of delayMicroseconds
double maxFreq;
double finalPeriod;
double basePeriod;
const double df = 135;                                                              // [Hz] frequency increments
unsigned int dt = 2950;                                                             // [us] time increments
const byte DIR = 9;                                                                 // direction pin on Arduino board
const byte CLOCK = 10;                                                              // clock pin on Arduino board
const byte LIMIT = 7;                                                               // limit switch
double nPulses;
double constPulses;
int sensor;
int counter = 0;

// NON CONSTANT VARIABLES
unsigned long initialTime;
double freq = startFreq;
unsigned long stepCount = 0;
unsigned long initialTime2;
double period;

// SETUP
void setup() {
  pinMode(DIR, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(CLOCK, LOW);
  Serial.begin(2000000);
  //  Serial.print(peakTime);
  //  Serial.print(" ");
  //  Serial.print(nPulses);
  //  Serial.print(" ");
  //  Serial.print(maxFreq);
  //  Serial.print(" ");
  sensor = analogRead(A0);
  homeleft();
  digitalWrite(DIR, HIGH);   //LOW -> TOWARDS MOTOR; HIGH -> AWAY FROM MOTOR
}

// HOME TO THE LEFT LIMIT SWITCH
void homeleft() {
  digitalWrite(DIR, LOW);    //LOW  -> TOWARDS MOTOR (LEFT)
  double homingFreq = 750;
  double homingPeriod = 1 / homingFreq * pow(10, 6);
  while (digitalRead(LIMIT) == HIGH) {   //Run at homing frequency until limit hit
    digitalWrite(CLOCK, HIGH);
    delayMicroseconds(homingPeriod / 2.0);
    digitalWrite(CLOCK, LOW);
    delayMicroseconds(homingPeriod / 2.0);
  }
}

void loop() {

  // HANDLING USER INPUT SECTION
  if (totalTime == 0 || strokeLength == 0 || cycles == 0) {
    counter = 0;                                                             // Reset counter
    Serial.println("Enter total time in seconds ");                          // Prompt User for input
    while (Serial.available() == 0) {}
    totalTime = Serial.parseFloat();                                         // Read user input into totalTime

    //    if (totalTime < 0.3) {
    //      totalTime = 0;
    //      Serial.println("Do not enter a total time < 0.3 seconds");
    //    } else {
    Serial.println("Enter stroke length in mm ");                            //Prompt User for input
    while (Serial.available() == 0) {}
    strokeLength = Serial.parseFloat();                                      //Read user input into height
    Serial.println("Enter number of cycles ");                               //Prompt User for input
    while (Serial.available() == 0) {}
    cycles = Serial.parseFloat();                                            //Read user input into height

    if (totalTime != 0 && strokeLength != 0) {
      Serial.print("Total time entered: ");
      Serial.println(totalTime);
      Serial.print("Stroke length entered: ");
      Serial.println(strokeLength);
      Serial.print("Number of cycles entered: ");
      Serial.println(cycles);
      Serial.print("Stroke Frequency: ");
      Serial.println(1.0 / (2.0 * totalTime));

      //Initialize variables from user input
      peakTime = sqrt(pow(totalTime, 2) - 3840 * strokeLength / pow(10, 6)); // [s] time for which max frequency should be maintained
      maxFreq = pow(10, 6) * (totalTime - peakTime) / 48;
      finalPeriod = 1 / (maxFreq) * pow(10, 6);                              // [us] final period corresponding to maximum frequency
      basePeriod = 1 / (startFreq) * pow(10, 6);                             // [us] base period or the period to ramp up from and ramp down to
      nPulses = ((maxFreq - startFreq) / 2.0) * .024 * maxFreq * .001;
      constPulses = maxFreq * peakTime;
      period = basePeriod;

      Serial.print("Max RPM: ");
      Serial.println(maxFreq / 200.0 * 60);
      Serial.println("Beginning motor control sequence in 3 seconds");
      delay(3000);                                                           // Give user time to read out
    }

    // MAIN MOTOR CONTROL SECTION OF LOOP
  } else if (counter < 2 * cycles) {
    stepCount = 0;
    initialTime2 = millis();
    sensor = analogRead(A0);
    //    Serial.println(sensor);

    // RAMP UP TO MAX FREQUENCY
    while (stepCount < nPulses && freq < maxFreq) {
      initialTime = micros();
      while (micros() - initialTime < dt) {
        digitalWrite(CLOCK, HIGH);
        delayMicroseconds(period / 2.0);
        digitalWrite(CLOCK, LOW);
        delayMicroseconds(period / 2.0);
        stepCount++;
        if (stepCount >= nPulses)
          break;
      }
      freq += df;
      period = 1 / freq * pow(10, 6);
    }

    // REMAIN AT MAX FREQUENCY
    stepCount = 0;
    for (int i = 0; i < constPulses; i++) {
      digitalWrite(CLOCK, HIGH);
      delayMicroseconds(period / 2.0);
      digitalWrite(CLOCK, LOW);
      delayMicroseconds(period / 2.0);
      stepCount++;
    }

    // RAMP DOWN FROM MAX FREQUENCY
    stepCount = 0;
    while (freq > startFreq)  {
      initialTime = micros();
      while (micros() - initialTime < dt) {
        digitalWrite(CLOCK, HIGH);
        delayMicroseconds(period / 2.0);
        digitalWrite(CLOCK, LOW);
        delayMicroseconds(period / 2.0);
        stepCount++;
      }
      freq -= df;
      period = 1 / freq * pow(10, 6);
    }

    sensor = analogRead(A0);
    //    Serial.println(millis() - initialTime2);
    //    Serial.println(stepCount);
    //    Serial.println(sensor);
    counter++;                                        // End of one half cycle, increment counter
    delay(0.1 * totalTime * pow(10, 3));              // Add delay between changing directions to match Gauss curve

    // DIRECTION SWITCH
    digitalWrite(DIR, !digitalRead(DIR));
  }
}

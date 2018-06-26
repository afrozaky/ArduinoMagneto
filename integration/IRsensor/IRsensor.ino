//-------------------------------------
// Tachometer
//-------------------------------------

// External interrpt pin for sensor
#define interruptPin 3

unsigned long lastUpdate = 0;  // for timing display updates
volatile long accumulator = 0;  // sum of last 8 revolution times
volatile unsigned long startTime = 0; // start of revolution in microseconds
volatile unsigned int revCount = 0; // number of revolutions since last display update
double fluxRPM = 0;
double fluxFreq = 0;
double oldfluxRPM;
double error = 1;
double eps=0.01;

//-------------------------------------
// Setup - runs once at startup
//-------------------------------------
void setup()
{
  // Initialize the serial port and display
  Serial.begin(2000000);

  // Enable the pullup resistor and attach the interrupt
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), tach_interrupt, FALLING);
}

//-------------------------------------
// Main Loop - runs repeatedly.
// Calculate fluxRPM and Update LCD Display
//-------------------------------------
void loop()
{
  while (error > eps) {
    if (millis() - lastUpdate > 1000) // update every second
    {
      // divide number of microseconds in a minute, by the average interval.
      if (revCount > 0)
      {
        oldfluxRPM = fluxRPM;
        fluxRPM = 60000000.0 / (accumulator >> 1);
        fluxFreq = fluxRPM / 60.0;
        error = abs(oldfluxRPM - fluxRPM) / fluxRPM;
      }          
      lastUpdate = millis();
      revCount = 0;
    }

    if (error <= eps) {
      Serial.print("Caculated fluxFreq: ");
      Serial.println(fluxFreq);
      Serial.print("Calculated fluxRPM: ");
      Serial.println(fluxRPM);
      detachInterrupt(interruptPin);
    }
  }
}

//-------------------------------------
// Interrupt Handler
// IR reflective sensor - target passed
// Calculate revolution time
//-------------------------------------
void tach_interrupt()
{
  // calculate the microseconds since the last interrupt
  long usNow = micros();
  long elapsed = usNow - startTime;
  startTime = usNow;  // reset the clock

  //  // Accumulate the last 8 interrupt intervals
  accumulator -= (accumulator >> 1);
  accumulator += elapsed;
  revCount++;
}


double totalTime = 0;
double strokeLength = 0;
double calculatedMaxFreq;
double peakTime;
double maxFreq;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(2000000);      // turn on Serial Port
}

void loop() {
  // put your main code here, to run repeatedly:
  if (totalTime == 0 || strokeLength == 0) {
    Serial.print("Enter total time in seconds > 0.3: ");      //Prompt User for input
    while (Serial.available() == 0) {}
    totalTime = Serial.parseFloat();              //Read user input into totalTime
    Serial.println(totalTime);

    if (totalTime < 0.3) {
      totalTime = 0;
      Serial.println("Do not enter a total time < 0.3 seconds");
    } else {
      Serial.print("Enter stroke length in mm: ");      //Prompt User for input
      while (Serial.available() == 0) {}
      strokeLength = Serial.parseFloat();              //Read user input into height
      Serial.println(strokeLength);

      peakTime = sqrt(pow(totalTime, 2) - 3840 * strokeLength / pow(10, 6));
      maxFreq = pow(10, 6) * (totalTime - peakTime) / 48;
      Serial.print("Peak time: ");
      Serial.println(peakTime);
      Serial.print("Max frequency: ");
      Serial.println(maxFreq);

      if (maxFreq > 5900) {
        Serial.println("Entered values yield a max frequency > 5900, increase total time or decrease stroke length");
        totalTime = strokeLength = 0;
      } else if (maxFreq < 2000) {
        Serial.println("Entered values yield a max frequency < 2000, decrease total time or increase stroke length");
        totalTime = strokeLength = 0;
      }

      if (totalTime != 0 && strokeLength != 0) {
        Serial.print("Total time entered: ");
        Serial.println(totalTime);
        Serial.print("Stroke length entered: ");
        Serial.println(strokeLength);
      }
    }
  }
}

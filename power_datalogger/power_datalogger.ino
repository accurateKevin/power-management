// Data logger for ARTIK IoT power management tutorial
#include <stdio.h>
#include <string.h>
#include <time.h>


// Set global parameters here.
  char logFile [160] = "/usr/local/etc/logfile"; // In run mode we'll build the log file path and name but use Logfile path and name when we're just playing
  const char logPath [80] = "/usr/tmp/"; // Path to where we want the logfile when we're live
  const int logTest = 1; // Set to 0 to build unique file name for each logging run, otherwise use default
  const char deLimit []="|"; // delimiter in data file
  const int voltPin = A0; // input pin for power supply voltage
  const int ampPin = A1; // input pin for power supply current
  const int trigger = 5; // pull this pin down to start logging
  const int lowDi = 6; // Status input includes all GPIO pins from here (LSB)...  
  const int highDi = 9; // ... to here.
  const long period = 200; // How many ms between samples


  

  
void setup() {
  // setup code here runs once

  // Setup digital inputs

  for (int a = lowDi; a <= highDi; a = a+1){
    pinMode (a, INPUT);
  };
  // Get time stamp

  time_t rawtime;
  struct tm * timeinfo;
  char timeStamp [80];
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (timeStamp,80,"%D %H:%M:%S",timeinfo); // This for printing in the file resolution to the second 24 hr clock 
  char logName [80];
  strftime (logName,80,"%F%H%M%S",timeinfo); // This for a unique log file name 
    

  // Create file
  FILE * pFile;
  if (logTest == 0 ){
    strcpy (logFile, logPath);
    strcat (logFile, logName);
    strcat (logFile, "log");
  }
  pFile = fopen (logFile,"w"); // delete if already exists
 
  if (pFile!=NULL){ // wait for the file to open and write header 
      fprintf (pFile, "%s\n%s\nmilliseconds | volts | amps | digital input\n", logFile, timeStamp);
      fclose (pFile);
  }



}

void loop() {
  // main code here runs repeatedly

  // begin logging when trigger is pulled low. Logging will end when pin goes high and you'll need to restart the program to start again.
//  while (digitalRead(trigger) == LOW){ // this for on-shot
  if (digitalRead(trigger) == LOW){ // this allows start, pause, restart under trigger control
    unsigned long thisMillis = millis(); // milliseconds since this program started 
    unsigned long lastMillis;

        if (thisMillis % period <= 2){ // is it time to take another sample? (can be up to 2 ms late and still get sampled)
          if (thisMillis - lastMillis > 3){ // avoid double-sampling
            getRecord (thisMillis);
            lastMillis = thisMillis;
          }
        }

  }
}

void getRecord(unsigned long thisMillis){
    // read analog sensors
  
    int volts = analogRead(voltPin);
    int amps = analogRead(ampPin);    
        
    // read digital inputs
    int digIns = 0;
    for (int a = highDi; a >= lowDi; a = a-1){
      digIns = digIns << 1;
      if (digitalRead(a) == HIGH){
        digIns = digIns | 1;
        }
      };
  


    
    // write a record
    FILE * pFile;
    pFile = fopen (logFile,"a+"); // append to the previously opened file
    if (pFile!=NULL) // wait for the file to open
      {
        fprintf (pFile, "%lu%s%d%s%d%s%d\n", thisMillis, deLimit, volts, deLimit, amps, deLimit, digIns); 
//        fprintf (pFile, "%lu\n", thisMillis); 

        fclose (pFile);
      }
}


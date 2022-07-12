// Include libraries for interfacing with the HX711 and the servo
#include "HX711.h"
#include <Servo.h>

// Configuration
#define calibration_factor -7050.0  // This value is obtained using the    HX711_Calibration sketch
int trialRuns = 5;    // This defines the number of times to measure the servo's force on the
                      // load cell. These measurments will be averaged to come up with a single reading.
float armLength = 2.75;   // This is the distance (in cm) between the servo's center of rotation and the load cell.
                          // This is based on the mechanical design of the test fixture.            

// Define connections between the HX711 and the Arduino
#define DT   3  // The HX711 DT pin connects to D3 on the Arduino
#define SCK  2  // The HX711 SCK pin connects to D2 on the Arduino

// Initialize the HX711
HX711 loadCell;

// Create a servo object
Servo testServo;

void setup() {
  loadCell.begin(DT,SCK);

  // Begin Serial communication
  Serial.begin(9600);
  Serial.println(" - Torque Measurement Tool - ");  // Print a heading
  Serial.println();

  // Set the pin used to control the servo
  testServo.attach(9); 

  loadCell.set_scale(calibration_factor); // This value is obtained by using the HX711_Calibration sketch
  loadCell.tare(); // Reset the scale to zero to compensate for any existing load

  // To begin the test, the servo horn should be attached so that it is making contact with
  // the load cell at an angle of 140 degrees on the servo
  testServo.write(140); // Move the servo into the load cell

  Serial.println("Initialization complete."); 
  Serial.println("Send 's' to begin testing. Send 'h' for help.");
  Serial.println();
}

void loop() {
  // If the user sends 's' over Serial, begin testing the torque
  if(Serial.read() == 's' || Serial.read() == 'S') {
    measureTorque();  
  }

  // If the user sends 'i' over Serial, show some instructions
  if(Serial.read() == 'h' || Serial.read() == 'h') {
    Serial.println("Right now, the Arduino has moved the servo into its starting position.");
    Serial.println("In the servo's current position, install the horn so that it is just touching the load cell.");
    Serial.println("When you are ready, send 's' over Serial and the Arduino will begin testing the servo's torque.");
    Serial.println("You will see the Arduino move the servo into the load cell five different times.");
    Serial.println("Each time the servo hits the load cell, the Arduino will take a reading.");
    Serial.println("Those readings will be averaged to calculate the torque delivered by the servo.");
    Serial.println("Keep an eye on the Serial monitor to see the results.");
    Serial.println();
    Serial.println("Send 's' to begin testing.");
    Serial.println();
  }
}

void measureTorque() {
   /*
      To test the servo's torque, the Arduino will move the servo arm so that it presses on the load
      cell. The resulting force will produce a reading from the load cell. The Arduino will take 
      five readings to compute an average force value. Because the distance between the servo's 
      center of rotation and the load cell is known from the frame design, the Arduino
      can calculate the torque produced by the servo.
    */
    Serial.println("Individual Readings: ");
  
    float individualReadings[trialRuns];  // This array will store the load cell readings for the five tests
  
    for(int i = 0; i < 5; i++) {
      testServo.write(180); // Move the servo away from the load cell
      delay(1000);   // Wait for the servo to move
      loadCell.tare(); // Reset the scale to zero to compensate for any existing load
      testServo.write(130); // Move the servo into the load cell. A 130 degree angle is actually inside the load
                            // cell, so the servo will be pushing towards that position, exerting force on 
                            // the load cell.   
      delay(1000);   // Wait for the servo to move   
      individualReadings[i] = loadCell.get_units(); // Take a measurment from the load cell
      Serial.print(individualReadings[i]);   // Print the measurment over Serial
      Serial.print("   ");
    }
  
    // Now that we have five individual readings, average them to get one average load reading
    float readingsSum = 0;  // Create a variable to store the sum of all readings
    // Loop through the array and add together all the readings
    for(int y = 0; y < trialRuns; y++) {
      readingsSum = readingsSum + individualReadings[y];  
    }
  
    float averageReading = readingsSum / trialRuns; // Divide by the numer of readings to get the average

    Serial.println();
    Serial.println();
    Serial.println("Average Reading:");   // Print the average reading over Serial
    Serial.println(averageReading);
  
    // From the average reading, calculate the torque delivered by the servo
    // using the formula T = F * r where T is the torque, F is the load cell
    // reading (a force), and r is the radius of rotation (the distance between
    // the servo and the load cell).
    // The units for the torque will be kg*cm
    float servoTorque = averageReading * armLength;   // Calculate the torque

    Serial.println();
    Serial.println("Torque:");    // Print the torque
    Serial.print(servoTorque);
    Serial.println(" kgcm");

    testServo.write(180); // Move the servo away from the load cell after the testing is complete
}
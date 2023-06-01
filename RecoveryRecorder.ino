//Recovery Recorder source code

//LiquidCrystal library
#include <LiquidCrystal.h> 

//Analog pins
const int buzzerPin = A0; // Buzzer pin
const int motionPin = A2; // Motion sensor pin

//Digital pins - Button, LED and slide switch
const int btnRecPin = 2; // Record button pin
const int btnAccPin = 3; // Accept button pin
const int btnDisPin = 4; // Discard button pin
const int ledPin = 5; // LED pin
const int switchPin = 6; // Slide switch pin

// LCD pins
const int lcdLed = 7, rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
// Initialize LCD with its connected pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long previousMillis = 0; // Variable to keep track of time
int seconds = 0; // Seconds counter
int minutes = 0; // Minutes counter

String textTimer = ""; // String to store timer text

bool recording = false; // Boolean to check if recording is active


// Setup
void setup() {
  // Setting pin modes
  pinMode(switchPin, INPUT);
  pinMode(motionPin, INPUT);
  pinMode(btnRecPin, INPUT_PULLUP);
  pinMode(btnAccPin, INPUT_PULLUP);
  pinMode(btnDisPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(lcdLed, OUTPUT);
  pinMode(rs, OUTPUT);
  pinMode(en, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);


  // Begin LCD and display start screen
  lcd.begin(16,2);
  displayStart();
  digitalWrite(lcdLed, HIGH);

  // Start the Serial Monitor for debugging
  Serial.begin(9600); 
}


// Loop function
void loop() {
  
  // Reading the switch value
  int switchValue = digitalRead(switchPin);

  // If the switch is OFF disable everything and return
  if(switchValue == LOW){
    digitalWrite(lcdLed, LOW);
    digitalWrite(ledPin, LOW);
    
    return;
  }
  else {
    // If the switch is ON turn on backlight and LED
    digitalWrite(lcdLed, HIGH);
    analogWrite(ledPin, 10);
  }

  // Read motion value and button states
  int motionValue = analogRead(motionPin);
  int btnRecState = digitalRead(btnRecPin);
  int btnAccState = digitalRead(btnAccPin);
  int btnDisState = digitalRead(btnDisPin);

  // For testing the motion sensor
  //Serial.println(motionValue);

  // Checking for motion with the motion sensor
  checkForMotion(motionValue);

  // Checking if the record button has been pressed
  checkRecButton(btnRecState);
  
  // Checking if the accept button has been pressed
  checkAccButton(btnAccState);
  
  // Checking if the discard button has been pressed
  checkDisButton(btnDisState);
}


// This function is used to display the starting screen of the LCD
void displayStart() {
  lcd.clear();
  lcd.print("    Recovery");
  lcd.setCursor(0,1);
  lcd.print("    Recorder");
}


// This function checks if motion has been detected. If it exceeds 100 it triggers
void checkForMotion(int motionValue) {
  if (motionValue > 100) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Record your");
    lcd.setCursor(0,1);
    lcd.print("  daily diary!");

    // Flash the LED and buzz the buzzer for three times
    for (int i = 0; i < 3; i++) {
      analogWrite(buzzerPin, 0);
      analogWrite(ledPin, 10);
      delay(100);
      analogWrite(buzzerPin, 255);
      analogWrite(ledPin, 255);
      delay(250);
    }

    analogWrite(buzzerPin, 0); // Turn off the buzzer
    analogWrite(ledPin, 10); // Set the LED to low brightness again

    delay(3000); // Wait for 3 seconds

    displayStart(); // Go back to the start screen

  } else {
    analogWrite(buzzerPin, 0);
  }
}


// This function checks the state of the record button, and if pressed starts "recording"
void checkRecButton(int btnRecState) {
  if (btnRecState == LOW && recording == false) {
    recording = true;
    seconds = 0;
    minutes = 0;

    digitalWrite(ledPin, HIGH); //Turn on led if recording

    lcd.clear();
    lcd.print("Recording audio");

    // Continue recording until stopped
    while (recording == true) {
      // Check the state of the accept and discard buttons
      int btnAccState = digitalRead(btnAccPin);
      int btnDisState = digitalRead(btnDisPin);

      // If the accept button is pressed, stop recording and "send the audio"
      if (btnAccState == LOW) {
        lcd.clear();
        lcd.print(" Sending audio");
        lcd.setCursor(0,1);
        lcd.print("     diary");
		
        // Flash LED 5 times when sending recording
        for (int i = 0; i < 5; i++) {
        analogWrite(ledPin, 255);
        delay(400);
        analogWrite(ledPin, 10);
        delay(400);
        }
      
        recording = false;  // Set recording bool back to false
        displayStart(); // Displays the start screen
        
        break;
      }

      // If the discard button is pressed, stop recording and "discard the audio"
      if (btnDisState == LOW) {

        lcd.clear();
        lcd.print("    Deleting");
        lcd.setCursor(0,1);
        lcd.print("    recording");

        delay(3000); // Wait 3 seconds

        recording = false; // Set recording bool back to false
        displayStart(); // Displays the start screen
        
        break;
      }
      
      // Print the recording timer on the LCD
      textTimer = minutesAndSeconds();
      lcd.setCursor(0, 1);
      lcd.print(textTimer);
    }
  }
}


// This function checks if the accept button is pressed when not recording, and provides feedback
void checkAccButton(int btnAccState){
  if (btnAccState == LOW && recording == false) {

    lcd.clear();
    lcd.print("   Nothing to");
    lcd.setCursor(0, 1);
    lcd.print("     accept");

    delay(3000);

    displayStart(); // Displays the start screen
  }
}


// This function checks if the discard button is pressed when not recording, and provides feedback
void checkDisButton(int btnDisState) {
  if (btnDisState == LOW && recording == false) {

    lcd.clear();
    lcd.print("   Nothing to");
    lcd.setCursor(0, 1);
    lcd.print("     delete");

    delay(3000); // Wait for 3 seconds
    displayStart(); // Displays the start screen
  }
}


// This function calculates the minutes and seconds elapsed since the start of recording
String minutesAndSeconds() {
  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // If one second has passed
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis; // Updates the previousMillis
    seconds++; // Increase the seconds counter

    // If 60 seconds have passed, increase the minutes counter and reset the seconds counter
    if (seconds == 60) {
      seconds = 0;
      minutes++;
    }

    // If 60 minutes have passed, reset the minutes counter
    if (minutes == 60) {
      minutes = 0;
    }

    // Format the minutes and seconds as "MM:SS"
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", minutes, seconds);

    return "     " + String(timeStr); // Return the formatted time string

  }

  // Return an empty string if one second has not passed yet
  return String("");
}


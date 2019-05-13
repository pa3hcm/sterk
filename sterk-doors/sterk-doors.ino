/*
 Project: sterk-doors
 Author:  Ernest Neijenhuis <http://pa3hcm.nl>
*/

// Pin labels
const int P_OVERTURE = 6;
const int P_STOP = 7;
const int P_PIR = 8;
const int P_WINCH_OPEN = 9;
const int P_WINCH_CLOSE = 10;
const int P_WINCH_BRAKE = 11;

void setup() {
  pinMode(P_OVERTURE, OUTPUT);
  pinMode(P_STOP, INPUT_PULLUP);
  pinMode(P_PIR, INPUT);
  pinMode(P_WINCH_OPEN, OUTPUT);
  pinMode(P_WINCH_CLOSE, OUTPUT);
  pinMode(P_WINCH_BRAKE, OUTPUT);
  Serial.begin(9600);
}

int state = 1; // 1=init, 2=waitForOpen, 3=open, 4=waitForClose, 5=close
boolean doorsAreOpen = 0;

void loop() {
  delay(100);  
  switch(state) {

    // Initialize
    case 1:
      Serial.println("Hello 8-)");
      digitalWrite(P_OVERTURE, LOW);
      digitalWrite(P_WINCH_BRAKE, LOW);
      Serial.println("Doors closing!");
      doors_close();
      doorsAreOpen = 0;
      Serial.println("Waiting for something to move...");
      state = 2;
      break;
      
    // Wait for movement...
    case 2:
      if(digitalRead(P_PIR) == HIGH) {
        state = 3;
      }
      break;

    // Open the doors (if not open yet)
    case 3:
      if(doorsAreOpen == 0) {
        // Send a activation pulse to the music player
        Serial.println("Yep... I saw something, let's open the doors!");
        digitalWrite(P_OVERTURE, HIGH);
        delay(100);
        digitalWrite(P_OVERTURE, LOW);
        // Open the doors
        doors_open();
        doorsAreOpen = 1;
        Serial.println("Entertainment in progress...");
        state = 4;
      }
      break;

    // While movement is detected, keep the doors open
    case 4:
      delay(10000);
      state = 5;
      break;

    // Close the doors (if open)
    case 5:
      if(doorsAreOpen == 1) {
        Serial.println("End of the show, doors are closing...");
        doors_close();
        doorsAreOpen = 0;
        Serial.println("Waiting for something to move...");
        state = 2;
      }
      break;

    // Invalid state catch
    default:
      Serial.println("OOPS...!?");
      state = 1;
      break;
  } // end of switch()

}

void doors_close() {
  // Ensure the winch is not running
  digitalWrite(P_WINCH_OPEN, LOW);
  digitalWrite(P_WINCH_CLOSE, LOW);
  // Only start the winch when door switch is not pressed
  if(digitalRead(P_STOP) == HIGH) {
    // Run the winch until the door switch is pressed
    while(digitalRead(P_STOP) == HIGH) {
      digitalWrite(P_WINCH_BRAKE, HIGH);
      digitalWrite(P_WINCH_CLOSE, HIGH);
      delay(10);
    }
  }
  // Stop the winch
  digitalWrite(P_WINCH_CLOSE, LOW);
  digitalWrite(P_WINCH_BRAKE, LOW);
}

void doors_open() {
  // Ensure the doors are fully closed
  //doors_close();
  // Ensure the winch is not running
  digitalWrite(P_WINCH_CLOSE, LOW);
  digitalWrite(P_WINCH_BRAKE, HIGH);
  digitalWrite(P_WINCH_OPEN, HIGH);
  delay(1000);
  // Switch should not be pressed after a second
  if(digitalRead(P_STOP) == HIGH) {
    // Run the winch for a while, until the doors are opened far enough
    delay(8000);
  }
  // Stop the winch
  digitalWrite(P_WINCH_OPEN, LOW);
  digitalWrite(P_WINCH_BRAKE, LOW);
}

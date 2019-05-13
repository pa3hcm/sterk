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

void setup() {
  pinMode(P_OVERTURE, OUTPUT);
  pinMode(P_STOP, INPUT_PULLUP);
  pinMode(P_PIR, INPUT);
  pinMode(P_WINCH_OPEN, OUTPUT);
  pinMode(P_WINCH_CLOSE, OUTPUT);
  Serial.begin(9600);
}

long openTime = 1000; // Time to keep the doors open (100 = 1 second)
int state = 1; // 1=init, 2=wait, 3=open, 4=close
boolean doorsAreOpen = 0;
long pirCount = 0;

void loop() {
  delay(100);  
  switch(state) {

    // Initialize
    case 1:
      digitalWrite(P_OVERTURE, LOW);
      Serial.println("Close the doors");
      doors_close();
      doorsAreOpen = 0;
      state = 2;
      break;

    // Handle PIR activity, else wait
    case 2:
      if(digitalRead(P_PIR) == HIGH) {
        pirCount = openTime;
        state = 3;
        delay(10);
      } else {
        if(pirCount > 1) {
          pirCount--;
          delay(10);
        } else {
          state = 4;
        }
      }
      break;

    // Open the doors (if not open yet)
    case 3:
      if(doorsAreOpen == 0) {
        // Send a activation pulse to the music player
        Serial.println("Start playing an overture");
        digitalWrite(P_OVERTURE, HIGH);
        delay(10);
        digitalWrite(P_OVERTURE, LOW);
        // Open the doors
        Serial.println("Open the doors");
        doors_open();
        doorsAreOpen = 1;
        state = 2;
      } else {
        Serial.println("Doors are already open");
      }
      break;

    // Close the doors (if open)
    case 4:
      if(doorsAreOpen == 1) {
        Serial.println("Close the doors");
        doors_close();
        doorsAreOpen = 0;
        state = 2;
      } else {
        Serial.println("Doors are already closed");
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
      digitalWrite(P_WINCH_CLOSE, HIGH);
      delay(10);
    }
  }
  // Stop the winch
  digitalWrite(P_WINCH_CLOSE, LOW);
}

void doors_open() {
  // Ensure the doors are fully closed
  doors_close();
  // Ensure the winch is not running
  digitalWrite(P_WINCH_CLOSE, LOW);
  digitalWrite(P_WINCH_OPEN, HIGH);
  delay(1000);
  // Switch should not be pressed after a second
  if(digitalRead(P_STOP) == HIGH) {
    // Run the winch for a while, until the doors are opened far enough
    delay(5000);
  }
  // Stop the winch
  digitalWrite(P_WINCH_OPEN, LOW);
}

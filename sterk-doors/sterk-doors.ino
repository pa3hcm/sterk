/*
 Project: sterk-doors
 Source:  http://github.com/pa3hcm/sterk
 Author:  Ernest Neijenhuis <http://pa3hcm.nl>
*/


/////////////////////////////////////////////////////////////////////////
// Constants

// Pin labels
const int P_OVERTURE    =  6; // Pin to trigger remote music player
const int P_STOP        =  7; // Switch pushed when doors fully closed
const int P_PIR         =  8; // PIR sensor for motion detection
const int P_WINCH_OPEN  =  9; // Drive winch to open the doors
const int P_WINCH_CLOSE = 10; // Drive winch to close the doors
const int P_WINCH_BRAKE = 11; // Activate door brake

// Pir timeout, number of loops (main loop) before closing the doors
// while there's nobody around anymore
const long PIR_TIMEOUT = 1000;



/////////////////////////////////////////////////////////////////////////
// setup routine

void setup() {
  pinMode(P_OVERTURE,    OUTPUT      );
  pinMode(P_STOP,        INPUT_PULLUP);
  pinMode(P_PIR,         INPUT       );
  pinMode(P_WINCH_OPEN,  OUTPUT      );
  pinMode(P_WINCH_CLOSE, OUTPUT      );
  pinMode(P_WINCH_BRAKE, OUTPUT      );
  Serial.begin(9600);
}



/////////////////////////////////////////////////////////////////////////
// Initialisation of variables

// State, used in main loop
// 1=init, 2=waitForOpen, 3=open, 4=waitForClose, 5=close
int state = 1;

// Stores door state, 0=open, 1=closed
bool doorsAreOpen = 0;

// Countdown counter, used when doors are open while nobody is around
// anymore
int pirTimeout = PIR_TIMEOUT;

// While closed, the doors try to open due to continuous pressure of the
// springs. So sometimes we have to pull the strings again. This counter
// relaxes this process.
int leaking = 0;
const int LEAKING_LIMIT = 200;


/////////////////////////////////////////////////////////////////////////
// Main loop

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
      if(digitalRead(P_STOP) == HIGH) {
        leaking++;
      }
      if(leaking > LEAKING_LIMIT) {
        Serial.println("Oops... doors about to open spontaneously.");
        doors_close();
        leaking = 0;
      }
      break;

    // Open the doors (if not open yet)
    case 3:
      if(doorsAreOpen == 0) {
        // Send a activation pulse to the music player
        Serial.println("Yep... I saw something, let's open the doors!");
        // Open the doors
        doors_open();
        doorsAreOpen = 1;
        digitalWrite(P_OVERTURE, HIGH);
        delay(100);
        digitalWrite(P_OVERTURE, LOW);
        Serial.println("Entertainment in progress...");
        pirTimeout = PIR_TIMEOUT;
        state = 4;
      }
      break;

    // While movement is detected, keep the doors open
    case 4:
      if(digitalRead(P_PIR) == HIGH) {
        pirTimeout = PIR_TIMEOUT;
        Serial.print(".");
      } else {
        pirTimeout--;
      }
      if(pirTimeout == (PIR_TIMEOUT/2)) {
        digitalWrite(P_OVERTURE, HIGH);
        delay(100);
        digitalWrite(P_OVERTURE, LOW);
      }
      if(pirTimeout <= 0) {
        state = 5;
      }
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



/////////////////////////////////////////////////////////////////////////
// doors_close() function
// Closes the doors, unless they are closed already

void doors_close() {
  int close_timeout = 2000;
  // Ensure the winch is not running
  digitalWrite(P_WINCH_OPEN, LOW);
  digitalWrite(P_WINCH_CLOSE, LOW);
  // Only start the winch when door switch is not pressed
  if(digitalRead(P_STOP) == HIGH) {
    // Run the winch until the door switch is pressed
    while((digitalRead(P_STOP) == HIGH) and (close_timeout > 0)) {
      digitalWrite(P_WINCH_BRAKE, HIGH);
      digitalWrite(P_WINCH_CLOSE, HIGH);
      delay(10);
      close_timeout--;
    }
  }
  // Stop the winch
  digitalWrite(P_WINCH_CLOSE, LOW);
  digitalWrite(P_WINCH_BRAKE, LOW);
  // In case of failure...
  if(close_timeout <= 0) {
  }
}



/////////////////////////////////////////////////////////////////////////
// doors_open() function
// Open the doors

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
    delay(8500);
  }
  // Stop the winch
  digitalWrite(P_WINCH_OPEN, LOW);
  digitalWrite(P_WINCH_BRAKE, LOW);
}

// the end

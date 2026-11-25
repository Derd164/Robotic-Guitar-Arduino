#include <Servo.h>

// Timing
const int capo = 0;
const int BPM = 160;
const int delay2 = 60000 / BPM;
const int SERVO_MOVE_TIME = 500;

// Servo objects
Servo serv1, serv2, serv3, serv4, serv5, serv6;

// Servo directions/states
bool Estate = 0;
bool Astate = 0;
bool Dstate = 0;
bool Gstate = 0;
bool Bstate = 0;
bool estate = 0;

// Temporary fret positions
const int fretPos[20] = {
  0, 175, 350, 500, 650,
  780, 920, 1050, 1170, 1280,
  1380, 1480, 1580, 14, 15,
  16, 17, 18, 19, 20
};

// Current stepper positions
int Epos = 0;
int Apos = 0;
int Dpos = 0;
int Gpos = 0;
int Bpos = 0;
int epos = 0;

// Stepper distances
int Estep = 0;
int Astep = 0;
int Dstep = 0;
int Gstep = 0;
int Bstep = 0;
int estep = 0;

unsigned long fretElapsedTime = 0;

// --------------------------------------------------
// Initializing delay to avoid code upload chaos
// --------------------------------------------------
bool initDelayB = 1;
void initDelay(void)
{
  if (initDelayB) delay(15000);
  initDelayB = 0;
}

// --------------------------------------------------
// Move all stepper motors to requested fret positions
// --------------------------------------------------
void fret(int E, int A, int D, int G, int B, int e) {
  unsigned long fretStartTime = micros();

  E += capo;
  A += capo;
  D += capo;
  G += capo;
  B += capo;
  e += capo;

  Estep = fretPos[E] - Epos;
  Astep = fretPos[A] - Apos;
  Dstep = fretPos[D] - Dpos;
  Gstep = fretPos[G] - Gpos;
  Bstep = fretPos[B] - Bpos;
  estep = fretPos[e] - epos;

  // Set stepper directions
  digitalWrite(2, Estep < 0 ? HIGH : LOW);
  digitalWrite(4, Astep < 0 ? HIGH : LOW);
  digitalWrite(6, Dstep < 0 ? HIGH : LOW);
  digitalWrite(8, Gstep < 0 ? HIGH : LOW);
  digitalWrite(10, Bstep < 0 ? HIGH : LOW);
  digitalWrite(12, estep < 0 ? LOW : HIGH);

  // Detach servos before reusing their pins for steppers
  serv1.detach();
  serv2.detach();
  serv3.detach();
  serv4.detach();
  serv5.detach();
  serv6.detach();

  // Enable steppers and disable servo operation
  digitalWrite(A0, LOW);

  bool Edone = false;
  bool Adone = false;
  bool Ddone = false;
  bool Gdone = false;
  bool Bdone = false;
  bool edone = false;

  int Esteps = 0;
  int Asteps = 0;
  int Dsteps = 0;
  int Gsteps = 0;
  int Bsteps = 0;
  int esteps = 0;

  while (!(Edone && Adone && Ddone && Gdone && Bdone && edone)) {
    if (Esteps < abs(Estep)) {
      digitalWrite(3, HIGH);
      Esteps++;
    } else {
      Edone = true;
    }

    if (Asteps < abs(Astep)) {
      digitalWrite(5, HIGH);
      Asteps++;
    } else {
      Adone = true;
    }

    if (Dsteps < abs(Dstep)) {
      digitalWrite(7, HIGH);
      Dsteps++;
    } else {
      Ddone = true;
    }

    if (Gsteps < abs(Gstep)) {
      digitalWrite(9, HIGH);
      Gsteps++;
    } else {
      Gdone = true;
    }

    if (Bsteps < abs(Bstep)) {
      digitalWrite(11, HIGH);
      Bsteps++;
    } else {
      Bdone = true;
    }

    if (esteps < abs(estep)) {
      digitalWrite(13, HIGH);
      esteps++;
    } else {
      edone = true;
    }

    delayMicroseconds(1000);

    digitalWrite(3, LOW);
    digitalWrite(5, LOW);
    digitalWrite(7, LOW);
    digitalWrite(9, LOW);
    digitalWrite(11, LOW);
    digitalWrite(13, LOW);

    delayMicroseconds(1000);
  }

  // Save the new positions
  Epos = fretPos[E];
  Apos = fretPos[A];
  Dpos = fretPos[D];
  Gpos = fretPos[G];
  Bpos = fretPos[B];
  epos = fretPos[e];

  fretElapsedTime = micros() - fretStartTime;

  // Disable steppers after movement is complete
  digitalWrite(A0, HIGH);
}

// --------------------------------------------------
// Pick selected strings with servos
// --------------------------------------------------
void pick(bool E, bool A, bool D, bool G, bool B, bool e, int noteDivision) {
  unsigned long noteDuration =
    (delay2 * 4000UL) / noteDivision;

  if (fretElapsedTime < noteDuration) {
    delay((noteDuration - fretElapsedTime) / 1000UL);
  }

  // Disable steppers BEFORE generating servo pulses
  digitalWrite(A0, HIGH);
  delayMicroseconds(20);

  if (E) {
    serv1.attach(3);
    Estate = !Estate;
    serv1.write(Estate ? 140 : 70);
  }

  if (A) {
    serv2.attach(5);
    Astate = !Astate;
    serv2.write(Astate ? 140 : 70);
  }

  if (D) {
    serv3.attach(7);
    Dstate = !Dstate;
    serv3.write(Dstate ? 140 : 70);
  }

  if (G) {
    serv4.attach(9);
    Gstate = !Gstate;
    serv4.write(Gstate ? 140 : 70);
  }

  if (B) {
    serv5.attach(11);
    Bstate = !Bstate;
    serv5.write(Bstate ? 120 : 50);
  }

  if (e) {
    serv6.attach(13);
    estate = !estate;
    serv6.write(estate ? 140 : 70);
  }

  // Let the servos finish moving
  delay(SERVO_MOVE_TIME);

  // Stop servo pulses to reduce buzzing
  serv1.detach();
  serv2.detach();
  serv3.detach();
  serv4.detach();
  serv5.detach();
  serv6.detach();

  // Ensure shared STEP pins are LOW after detaching
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  digitalWrite(7, LOW);
  digitalWrite(9, LOW);
  digitalWrite(11, LOW);
  digitalWrite(13, LOW);

  // Keep steppers disabled during servo operation
  digitalWrite(A0, HIGH);
}

// --------------------------------------------------
// Setup
// --------------------------------------------------
void setup() {
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);

  for (int pin = 2; pin <= 13; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

// --------------------------------------------------
// Main loop
// --------------------------------------------------
void loop() {
  initDelay(); // Don't touch this line
  // "Smoke on the Water" demo song
  fret(0, 0, 0, 0, 0, 4);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 7);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 9);
  pick(0, 0, 0, 0, 0, 1, 2);
  
  fret(0, 0, 0, 0, 0, 4);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 7);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 10);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 9);
  pick(0, 0, 0, 0, 0, 1, 2);

  fret(0, 0, 0, 0, 0, 4);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 7);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 9);
  pick(0, 0, 0, 0, 0, 1, 2);

  fret(0, 0, 0, 0, 0, 4);
  pick(0, 0, 0, 0, 0, 1, 4);
  fret(0, 0, 0, 0, 0, 7);
  pick(0, 0, 0, 0, 0, 1, 4);
  // pick(1, 1, 1, 1, 1, 1, 4);
  // pick(1, 1, 1, 1, 1, 1, 4);
  // pick(1, 0, 0, 0, 0, 0, 4);
  // delay(500);
  // pick(1, 0, 0, 0, 0, 0, 4);
  // delay(500);
  // pick(0, 0, 0, 0, 0, 1, 4);
  // delay(500);
  // pick(0, 0, 0, 0, 0, 1, 4);
  // delay(500);
  // fret(5, 5, 5, 5, 5, 5);
  // fret(1, 1, 1, 1, 1, 1);
  // fret(0, 0, 0, 0, 0, 0);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 1);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 2);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 3);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 4);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 5);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 6);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 7);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 8);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 9);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 10);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 11);
  // delay(2000);
  // fret(0, 0, 0, 0, 0, 12);
  // delay(2000);
}
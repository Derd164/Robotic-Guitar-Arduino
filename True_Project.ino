#include <Servo.h>   // Include servo library
const int capo = 0; // Change depending on where capo is for song.
const int BPM = 160; // Change depending on beats per minute of song
const int delay2 = 60000/BPM;

Servo serv1, serv2, serv3, serv4, serv5, serv6; // declaring servos
bool Estate = 0, Astate = 0, Dstate = 0, Gstate = 0, Bstate = 0, estate = 0; // control pick direction
const int fretPos[21] = {0 /*open*/, 1 /*first fret etc*/, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // Temporary fret location values to be callibrated later
int Epos = 0, Apos = 0, Dpos = 0, Gpos = 0, Bpos = 0, epos = 0; // where the stepper motor is currently located
int Estep = 0, Astep = 0, Dstep = 0, Gstep = 0, Bstep = 0, estep = 0; // How far the stepper motor needs to move from current position

void fret(int E, int A, int D, int G, int B, int e) {
  E += capo; A += capo; D += capo; G += capo; B += capo; e += capo; // desired fret + capo
  Estep = fretPos[E] - Epos; // step size & new position = desired position - current position
  digitalWrite(2, Estep < 0 ? 1 : 0); // change direction depending on if too high or too low. Might need to swap direction bools.
  Astep = fretPos[A] - Apos;
  digitalWrite(4, Astep < 0 ? 1 : 0);
  Dstep = fretPos[D] - Dpos;
  digitalWrite(6, Dstep < 0 ? 1 : 0);
  Gstep = fretPos[G] - Gpos;
  digitalWrite(8, Gstep < 0 ? 1 : 0);
  Bstep = fretPos[B] - Bpos;
  digitalWrite(10, Bstep < 0 ? 1 : 0);
  estep = fretPos[e] - epos;
  digitalWrite(12, estep < 0 ? 1 : 0);

  bool Edone = 0, Adone = 0, Ddone = 0, Gdone = 0, Bdone = 0, edone = 0;
  int Esteps = 0, Asteps = 0, Dsteps = 0, Gsteps = 0, Bsteps = 0, esteps = 0;
  serv1.detach(); serv2.detach(); serv3.detach(); serv4.detach(); serv5.detach(); serv6.detach(); // detach all servo motors
  digitalWrite(A0, LOW); // enable steppers, disable servos
  while (!(Edone && Adone && Ddone && Gdone && Bdone && edone))
  {
    if (Esteps < abs(Estep)) {
      digitalWrite(3, HIGH);
      Esteps++;
    }
    else Edone = 1;
    if (Asteps < abs(Astep)) {
      digitalWrite(5, HIGH);
      Asteps++;
    }
    else Adone = 1;
    if (Dsteps < abs(Dstep)) {
      digitalWrite(7, HIGH);
      Dsteps++;
    }
    else Ddone = 1;
    if (Gsteps < abs(Gstep)) {
      digitalWrite(9, HIGH);
      Gsteps++;
    }
    else Gdone = 1;
    if (Bsteps < abs(Bstep)) {
      digitalWrite(11, HIGH);
      Bsteps++;
    }
    else Bdone = 1;
    if (esteps < abs(estep)) {
      digitalWrite(13, HIGH);
      esteps++;
    }
    else edone = 1;
    delayMicroseconds(50); // Tune to lowest possible once motor is acquired
    digitalWrite(3, LOW);
    digitalWrite(5, LOW);
    digitalWrite(7, LOW);
    digitalWrite(9, LOW);
    digitalWrite(11, LOW);
    digitalWrite(13, LOW);
    delayMicroseconds(50); // Tune to lowest possible once motor is acquired
  }
  Epos = fretPos[E];
  Apos = fretPos[A];
  Dpos = fretPos[D];
  Gpos = fretPos[G];
  Bpos = fretPos[B];
  epos = fretPos[e];
}

void pick(bool E, bool A, bool D, bool G, bool B, bool e) {
  
  digitalWrite(A0, HIGH); // enable servos, disable steppers
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
}

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, OUTPUT);
  for (int pin = 2; pin <= 13; pin++) { // set all digital pins to output
  pinMode(pin, OUTPUT);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  // pick(1, 1, 1, 1, 1, 1); // Pick the string
  fret(5, 5, 5, 5, 5, 5); // Move to next position
  delay(delay2); // Wait 1 second
  fret(0, 0, 0, 0, 0, 0);
  delay(delay2); // Wait 1 second
}

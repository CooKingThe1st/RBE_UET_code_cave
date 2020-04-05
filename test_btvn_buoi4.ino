#define LIMIT_DISTANCE 20 //cm
#define LIMIT_BLOCK_TIME 2000 // ms
#define TIME_QUARTER_TURN 750 // ms

// srf05 pin
#define TRIG 13
#define ECHO 12

// motor pin
#define MTOR_Fp 7 // front wheel, for direction 
#define MTOR_Fn 6 // DEFAULT: CW
#define MTOR_Bp 4 // back wheel, for acceleration
#define MTOR_Bn 3 // DEFAULT: POS

void setup() {
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(MTOR_Fp, OUTPUT);
  pinMode(MTOR_Fn, OUTPUT);
  pinMode(MTOR_Bp, OUTPUT);
  pinMode(MTOR_Bn, OUTPUT);

  pinMode(ECHO, INPUT);
}

void loop() {
  static int srf05_block_count = 0;
  static signed int srf05_last_block = 0;

  // get Distance
  int srf05_distance = srf05_getDistance();
  delay(200);

  if (srf05_distance <= LIMIT_DISTANCE)
  {
    srf05_block_count++;
    srf05_last_block = millis();

    // run backward a lil bit then stop the back wheel
    digitalWrite(MTOR_Bp, LOW);
    digitalWrite(MTOR_Bn, HIGH);
    delay(750);
    digitalWrite(MTOR_Bn, LOW);

    if (srf05_block_count == 1)
      srf05_solve_first();
    else if (srf05_block_count == 2)
      srf05_solve_second();
    else if (srf05_block_count == 3)
      srf05_solve_third(); // may stuck forever here

  }
  else if (millis() - srf05_last_block > LIMIT_BLOCK_TIME) // reset block count
    srf05_block_count = 0;

  // always running back wheel
  digitalWrite(MTOR_Bp, HIGH);
  digitalWrite(MTOR_Bn, LOW);
}

int srf05_getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(50);
  digitalWrite(TRIG, LOW);

  long responseTime = pulseIn(ECHO, HIGH, 5000);

  Serial.println(responseTime);
  
  int distance = (float)(responseTime / 2) * ( 340.0 * 100 / 1000000);

  Serial.print("Distance now: ");
  Serial.println(distance);
  return distance;
}

void srf05_solve_first()
{
  // just turn left ONCE = CCW
  digitalWrite(MTOR_Fp, LOW);
  digitalWrite(MTOR_Fn, HIGH);
  delay(TIME_QUARTER_TURN); // change this if not enough
  digitalWrite(MTOR_Fn, LOW);
}

void srf05_solve_second()
{ 
  // turn right TWICE = CW x 2
  digitalWrite(MTOR_Fp, HIGH);
  digitalWrite(MTOR_Fn, LOW);
  delay(TIME_QUARTER_TURN * 2); // change this if not enough
  digitalWrite(MTOR_Fp, LOW);
}

void srf05_solve_third()
{
  // spin to find a way out
  digitalWrite(MTOR_Fn, LOW);
  while (srf05_getDistance() <= LIMIT_DISTANCE)
  {
    digitalWrite(MTOR_Fp, HIGH);
    delay(250);
    digitalWrite(MTOR_Fp, LOW);
  }
}

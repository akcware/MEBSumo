enum TargetAim
{
  Idle,
  Left,
  Right
};

#define OmronSol A3
#define OmronSag A2
#define solQtr A5 //BOZUK //sol bozuk
#define sagQtr A4
#define MotorR1 9
#define MotorR2 10
#define MotorL1 11
#define MotorL2 3

#define led_buzzer 13

#define DS1 12
#define DS2 8
#define DS3 7

#define button 6
#define pot A6
#define ir 2

int qtr = 700;
bool irr = false;

TargetAim currentTargetStatus = Idle;

void setup()
{
  pinMode(OmronSol, INPUT_PULLUP);
  pinMode(OmronSag, INPUT_PULLUP);

  // pinMode(solQtr, INPUT_PULLUP);
  pinMode(sagQtr, INPUT_PULLUP);

  pinMode(MotorR1, OUTPUT);
  pinMode(MotorR2, OUTPUT);
  pinMode(MotorL1, OUTPUT);
  pinMode(MotorL2, OUTPUT);

  pinMode(led_buzzer, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(pot, INPUT_PULLUP); //trimpot
  pinMode(ir, INPUT);

  pinMode(DS1, INPUT_PULLUP);
  pinMode(DS2, INPUT_PULLUP);
  pinMode(DS3, INPUT_PULLUP);

  digitalWrite(led_buzzer, LOW);

  digitalWrite(MotorR1, LOW);
  digitalWrite(MotorR2, LOW);
  digitalWrite(MotorL1, LOW);
  digitalWrite(MotorL2, LOW);

  tone(led_buzzer, 200, 350);
  delay(300);
  noTone(led_buzzer);

  tone(led_buzzer, 300, 200);
  tone(led_buzzer, 4000, 500);
  noTone(led_buzzer);
}

void loop()
{
  wait();
}

void wait()
{
  qtr = analogRead(pot);

  if (!digitalRead(solQtr) && !digitalRead(sagQtr))
  {
    tone(led_buzzer, 300, 10);
  }
  else if (!digitalRead(OmronSol) || !digitalRead(OmronSag))
  {
    digitalWrite(led_buzzer, HIGH);
  }
  else
  {
    noTone(led_buzzer);
    digitalWrite(led_buzzer, LOW);
  }
  irr = digitalRead(ir);

  if (irr || !digitalRead(button))
  {
    dipswitch_settings();
    noTone(led_buzzer);
    start();
    return;
  }

  wait();
}

void start()
{
  if (!digitalRead(ir) && irr)
  {
    analogWrite(MotorR1, 0);
    analogWrite(MotorR2, 0);
    analogWrite(MotorL1, 0);
    analogWrite(MotorL2, 0);

    irr = 0;
    wait();
    return;
  }

  if (digitalRead(solQtr))
  { // siyah
    saldir();
  }
  else
  {
    sahayaDon();
  }

  // start(); // TODO: Düzgün çalışmazsa aktifleştir.
}

void saldir()
{
  if (!digitalRead(OmronSol) && !digitalRead(OmronSag))
  { // rakip ön
    analogWrite(MotorR1, 175);
    analogWrite(MotorR2, 0);
    analogWrite(MotorL1, 175);
    analogWrite(MotorL2, 0);

    digitalWrite(led_buzzer, HIGH);
    currentTargetStatus = Idle;
  }
  else if (!digitalRead(OmronSol) && digitalRead(OmronSag)) // rakip sol
  {
    analogWrite(MotorR1, 0);
    analogWrite(MotorR2, 180);
    analogWrite(MotorL1, 90);
    analogWrite(MotorL2, 0);

    digitalWrite(led_buzzer, HIGH);
    currentTargetStatus = Left;
  }
  else if (digitalRead(OmronSol) && !digitalRead(OmronSag))
  { // rakip sağ
    analogWrite(MotorR1, 90);
    analogWrite(MotorR2, 0);
    analogWrite(MotorL1, 0);
    analogWrite(MotorL2, 180);

    digitalWrite(led_buzzer, HIGH);
    currentTargetStatus = Right;
  }
  else
  { // rakip gözükmüyorsa
    digitalWrite(led_buzzer, LOW);

    if (currentTargetStatus == Left)
    {
      // Eğer eksiden solda gözüküyorsa sürekli sola doğru dön
      leftTurnOver();
    }
    else if (currentTargetStatus == Right)
    {
      rightTurnOver();
    }
  }
}

void leftTurnOver()
{
  analogWrite(MotorR1, 0);
  analogWrite(MotorR2, 220);
  analogWrite(MotorL1, 220);
  analogWrite(MotorL2, 0);
  delay(2);
}

void rightTurnOver()
{
  analogWrite(MotorR1, 220);
  analogWrite(MotorR2, 0);
  analogWrite(MotorL1, 0);
  analogWrite(MotorL2, 220);
  delay(2);
}

void sahayaDon()
{
  if (analogRead(sagQtr) < qtr && analogRead(sagQtr) >= qtr)
  {
    backward(204);

    delay(150);

    analogWrite(MotorR1, 204);
    analogWrite(MotorR2, 0);
    analogWrite(MotorL1, 0);
    analogWrite(MotorL2, 204);

    delay(100);
    digitalWrite(led_buzzer, LOW);
  }
  else if (analogRead(sagQtr) >= qtr && analogRead(sagQtr) < qtr)
  {
    backward(204);

    delay(150);

    analogWrite(MotorR1, 0);
    analogWrite(MotorR2, 204);
    analogWrite(MotorL1, 204);
    analogWrite(MotorL2, 0);

    delay(100);
    digitalWrite(led_buzzer, LOW);
  }
  else if (analogRead(sagQtr) < qtr && analogRead(sagQtr) < qtr)
  {
    if (currentTargetStatus == Left)
    {
      backward(204);

      delay(150);

      analogWrite(MotorR1, 0);
      analogWrite(MotorR2, 204);
      analogWrite(MotorL1, 204);
      analogWrite(MotorL2, 0);
    }
    else if (currentTargetStatus == Right)
    {
      backward(204);

      delay(150);

      analogWrite(MotorR1, 204);
      analogWrite(MotorR2, 0);
      analogWrite(MotorL1, 0);
      analogWrite(MotorL2, 204);
    }
  }
}

void backward(int speed)
{
  analogWrite(MotorR1, 0);
  analogWrite(MotorR2, speed);
  analogWrite(MotorL1, 0);
  analogWrite(MotorL2, speed);
}

struct DipSwitch
{
  bool a;
  bool b;
  bool c;
};

void dipswitch_settings()
{
  DipSwitch dipswitch = {
      digitalRead(DS1),
      digitalRead(DS2),
      digitalRead(DS3)};

  if (!dipswitch.a && !dipswitch.b && dipswitch.c) // sağ
  {
    analogWrite(MotorR1, 200);
    analogWrite(MotorR2, 0);
    analogWrite(MotorL1, 0);
    analogWrite(MotorL2, 200);

    delay(120);

    analogWrite(MotorR1, 160);
    analogWrite(MotorR2, 0);
    analogWrite(MotorL1, 160);
    analogWrite(MotorL2, 0);

    delay(20);

    currentTargetStatus = Right;
  }
  else if (!dipswitch.a && dipswitch.b && !dipswitch.c) // Sol;
  {
    analogWrite(MotorR1, 0);
    analogWrite(MotorR2, 200);
    analogWrite(MotorL1, 200);
    analogWrite(MotorL2, 0);

    delay(120);

    analogWrite(MotorR1, 160);
    analogWrite(MotorR2, 0);
    analogWrite(MotorL1, 160);
    analogWrite(MotorL2, 0);

    delay(20);

    currentTargetStatus = Left;
  }
  else if (!dipswitch.a && dipswitch.b && dipswitch.c)
    rightTurnOver();
  else if (dipswitch.a && !dipswitch.b && !dipswitch.c)
    leftTurnOver();
}
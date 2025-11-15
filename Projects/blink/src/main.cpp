#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  pinMode(26, OUTPUT);
  pinMode(0, INPUT);
  pinMode(13, OUTPUT); // Set GPIO pin 2 as output
}

void loop()
{

  if (digitalRead(0) == LOW)
  {
    digitalWrite(13, HIGH); // Turn LED on
    digitalWrite(26, HIGH);
    delay(500);             // Wait for 1 second
    digitalWrite(13, LOW);  // Turn LED off
    delay(500);             // Wait for 1 second
    digitalWrite(13, HIGH); // Turn LED on
    digitalWrite(26, LOW);
    delay(500);            // Wait for 1 second
    digitalWrite(13, LOW); // Turn LED off
    delay(500);            // Wait for 1 second
  }
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}
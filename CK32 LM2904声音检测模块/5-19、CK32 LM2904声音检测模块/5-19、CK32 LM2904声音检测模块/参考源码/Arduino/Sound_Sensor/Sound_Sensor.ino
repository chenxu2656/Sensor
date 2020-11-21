int sound_din=2;
int sound_ain=A0;
int ad_value;
void setup()
{
  pinMode(sound_din,INPUT);
  pinMode(sound_ain,INPUT);
  Serial.begin(9600);
}
void loop()
{
  ad_value=analogRead(sound_ain);
  if(digitalRead(sound_din)==LOW)
  {
    Serial.println("Noise!");
    Serial.println(ad_value);
  }
  else
  {
    Serial.println("Quiet!");
  }
  delay(500);
}

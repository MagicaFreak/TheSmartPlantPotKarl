int adc_id = 0;//Analog Pin


void setup() 
{
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int B = analogRead(adc_id); // get adc value
  Serial.print(F("Bodenfeuchtigkeit: "));
  Serial.print(B);
  erial.Print(F("%"))
// Berechnungen für % wert

}

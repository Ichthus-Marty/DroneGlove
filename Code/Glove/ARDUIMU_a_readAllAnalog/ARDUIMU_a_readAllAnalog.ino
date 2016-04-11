/* uploaded to the arduIMU
 *  This reads the flexsensor data and prints the values.
 *  When you connect the arduIMU tx port to the arduino rx port
 *  you can read the values trough the arduino.
 */

byte analogPins[] = { 
  A0, A1, A2, A3, A4, A5, A6, A7 };  

void setup() {
  for(int i=0;i<8;i++){
  pinMode(analogPins[i], INPUT_PULLUP);  
  }
  Serial.begin(9600, SERIAL_8N1);
}

void loop() {
  for(int i=0;i<8;i++){
  Serial.print(analogRead(analogPins[i]));    
  Serial.print("\t");
  delay(30);
  }
  Serial.println();
}

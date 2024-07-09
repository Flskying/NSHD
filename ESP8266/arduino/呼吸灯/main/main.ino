int led = LED_BUILTIN;
void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
}
void loop() {
//   put your main code here, to run repeatedly:
  for( int i = 0 ; i<=200 ; i++){
  analogWrite(led,i);
  delay(4);
  }
  for( int j = 200 ; j>=0 ; j--){
  analogWrite(led,j);
  delay(4);   
  }
//  digitalWrite(led,LOW);
//  delay(4000);
//  digitalWrite(led,HIGH);
//  delay(4000);
}

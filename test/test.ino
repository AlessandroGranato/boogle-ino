void setup() {   
  // initialize inbuilt LED pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// loop function runs over and over  again forever
void loop() {
  digitalWrite(LED_BUILTIN, LOW);   
  delay(1000);              
  digitalWrite(LED_BUILTIN, HIGH);    
  delay(300000);             
}

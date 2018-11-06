

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
pinMode(6, OUTPUT);
pinMode(9, OUTPUT);


}

void loop() {
 getSample();

}

double getSample(){
  char buf [20];
  char buf2 [20];
  int counter =0;
  double samples[10] ;
  
  while(counter < 3){
  turnOnIntake(1000);
  delay(100);
  turnOffIntake();
  delay(100);
  turnOnSolution(100000);
  delay(100); 
  turnOffSolution();
  delay(100);
    samples[counter] = getSensorReading();
    counter++;
     sprintf(buf, "Sample %3d taken\n", counter);
     Serial.print(buf);
  }
  double sum = 0;
  int i = 0;
  for(i;i < 3;i++){
   sum = sum + samples[i];
  // Serial.println(sum);
  }
  double average = sum/3;
  Serial.print("The average is: ");
  Serial.println(average);
  return average; 
  
}

void turnOnIntake(int time){
  digitalWrite(6, HIGH);
  Serial.print("Intake Pump On\n");
  delay(time);
}

void turnOffIntake (){
    digitalWrite(6, LOW);
  Serial.print("Intake Pump Off\n");
}

void turnOnSolution(int time){
    digitalWrite(9, HIGH);
  Serial.print("Solution Pump On\n");
  delay(time);
  
}

void turnOffSolution(){
    digitalWrite(9, LOW);
  Serial.print("Solution Pump Off\n");
}

double getSensorReading(){
  double data = random(15,20);
 // Serial.println(data);
  return data;
}

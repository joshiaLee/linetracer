#include <Servo.h>
Servo servoLeft,servoRight;

long now = 0;                 //현재 시간을 저장할 변수
long w = 0;               //blinking 함수를 위해 2초시간 재는용 변수
long t[2] = {0};          // 시간을 저장할 배열 

long periodOfTime = 0;    // 시간간격을 저장할 변수

int i = 0;                // Stop함수 호출횟수 

int led_L = 6;            // 왼쪽 방향등 6번 led_L
int led_R = 4;            // 오른쪽 방향등 4번 led_R

int IR_Left = 0;          // 왼쪽 적외선 센서 
int IR_Center = 1;        // 가운데 적외선 센서
int IR_Right = 2;         // 오른쪽 적외선 센서



int Time = 50;            // loop의 주기  (= 센서를 다시 읽는 주기) 

int TS = 500;             // 적외선 센서 판정감도


int number = 0;             // 직전방향이 왼쪽인지 오른쪽인지 저장하는 변수
int shift = 0;              //  직전방향으로 옮기게 해주는 변수

long term;                //시간을 저장하여 확인하는 용도


int k; // 멜로디 배열의 첨자용 변수

int speakerPin = 9; // 피에조 9번으로 설정

int melody[] = {  3800,0, 1318,1568,1568,1318,1568,1568,1760,1760,1760,1760,1760,0
                  ,1568,1568,1568,1568,1396,1396,1396,1396,1318,1318,1318,1318,1318,0
                  ,1318,1568,1568,1568,1318,1568,1568,1760,1760,1318,1318,1568,0
                  ,1396,1396,1396,1396,1318,1318,1318,1318,1174,1174,1568,1568,1046,0};  // 출발 시작음 + 1초 무음 + 자전거 멜로디

int durations[] = {1,1,4,4,2,4,4,2,4,4,4,4,4/3,4
                    ,4,4,4,4,4,4,4,4,4,4,4,4,4/3,4
                    ,4,4,4,4,4,4,2,4,4,4,4,4/3,4
                    ,4,4,4,4,4,4,4,4,4,4,4,4,4/3,4};  // 출발시작음 + 1초 무음  + 자전거 멜로디의 박자



int repeat;  //같은음 반복 횟수

int count; // repeat랑 같아 질때까지 루프를 돌아주면서 루프딜레리를 이용하여 음을 연주할 시간끌기.




void setup() 
{
    
  Serial.begin(9600);    // 통신시작
  
  servoLeft.attach(13);   // 왼쪽 서보 모터 13 연결
  servoRight.attach(12);  // 오른쪽 서보 모터 12 연결

  pinMode(IR_Left,INPUT);
  pinMode(IR_Center,INPUT);
  pinMode(IR_Right,INPUT); // 적외선센서 아날로그 인풋으로 설정
  
  pinMode(led_L,OUTPUT);
  pinMode(led_R,OUTPUT);   // led 아웃풋으로 설정

  Serial.println(" ");
  Serial.print("LVE");  
  Serial.print("\t");  
  Serial.print("CVE");
  Serial.print("\t");  
  Serial.print("RVE");
  Serial.print("\t");
  Serial.print("SHI");
  Serial.print("\t");
  Serial.print("POT");
  Serial.print("\t");
  Serial.print("ACT");
  Serial.print("\t");
  Serial.println("TIM"); // 해당값을 관찰하기 위해서 출력

}




void loop() 
{
   
    int LV;
    int CV;
    int RV;  // 센서로부터 들어오는값을 저장

    while(term == 0) 
    {
      LV = analogRead(IR_Left);
      CV = analogRead(IR_Center);
      RV = analogRead(IR_Right);
      
      starting(LV,CV,RV);      // 센서값을 계속 받으면서 starting함수의 인수로 제공한다. 함수는 조건을만족하면 출발알림음을 시작하고 무한루프를 빠져나가게 term 을 1로 변환시킨다.
    }
     
      LV = analogRead(IR_Left);
      CV = analogRead(IR_Center);
      RV = analogRead(IR_Right);
    
     

    if( w < 2000 )
   {
    w = millis() - now; // 출발시작음이 시작될때 led를 1초 키고 1초 끄기위해서 설정한 변수 
    blinking(); // 2초동안 블링클 함수 실행 
   }
                        

    if( k !=  sizeof(melody) / sizeof(melody[0]) )
    playNote();      // 주행중 음연주하기 k가 55면 완주이므로 더이상 호출하지 않음

 
 
  Serial.print(LV);
  Serial.print("\t");
  Serial.print(CV);
  Serial.print("\t");
  Serial.print(RV);
  Serial.print("\t");  // 값의변화를 확인하기위해 출력한다
  

  if( LV > 900 && CV > 900 && RV > 900 )
  {
    Stop(i); // 세개의센서 모두 큰값이 나오면 stop조건인지 아니면 교차로인지 확인하기 위해 stop함수를 호출한다 . 인수로 i를 넘겨줌 ( 시간을 차곡차곡 넣기 위함 )
    i++;  // 함수의 호출횟수를 증가시킨다.
  } 

  if( LV > TS && CV > TS && RV > TS )
  {
      if(shift == 3) // 정지조건을 따졌다면 periodOfTime을 출력시킴 <- 정지조건 관찰용임(실전에 필요없다.)
      {
        forward();  
        Serial.print(periodOfTime);
        Serial.print("\t");
        Serial.print("FOR");
        Serial.print("\t"); 
        
      }
      else
      {
        forward();
        Serial.print("\t");
        Serial.print("FOR");
        Serial.print("\t"); 
      }
       
      term = millis();
      Serial.println(term); // 시간측정 (loop 주기 확인용)
      
      shift = 0; // shift를 0으로 초기화하여 후의 shift 변화를 관찰하기 위함
  }

  else if( LV < TS && CV > TS && RV < TS )
  {
    
      forward();
      
      Serial.print("\t");
      Serial.print("FOR");
      Serial.print("\t");
      term = millis();
      Serial.println(term);
        
  }

  else if( LV > TS && CV < TS && RV < TS)
  {
    
      number = 1;
      turnLeft();
      Serial.print("LEF");
      Serial.print("\t");
      term = millis();
      Serial.println(term);
     
  }

  
  else if( LV > TS && CV > TS && RV < TS )
  {
    
      number = 1;
      turnLeft();
      Serial.print("LEF");
      Serial.print("\t");
      term = millis();
      Serial.println(term);    
  }


  else if( LV < TS && CV < TS && RV > TS )
  {
    
      number = 2;
      turnRight();
      Serial.print("RIT");  
      Serial.print("\t");
      term = millis();
      Serial.println(term);   
  }
  
  
  else if( LV < TS && CV > TS && RV > TS )
  {
      
      number = 2;
      turnRight();
      Serial.print("RIT");
      Serial.print("\t");
      term = millis();
      Serial.println(term);      
  }

  else // (0, 0, 0) 이나  (1, 0, 1)가 감지됐을때 직전방향으로 회전하기 
  {
   
    shift = number; // 직전 방향의 정보를 shift에게 저장
   
    switch(shift) // 직전방향으로 이동
    {
      case 1: turnSleft(); Serial.print("SPL"); break; // 1일 경우 반시계방향 회전운동  하게함
      case 2: turnSright(); Serial.print("SPR"); break; // 2일 경우 시계방향 회전운동  하게함
    }

    Serial.print("\t");
    
    term = millis();
   
    Serial.println(term);
      
    shift = 0;    
  
  }
 

 
}




void starting(int L, int C, int R) //출발알림음 함수
{
     if(L > 900 && C > 900 && R > 900)
     {
        digitalWrite(led_L,HIGH);
        tone(9,2092,500);
        delay(500);
        digitalWrite(led_L,LOW);
        delay(500);
  
        digitalWrite(led_R,HIGH);
        tone(9,2092,500);
        delay(500);
        digitalWrite(led_R,LOW);
        delay(500);
        
        term = 1;
        now = millis();
     }
      // 첫 시작줄을 만났을때 출발알림음 삐,삐 소리내기. 만나기전까지는 while을 빠져나가지 못하게 설정
}



void blinking() // 출발시작음과함께 양쪽 led 1초키고 1초 끄는 블링클 함수
{

    if(w < 1000)
    {
        digitalWrite(led_L,HIGH);
        digitalWrite(led_R,HIGH); 
    }

    else if(w < 2000)
    {
        digitalWrite(led_L,LOW);
        digitalWrite(led_R,LOW);                                          
    }    

}



void playNote() 
{

    int duration;
    
    duration = 1000 / durations[k]; // 각음마다 지속시간을 계산한다.
  
    repeat = duration / Time; // loop 주기가 50ms이므로 지속시간을 50ms로 쪼갠다음 그 음을 반복할 횟수 구하기 (tone 함수뒤에 delay를 따로 쓰지않고  loop의 딜레이를 이용해서 음을 내기위함 )  
                            // 주행중 연주가 가능

    if(count == 0)
    {
      tone(speakerPin,melody[k],duration);
      count++;   // count가 0이 되면 다른음을 시작한다
    }
 
    else if(count < repeat)
    {
      count++; // count가 repeat가 될때까지 반복한다 한음을 지속시간만큼 연주하기위해 시간끌기용
    }
 
    else if(count == repeat)
    { 
    
      k++;
      count = 0;  // count가 repeat랑 같아지면 다른음을 연주하기위해 k를 증가시키고 count를 0으로 초기화한다. 
    
    }



}


void forward()  // 전진 함수
{
  digitalWrite(led_L,LOW);
  digitalWrite(led_R,LOW);
  
  Serial.print(shift);
  Serial.print("\t");
   
  servoLeft.writeMicroseconds(1550);
  servoRight.writeMicroseconds(1450);
  delay(Time);
  
}

void turnLeft() // 좌회전 함수
{ 
  digitalWrite(led_L,HIGH);
  digitalWrite(led_R,LOW);
  
  Serial.print(shift);
  Serial.print("\t\t");
  
  
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1450);
  delay(Time);
   
}

void turnRight() // 우회전 함수
{
  digitalWrite(led_L,LOW);
  digitalWrite(led_R,HIGH);
  
  Serial.print(shift);
  Serial.print("\t\t");
  
  
  servoLeft.writeMicroseconds(1550);
  servoRight.writeMicroseconds(1500);
  delay(Time);
  
}


void turnSleft() // 반시계방향 스핀 함수
{
  digitalWrite(led_L,HIGH);
  digitalWrite(led_R,LOW);

  Serial.print(shift);
  Serial.print("\t\t");
  
  servoLeft.writeMicroseconds(1450);
  servoRight.writeMicroseconds(1450);
  
  delay(Time);
}

void turnSright() // 시계방향 스핀 함수
{
  digitalWrite(led_L,LOW);
  digitalWrite(led_R,HIGH);

  Serial.print(shift);
  Serial.print("\t\t");
  
  servoLeft.writeMicroseconds(1550);
  servoRight.writeMicroseconds(1550);
  
  delay(Time);
}




void Stop(int r) //  정지함수 (조건에 맞으면 라인트레이서 정지)
{
   shift = 3; // 3번은 스탑함수 호출했다는 의미
    
   int j = r % 2; // 호출한 순서대로 차례대로 t[0],t[1]에 넣기위한 스케일링
   
   
   t[j] = millis(); // t배열에 시간넣기

  if( i > 0 )
  {
    if(t[0] < t[1])
      periodOfTime = t[1] - t[0];
    else
      periodOfTime = t[0] - t[1]; // 큰값에서 작은값을 빼면 시간차가 구해지고 이시간차이는 정지조건을 확인하는데 쓰인다.
  }
 
 /* if( 120 < periodOfTime && periodOfTime < 240 ) // 만약 시간간격이 이구간이라면 긴줄 두개가 있는 마지막 지점이므로 정지한다. -> 정지조건 만족
  {
    
    servoLeft.writeMicroseconds(1500);
    servoRight.writeMicroseconds(1500);
    
    delay(Time);

    ending();         // 엔딩함수 호출

    servoLeft.detach();
    servoRight.detach(); //더이상 라인트레이서가 동작하지 않게 연결분리로 마무리
    
    
  } */ 

 
}

void ending()
{ 
       
        digitalWrite(led_L,HIGH);
        tone(9,2092,500);
        delay(1000);
        
        digitalWrite(led_R,HIGH);
        tone(9,2092,500);
        delay(1000);
        
        digitalWrite(led_L,LOW);
        digitalWrite(led_R,LOW);
        
        tone(9,3800,1000);
        delay(1000);
        
}

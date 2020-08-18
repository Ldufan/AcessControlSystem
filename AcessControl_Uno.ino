/*
Github项目地址：https://github.com/Ldufan/AcessControlSystem；
欢迎Star；
此代码烧录在Arduino中；
*/

#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <IRremote.h>

#define SS_PIN 10 //自定义RC522的两个PIN口
#define RST_PIN 9
#define RECV_PIN 5 //红外接收引脚
#define CARDS 4 //卡片数目
#define SERVO_PIN 6 //舵机端口
#define BUZZ_PIN 8 //蜂鸣器引脚

long int IR_ID = 0xFFABCD; //定义红外变量
String ESP01S_Data = "";
decode_results results;
const byte User_ID[CARDS][4] = {  {0xDD, 0xAA, 0xBB, 0x11}, //校园卡
                                  {0xAD, 0x99, 0xDD, 0xEE},
                                };  //保存多个卡片UID值

MFRC522 rfid(SS_PIN, RST_PIN); //新建RC522对象
Servo myservo;
SoftwareSerial EspSerial(3, 2); // RX, TX
IRrecv irrecv(RECV_PIN);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); //初始化
  Led_Blink(100, 2);
  pinMode(BUZZ_PIN, OUTPUT);
  Buzzer_Beep(100, 2);
  myservo.attach(SERVO_PIN);
  myservo.write(0);

  Serial.begin(9600); //UNO与PC串口通讯
  EspSerial.begin(115200); //UNO与ESP01S通讯
  SPI.begin();     //初始化
  rfid.PCD_Init();
  irrecv.enableIRIn();
  delay(50);
  Serial.println("Success!");
  while(Serial.read() >= 0){ }
  while(EspSerial.read() >= 0){ }
}

void loop() {
// 若没有读到卡并且串口未收到数据并且红外无数据，return
  if (!rfid.PICC_IsNewCardPresent() & EspSerial.available() == 0 & irrecv.decode(&results) == 0) {
    return;
  }

// 红外遥控部分
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX); //输出指令信息
    irrecv.resume();                    //接收下一指令
    if (results.value == IR_ID) {
      EspSerial.print("OPEND");
      Success();
      EspSerial.print("CLOSED");
      Serial.println(F("IRremote"));
    }
  }

// WiFi控制部分
  if(EspSerial.available() > 0){ //读取串口数据
    while(EspSerial.available() > 0){
      ESP01S_Data += (char)(EspSerial.read());
      delay(2);
    }
    Serial.print("ESP01S_Data:");
    Serial.println(ESP01S_Data);

    if (ESP01S_Data == "OPEN"){ //判断
      Success();
      EspSerial.print("CLOSED");
      Serial.println(F("WiFi"));
    }
    else if (ESP01S_Data == "CLOSE"){
      Buzzer_Beep(100, 1);
      Led_Blink(100, 5);
    }
    ESP01S_Data = "";
  }

//射频卡部分
  if (rfid.PICC_ReadCardSerial()) {
    //输出uid
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    //验证
    for (byte num  = 0; num < CARDS; num++) {
      byte i;
      for (i = 0; i < rfid.uid.size; i++){
        if (rfid.uid.uidByte[i] != User_ID[num][i]) break;
      }
      if (i == rfid.uid.size){
        EspSerial.print("OPEND");
        Success();
        EspSerial.print("CLOSED");
        Serial.println(F("RC522"));
        break;
      }
      else if (num == CARDS - 1){
        Serial.println(F("Failed!"));
        Buzzer_Beep(50, 3);
      }
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void Success() {
  Buzzer_Beep(100, 1);
  Led_Blink(50, 2);
  myservo.write(180);
  delay(3000);
  myservo.write(0);
  delay(500);
}

void Buzzer_Beep(int t, int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(BUZZ_PIN, HIGH);
    delay(t);
    digitalWrite(BUZZ_PIN, LOW);
    if (i < n-1 ) {
      delay(t);
    }
  }
}

void Led_Blink(int t, int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(t);
    digitalWrite(LED_BUILTIN, LOW);
    if (i < n-1 ) {
      delay(t);
    }
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

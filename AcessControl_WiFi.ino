/*
Github项目地址：https://github.com/Ldufan/AcessControlSystem；
欢迎Star；
此代码烧录在ESP01S中；
有问题参考blinker官方文档；
*/
#define BLINKER_WIFI

#include <Blinker.h>

char auth[] = "你设备的Blinker秘钥";
char ssid[] = "WifI名称";
char pswd[] = "WifI密码";
String UNO_Data = "";

BlinkerButton Button1("OPEN"); //按钮名称跟APP对应

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  while(Serial.read() >= 0){ }

  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);
  Button1.attach(button1_callback);
}

void loop() {
  Blinker.run();
  if (Serial.available() > 0){
    while(Serial.available() > 0){
      UNO_Data += (char)(Serial.read());
      Blinker.delay(2);
    }
    if(UNO_Data == "CLOSED") { //指令跟UNO端对应即可
      Blinker.vibrate();
      Blinker.push("Opend"); //APP通知
      Button1.icon("fas fa-lock-alt");
      Button1.color("#666666");
      Button1.text("已复位");
      Button1.print("off");
    }
    else if(UNO_Data == "OPEND") {
      Blinker.vibrate();
      Button1.icon("fas fa-lock-open-alt");
      Button1.color("#CC0033");
      Button1.text("已打开");
      Button1.print("on");
    }
    UNO_Data = "";
  }
}

void button1_callback(const String & state) {
  if (state == BLINKER_CMD_ON) {
    Serial.print("OPEN");
    Blinker.vibrate();
    Button1.icon("fas fa-lock-open-alt");
    Button1.color("#CC0033");
    Button1.text("已打开");
    Button1.print("on");
  }
  else if (state == BLINKER_CMD_OFF) {
    Serial.print("CLOSE");
    Blinker.vibrate();
    Button1.print("off");

  }
}

void dataRead(const String & data) {
  Blinker.vibrate();
  Blinker.print("None", data);
}
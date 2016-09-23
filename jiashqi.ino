#include"ESP8266.h"
#include "I2Cdev.h"
#include <Wire.h>
#include<string.h>
#include<stdio.h>
#include <stdlib.h>

#define outletPin 6

//===================================================
/*定义无线的名称和密码*/
#define SSID        "iPhone"
#define PASSWORD    "18611668223"
/*服务器名称和端口*/
#define HOST_NAME   "api.yeelink.net"
#define HOST_PORT   (80)

#define INTERVAL_NET             10000
#define INTERVAL_SENSOR          10000

String Device_ID =  "350487";
String Sensor_ID_Tem_In =  "393577";
String Sensor_ID_Hum_In =  "393578";
String Sensor_ID_Win_Switch = "394067";

ESP8266 wifi(Serial1);    //定义一个ESP8266的对象

unsigned long net_time2 = millis();

boolean record = false;
int start = 0;
int end = 0;

float temIn,humIn,state_win;
float state=10;

void setup()
{
    // 初始化wifi通信波特率
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(outletPin, OUTPUT);
  digitalWrite(outletPin,LOW);
  
//************************wifi设置
    Serial.print("setup begin\r\n");
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());

    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }

    if (wifi.joinAP(SSID, PASSWORD)) {      //加入无线网
        Serial.print("Join AP success\r\n");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }   
    Serial.print("setup end\r\n");
}

void loop()
{
  if (net_time2 > millis()) net_time2 = millis();
  if (millis() - net_time2 > INTERVAL_NET) 
  {
    temIn = getValue(Device_ID, Sensor_ID_Tem_In);
    Serial.print("temIn:");
    Serial.println(temIn);
    delay(1000);
    humIn = getValue(Device_ID, Sensor_ID_Hum_In);
    Serial.print("humIn:");
    Serial.println(humIn);
    delay(1000);
    state_win = getValue(Device_ID, Sensor_ID_Win_Switch);
    Serial.print("state_win:");
    Serial.println(state_win);
    delay(1000);
  }
   
   if(state_win==1)
   { 
        digitalWrite(outletPin,LOW);
        Serial.println("\r\n");
        Serial.println("power off");
        Serial.println("\r\n");
    }
   
   if(state_win==0)   //state_win=0，窗户关闭，加湿器才可以工作
   {
  if(temIn>=0 && temIn <= 8)   //室内温度 0-8度 
  {
     if( humIn < 40.0)
     {
       if(state==10 || state==0)
       {
        digitalWrite(outletPin,HIGH);
        Serial.println("\r\n");
        Serial.println("power on");
        Serial.println("\r\n");
        state=1;  //加湿器打开
       }
     }
     else
     {
      if(state==10 || state==1)
      {
       digitalWrite(outletPin,LOW);
       Serial.println("\r\n");
       Serial.println("power off");
       Serial.println("\r\n");
       state=0;  //加湿器关闭
      }
     }
  }
  else if( temIn>8 && temIn <= 28) //室内温度 8-28度 
  {    
      if( humIn <50  )
      {
        if(state==10 || state==0)
        {
          digitalWrite(outletPin,HIGH);
          Serial.println("\r\n");
          Serial.println("power on");
          Serial.println("\r\n");
         state=1;   //加湿器打开
         }
       }
      else
      {
        if(state==10 || state==1)
        {
          digitalWrite(outletPin,LOW);
          Serial.println("\r\n");
          Serial.println("power off");
          Serial.println("\r\n");
          state=0;   //加湿器关闭
        }
      }
  }
  else if( temIn > 28)   // 室内温度 大于28
  {
       if( humIn <=40)
       {
         if(state==10 || state==0)
          {
            digitalWrite(outletPin,HIGH);
            Serial.println("\r\n");
            Serial.println("power on");
            Serial.println("\r\n");
            state=1;   //加湿器打开
           }
        }
        else
        {
          if(state==10 || state==1)
          {
            digitalWrite(outletPin,LOW);
            Serial.println("\r\n");
            Serial.println("power off");
            Serial.println("\r\n");
            state=0;   //加湿器关闭
          }
        }
     }
 }
  delay(1000);
}

/*===================从yeelink获取传感器三个温湿度值、、、要修改，只获取室内温度、湿度*/
float getValue(String Device_ID, String Sensor_ID) 
{
   String postString;    //用于存储发送数据的字符串
   String jsonToSend;    //用于存储发送的json格式参数

  uint8_t buffer[512] = {0};      //用于存储Responce的全部信息
  char infoData[60] = {0};        //用于存储Responce的关键信息

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print("create tcp ok\r\n");
  } else {
    Serial.print("create tcp err\r\n");
  }

  postString = "GET ";        //发起get请求
  postString += "/v1.0/device/";
  postString += Device_ID;
  postString += "/sensor/";
  postString += Sensor_ID;
  postString += "/datapoints";
  postString += " HTTP/1.1";
  postString += "\r\n";
  postString += "Host: ";
  postString += HOST_NAME;
  postString += "\r\n";
  postString += "Accept: */*";
  postString += "\r\n";
  postString += "U-ApiKey:8423aedeebc8701a23cdfcc5749bd141\r\n";
  postString += "Connection: close\r\n";
  postString += "\r\n";

  const char *getArray = postString.c_str();        //发送get请求，为了得到Responce
  Serial.println(getArray);
  wifi.send((const uint8_t*)getArray, strlen(getArray));

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);   //接收长度函数
  if (len > 0) {          //如果接收到了
    for (uint32_t i = 0; i < len; i++) {    //将所有的字符遍历一遍，找到接收信息的头尾
      if ((char)buffer[i] == '{') {
        start = i;
      }
      if ((char)buffer[i] == '}') {
        end = i;
      }
    }
    
    int index = 0;        //infoData数组的指针
    for (uint32_t i = start; i <= end; i++) {     //存储Responce的关键信息一个个存在infoData数组里
      infoData[index] = (char)buffer[i];          //infoData[0]='{'      infoData[end-start+1]='}'  最后index=字符串长度
      index++;
    }

    Serial.print("Received:[");     //打印收到的信息
    Serial.print(infoData);
    Serial.print("]\r\n");

    char Data_char[5] = {0};
    int count = 0;
    for (uint32_t i = 43; i <= index - 2; i++)
    {
      Data_char[count] =  infoData[i];
      count++;
    }
    //atof(Data_char);
    return atof(Data_char);
  }

  if (wifi.releaseTCP()) {
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
  }
  getArray = NULL;
}


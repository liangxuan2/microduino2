//红外控制模块
#include <ESP8266.h>
#include "I2Cdev.h"
#include <Wire.h>
#include <stdlib.h>
#include <IRremote.h>
#include <math.h>

/*定义无线的名称和密码*/
#define SSID        "iPhone"
#define PASSWORD    "18611668223"
/*服务器名称和端口*/
#define HOST_NAME   "api.yeelink.net"
#define HOST_PORT   (80)

#define INTERVAL_NET             10000
#define INTERVAL_SENSOR          10000

String Device_ID =  "350487";
String Sensor_ID_T_I =  "393577";
String Sensor_ID_T_O =  "393349";
String Sensor_ID_Switch = "393581";
String Sensor_ID_UserTem = "393753";

ESP8266 wifi(Serial1);    //定义一个ESP8266的对象

IRsend irsend;

unsigned long net_time1 = millis();
unsigned long net_time2 = millis();
unsigned long sensor_time = millis();

float TemData;       //存储温度数据
float HumData;       //存储湿度数据
float TemData_In;       //存储温度数据
float TemData_Out;       //存储温度数据
int AC_State;          //存储空调状态
int UserTem;            //用户设定的空调温度
int preSTem = 0;        //避免重复设定温度的参数

boolean record = false;
int start = 0;
int end = 0;

void setup(void)
{
    Serial.begin(115200);
    //while (!Serial); // wait for Leonardo enumeration, others continue immediately
    
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

void loop(void)
{
  if (net_time2 > millis()) net_time2 = millis();
  if (millis() - net_time2 > INTERVAL_NET) {
    TemData_In = getValue(Device_ID, Sensor_ID_T_I);
    TemData_Out = getValue(Device_ID, Sensor_ID_T_O);
    UserTem = getValue(Device_ID, Sensor_ID_UserTem); //更新用户输入的数据
    net_time2 = millis();

  int value = 1;
  if (UserTem == 1)
     {if (TemData_In <= 24 && TemData_Out <=25)
      value = 0;
     else if (TemData_Out > 27 && TemData_Out < 30)
      value = floor(TemData_Out) - 3;
     else if (TemData_Out >= 30 && TemData_Out <= 33)
      value = floor(TemData_Out) - 4;
     else if (TemData_Out > 33)
      value = floor(TemData_Out) - 5;
      Serial.print("AutoSet");
      Serial.print(value);
      Serial.print("\r\n");}
  else
    value = UserTem;
    

      if (value != preSTem)
  {
    if (value == 22)
      {irsend.sendSony(0x1FE807F, 32);
      }
    else if (value == 23)
      {irsend.sendSony(0x1FE40BF, 32);
      }
    else if (value == 24)
      {irsend.sendSony(0x1FEC03F, 32);
      }
    else if (value == 25)
      {irsend.sendSony(0x1FE20DF, 32);
      }
    else if (value == 26)
      {irsend.sendSony(0x1FEA05F, 32);
      }
    else if (value == 27)
      {irsend.sendSony(0x1FE609F, 32);
      }
    else if (value == 28)
      {irsend.sendSony(0x1FEE01F, 32);
      }
    else if (value == 29)
      {irsend.sendSony(0x1FE10EF, 32);
      }
    else if (value == 30)
      {irsend.sendSony(0x1FE906F, 32);
      }
    else if (value == 0)
      {irsend.sendSony(0x1FE708F, 32);
      }
    else if (value == 1)
      {irsend.sendSony(0x1FEF807, 32);
      }
    Serial.print("Value is ");
    Serial.print(value);
    Serial.print("\r\n");
    preSTem = value;
    }
  }
  
}

float getValue(String Device_ID, String Sensor_ID) {
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
  char Data_char[5] = {0};
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

    int count = 0;
    for (uint32_t i = 43; i <= index - 2; i++)
    {
      Data_char[count] =  infoData[i];
      count++;
    }
    //atof(Data_char);
  }

  if (wifi.releaseTCP()) {
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
  }
  getArray = NULL;
  return atof(Data_char);
}

//int AutoRun(int value)
//    {
//      Serial.print("Mode:AutoRun\r\n");
//     if (TemData_In <= 24 || value <=27)
//      value = 0;
//     else if (value > 27 && value < 30)
//      value += 3;
//     else if (value >= 30 && value <= 33)
//      value += 4;
//     else if (value > 33)
//      value += 5;
//
//      return value;
//      }



  

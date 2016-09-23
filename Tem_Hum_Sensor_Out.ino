#include <ESP8266.h>
#include <AM2321.h>
#include "I2Cdev.h"
#include <Wire.h>

/*定义无线的名称和密码*/
#define SSID        "iPhone"
#define PASSWORD    "18611668223"
/*服务器名称和端口*/
#define HOST_NAME   "api.yeelink.net"
#define HOST_PORT   (80)

#define INTERVAL_NET             10000
#define INTERVAL_SENSOR          10000

String Device_ID =  "350487";
String Sensor_ID_Tem_Out =  "393349";
String Sensor_ID_Hum_Out =  "393350";

ESP8266 wifi(Serial1);    //定义一个ESP8266的对象

AM2321 am2321;

unsigned long net_time1 = millis();
unsigned long net_time2 = millis();
unsigned long sensor_time = millis();

float TemData;       //存储温度数据
float HumData;       //存储湿度数据

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
  //传感器采样时间间隔
  if (sensor_time > millis()) sensor_time = millis();
  if (millis() - sensor_time > INTERVAL_SENSOR) {
    updateTHData();           //读串口中的温湿度传感器数据
    sensor_time = millis();
  }
  delay(1000);
  //数据上传时间间隔
  
  if (net_time1 > millis()) net_time1 = millis();
  if (millis() - net_time1 > INTERVAL_NET) {
    updateData(Device_ID, Sensor_ID_Tem_Out, TemData);           //上传温度传感器数据
    updateData(Device_ID, Sensor_ID_Hum_Out, HumData);           //上传湿度传感器数据
    net_time1 = millis();
  }
  //数据侦听时间间隔
  delay(1000);
}

void updateData(String Device_ID, String Sensor_ID, float value) 
{
  //上传数据
  String postString;    //用于存储发送数据的字符串
  String jsonToSend;    //用于存储发送的json格式参数

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");
  } else {
    Serial.print("create tcp err\r\n");
  }

//postString将存储传输请求，格式很重要
  postString = "POST ";         //post发送方式，后要有空格
  postString += "/v1.0/device/";
  postString += Device_ID;
  postString += "/sensor/";
  postString += Sensor_ID;
  postString += "/datapoints";
  postString += " HTTP/1.1";                                      //空格+传输协议
  postString += "\r\n";
  postString += "Host: ";                                         //Host：+空格
  postString += HOST_NAME;
  postString += "\r\n";
  postString += "Accept: */*";
  postString += "\r\n";
  postString += "U-ApiKey:8423aedeebc8701a23cdfcc5749bd141\r\n";  //服务器的APIKEY

  jsonToSend = "{\"value\":";             //要发送的json格式数据
  jsonToSend += value;
  jsonToSend += "}";

  postString += "Content-Length: ";
  postString += jsonToSend.length();
  postString += "\r\n";
  postString += "Content-Type: application/x-www-form-urlencoded\r\n";
  postString += "Connection: close\r\n";
  postString += "\r\n";

  postString += jsonToSend;     //将json请求放在最后

  const char *postArray = postString.c_str();     //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)

  if (wifi.releaseTCP()) {            //释放TCP连接
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
  }
  postArray = NULL;       //清空数组，等待下次传输数据
}

void updateTHData()
{
  am2321.read();
  TemData = am2321.temperature / 10.0;
  HumData = am2321.humidity / 10.0;
}


     

#include <ESP8266.h>
#include "I2Cdev.h"
#include <Wire.h>
#include <stdlib.h>
#include<string.h>

/*定义无线的名称和密码*/
#define SSID        "iPhone"
#define PASSWORD    "18611668223"
/*服务器名称和端口*/
#define HOST_NAME   "api.yeelink.net"
#define HOST_PORT   (80)

#define INTERVAL_NET             10000
#define INTERVAL_SENSOR          10000

String Device_ID="350487";
String Sensor_ID_Tem_In="393577";
String Sensor_ID_Hum_Out="393350";
String Sensor_ID_Win_State="394067";
String Sensor_ID_APP_Win_state="394104";

ESP8266 wifi(Serial1);   

unsigned long net_time1=millis();
unsigned long net_time2=millis();

boolean record = false;
int start=0;
int end=0;
//====================================================

#include <Servo.h>       //调用库 舵机
Servo myservo;  // create servo object to control a servo
// a maximum of eight servo objects can be created
#define servo_pin 6


//相关常量声明
int pos,pos1=50,pos2=0,i=0;    // variable to store the servo position;
#define hum0 80
float humOu,temIn,HHMM,HHMM0;
float AC_State, //空调工作状态 
      state_win, //向加湿器发送窗开关状态
      state=10,
      state_app ;//app手动控制

void setup(void)
{
  myservo.attach(servo_pin);  // attaches the servo on pin SDA to the servo object

  //初始化wifi通信波特率
  Serial.begin(115200);
  Serial1.begin(115200);

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
   if (millis() - net_time1 > INTERVAL_NET)
   {
    state_app = getValue(Device_ID,Sensor_ID_APP_Win_state);
    Serial.print("state_app:");
    Serial.println(state_app);
    Serial.print("\r\n");
    HHMM=getHHMM(Device_ID, Sensor_ID_Hum_Out);
    Serial.print("HHMM:");
    Serial.println(HHMM);
    Serial.print("\r\n");
    humOu = getValue(Device_ID, Sensor_ID_Hum_Out);
    Serial.print("humOu:");
    Serial.println(humOu);
    Serial.print("\r\n");
    temIn=getValue(Device_ID,Sensor_ID_Tem_In);
    Serial.print("temIn:");
    Serial.println(temIn);
    Serial.print("\r\n");       
    if(temIn>=24) AC_State=1;
    else  AC_State=0;      
    //AC_State=get_AC_State(userTem,temIn);
    //AC_State=getValue(Device_ID,Sensor_ID_AC_State);
    Serial.print("AC_State:");
    Serial.println(AC_State);
    Serial.print("\r\n");
    net_time1=millis();
   }
   delay(1000);

   window();
   delay(1000);
}

void window()
{
  /*
  1.是否app手动控制是总前提
  2.自动控制后，窗外是否下雨是大前提
  2.空调不工作时，每日上午定时打开4h，中午关闭2h；空调工作时，空调每工作2h，窗户自动打开30min*/

/* ***********app手动控制*/
if(state_app==1 && humOu<80) //开窗
{
   if(state==10||state==0)
  {for(pos=0;pos<=50;pos+=1) // goes from 0 degrees to 180 degrees
   {myservo.write(pos1);              // tell servo to go to position in variable 'pos'
    delay(30);}// waits 15ms for the servo to reach the position}
    state_win=1;     //wifi上传
    updateData(Device_ID, Sensor_ID_Win_State, state_win);
    Serial.println("send 1"); 
    state=1;
    }
 }
if(state_app==2||humOu>=80) //关窗
 {
     if(state==10||state==1)
   {for(pos=50;pos>=0;pos-=1)
       {myservo.write(pos2);            
       delay(30); }
      state_win=0;
      updateData(Device_ID, Sensor_ID_Win_State, state_win);
      Serial.println("send 0"); 
      state=0;
    }
}

/*进入自动控制*/
if(state_app==0)   
{
//窗外下雨
if(humOu>= hum0)  
{   
  if(state==10||state==1)  //窗户关闭
  {for(pos=50;pos>=0;pos-=1) 
     {myservo.write(pos2);             
     delay(30);}
    }
    state_win=0;      
    updateData(Device_ID, Sensor_ID_Win_State, state_win);
    Serial.println("send 0"); 
    state=0;
}

//窗外不下雨
else    
{
//空调不工作,窗户每日定时开关
  if (AC_State==0)   
  {
    //窗户打开
    if((HHMM>=8.00 && HHMM<=12.00)||(HHMM>=15.00 && HHMM<=20.00))//  早上八点到十二点,下午三点到七点，开窗
    {
      if(state==10||state==0)
      {for(pos=0;pos<=50;pos+=1)
        {myservo.write(pos1);              
         delay(15);}
        state_win=1;
        updateData(Device_ID, Sensor_ID_Win_State, state_win);
        Serial.println("send 1"); 
        state = 1;
       }
    }   
   else
  {    //关窗户
    if(state==10||state==1)
    {for(pos=50;pos>=0;pos-=1)
     {myservo.write(pos2);              
       delay(15);}
       state_win=0;
       updateData(Device_ID, Sensor_ID_Win_State, state_win);
       Serial.println("send 0"); 
       state = 10;
     }
   }
  }
    
//空调工作
 if(AC_State==1)   
  {   
    //空调状态从0变为1的一瞬间，获取当前时间HHMM0，设为初始时间并保持不变                            
     if(i<1)
      { HHMM0=HHMM;}   
      i++;
      Serial.print("HHMM0:");
      Serial.println(HHMM0); 
      
    /*空调每工作2h，窗户打开30min,假定空调最长工作时间是12h*/
    if( HHMM==HHMM0+0.02||HHMM==HHMM0+1.50 )
     {
       if(state==10||state==0)   //  窗户打开
        {for(pos=0;pos<=50;pos+=1) 
          { myservo.write(pos1);
          delay(15);}
         state_win=1;  
         updateData(Device_ID, Sensor_ID_Win_State, state_win);
         Serial.println("send 1");
         state=1;
         }
      }

  /*开窗30min后关闭*/    
   if(HHMM==HHMM0+0.03||HHMM==HHMM0+0.20)  
    {  
      if(state==10||state==1)  //窗户关闭
      {for(pos=50;pos>=0;pos-=1)
        {myservo.write(pos2);            
          delay(30);}
       state_win=0;
       updateData(Device_ID, Sensor_ID_Win_State, state_win);
       Serial.println("send 0");
       state=0;
      }      
     }
   }
 }
}
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
  //Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)

  if (wifi.releaseTCP()) {            //释放TCP连接
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
  }
  postArray = NULL;       //清空数组，等待下次传输数据
}


/*从yeelink获取传感器三个温湿度值*/
float getValue(String Device_ID, String Sensor_ID) 
{
   String postString;    //用于存储发送数据的字符串
   String jsonToSend;    //用于存储发送的json格式参数

  uint8_t buffer[512] = {0};      //用于存储Responce的全部信息
  char infoData[60] = {0};        //用于存储Responce的关键信息

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print("create tcp ok\r\n");
  }else {
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
  //Serial.println(getArray);
  wifi.send((const uint8_t*)getArray, strlen(getArray));

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);   //接收长度函数
  if (len > 0) {          //如果接收到了
    for (uint32_t i = 0; i < len; i++) 
    {    //将所有的字符遍历一遍，找到接收信息的头尾
      if ((char)buffer[i] == '{') 
      {
        start = i;
      }
      if ((char)buffer[i] == '}') 
      {
        end = i;
      }
    }
        
    int index = 0;        //infoData数组的指针
    for (uint32_t i = start; i <= end; i++) {
      //存储Responce的关键信息一个个存在infoData数组里
      infoData[index] = (char)buffer[i];          //infoData[0]='{'      infoData[end-start+1]='}'  最后index=字符串长度
      index++;
    }
    Serial.print("Received:[");     //打印收到的信息
    Serial.print(infoData);
    Serial.print("]\r\n");

    char Data_char[5] = {0};
    int count = 0;
    for (uint32_t i = 43; i <= index - 2; i++){
      Data_char[count] =  infoData[i];
      count++;
     }
    return atof(Data_char);
  }

  if (wifi.releaseTCP()){
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
  }
  getArray = NULL;
}

/*获取当前时间*/
float getHHMM(String Device_ID, String Sensor_ID) 
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
  //Serial.println(getArray);
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
    for (uint32_t i = 25; i <= 29; i++){
      Data_char[count] =  infoData[i];
      count++;
    }
    Data_char[2] = '.';
    return atof(Data_char);
    Serial.print("Received:[");     //打印收到的信息
    Serial.print(atof(Data_char));
    Serial.print("]\r\n");   
  }

  if (wifi.releaseTCP()) {
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
  }
  getArray = NULL;
  Serial.print("Received:[");     //打印收到的信息
  Serial.print(atof(Data_char));
  Serial.print("]\r\n");
  return atof(Data_char);
}





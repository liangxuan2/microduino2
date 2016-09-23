package com.example.smart;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;

import org.apache.http.util.ByteArrayBuffer;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;


public class Tem_Hum extends Activity {


    public TextView tv1;
    public TextView tv2;
    public TextView tv3;
    public TextView tv4;

    Handler hd1 =new Handler(){
        public void dispatchMessage(Message msg1) {
            switch(msg1.what){
                case 1:
                    JSONObject jsonobj=null;
                    try {
                        jsonobj=new JSONObject(msg1.getData().getString("mystring"));
                        String timestamp=jsonobj.getString("timestamp");
                        String value=jsonobj.getString("value");
                        tv1.setText("时间:"+timestamp+"室内湿度:"+value+"%");
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    break;
                default:
                    break;
            }
        }
    };
    Handler hd2 =new Handler(){
        public void dispatchMessage(Message msg2) {
            switch(msg2.what){
                case 1:
                    JSONObject jsonobj=null;
                    try {
                        jsonobj=new JSONObject(msg2.getData().getString("mystring"));
                        String timestamp=jsonobj.getString("timestamp");
                        String value=jsonobj.getString("value");
                        tv2.setText("室内温度是"+value+"℃");
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    break;
                default:
                    break;

            }
        }
    };
    Handler hd3 =new Handler(){
        public void dispatchMessage(Message msg3) {
            switch(msg3.what){
                case 1:
                    JSONObject jsonobj=null;
                    try {
                        jsonobj=new JSONObject(msg3.getData().getString("mystring"));
                        String timestamp=jsonobj.getString("timestamp");
                        String value=jsonobj.getString("value");
                        tv3.setText("室外湿度是"+value+"%");
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    break;
                default:
                    break;

            }
        }
    };
    Handler hd4 =new Handler(){
        public void dispatchMessage(Message msg4) {
            switch(msg4.what){
                case 1:
                    JSONObject jsonobj=null;
                    try {
                        jsonobj=new JSONObject(msg4.getData().getString("mystring"));
                        String timestamp=jsonobj.getString("timestamp");
                        String value=jsonobj.getString("value");
                        tv4.setText("室外温度是"+value+"℃");
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    break;
                default:
                    break;

            }
        }
    };



    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.hum_tem);

        tv1 = (TextView)findViewById(R.id.TV1);
        tv2 = (TextView)findViewById(R.id.TV2);
        tv3 = (TextView)findViewById(R.id.TV3);
        tv4 = (TextView)findViewById(R.id.TV4);


        Thread td1 =new Thread(){
            String myString;
            @Override
            public void run() {
                //获取信息
                try {
                    URL uri = new URL("http://api.yeelink.net/v1.0/device/350487/sensor/393578/datapoints");//注意，这里的URL地址必须为网络地址，
                    URLConnection ucon = uri.openConnection();
                    InputStream is = ucon.getInputStream();
                    BufferedInputStream bis = new BufferedInputStream(is);
                    ByteArrayBuffer baf = new ByteArrayBuffer(100);
                    int current = 0;
                    while((current = bis.read()) != -1) {
                        baf.append((byte)current);
                    }

                    myString = new String(baf.toByteArray(), "GBK");
                    //异常处理
                } catch(Exception e) {
                    myString = e.getMessage();
                }
                //打包信息
                Message msg1=new Message();
                Bundle bundle = new Bundle();
                bundle.putString("mystring", myString);
                msg1.setData(bundle);
                msg1.what=1;
                hd1.sendMessage(msg1);
            }

        };
        td1.start();
        Thread td2 =new Thread(){
            String myString;
            @Override
            public void run() {
                //获取信息
                try {
                    URL uri = new URL("http://api.yeelink.net/v1.0/device/350487/sensor/393577/datapoints");//注意，这里的URL地址必须为网络地址，
                    URLConnection ucon = uri.openConnection();
                    InputStream is = ucon.getInputStream();
                    BufferedInputStream bis = new BufferedInputStream(is);
                    ByteArrayBuffer baf = new ByteArrayBuffer(100);
                    int current = 0;
                    while((current = bis.read()) != -1) {
                        baf.append((byte)current);
                    }

                    myString = new String(baf.toByteArray(), "GBK");
                    //异常处理
                } catch(Exception e) {
                    myString = e.getMessage();
                }
                //打包信息
                Message msg2=new Message();
                Bundle bundle = new Bundle();
                bundle.putString("mystring", myString);
                msg2.setData(bundle);
                msg2.what=1;
                hd2.sendMessage(msg2);
            }

        };
        td2.start();
        Thread td3 =new Thread(){
            String myString;
            @Override
            public void run() {
                //获取信息
                try {
                    URL uri = new URL("http://api.yeelink.net/v1.0/device/350487/sensor/393350/datapoints");//注意，这里的URL地址必须为网络地址，
                    URLConnection ucon = uri.openConnection();
                    InputStream is = ucon.getInputStream();
                    BufferedInputStream bis = new BufferedInputStream(is);
                    ByteArrayBuffer baf = new ByteArrayBuffer(100);
                    int current = 0;
                    while((current = bis.read()) != -1) {
                        baf.append((byte)current);
                    }

                    myString = new String(baf.toByteArray(), "GBK");
                    //异常处理
                } catch(Exception e) {
                    myString = e.getMessage();
                }
                //打包信息
                Message msg3=new Message();
                Bundle bundle = new Bundle();
                bundle.putString("mystring", myString);
                msg3.setData(bundle);
                msg3.what=1;
                hd3.sendMessage(msg3);
            }

        };
        td3.start();
        Thread td4 =new Thread(){
            String myString;
            @Override
            public void run() {
                //获取信息
                try {
                    URL uri = new URL("http://api.yeelink.net/v1.0/device/350487/sensor/393349/datapoints");//注意，这里的URL地址必须为网络地址，
                    URLConnection ucon = uri.openConnection();
                    InputStream is = ucon.getInputStream();
                    BufferedInputStream bis = new BufferedInputStream(is);
                    ByteArrayBuffer baf = new ByteArrayBuffer(100);
                    int current = 0;
                    while((current = bis.read()) != -1) {
                        baf.append((byte)current);
                    }

                    myString = new String(baf.toByteArray(), "GBK");
                    //异常处理
                } catch(Exception e) {
                    myString = e.getMessage();
                }
                //打包信息
                Message msg4=new Message();
                Bundle bundle = new Bundle();
                bundle.putString("mystring", myString);
                msg4.setData(bundle);
                msg4.what=1;
                hd4.sendMessage(msg4);
            }
        };
        td4.start();
    }
}


package com.example.smart;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.Toast;

public class Adjust extends Activity {

    private EditText TemText;
    private EditText HumText;
    String response1;


    public Handler hd=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case 0:
                    Toast.makeText(Adjust.this,response1,Toast.LENGTH_LONG).show();
                    break;


            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.adjust);
        TemText=(EditText)findViewById(R.id.Tem);
        HumText=(EditText)findViewById(R.id.Hum);

        Button uploadbtn=(Button)findViewById(R.id.save);



        uploadbtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {
                Thread td2=new Thread(){
                    public void run(){
                        //发送温度值
                        String tem="{\"timestamp\":\"2016-09-18T16:13:14\",\"value\":"+TemText.getText().toString()+"}";
                        System.out.println(tem);
                        response1=GetPostUtil.sendPost("http://api.yeelink.net/v1.0/device/350487/sensor/393753/datapoints",tem);

                        //发送湿度值
                        String hum="{\"timestamp\":\"2016-09-18T16:13:14\",\"value\":"+HumText.getText().toString()+"}";
                        response1=GetPostUtil.sendPost("http://api.yeelink.net/v1.0/device/350487/sensor/394104/datapoints", hum);
                        //todo
                    }
                };
                td2.start();

                hd.sendEmptyMessage(0);

            }
        });
       }}








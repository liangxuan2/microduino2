package com.example.smart;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		Button btn1=(Button)findViewById(R.id.btn1);
				btn1.setOnClickListener(new View.OnClickListener() {
					
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub
						Intent intent1=new Intent(MainActivity.this,Tem_Hum.class );
						 startActivity(intent1);
					}
				});
				Button btn2=(Button)findViewById(R.id.btn2);
		        btn2.setOnClickListener(new View.OnClickListener() {
		            @Override
		            public void onClick(View v) {
		                Intent intent2=new Intent(MainActivity.this,Adjust.class);
		                startActivity(intent2);
		            }

		        });
		
				
				
	}
}

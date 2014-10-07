package com.clipshare.csclientui;

import com.clipshare.csclient.R;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class CSClientMain extends ActionBarActivity {
	
	private EditText etIPAddress = null;
	private Button btConnect = null;
	private Button btDisconnect = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_csclient_main);
		
		etIPAddress = (EditText)findViewById(R.id.etCSCMIPAddress);
		btConnect = (Button)findViewById(R.id.btCSCMConnect);
		btDisconnect = (Button)findViewById(R.id.btCSCMDisconnect);
		
		btConnect.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				String ipAddress = etIPAddress.getText().toString();
				if(ipAddress != null && !ipAddress.isEmpty()) {
					validateIpAddress(ipAddress);
				}
			}
		});
		
		btDisconnect.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				
			}
		});
	}
	
	private boolean validateIpAddress(String ipAddress) {
		return true;
	}

}

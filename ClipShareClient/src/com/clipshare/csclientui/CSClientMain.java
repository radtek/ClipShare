package com.clipshare.csclientui;

import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.clipshare.csclient.R;
import com.clipshare.csserverconn.ServerConnectorProxy;

public class CSClientMain extends ActionBarActivity implements View.OnClickListener {
	
	private EditText etIPAddress = null;
	private Button btConnect = null;
	private Button btDisconnect = null;
	
	private ServerConnectorProxy connectorProxy = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_csclient_main);
		
		connectorProxy = new ServerConnectorProxy(this);
		
		initControls();
	}
	
	private void initControls() {
		etIPAddress = (EditText)findViewById(R.id.etCSCMIPAddress);
		btConnect = (Button)findViewById(R.id.btCSCMConnect);
		btDisconnect = (Button)findViewById(R.id.btCSCMDisconnect);
		
		btConnect.setOnClickListener(this);
		btDisconnect.setOnClickListener(this);
		
		btDisconnect.setEnabled(false);
	}
	
	private boolean validateIpAddress(String ipAddress) {
		String[] ipOctets = ipAddress.split("\\.");
		if(ipOctets == null || ipOctets.length < 4)
			return false;
		
		for (String octet : ipOctets) {
			try {
				int octetVal = Integer.parseInt(octet);
				if(octetVal < 0 || octetVal > 255)
					return false;
			} catch (NumberFormatException nfe) {
				return false;
			}
		}
		
		return true;
	}
	
	private void setAllControlsStatus(boolean forConnect) {
		btConnect.setEnabled(forConnect);
		etIPAddress.setEnabled(forConnect);
		btDisconnect.setEnabled(!forConnect);
	}

	@Override
	public void onClick(View v) {
		switch(v.getId()) {
			case R.id.btCSCMConnect:if(validateIpAddress(etIPAddress.getText().toString())) {
										connectorProxy.handleConnectRequest(etIPAddress.getText().toString());
										setAllControlsStatus(false);
									} else {
										Toast.makeText(getApplicationContext(), "Invalid IP address. Please check and try again.", Toast.LENGTH_LONG).show();
									}
								    break;
			case R.id.btCSCMDisconnect:setAllControlsStatus(true);
									   break;
		}
	}

}

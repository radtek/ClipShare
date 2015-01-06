package com.clipshare.csserverconn;

import com.clipshare.common.Constants;
import com.clipshare.csclientservice.CSClientService;

import android.app.Activity;
import android.content.*;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;


public class ServerConnectorProxy {
	
	private Handler serviceMsgHandler = null;
	private Intent serviceIntent = null;
	
	private Activity parentActivity = null; 
	
	public ServerConnectorProxy(Activity mainActivity) {
		serviceMsgHandler = new ServiceMessageHandler();
		serviceIntent = new Intent(mainActivity.getApplicationContext(), CSClientService.class);
		
		parentActivity = mainActivity;
	}
	
	public void handleConnectRequest(String uiIpAddress) {
		if(parentActivity != null && !CSClientService.isRunning()) {
			serviceIntent.putExtra(Constants.IP_KEY, uiIpAddress);
			
			Messenger messenger = new Messenger(serviceMsgHandler);
			serviceIntent.putExtra(Constants.MESSENGER_KEY, messenger);
			
			parentActivity.startService(serviceIntent);
		}
	}
	
	public void handleDisconnectRequest() {
		if(CSClientService.isRunning())
			parentActivity.stopService(serviceIntent);
	}
	
	private class ServiceMessageHandler extends Handler {
		@Override
		public void handleMessage(Message message) {
			
		}
	}
}

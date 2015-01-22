package com.clipshare.csserverconn;

import com.clipshare.common.Constants;
import com.clipshare.csclientservice.CSClientService;
import com.clipshare.csclientui.CSClientMain;

import android.app.Activity;
import android.content.*;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.widget.Toast;


public class ServerConnectorProxy {
	
	private Handler serviceMsgHandler = null;
	private Intent serviceIntent = null;
	
	private CSClientMain parentActivity = null; 
	
	public ServerConnectorProxy(CSClientMain mainActivity) {
		serviceMsgHandler = new ServiceMessageHandler();
		serviceIntent = new Intent(CSClientService.class.getName());
		
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
			int messageValue = message.getData().getInt(Constants.SERVICE_MSG_KEY);
			switch(messageValue) {
				case Constants.SERVICE_MSG_SERVICE_STOP: parentActivity.handleServiceStop();
														 break;
			}
		}
	}
}

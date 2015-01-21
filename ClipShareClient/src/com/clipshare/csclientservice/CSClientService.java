package com.clipshare.csclientservice;

import com.clipshare.common.Constants;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Messenger;

public class CSClientService extends Service {
	
	private static CSClientService thisService = null;
	
	private String ipAddress = null;
	private Messenger messenger = null;
	private ConnCreator connCreator = null;
	
	public CSClientService() {
		
	}
	
	@Override
	public void onCreate() {
		connCreator = new ConnCreator(this);
		
		thisService = this;
		
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if(connCreator != null && !connCreator.isRunning()) {			
			messenger = (Messenger)intent.getExtras().get(Constants.MESSENGER_KEY);
			ipAddress = (String)intent.getExtras().getString(Constants.IP_KEY);
			
			connCreator.setIp(ipAddress);
			connCreator.setMessenger(messenger);
			connCreator.startThread();
		}
		
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		if(connCreator != null) {
			if(connCreator.isRunning())
				connCreator.stopThread(false);
			
			connCreator = null;
		}
		
		thisService = null;
		
		super.onDestroy();
	}
	
	public void destroy() {
		stopSelf();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	public static boolean isRunning() {
		if(thisService == null)
			return false;
		
		return thisService.connCreator.isRunning();
	}

}

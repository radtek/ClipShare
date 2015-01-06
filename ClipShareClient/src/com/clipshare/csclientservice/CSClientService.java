package com.clipshare.csclientservice;

import com.clipshare.common.Constants;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Messenger;

public class CSClientService extends Service {
	
	private static boolean isRunning = false;
	
	private String ipAddress = null;
	private Messenger messenger = null;
	
	public CSClientService() {
		
	}
	
	@Override
	public void onCreate() {
		isRunning = false;
		
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if(!isRunning) {
			ipAddress = (String)intent.getExtras().getString(Constants.IP_KEY);
			messenger = (Messenger)intent.getExtras().get(Constants.MESSENGER_KEY);
			
			isRunning = true;
		}
	
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		isRunning = false;
		
		super.onDestroy();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	public static boolean isRunning() {
		return isRunning;
	}
}

package com.clipshare.csclientservice;

import com.clipshare.csserverconn.ServerConnectorProxy;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class CSClientService extends Service {
	
	private ServerConnectorProxy serverConnectorProxy = null;
	
	public CSClientService() {
		
	}
	
	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// TODO Auto-generated method stub
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	
}

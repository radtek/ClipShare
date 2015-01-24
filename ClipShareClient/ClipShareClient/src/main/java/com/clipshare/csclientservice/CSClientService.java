package com.clipshare.csclientservice;

import com.clipshare.common.Constants;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

public class CSClientService extends Service {
	
	private static CSClientService thisService = null;
	
	private String ipAddress = null;
	private Messenger messenger = null;
	private ConnCreator connCreator = null;
	
	public CSClientService() {
		
	}
	
	@Override
	public void onCreate() {
		thisService = this;
		
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		synchronized (thisService) {
			if(connCreator == null)
				connCreator = new ConnCreator(thisService);
			if(!connCreator.isRunning()) {
				messenger = (Messenger)intent.getExtras().get(Constants.MESSENGER_KEY);
				ipAddress = (String)intent.getExtras().getString(Constants.IP_KEY);
				
				connCreator.setIp(ipAddress);
				connCreator.setMessenger(messenger);
				connCreator.startThread();
			}
		}
		
		return super.onStartCommand(intent, flags, startId);
	}
	
	/*what if both service stop and stopThread of ConnCreator are called at close intervals? */
	@Override
	public void onDestroy() {
		if(connCreator != null) {
			if(connCreator.isRunning())
				connCreator.stopThread(false);
			
			connCreator = null;
		}
		
		thisService = null;
		
		sendServiceStopToActivity();
		
		super.onDestroy();
	}
	
	private void sendServiceStopToActivity() {
		Message serviceStopMsg= Message.obtain();
		
		Bundle bundle = new Bundle();
		bundle.putInt(Constants.SERVICE_MSG_KEY, Constants.SERVICE_MSG_VAL_SERVICE_STOP);
		
		serviceStopMsg.setData(bundle);
		
		try {
			messenger.send(serviceStopMsg);
		} catch (RemoteException re) {
	
		}
	}
	
	public void destroy() {
		stopSelf();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	public static boolean isRunning() {
		if(thisService == null || thisService.connCreator == null)
			return false;
		
		return thisService.connCreator.isRunning();
	}

}

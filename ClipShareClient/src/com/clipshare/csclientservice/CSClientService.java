package com.clipshare.csclientservice;

import java.util.concurrent.Semaphore;

import com.clipshare.common.Constants;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Messenger;

public class CSClientService extends Service {
	
	private static CSClientService thisService = null;
	
	private Semaphore serviceStopSemaphore = null;
	private ServiceStopListener serviceStopListener = null;
	
	private String ipAddress = null;
	private Messenger messenger = null;
	private ConnCreator connCreator = null;
	
	public CSClientService() {
		
	}
	
	@Override
	public void onCreate() {
		serviceStopSemaphore = new Semaphore(-1);
		
		serviceStopListener = new ServiceStopListener(serviceStopSemaphore);
		connCreator = new ConnCreator(serviceStopSemaphore);
		
		thisService = this;
		
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if(connCreator != null && !connCreator.isRunning()) {			
			messenger = (Messenger)intent.getExtras().get(Constants.MESSENGER_KEY);
			ipAddress = (String)intent.getExtras().getString(Constants.IP_KEY);
			
			Thread serviceStopListenerThread = new Thread(serviceStopListener);
			serviceStopListenerThread.start();
			
			connCreator.setIp(ipAddress);
			connCreator.startThread();
		}
		
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		if(connCreator != null) {
			if(connCreator.isRunning())
				connCreator.stopThread();
			
			connCreator = null;
		}
		
		thisService = null;
		
		serviceStopListener = null;
		serviceStopSemaphore = null;
		
		super.onDestroy();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	public static boolean isRunning() {
		return thisService.connCreator.isRunning();
	}
	
	private class ServiceStopListener implements Runnable {
		
		private Semaphore stopSemaphore = null;
		
		public ServiceStopListener(Semaphore semaphore) {
			stopSemaphore = semaphore;
		}
		
		@Override
		public void run() {
			try {
				stopSemaphore.acquire();
				CSClientService.this.stopSelf();
			} catch (InterruptedException e) {
				
			}
		}
	}
}

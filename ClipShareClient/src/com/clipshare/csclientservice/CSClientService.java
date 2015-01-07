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
	
	private String ipAddress = null;
	private Messenger messenger = null;
	private ConnCreator connCreator = null;
	
	public CSClientService() {
		
	}
	
	@Override
	public void onCreate() {
		connCreator = null;
		thisService = this;
		serviceStopSemaphore = null;
		
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if(!connCreator.isRunning()) {
			ipAddress = (String)intent.getExtras().getString(Constants.IP_KEY);
			messenger = (Messenger)intent.getExtras().get(Constants.MESSENGER_KEY);
			
			serviceStopSemaphore = new Semaphore(-1);
			
			Thread serviceStopListenerThread = new Thread(new ServiceStopListener(serviceStopSemaphore));
			serviceStopListenerThread.start();

			connCreator = new ConnCreator(ipAddress, serviceStopSemaphore);
			connCreator.startThread();
		}
	
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		if(connCreator != null) {
			if(!connCreator.isRunning())
				connCreator.stopThread();
			
			connCreator = null;
		}
		
		thisService = null;
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
				//
			}
		}
	}
}

package com.clipshare.csclientservice;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

import android.os.Bundle;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

import com.clipshare.common.Constants;

public class ConnCreator implements Runnable {
	
	private boolean isRunning = false;
	private String ipAddress = null;
	private Messenger messenger = null;
	
	private Socket clientSocket = null;
	private Thread connCreatorThread = null;
	private CSClientService parentService = null;
	
	public ConnCreator(CSClientService service) {
		parentService = service;
	}
	
	private void sendMessage(String mainKey, int mainVal, HashMap<String, String> extras) {
		Message message = Message.obtain();
		
		Bundle bundle = new Bundle();
		
		bundle.putInt(mainKey, mainVal);
		if(extras != null && extras.size() > 0) {
			Bundle extrasBundle = new Bundle();
			
			Set<String> extrasKeySet = extras.keySet();
			Iterator<String> extrasKeySetIterator = extrasKeySet.iterator();
			while(extrasKeySetIterator.hasNext()) {
				String curKey = extrasKeySetIterator.next();
				extrasBundle.putString(curKey, extras.get(curKey));
			}
			
			bundle.putBundle(Constants.SERVICE_MSG_EXTRAS_KEY,extrasBundle); 
		}
		
		message.setData(bundle);
		
		try {
			messenger.send(message);
		} catch (RemoteException re) {
			
		}
	}
	
	public void run() {
		try {
			clientSocket = new Socket();
			
			clientSocket.connect(new InetSocketAddress(ipAddress, Constants.SERVER_PORT), Constants.SERVER_CONNECT_TIMEOUT_MS);
			if(clientSocket.isConnected())
				clientSocket.close();
		} catch (UnknownHostException uhe) {
			
		} catch (IOException ioe) {
			sendMessage(Constants.SERVICE_MSG_KEY, Constants.SERVICE_MSG_VAL_HOST_NOT_FOUND, null);
		} finally {
			stopThread(true);
		}
	}
	
	public void setIp(String ip) {
		ipAddress = ip;
	}
	
	public String getIp() {
		return ipAddress;
	}
	
	public void setMessenger(Messenger msngr) {
		messenger = msngr;
	}
	
	public Messenger getMessenge() {
		return messenger;
	}
	
	public synchronized void startThread() {
		if(connCreatorThread == null)
			connCreatorThread = new Thread(this);
		if(!connCreatorThread.isAlive())
			connCreatorThread.start();
		
		isRunning = true;
	}
	
	public synchronized void stopThread(boolean stopService) {
		if(connCreatorThread != null)
			connCreatorThread = null;
		
		isRunning = false;
		
		if(stopService)
			parentService.destroy();
	}
	
	public boolean isRunning() {
		return isRunning;
	}
}

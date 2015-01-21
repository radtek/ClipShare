package com.clipshare.csclientservice;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.concurrent.Semaphore;

import android.os.Bundle;
import android.os.Message;
import android.os.Messenger;

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
	
	public void run() {
		try {
			clientSocket = new Socket(InetAddress.getByName(ipAddress), Constants.SERVER_PORT);
			if(clientSocket.isConnected()) {	
				clientSocket.close();
			}			
		} catch (UnknownHostException uhe) {
			
		} catch (IOException ioe) {
			
		} finally {
			stopThread(true);
		}
	}
	
	public void setIp(String ip) {
		ipAddress = ip;
	}
	
	public void setMessenger(Messenger msngr) {
		messenger = msngr;
	}
	
	public void startThread() {
		connCreatorThread = new Thread(this);
		connCreatorThread.start();
		
		isRunning = true;
	}
	
	public void stopThread(boolean stopService) {
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

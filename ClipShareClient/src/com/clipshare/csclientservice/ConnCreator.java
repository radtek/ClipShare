package com.clipshare.csclientservice;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.concurrent.Semaphore;

public class ConnCreator implements Runnable {
	
	private boolean isRunning = false;
	private String ipAddress = null;
	private Semaphore stopSemaphore = null;
	
	private Socket clientSocket = null;
	private Thread connCreatorThread = null;
	
	public ConnCreator(String ip, Semaphore semaphore) {
		ipAddress = ip;
		stopSemaphore = semaphore;
	}
	
	public void run() {
		try {
			clientSocket = new Socket(InetAddress.getByName(ipAddress), 1111);
			if(!clientSocket.isConnected())
				return;
			
		} catch (UnknownHostException uhe) {
			//
		} catch (IOException ioe) {
			//
		} finally {
			try {
				clientSocket.close();
				stopThread();
			} catch (IOException e) {
				//
			}
		}
	}
	
	public void startThread() {
		connCreatorThread = new Thread(this);
		connCreatorThread.start();
		
		isRunning = true;
	}
	
	public void stopThread() {
		if(connCreatorThread != null)
			connCreatorThread = null;
		
		stopSemaphore.release();
		isRunning = false;
	}
	
	public boolean isRunning() {
		return isRunning;
	}
}

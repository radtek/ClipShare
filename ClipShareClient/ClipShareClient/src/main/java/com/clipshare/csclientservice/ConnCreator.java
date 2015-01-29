package com.clipshare.csclientservice;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;
import java.util.Timer;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeoutException;

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
    private Thread senderThread = null;
    private Thread receiverThread = null;

    public static boolean DISCONNECTING = false;

    private static final String HANDSHAKE_MSG = "HELLO";
    private static final int READ_TIMEOUT = 10;
	
	public ConnCreator() {

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

    private int performHandshake(Socket client) {
        try {
            BufferedReader recvReader = new BufferedReader(new InputStreamReader(client.getInputStream()));
            String serverHandshake = recvReader.readLine();
            if(serverHandshake.compareTo(HANDSHAKE_MSG) != 0)
                return 1;
        } catch (IOException ioe) {
            return 1;
        } catch (Exception toe) {
            return 1;
        }

        try {
            PrintWriter sendWriter = new PrintWriter(new OutputStreamWriter(client.getOutputStream()));
            sendWriter.write(HANDSHAKE_MSG + "\n");
            sendWriter.flush();
        } catch (IOException ioe) {
            return 2;
        }

        return 0;
    }
	
	public void run() {
        boolean connected = false;
        clientSocket = new Socket();

        try {
            clientSocket.connect(new InetSocketAddress(ipAddress, Constants.SERVER_PORT), Constants.SERVER_CONNECT_TIMEOUT_MS);
            clientSocket.setSoTimeout(READ_TIMEOUT * 1000);

            if(performHandshake(clientSocket) == 0) {
                connected = true;
            } else {
                HashMap<String, String> errorMsgMap = new HashMap<String, String>();
                errorMsgMap.put(Constants.SERVICE_MSG_ERROR_KEY, Constants.SERVICE_MSG_ERROR_HANDSHAKE_TEXT);
                sendMessage(Constants.SERVICE_MSG_KEY, Constants.SERVICE_MSG_VAL_ERROR, errorMsgMap);
            }
        } catch (UnknownHostException uhe) {

        } catch (IOException ioe) {
            sendMessage(Constants.SERVICE_MSG_KEY, Constants.SERVICE_MSG_VAL_HOST_NOT_FOUND, null);
        }

        if(connected) {
            Semaphore stopSemaphore = new Semaphore(0);

            if (clientSocket.isConnected()) {
                senderThread = new Thread(new ClientSender(clientSocket, messenger, stopSemaphore));
                senderThread.start();

                receiverThread = new Thread(new ClientReceiver(clientSocket, messenger, stopSemaphore));
                receiverThread.start();
            }

            try {
                stopSemaphore.acquire();
            } catch (InterruptedException ie) {

            }
        }

        if(!DISCONNECTING)
            stopThread();
	}
	
	public void setIp(String ip) {
		ipAddress = ip;
	}

	public void setMessenger(Messenger msngr) {
		messenger = msngr;
	}

	public synchronized void startThread() {
        DISCONNECTING = false;

		if(connCreatorThread == null)
			connCreatorThread = new Thread(this);
		if(!connCreatorThread.isAlive())
			connCreatorThread.start();

		isRunning = true;
	}
	
	public synchronized void stopThread() {
        if(isRunning) {
            DISCONNECTING = true;

            //need to add timeout
            if (senderThread != null)
                while (senderThread.isAlive()) ;
            if (receiverThread != null)
                while (receiverThread.isAlive()) ;

            if (clientSocket.isConnected())
                try {
                    clientSocket.close();
                } catch (IOException ioe) {

                }

            connCreatorThread = null;
            senderThread = null;
            receiverThread = null;

            isRunning = false;

            sendServiceStopToActivity();
        }
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
	
	public boolean isRunning() {
		return isRunning;
	}
}

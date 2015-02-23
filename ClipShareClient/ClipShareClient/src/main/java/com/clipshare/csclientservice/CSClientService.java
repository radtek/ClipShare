package com.clipshare.csclientservice;

import com.clipshare.common.Constants;

import android.app.Service;
import android.content.ClipboardManager;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

public class CSClientService extends Service {

	private String ipAddress = null;
	private Messenger serverMessenger = null;
	private ConnCreator connCreator = null;

    private ClipboardManager clipboardManager = null;

    private final Messenger commandMessenger = new Messenger(new ServiceCommandHandler());
	
	public CSClientService() {

	}
	
	@Override
	public void onCreate() {
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		return super.onStartCommand(intent, flags, startId);
	}

    private synchronized void start() {
        if(clipboardManager == null)
            clipboardManager = (ClipboardManager)getSystemService(CLIPBOARD_SERVICE);

        if(clipboardManager != null) {
            if (connCreator == null)
                connCreator = new ConnCreator();
            if (!connCreator.isRunning()) {
                connCreator.setIp(ipAddress);
                connCreator.setMessenger(serverMessenger);
                connCreator.setClipboardManager(clipboardManager);
                connCreator.startThread();
            }

            Intent intent = new Intent(Constants.SERVICE_STATUS_BR_INTENT_NAME);
            intent.putExtra(Constants.SERVICE_STATUS_KEY, 1);
            sendBroadcast(intent);
        }
    }

    //this is not called when the connection is terminated, hence connCreator is not set to null in that case. Need to fix.
    public void stop() {
        if(clipboardManager != null) {
            if (connCreator != null) {
                if (connCreator.isRunning())
                    connCreator.stopThread();

                connCreator = null;
            }

            Intent intent = new Intent(Constants.SERVICE_STATUS_BR_INTENT_NAME);
            intent.putExtra(Constants.SERVICE_STATUS_KEY, 0);
            sendBroadcast(intent);
        }
    }
	
	@Override
	public void onDestroy() {
        stop();

		super.onDestroy();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return commandMessenger.getBinder();
	}

    public class ServiceCommandHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Bundle data = msg.getData();

            switch(data.getInt(Constants.SERVICE_COMMAND_KEY)) {
                case Constants.SERVICE_COMMAND_VAL_SETIP: ipAddress = data.getString(Constants.SERVICE_COMMAND_SETIP_KEY);
                        break;
                case Constants.SERVICE_COMMAND_VAL_SETMESSENGER: serverMessenger = (Messenger)data.getParcelable(Constants.SERVICE_COMMAND_SETMESSENGER_KEY);
                        break;
                case Constants.SERVICE_COMMAND_VAL_START: start();
                        break;
                case Constants.SERVICE_COMMAND_VAL_STOP: stop();
                        break;
            }
        }
    }
}

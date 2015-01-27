package com.clipshare.csserverconn;

import com.clipshare.common.Constants;
import com.clipshare.csclientservice.CSClientService;
import com.clipshare.csclientui.CSClientMain;

import android.app.Service;
import android.content.*;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;

public class ServerConnectorProxy {

    private String ipAddress = null;

    private Messenger serviceCmdMessenger = null;
	private Handler serviceMsgHandler = null;
	private Intent serviceIntent = null;
    private ServiceConnection serviceConnection = null;
	
	private CSClientMain parentActivity = null;
	
	public ServerConnectorProxy(CSClientMain mainActivity) {
		serviceMsgHandler = new ServiceMessageHandler();
		serviceIntent = new Intent(CSClientService.class.getName());

        serviceConnection = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                serviceCmdMessenger = new Messenger(service);

                startService();
            }

            @Override
            public void onServiceDisconnected(ComponentName name) {
                serviceCmdMessenger = null;
            }
        };
		
		parentActivity = mainActivity;
	}

    private void startService() {
        sendCommandToService(Constants.SERVICE_COMMAND_VAL_SETIP, ipAddress);
        sendCommandToService(Constants.SERVICE_COMMAND_VAL_SETMESSENGER, null);
        sendCommandToService(Constants.SERVICE_COMMAND_VAL_START, null);
    }

    private void sendCommandToService(int cmd, Object payload) {
        Message message = Message.obtain();
        Bundle bundle = new Bundle();

        switch(cmd) {
            case Constants.SERVICE_COMMAND_VAL_SETIP: bundle.putInt(Constants.SERVICE_COMMAND_KEY, cmd);
                                                      String ip = (String)payload;
                                                      bundle.putString(Constants.SERVICE_COMMAND_SETIP_KEY, ip);
                                                      break;
            case Constants.SERVICE_COMMAND_VAL_SETMESSENGER: Messenger serviceMessenger = new Messenger(serviceMsgHandler);
                                                             bundle.putInt(Constants.SERVICE_COMMAND_KEY, cmd);
                                                             bundle.putParcelable(Constants.SERVICE_COMMAND_SETMESSENGER_KEY, serviceMessenger);
                                                             break;
            case Constants.SERVICE_COMMAND_VAL_START: bundle.putInt(Constants.SERVICE_COMMAND_KEY, cmd);
                                                      break;
            case Constants.SERVICE_COMMAND_VAL_STOP: bundle.putInt(Constants.SERVICE_COMMAND_KEY, cmd);
                                                     break;
        }

        try {
            message.setData(bundle);
            serviceCmdMessenger.send(message);
        } catch (RemoteException re) {

        }
    }
	
	public void handleConnectRequest(String uiIpAddress) {
		if(parentActivity != null && !CSClientService.isRunning()) {
            ipAddress = uiIpAddress;
            parentActivity.bindService(serviceIntent, serviceConnection, Context.BIND_AUTO_CREATE);
		}
	}
	
	public void handleDisconnectRequest() {
		if(CSClientService.isRunning()) {
            sendCommandToService(Constants.SERVICE_COMMAND_VAL_STOP, null);
        }
	}

	private class ServiceMessageHandler extends Handler {
		@Override
		public void handleMessage(Message message) {
			int messageValue = message.getData().getInt(Constants.SERVICE_MSG_KEY);
			switch(messageValue) {
				case Constants.SERVICE_MSG_VAL_SERVICE_STOP:parentActivity.unbindService(serviceConnection);
                                                            parentActivity.handleServiceStop();
														    break;
				case Constants.SERVICE_MSG_VAL_HOST_NOT_FOUND: Toast.makeText(parentActivity, Constants.HOST_NOT_FOUND_TEXT, Toast.LENGTH_LONG).show();
														   break;
			}
		}
	}
}

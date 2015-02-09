package com.clipshare.csclientservice;

import android.os.Bundle;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

import com.clipshare.common.Constants;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.Semaphore;

public class ClientReceiver implements Runnable {

    private Socket client = null;
    private Messenger messenger = null;
    private Semaphore semaphore = null;

    public ClientReceiver(Socket clientSocket, Messenger serviceMessenger, Semaphore stopSemaphore) {
        client = clientSocket;
        messenger = serviceMessenger;
        semaphore = stopSemaphore;
    }

    private void reportError(String errorText) {
        Message message = Message.obtain();

        Bundle bundle = new Bundle();
        bundle.putInt(Constants.SERVICE_MSG_KEY, Constants.SERVICE_MSG_VAL_CONNECTION_TERM);
        bundle.putString(Constants.SERVICE_MSG_ERROR_KEY, errorText);

        message.setData(bundle);
        try {
            messenger.send(message);
        } catch (RemoteException re) {

        }
    }

    public void run() {
        try {
            BufferedReader clientReader = new BufferedReader(new InputStreamReader(client.getInputStream()));

            while(!ConnCreator.DISCONNECTING) {
                client.setSoTimeout(Constants.CONNECTION_READ_TIMEOUT_S * 1000);

                try {
                    int inCmd = clientReader.read();

                    if(inCmd == -1) {
                        reportError(Constants.SERVICE_MSG_CONNECTION_TERM_CLOSED_TEXT);
                        break;
                    }
                    char inMsgType = (char)inCmd;

                    if (inMsgType == Constants.CONNECTION_ALIVE_MSG) {
                        continue;
                    } else if(inMsgType == Constants.CONNECTION_DATA_MSG) {

                    } else if(inMsgType == Constants.CONNECTION_END_MSG) {

                    } else {
                        //
                    }
                } catch (SocketTimeoutException ste) {
                    reportError(Constants.SERVICE_MSG_CONNECTION_TERM_TIMEOUT_TEXT);
                    break;
                } catch (IOException ioe) {
                    reportError(Constants.SERVICE_MSG_CONNECTION_TERM_IO_TEXT);
                    break;
                }
            }

            semaphore.release();
        } catch (IOException ioe) {

        }
    }

}

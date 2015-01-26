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

    public void run() {
        try {
            BufferedReader clientReader = new BufferedReader(new InputStreamReader(client.getInputStream()));

            while(!ConnCreator.DISCONNECTING) {

            }

            semaphore.release();
        } catch (IOException ioe) {

        }
    }

}

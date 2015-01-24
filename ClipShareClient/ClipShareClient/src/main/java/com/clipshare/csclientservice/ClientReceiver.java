package com.clipshare.csclientservice;

import android.os.Messenger;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
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

            while(!ConnCreator.DISCONNECTING && client.isConnected()) {

            }

            semaphore.release();
        } catch (IOException ioe) {

        }
    }

}

package com.clipshare.csclientservice;

import android.os.Messenger;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.Semaphore;

public class ClientSender implements Runnable {

    private Socket client = null;
    private Messenger messenger = null;
    private Semaphore semaphore = null;

    public ClientSender(Socket clientSocket, Messenger serviceMessenger, Semaphore stopSemaphore) {
        client = clientSocket;
        messenger = serviceMessenger;
        semaphore = stopSemaphore;
    }

    public void run() {
        try {
            PrintWriter clientSender = new PrintWriter(new OutputStreamWriter(client.getOutputStream()));

            while(!ConnCreator.DISCONNECTING && client.isConnected()) {

            }

            semaphore.release();
        } catch (IOException ioe) {

        }
    }
}

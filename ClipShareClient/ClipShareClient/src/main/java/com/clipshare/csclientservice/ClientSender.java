package com.clipshare.csclientservice;

import android.content.ClipData;
import android.content.ClipDescription;
import android.content.ClipboardManager;
import android.os.Messenger;
import android.util.Log;

import com.clipshare.common.Constants;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.Semaphore;

public class ClientSender implements Runnable {

    private Socket client = null;
    private Messenger messenger = null;
    private Semaphore semaphore = null;
    private ClipboardManager clipboardManager = null;

    public ClientSender(Socket clientSocket, Messenger serviceMessenger, Semaphore stopSemaphore, ClipboardManager clipManager) {
        client = clientSocket;
        messenger = serviceMessenger;
        semaphore = stopSemaphore;
        clipboardManager = clipManager;
    }

    public void run() {
        try {
            final PrintWriter clientSender = new PrintWriter(new OutputStreamWriter(client.getOutputStream()));

            clipboardManager.addPrimaryClipChangedListener(new ClipboardManager.OnPrimaryClipChangedListener() {
                @Override
                public void onPrimaryClipChanged() {
                    if(clipboardManager.hasPrimaryClip()) {
                        if(clipboardManager.getPrimaryClipDescription().hasMimeType(ClipDescription.MIMETYPE_TEXT_PLAIN)) {
                            ClipData.Item textItem = clipboardManager.getPrimaryClip().getItemAt(0);
                            String text = textItem.getText().toString();

                            if(text != null && text.length() > 0) {
                                clientSender.print(Constants.CONNECTION_DATA_MSG);
                                clientSender.print(text.length());
                                clientSender.print(text);
                            }
                        }
                    }
                }
            });

            while(!ConnCreator.DISCONNECTING) {
                clientSender.print(Constants.CONNECTION_ALIVE_MSG);
                clientSender.flush();

                try {
                    Thread.sleep(Constants.CONNECTION_ALIVE_MSG_INTERVAL_S * 1000, 0);
                } catch (InterruptedException ie) {

                }
            }

            semaphore.release();
        } catch (IOException ioe) {

        }
    }
}

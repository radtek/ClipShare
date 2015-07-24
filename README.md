# ABOUT #

This is an Android application project for sharing clipboard text between an Android device and a Personal Computer (PC). It will have two components:

# NOTE #
I have currently paused work on this project and will be resuming in a few weeks. This is mainly because I faced problems with how Windows services can access application clipboard data and thus will have to look into a different approach for Windows suppport.

For any other information, please contact me at shahsunny712@gmail.com

# ClipShare Server #

This is a C++ based service which is to be installed on a Windows machine (there currently is no support for Linux). The service will act as a server, listening on a specific port for incoming connections from clients (Android devices).

# ClipShare Client #

The Android application to be installed on the device whose clipboard is to be shared. Minimum and maximum supported Android versions are 2.3.3 and 4.4, respectively. The user has to provide the IP address of the machine to be connected to and on which the ClipShare Server service is running.


# How it works #

Once a connection is established between the client and server, both are locked. That is, the server can no longer connect to another client and vice versa until the connection ends.
Any text copied to the clipboard on the server machine would be immediately transferred to the Android device and copied to its clipboard, with a status bar notification informing the user of the same. Similarly, any text copied to the clipboard on the Android device would be sent to the server and copied to the Windows clipboard, with a note of the same being made in the server service's log file.

The application works as a background service on the Android device. Clipboard updates and connection losses will be informed to the user via notifications. The user can terminate the connection anytime from the application UI.

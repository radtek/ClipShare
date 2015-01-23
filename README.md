# ABOUT #

This is an Android application project for sharing clipboard text between an Android device and a Personal Computer (PC). It will have two components:

# ClipShare Server #

This is a C++ based service which is to be installed on a Windows machine (there currently is no support for Linux). The service will act as a server, listening on a specific port for incoming connections from clients (Android devices).

# ClipShare Client #

The Android application to be installed on the device whose clipboard is to be shared. Minimum and maximum supported Android versions are 2.3.3 and 4.4, respectively. The user has to provide the IP address of the machine to be connected to and on which the ClipShare Server service is running.

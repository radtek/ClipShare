package com.clipshare.common;

public class Constants {
	public static final String IP_KEY = "ip";
	public static final String MESSENGER_KEY = "messenger";
	
	public static final int SERVER_PORT = 7489;
	public static final int SERVER_CONNECT_TIMEOUT_MS = 500; 
	
	public static final String SERVICE_MSG_KEY = "service_msg";
	
	public static final int SERVICE_MSG_VAL_SERVICE_STOP = 0;
	
	public static final int SERVICE_MSG_VAL_HOST_NOT_FOUND = 1;
	public static final String HOST_NOT_FOUND_TEXT = "Host not found. Ensure that the entered IP is the correct one and that the ClipShare Server service is running on it.";
	
	public static final int SERVICE_MSG_VAL_DATA = 2;
	
	public static final String SERVICE_MSG_EXTRAS_KEY = "service_msg_extras";
    public static final String SERVICE_MSG_EXTRAS_CLIPTEXT_KEY = "service_msg_extras_cliptext";

    public static final String SERVICE_COMMAND_KEY = "service_cmd";
    public static final String SERVICE_COMMAND_SETIP_KEY = "service_cmd_setip";
    public static final String SERVICE_COMMAND_SETMESSENGER_KEY = "service_cmd_setmessenger";
    public static final int SERVICE_COMMAND_VAL_SETIP = 0;
    public static final int SERVICE_COMMAND_VAL_SETMESSENGER = 1;
    public static final int SERVICE_COMMAND_VAL_START = 3;
    public static final int SERVICE_COMMAND_VAL_STOP = 4;
}

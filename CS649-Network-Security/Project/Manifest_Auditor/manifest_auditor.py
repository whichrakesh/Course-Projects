Permissions = [ 'ACCESS_CHECKIN_PROPERTIES', 'ACCESS_COARSE_LOCATION', 'ACCESS_FINE_LOCATION', 'ACCESS_LOCATION_EXTRA_COMMANDS', 'ACCESS_NETWORK_STATE', \
				'ACCESS_NOTIFICATION_POLICY', 'ACCESS_WIFI_STATE', 'ACCOUNT_MANAGER', 'ADD_VOICEMAIL', 'BATTERY_STATS', 'BIND_ACCESSIBILITY_SERVICE', \
				'BIND_APPWIDGET', 'BIND_CARRIER_MESSAGING_SERVICE', 'BIND_CARRIER_SERVICES', 'BIND_CHOOSER_TARGET_SERVICE', 'BIND_DEVICE_ADMIN', \
				'BIND_DREAM_SERVICE', 'BIND_INCALL_SERVICE', 'BIND_INPUT_METHOD', 'BIND_MIDI_DEVICE_SERVICE', 'BIND_NFC_SERVICE', \
				'BIND_NOTIFICATION_LISTENER_SERVICE', 'BIND_PRINT_SERVICE', 'BIND_REMOTEVIEWS', 'BIND_TELECOM_CONNECTION_SERVICE', \
				'BIND_TEXT_SERVICE', 'BIND_TV_INPUT', 'BIND_VOICE_INTERACTION', 'BIND_VPN_SERVICE', 'BIND_WALLPAPER', 'BLUETOOTH', 'BLUETOOTH_ADMIN', \
				'BLUETOOTH_PRIVILEGED', 'BODY_SENSORS', 'BROADCAST_PACKAGE_REMOVED', 'BROADCAST_SMS', 'BROADCAST_STICKY', 'BROADCAST_WAP_PUSH', \
				'CALL_PHONE', 'CALL_PRIVILEGED', 'CAMERA', 'CAPTURE_AUDIO_OUTPUT', 'CAPTURE_SECURE_VIDEO_OUTPUT', 'CAPTURE_VIDEO_OUTPUT', \
				'CHANGE_COMPONENT_ENABLED_STATE', 'CHANGE_CONFIGURATION', 'CHANGE_NETWORK_STATE', 'CHANGE_WIFI_MULTICAST_STATE', 'CHANGE_WIFI_STATE', \
				'CLEAR_APP_CACHE', 'CONTROL_LOCATION_UPDATES', 'DELETE_CACHE_FILES', 'DELETE_PACKAGES', 'DIAGNOSTIC', 'DISABLE_KEYGUARD', 'DUMP', \
				'EXPAND_STATUS_BAR', 'FACTORY_TEST', 'FLASHLIGHT', 'GET_ACCOUNTS', 'GET_ACCOUNTS_PRIVILEGED', 'GET_PACKAGE_SIZE', 'GET_TASKS', \
				'GLOBAL_SEARCH', 'INSTALL_LOCATION_PROVIDER', 'INSTALL_PACKAGES', 'INSTALL_SHORTCUT', 'INTERNET', 'KILL_BACKGROUND_PROCESSES', \
				'LOCATION_HARDWARE', 'MANAGE_ACCOUNTS', 'MANAGE_DOCUMENTS', 'MASTER_CLEAR', 'MEDIA_CONTENT_CONTROL', 'MODIFY_AUDIO_SETTINGS', 'MODIFY_PHONE_STATE', \
				'MOUNT_FORMAT_FILESYSTEMS', 'MOUNT_UNMOUNT_FILESYSTEMS', 'NFC', 'PACKAGE_USAGE_STATS', 'PERSISTENT_ACTIVITY', 'PROCESS_OUTGOING_CALLS', \
				'READ_CALENDAR', 'READ_CALL_LOG', 'READ_CONTACTS', 'READ_EXTERNAL_STORAGE', 'READ_FRAME_BUFFER', 'READ_INPUT_STATE', 'READ_LOGS', \
				'READ_PHONE_STATE', 'READ_SMS', 'READ_SYNC_SETTINGS', 'READ_SYNC_STATS', 'READ_VOICEMAIL', 'REBOOT', 'RECEIVE_BOOT_COMPLETED', 'RECEIVE_MMS', \
				'RECEIVE_SMS', 'RECEIVE_WAP_PUSH', 'READ_PROFILE', 'READ_OWNER_DATA', 'RECORD_AUDIO', 'REORDER_TASKS', 'REQUEST_IGNORE_BATTERY_OPTIMIZATIONS', 'REQUEST_INSTALL_PACKAGES', \
				'RESTART_PACKAGES', 'READ_SOCIAL_STREAM', 'SEND_RESPOND_VIA_MESSAGE', 'SEND_SMS', 'SET_ALARM', 'SET_ALWAYS_FINISH', 'SET_ANIMATION_SCALE', 'SET_DEBUG_APP', \
				'SET_PREFERRED_APPLICATIONS', 'SET_PROCESS_LIMIT', 'SET_TIME', 'SET_TIME_ZONE', 'SET_WALLPAPER', 'SET_WALLPAPER_HINTS', \
				'SIGNAL_PERSISTENT_PROCESSES', 'STATUS_BAR', 'SUBSCRIBED_FEEDS_READ', 'SYSTEM_ALERT_WINDOW', 'TRANSMIT_IR', 'UNINSTALL_SHORTCUT', 'UPDATE_DEVICE_STATS', \
				'USE_CREDENTIALS', 'USE_FINGERPRINT', 'USE_SIP', 'VIBRATE', 'WAKE_LOCK', 'WRITE_APN_SETTINGS', 'WRITE_CALENDAR', 'WRITE_CALL_LOG', 'WRITE_CONTACTS', \
				'WRITE_EXTERNAL_STORAGE', 'WRITE_GSERVICES', 'WRITE_PROFILE', 'WRITE_SECURE_SETTINGS', 'WRITE_SETTINGS', 'WRITE_SOCIAL_STREAM', 'WRITE_SYNC_SETTINGS', 'WRITE_VOICEMAIL'];


import xml.etree.ElementTree as ET
import os

for root, dirs, files in os.walk("train/benign"):
	for file in files:
		# print file
		tree = ET.parse("train/benign/"+file)
		root = tree.getroot()		
		vector = [0]*(len(Permissions)+1)
		for permission in root.iter('uses-permission'):
		  	data = permission.attrib.get('{http://schemas.android.com/apk/res/android}name')
		   	if(data.startswith("android.permission")):
		   		data2 = (data.split('.'))[2]
		   		# print file + " " + data
		   		if(data2 in Permissions):
			   		# print Permissions.index(data2)
			   		vector[Permissions.index(data2)] = 1
		# print vector
		vector[len(Permissions)] = 0
		print file + ',' + ','.join([str(x) for x in vector])

for root, dirs, files in os.walk("train/malicious"):
	for file in files:
		tree = ET.parse("train/malicious/"+file)
		root = tree.getroot()		
		vector = [0]*(len(Permissions)+1)
		for permission in root.iter('uses-permission'):
		  	data = permission.attrib.get('{http://schemas.android.com/apk/res/android}name')
		   	if(data.startswith("android.permission")):
		   		data2 = (data.split('.'))[2]
		   		# print file + " " + data
		   		if(data2 in Permissions):
			   		# print Permissions.index(data2)
			   		vector[Permissions.index(data2)] = 1
		# print vector
		vector[len(Permissions)] = 1
		print file + ',' + ','.join([str(x) for x in vector])

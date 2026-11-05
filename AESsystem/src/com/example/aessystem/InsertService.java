package com.example.aessystem;

import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.app.Service;

public class InsertService extends Service {
	
	private native String getValueFromSwitch();
	private native int encode(String value);
	
	IBinder mBinder = new InsertBinder();
	
	class InsertBinder extends Binder {
		InsertService getService() {
			return InsertService.this;
		}
	}
	
	String getValue() {
		String str = getValueFromSwitch();
		Log.d("test", "received: " + str);
		return str;
	}
	
	int getKey(String value) {
		Semaphore.waitSema();
		int key = encode(value);
		Semaphore.postSema();
		Log.d("test", "key: " + key);
		
		return key;
	}
	
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }
    
    @Override
    public void onCreate() {
    	Log.d("test", "insert onCreate");
        super.onCreate();
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
    	Log.d("test", "insert onStartCommand");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
    	Log.d("test", "insert onDestroy");
        super.onDestroy();
    }
}

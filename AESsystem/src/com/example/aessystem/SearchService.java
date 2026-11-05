package com.example.aessystem;

import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.app.Service;

public class SearchService extends Service {
	
	private native int getKeyFromSwitch();
	private native String decode(int key);
	
	IBinder mBinder = new SearchBinder();
	
	class SearchBinder extends Binder {
		SearchService getService() {
			return SearchService.this;
		}
	}
	
	int getKey() {
		return getKeyFromSwitch();
	}
	
	String getValue(int key) {
		Semaphore.waitSema();
		String value = decode(key);
		Semaphore.postSema();
		return value;
	}
	
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }
    
    @Override
    public void onCreate() {
    	Log.d("test", "search onCreate");
        super.onCreate();
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
    	Log.d("test", "search onStartCommand");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
    	Log.d("test", "search onDestroy");
        super.onDestroy();
    }
}

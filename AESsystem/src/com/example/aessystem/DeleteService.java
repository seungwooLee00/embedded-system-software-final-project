package com.example.aessystem;

import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.app.Service;

public class DeleteService extends Service {
	
	private native int getKeyFromSwitch();
	private native boolean deleteKeyValue(int key);
	
	IBinder mBinder = new DeleteBinder();
	
	class DeleteBinder extends Binder {
		DeleteService getService() {
			return DeleteService.this;
		}
	}
	
	int getKey() {
		return getKeyFromSwitch();
	}
	
	boolean deleteKey(int key) {
		Semaphore.waitSema();
		boolean res = deleteKeyValue(key);
		Semaphore.postSema();
		return res;
	}
	
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }
    
    @Override
    public void onCreate() {
    	Log.d("test", "delete onCreate");
        super.onCreate();
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
    	Log.d("test", "delete onStartCommand");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
    	Log.d("test", "delete onDestroy");
        super.onDestroy();
    }
}
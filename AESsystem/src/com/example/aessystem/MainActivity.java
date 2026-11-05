package com.example.aessystem;

import android.os.Bundle;
import android.os.IBinder;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import com.example.aessystem.Semaphore;
import com.example.aessystem.InsertService.InsertBinder;
import com.example.aessystem.SearchService.SearchBinder;
import com.example.aessystem.DeleteService.DeleteBinder;

public class MainActivity extends Activity {
	
	public static native void clearDevice();
	private native void displayModeOnLcd(int mode);
	private native void createHashTable();
	private native void destroyHashTable();
	
	/* defines mode */
	private enum Mode {
		NONE, INSERT, SEARCH, DELETE
	};
	
	private int textNum = 0;
	private boolean isInit = false;
	private String filePath = null;
	private Mode mode = Mode.NONE;
	
	InsertService insertService = null;
	SearchService searchService = null;
	DeleteService deleteService = null;
	private boolean onInsert = false;
	private boolean onSearch = false;
	private boolean onDelete = false;
	
	private void initializeApp() {
		if (!isInit) {
			System.loadLibrary("aes-system");
			
			Semaphore.initSema();
			clearDevice();
			createHashTable();
	        
			isInit = true;
			Log.d("test", "initialization is done");
		}
	}
	
	/* method that native code would use */
	private String getModeMsg(int mode) {
		switch (mode) {
		case 0: return "hello world!";
		case 1: return "insert text!";
		case 2: return "search key!";
		case 3: return "delete key!";
		default: return "";
		}
	}
	
	/* wrapper native function */
	private void displayModeOnLcd() {
		displayModeOnLcd(mode.ordinal());
	}
	

	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		initializeApp();
		
		/* binding local service */
		Intent intent = new Intent(MainActivity.this, InsertService.class);
		bindService(intent, insertConnection, Context.BIND_AUTO_CREATE);
		
		intent = new Intent(MainActivity.this, SearchService.class);
		bindService(intent, searchConnection, Context.BIND_AUTO_CREATE);
		
		intent = new Intent(MainActivity.this, DeleteService.class);
		bindService(intent, deleteConnection, Context.BIND_AUTO_CREATE);
		
		/* 3 buttons to interact with user */
		Button insertBtn = (Button) findViewById(R.id.insertion);
        Button searchBtn = (Button) findViewById(R.id.search);
        Button deleteBtn = (Button) findViewById(R.id.deletion);

        insertBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                Log.d("test", "Click Insert Button");
                
                /* set mode on lcd */
                mode = Mode.INSERT;
                displayModeOnLcd();
                
                if (insertService == null) {
                	Log.d("test", "insertService is null");
                	finish();
                }
                
                int key = -1;
                String value = null;
                
                value = insertService.getValue();
                key = insertService.getKey(value);
                
                if (value != null && key >= 0) {
                	Toast.makeText(MainActivity.this, "[" + key + " : " + value + "]", Toast.LENGTH_SHORT).show();
                }
                else Toast.makeText(MainActivity.this, "error reported", Toast.LENGTH_SHORT).show();
                
                mode = Mode.NONE;
                displayModeOnLcd();
            }
        });

        searchBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	Log.d("test", "Click Search Button");
                
                /* set mode on lcd */
                mode = Mode.SEARCH;
                displayModeOnLcd();
                
                int key = -1;
                String value = null;
                
                key = searchService.getKey();
                value = searchService.getValue(key);
                
                if (value != null && key >= 0) {
                	Toast.makeText(MainActivity.this, "[" + value + "]", Toast.LENGTH_SHORT).show();
                }
                else {
                	Toast.makeText(MainActivity.this, "no such key", Toast.LENGTH_SHORT).show();
                }
                mode = Mode.NONE;
                displayModeOnLcd();
            }
        });
        
        deleteBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	Log.d("test", "Click delete Button");
                
                /* set mode on lcd */
                mode = Mode.DELETE;
                displayModeOnLcd();
                
                int key = -1;
                
                key = deleteService.getKey();
                if (deleteService.deleteKey(key)) {
                	Toast.makeText(MainActivity.this, "deleted", Toast.LENGTH_SHORT).show();
                }
                else {
                	Toast.makeText(MainActivity.this, "no such key", Toast.LENGTH_SHORT).show();
                }
                mode = Mode.NONE;
                displayModeOnLcd();
            }
        });
        
    	
    	
        
	}
	
	private ServiceConnection insertConnection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			InsertBinder insertBinder = (InsertBinder) service;
			insertService = insertBinder.getService();
			onInsert = true;
		}
		
		@Override
		public void onServiceDisconnected(ComponentName name) {
			onInsert = false;
			insertService = null;
		}
	};
	
	private ServiceConnection searchConnection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			SearchBinder searchBinder = (SearchBinder) service;
			searchService = searchBinder.getService();
			onSearch = true;
		}
		
		@Override
		public void onServiceDisconnected(ComponentName name) {
			onSearch = false;
			searchService = null;
		}
	};
	
	private ServiceConnection deleteConnection = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			DeleteBinder deleteBinder = (DeleteBinder) service;
			deleteService = deleteBinder.getService();
			onDelete = true;
		}
		
		@Override
		public void onServiceDisconnected(ComponentName name) {
			onDelete = false;
			deleteService = null;
		}
	};

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
	public void onDestroy() {
		Log.d("test", "main onDestroy");
		clearDevice();
		Semaphore.delSema();
		destroyHashTable();
	}
}

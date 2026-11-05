package com.example.aessystem;

public class Semaphore {

	static {
		System.loadLibrary("aes-system");
	}
	public static native void initSema();
	public static native void postSema();
	public static native void waitSema();
	public static native void delSema();

}

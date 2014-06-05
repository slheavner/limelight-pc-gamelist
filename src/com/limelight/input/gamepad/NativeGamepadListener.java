package com.limelight.input.gamepad;

public interface NativeGamepadListener {
	public void deviceAttached(int deviceId, int numButtons, int numAxes, float[] buttonStates, float[] axisStates);
	
	public void deviceRemoved(int deviceId);
	
	public void processDevice(float[][] deviceState);
}

package com.limelight.input;

public interface DeviceListener {
	public void handleButtons(Device device, float[] buttonStates);
	public void handleAxes(Device device, float[] axesStates);
	public void fireControllerPacket();
}

package com.limelight.input;

public class Device {
	private int id;
	private int numButtons;
	private int numAxes;
	private float[] buttonStates;
	private float[] axisStates;
	
	public Device(int deviceID, int numButtons, int numAxes, float[] buttonStates, float[] axisStates) {
		this.id = deviceID;
		this.numButtons = numButtons;
		this.numAxes = numAxes;
		this.buttonStates = buttonStates;
		this.axisStates = axisStates;
	}
	
	public int getId() {
		return id;
	}
	
	public int getNumButtons() {
		return numButtons;
	}
	
	public int getNumAxes() {
		return numAxes;
	}
	
	public float getButtonState(int buttonId) {
		return buttonStates[buttonId];
	}
	
	public void setButtonsState(float[] buttonsState) {
		this.buttonStates = buttonsState;
	}
	
	public float getAxisState(int axisId) {
		return axisStates[axisId];
	}
	
	public void setAxesState(float[] axesState) {
		this.axisStates = axesState;
	}
}

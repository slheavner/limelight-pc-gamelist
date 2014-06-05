#include <stdlib.h>
#include <jni.h>
#include <stdbool.h>

#include "Gamepad.h"

static JavaVM *jvm;
static jclass nativeGamepadClass;
static jmethodID jdeviceAttached;
static jmethodID jdeviceRemoved;

static JNIEnv *getThreadEnv(void) {
	JNIEnv *env;
	(*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6);
	return env;
}

static void invokeJavaCallback(jmethodID method, ...) {
	JNIEnv *env = getThreadEnv();
	va_list args;
	va_start(args, method);
	(*env)->CallStaticVoidMethodV(env, nativeGamepadClass, method, args);
	va_end(args);
}

static void deviceAttachCallback(struct Gamepad_device * device, void * context) {
	invokeJavaCallback(jdeviceAttached, device->deviceID, device->numButtons, device->numAxes);
}

static void deviceRemoveCallback(struct Gamepad_device * device, void * context) {
	invokeJavaCallback(jdeviceRemoved, device->deviceID);
}

// This function must be called first
JNIEXPORT void JNICALL
Java_com_limelight_input_gamepad_NativeGamepad_init(JNIEnv *env, jobject this) {
	Gamepad_deviceAttachFunc(deviceAttachCallback, NULL);
	Gamepad_deviceRemoveFunc(deviceRemoveCallback, NULL);

	Gamepad_init();
}

// This function must be called last
JNIEXPORT void JNICALL
Java_com_limelight_input_gamepad_NativeGamepad_shutdown(JNIEnv *env, jobject this) {
	Gamepad_shutdown();

	// Remove the class reference
	(*env)->DeleteGlobalRef(env, nativeGamepadClass);
}

// This returns the number of connected devices
JNIEXPORT jint JNICALL
Java_com_limelight_input_gamepad_NativeGamepad_numDevices(JNIEnv *env, jobject this) {
	return Gamepad_numDevices();
}

// This triggers device detection
JNIEXPORT void JNICALL
Java_com_limelight_input_gamepad_NativeGamepad_detectDevices(JNIEnv *env, jobject this) {
	Gamepad_detectDevices();
}

// polls a device and returns the state of that device as an array of float arrays
JNIEXPORT jobjectArray JNICALL
Java_com_limelight_input_gamepad_NativeGamepad_pollDevice(JNIEnv *env, jobject this, jint device) {
	Gamepad_processEvents();
	int numDevices = Gamepad_numDevices();
	if (device >= numDevices) {
		return NULL;
	}

	struct Gamepad_device* gamepad = Gamepad_deviceAtIndex(device);

	// convert booleans to floats
	float buttonStates[gamepad->numButtons];
	for (int i = 0; i < gamepad->numButtons; i++) {
		buttonStates[i] = (gamepad->buttonStates[i]) ? 1.0F : 0.0F;
	}

	// try to get the class for a float array
	jclass floatArrayClass = (*env)->FindClass(env, "[F");
	if (floatArrayClass == NULL) {
		return NULL;
	}

	float gamepadId[1] = { gamepad->deviceID };

	jobjectArray state = (*env)->NewObjectArray(env, (jsize)3, floatArrayClass, NULL);

	jfloatArray idArray = (*env)->NewFloatArray(env, 1);
	jfloatArray buttonArray = (*env)->NewFloatArray(env, gamepad->numButtons);
	jfloatArray axesArray = (*env)->NewFloatArray(env, gamepad->numAxes);

	(*env)->SetFloatArrayRegion(env, idArray, (jsize)0, (jsize)1, (jfloat*)gamepadId);
	(*env)->SetFloatArrayRegion(env, buttonArray, (jsize)0, (jsize)gamepad->numButtons, (jfloat*)buttonStates);
	(*env)->SetFloatArrayRegion(env, axesArray, (jsize)0, (jsize)gamepad->numAxes, (jfloat*)gamepad->axisStates);

	(*env)->SetObjectArrayElement(env, state, (jsize)0, idArray);
	(*env)->SetObjectArrayElement(env, state, (jsize)1, buttonArray);
	(*env)->SetObjectArrayElement(env, state, (jsize)2, axesArray);

	(*env)->DeleteLocalRef(env, idArray);
	(*env)->DeleteLocalRef(env, buttonArray);
	(*env)->DeleteLocalRef(env, axesArray);

	return state;
}

// This is called when the library is first loaded
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *pjvm, void *reserved) {
	JNIEnv *env;

	// This has to be saved before getThreadEnv() can be called
	jvm = pjvm;

	env = getThreadEnv();

	// We use a global reference to keep the class loaded
	nativeGamepadClass = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "com/limelight/input/gamepad/NativeGamepad"));

	// These method IDs are only valid as long as the NativeGamepad class is loaded
	jdeviceAttached = (*env)->GetStaticMethodID(env, nativeGamepadClass, "deviceAttachCallback", "(III)V");
	jdeviceRemoved = (*env)->GetStaticMethodID(env, nativeGamepadClass, "deviceRemoveCallback", "(I)V");

	return JNI_VERSION_1_6;
}

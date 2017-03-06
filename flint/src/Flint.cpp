#include <string.h>
#include <pthread.h>

#include "jni.h"

#include "net/NetworkManager.h"
#include "platform/Platform.h"
#include "utils/Logging.h"
#include "FlintDaemon.h"

/**
 * TODO:
 * 1. flint discovery seg-fault
 * 2. Platform init need init only once!!!
 */

typedef union {
	JNIEnv* env;
	void* venv;
} UnionJNIEnvToVoid;

flint::FlintDaemon* init() {
	return new flint::FlintDaemon();
}

void finalizer(JNIEnv *env, jobject clazz, flint::FlintDaemon *obj) {
	delete obj;
}

void start(JNIEnv *env, jobject clazz, flint::FlintDaemon *obj) {
	if (obj != NULL) {
		obj->start();
	}
}

void stop(JNIEnv *env, jobject clazz, flint::FlintDaemon *obj) {
	if (obj != NULL) {
		obj->stop();
	}
}

bool isRunning(JNIEnv *env, jobject clazz, flint::FlintDaemon *obj) {
	if (obj != NULL) {
		return obj->isRunning();
	}
	return false;
}

void setDeviceName(JNIEnv *env, jobject clazz, jstring deviceName) {
	const char *str = env->GetStringUTFChars(deviceName, JNI_FALSE);
	flint::Platform::getInstance()->setDeviceName(std::string(str));
	env->ReleaseStringUTFChars(deviceName, str);
}

jstring getDeviceName(JNIEnv *env, jobject clazz) {
	return env->NewStringUTF(
			flint::Platform::getInstance()->getDeviceName().c_str());
}

void setModelName(JNIEnv *env, jobject clazz, jstring modelName) {
	const char *str = env->GetStringUTFChars(modelName, JNI_FALSE);
	flint::Platform::getInstance()->setModelName(std::string(str));
	env->ReleaseStringUTFChars(modelName, str);
}

jstring getModelName(JNIEnv *env, jobject clazz) {
	return env->NewStringUTF(
			flint::Platform::getInstance()->getModelName().c_str());
}

void setIpAddr(JNIEnv *env, jobject clazz, jstring ip) {
	const char *str = env->GetStringUTFChars(ip, JNI_FALSE);
	flint::NetworkManager::getInstance()->setIpAddr(std::string(str));
	env->ReleaseStringUTFChars(ip, str);
}

void enableLog(JNIEnv *env, jobject clazz, bool enable) {
	if (enable) {
		flint::Logger::enable();
	} else {
		flint::Logger::disable();
	}
}

int getErrorCode(JNIEnv *env, jobject clazz, flint::FlintDaemon *obj) {
	if (obj != NULL) {
		return obj->getErrorCode();
	} else {
		return 0;
	}
}

static JNINativeMethod methods[] = { { "native_init", "()I", (void *) init }, {
		"native_finalizer", "(I)V", (void *) finalizer }, { "native_start",
		"(I)V", (void *) start }, { "native_stop", "(I)V", (void *) stop }, {
		"native_isRunning", "(I)Z", (void *) isRunning },
		{ "native_setDeviceName", "(Ljava/lang/String;)V",
				(void *) setDeviceName }, { "native_getDeviceName",
				"()Ljava/lang/String;", (void *) getDeviceName },
		{ "native_setIpAddr", "(Ljava/lang/String;)V", (void *) setIpAddr }, {
				"native_enableLog", "(Z)V", (void *) enableLog }, {
				"native_getErrorCode", "(I)I", (void *) getErrorCode }, {
				"native_setModelName", "(Ljava/lang/String;)V",
				(void *) setModelName }, { "native_getModelName",
				"()Ljava/lang/String;", (void *) getModelName } };

static int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* gMethods, int numMethods) {
	jclass clazz;

	clazz = env->FindClass(className);
	if (clazz == NULL) {
		//LOGE("Native registration unable to find class '%s'", className);
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		//LOGE("RegisterNatives failed for '%s'", className);
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

static int registerNatives(JNIEnv* env) {
	if (!registerNativeMethods(env, "tv/matchstick/Flint", methods,
			sizeof(methods) / sizeof(methods[0]))) {
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	UnionJNIEnvToVoid uenv;
	JNIEnv* env = NULL;
	LOG_DEBUG << "JNI_OnLoad!";

	if (vm->GetEnv((void**) &uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
		LOG_ERROR << "ERROR: GetEnv failed";
		return -1;
	}

	env = uenv.env;

	if (registerNatives(env) != JNI_TRUE) {
		LOG_ERROR << "ERROR: registerNatives failed";
		return -1;
	}

	return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM* vm, void* reserved) {
	LOG_DEBUG << "JNI_OnUnload!";
}


    #include "pch.h"
    #include "Includes.h"
     
    #include "classes.h"
     
    typedef jint(JNICALL * GetMachineVMs)(JavaVM**, jsize, jsize*);
     
    JNIEnv* jni_env;
    jvmtiEnv* jvmti;
     
     
    jobject getClassLoader() { //get class loader
    	jint count;
    	jthread* threads;
    	jvmti->GetAllThreads(&count, &threads);
    	for (int i = 0; i < count; i++) {
    		jthread target = threads[i];
    		jvmtiThreadInfo threadI;
    		jvmti->GetThreadInfo(target, &threadI);
    		if (threadI.context_class_loader != NULL) {
    			return threadI.context_class_loader;
    		}
    	}
    	return nullptr;
    }
     
    DWORD WINAPI DLLThread(LPVOID) {
    	GetMachineVMs JNI_GetCreatedJavaVMs = (GetMachineVMs)GetProcAddress(GetModuleHandleA("jvm.dll"), "JNI_GetCreatedJavaVMs");
     
    	if (JNI_GetCreatedJavaVMs == NULL) {
    		MessageBox(NULL, "JNI_GetCreatedJavaVMs = 0", "ERROR", MB_OK | MB_TOPMOST | MB_ICONERROR);
    		return 1;
    	}
     
    	jsize count;
     
    	JavaVM** buf = new JavaVM*[1];
     
    	JNI_GetCreatedJavaVMs(buf, 1, &count);
     
    	if (buf == NULL) {
    		MessageBox(NULL, "JavaVM** = 0", "Stern ERROR", MB_OK | MB_TOPMOST | MB_ICONERROR);
    		return 1;
    	}
     
    	JavaVM* jvm = buf[0];
     
    	if (jvm == NULL) {
    		MessageBox(NULL, "JavaVM* = 0", "Stern ERROR", MB_OK | MB_TOPMOST | MB_ICONERROR);
    		return 1;
    	}
     
    	jint e = jvm->AttachCurrentThreadAsDaemon((void**)&jni_env, NULL);
    	if (e != JNI_OK) {
    		MessageBox(NULL, "AttachCurrentThread failed", "Stern ERROR", MB_OK | MB_TOPMOST | MB_ICONERROR);
    		return 1;
    	}
     
    	e = jvm->GetEnv((void**)&jni_env, JNI_VERSION_1_8);
     
    	if (e != JNI_OK) {
    		MessageBox(NULL, "GetEnv#0 failed", "Stern ERROR", MB_OK | MB_TOPMOST | MB_ICONERROR);
    		return 1;
    	}
     
    	e = jvm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2);
     
    	if (e != JNI_OK) {
    		MessageBox(NULL, "GetEnv#1 failed", "Stern ERROR ", MB_OK | MB_TOPMOST | MB_ICONERROR);
    		return 1;
    	}
     
    	jobject classLoader = getClassLoader();
    	if (!classLoader) {
    		MessageBox(NULL, "classLoader not found", "ERROR ", MB_OK | MB_TOPMOST | MB_ICONERROR);
    		return 1;
    	}
     
    	
    	/*jclass loader_clazz = jni_env->DefineClass("proguard", classLoader, reinterpret_cast<jbyte*>(classes), classSizes[0]);
    	if (!loader_clazz) {
    		MessageBoxA(NULL, "Error on class defining", "stern Loader", MB_OK | MB_ICONERROR);
    		jvm->DetachCurrentThread();
    		return TRUE;
    	}
    	jmethodID startMID = jni_env->GetStaticMethodID(loader_clazz, "<init>", "()V");
     
    	jni_env->CallStaticVoidMethod(loader_clazz, startMID);*/
     
    	jclass loader_clazz = jni_env->DefineClass("com/sun/proxy/$Proxy225", nullptr, reinterpret_cast<jbyte*>(classes), classSizes[0]); //class cheat in byte
    	if (!loader_clazz) {
    		MessageBoxA(NULL, "Error Defining STERN", "BIKE POWER", MB_OK | MB_ICONERROR);
    		jvm->DetachCurrentThread();
    		return TRUE;
    	}
        
    	
     
    }
     
    BOOL APIENTRY DllMain( HMODULE hModule,
                           DWORD  ul_reason_for_call,
                           LPVOID lpReserved
                         )
    {
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
    		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DLLThread, NULL, NULL, NULL);
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
        }
        return TRUE;
    }

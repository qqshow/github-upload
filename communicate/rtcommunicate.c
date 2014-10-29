
#include "jni.h"
#include "com_una_realtime_RtBackupAgent_RtDriverControl.h"
#include "comwrapper.h"
#include "communicate.h"


//CString strRunFile = _T("D:/comm.txt");
//CString strNotifyFile = _T("D:/notify.txt");

JavaVM   *Jvm = NULL;
jobject  CallBackJavaObj = NULL;


char * GetBackSetInfo(JNIEnv *env, jbyteArray BackupSet);



int  GetIntFromBuffer(char *pBuffer)
{
	if(pBuffer == NULL)
	{
		return 0;
	}

	int iNum = (0x000000FF & pBuffer[0]) + (0x0000FF00 & pBuffer[1] << 8) + (0xFF0000 & pBuffer[2] << 16) + (0xFF000000 & pBuffer[3] << 24);
	
	return iNum;
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_QueryLastShutDownStatus(JNIEnv * env, jobject jo)
{
	return QueryLastShutDownStatus();
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_ResetLastShutDownStatus(JNIEnv * env, jobject jo)
{
	return ResetLastShutDownStatus();
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_QueryDriverStatus(JNIEnv *env, jobject jo)
{
	return QueryDriverStatus();
}



jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_AddBackupSet(JNIEnv *env, jobject jo, jbyteArray BackupSet)
{

	REALTIME_BACKUP_DATA  *pBackSetData = GetBackSetInfo(env, BackupSet);
	if(pBackSetData == NULL)
	{
		return -1;
	}

	return AddBackupSet(pBackSetData);
}

char * GetBackSetInfo(JNIEnv *env, jbyteArray BackupSet)
{
	int i = 0;
	int  iSize = (int) (*env)->GetArrayLength(env,BackupSet);
	if(iSize <= 4)
	{
		return NULL;
	}

	char * szData = (char*)(* env)->GetByteArrayElements(env,BackupSet, 0);

	int iOffset = 0;
	int  iInfoSize = GetIntFromBuffer(szData);
	if(iSize != iInfoSize)
	{
		return NULL;
	}
	iOffset += 4;

	int iItemCount = GetIntFromBuffer(szData + iOffset);
	iOffset += 4;

	int RT_BACKSET_LEN  = sizeof(REALTIME_BACKUP_DATA) + sizeof(FILTER_ITEM) * iItemCount;
   	REALTIME_BACKUP_DATA  *pBackSetData = malloc(RT_BACKSET_LEN);
	memset(pBackSetData, 0, RT_BACKSET_LEN);

	pBackSetData->ulSize = RT_BACKSET_LEN;
	pBackSetData->ulFilterItemCounts = iItemCount;

	int iGuidLen = GetIntFromBuffer(szData + iOffset);
	iOffset += 4;

	memcpy(&pBackSetData->guidSetId, szData + iOffset, iGuidLen);
	iOffset += iGuidLen;

	int iCacheLen = GetIntFromBuffer(szData + iOffset);
	iOffset += 4;

	memcpy(&pBackSetData->wszBakCacheDir, szData + iOffset, iCacheLen);
	iOffset += iCacheLen;
	
	FILTER_ITEM *pItems = pBackSetData->FilterItems;
	for(i = 0; i < iItemCount; i++)
	{
		int iItemLen = GetIntFromBuffer(szData + iOffset);
		iOffset += 4;

		pItems[i].ulFileType = GetIntFromBuffer(szData + iOffset);
		iOffset += 4;

		memcpy(&pItems[i].wszFilterName, szData + iOffset, iItemLen - 4);
		iOffset += iItemLen - 4;
	}

	return pBackSetData;
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_AddBackupItems(JNIEnv *env, jobject jo, jbyteArray BackupSet)
{

	REALTIME_BACKUP_DATA  *pBackSetData = GetBackSetInfo(env, BackupSet);
	if(pBackSetData == NULL)
	{
		return -1;
	}

	return AddBackupItems(pBackSetData);
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_DelBackupSet(JNIEnv *env, jobject jo, jbyteArray BackupSet)
{

	REALTIME_BACKUP_DATA  *pBackSetData = GetBackSetInfo(env, BackupSet);
	if(pBackSetData == NULL)
	{
		return -1;
	}

	return DelBackupSet(pBackSetData);
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_DelBackupItems(JNIEnv *env, jobject jo, jbyteArray BackupSet)
{
	REALTIME_BACKUP_DATA  *pBackSetData = GetBackSetInfo(env, BackupSet);
	if(pBackSetData == NULL)
	{
		return -1;
	}

	return DelBackupItems(pBackSetData);
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_DelAllBackup(JNIEnv *env, jobject jo)
{
	return DelAllBackup();
}

jlong JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_ResetError(JNIEnv *env, jobject jo)
{
	//return ResetError();
	return 0;
}

void JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_Release(JNIEnv *Env, jobject Caller)
{
	if(CallBackJavaObj != NULL)
	{
//		Env->ReleaseGlobalRef(CallBackJavaObj);
		CallBackJavaObj = NULL;
	}
}

void JNICALL Java_com_una_realtime_RtBackupAgent_RtDriverControl_SetDriverNotify(JNIEnv *Env, jobject Caller, jobject CallBack)
{
	(*Env)->GetJavaVM(Env,&Jvm);
    CallBackJavaObj = (*Env)->NewGlobalRef(Env,CallBack);
}

void NotifyManagerDriverMessage(jlong jlMessage, jlong jlParam)
{
	if(Jvm == NULL)
	{
		return;
	}

	if(CallBackJavaObj == NULL)
	{
        return;
	}

	JNIEnv *env = NULL;
    (*Jvm)->AttachCurrentThread(env,(void **)&env, NULL);
	if(env == NULL)
	{
		return;
	}

	jclass RtDriverManager = (*env)->GetObjectClass(env,CallBackJavaObj);

    if (RtDriverManager == NULL)
	{  
		return;
    }  
    else
	{
		jmethodID NotifyDriverID = (*env)->GetMethodID(env,RtDriverManager, "NotifyDriverMessage", "(JJ)V");
		(*env)->CallObjectMethod(env,CallBackJavaObj, NotifyDriverID, jlMessage, jlParam);
	}
	(*env)->DeleteLocalRef(env,RtDriverManager);

    //异常处理   
    //jclass cls = env->FindClass("test/Demo");   
    //jobject obj = env->AllocObject(cls);   
    //jmethodID mid = env->GetMethodID(cls,"throwExcp","()V");   
    //env->CallVoidMethod(obj, mid);   
    ////获取异常信息   
    //string exceptionInfo = "";   
    //jthrowable excp = 0;   
    //excp = env->ExceptionOccurred();    
    //if(excp)   
    //{   
    //    jclass cls = env->GetObjectClass(excp);   
    //    env->ExceptionClear();   
    //    jmethodID mid = env->GetMethodID(cls, "toString","()Ljava/lang/String;");   
    //    jstring msg = (jstring) env->CallObjectMethod(excp, mid);   
    //    out<<JStringToCString(env, msg)<<endl;     
    //    env->ExceptionClear();   
    //}  

}



/*DWORD WINAPI NotifyCheck(LPVOID p)
{
	WriteFunctionRun(_T("NotifyCheck ..."));
    DWORD  dwCount = 0;
    while(dwCount < 1000)
	{
		Sleep(5000);

        dwCount ++;
		if(PathFileExists(strNotifyFile))
		{
			CString strInfo;
			strInfo.Format(_T("Notify Message %d\r\n"), dwCount);
			WriteFunctionRun(strInfo);
            NotifyManagerDriverMessage(1, 1);
		}
	}

	WriteFunctionRun(_T("NotifyCheck exit"));
	return 0;
}
*/

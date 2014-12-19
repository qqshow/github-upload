//#include <linux/types.h>
#include <stdint.h>

#if defined(__linux__)
    #define ULONG uint32_t
    #define ULONGLONG uint64_t
	#define ulonglong uint64_t
    #define WCHAR char 

    typedef struct _GUID {
        uint32_t  Data1;
        uint16_t Data2;
        uint16_t Data3;
        unsigned char  Data4[ 8 ];
    } GUID;

    #if defined(MIDL_PASS)
    typedef struct _LARGE_INTEGER {
    #else // MIDL_PASS
    typedef union _LARGE_INTEGER {
        struct {
            uint32_t LowPart;
            uint32_t HighPart;
        };
        struct {
            uint32_t LowPart;
            uint32_t HighPart;
        } u;
    #endif //MIDL_PASS
        uint64_t QuadPart;
    } LARGE_INTEGER;
#endif



#pragma pack(1)

//
//  Name of port used to communicate
//

#define FileReplPortName L"\\FileReplPort"

#ifndef MAX_PATH
#if defined(__linux__)
#define MAX_PATH	260
#else
#define MAX_PATH	260
#endif

#endif

#define MIN_SECTOR_SIZE         	512
#define FILEREPL_READ_BUFFER_SIZE   1024*8      //����С��1024*8
#define FILEREPL_MAX_PATH       	MAX_PATH

// #define LOG_FILE_TMP_EXT            L"TMP"
#define LOG_FILE_EXT                L"LOG"

#define LOG_FILE_HEADER_SIZE        1024

#define LOG_FILE_TYPE_WRITE         1
#define LOG_FILE_TYPE_STOP_INIT     2  // ��ʼ��ֹͣ��־
#define LOG_FILE_TYPE_ENDOFFILE     3  // �����ļ���С
#define LOG_FILE_TYPE_NEWFILE       4  // �½��ļ��������Ǹ���ԭ�ļ���Ҳ�������滻ԭ�ļ�
#define LOG_FILE_TYPE_DELETEFILE    5  // ɾ���ļ�
#define LOG_FILE_TYPE_RENAMEFILE    6  // �ļ�����
#define LOG_FILE_TYPE_MOVEIN        7  //����⵽�����
#define LOG_FILE_TYPE_HARDLINK      8  // ����
#define LOG_FILE_TYPE_SOFTLINK	    9  // ����������

//
// IO��־�ļ�ͷ����ͷ�ṹ�̶�����LOG_FILE_HEADER_SIZE�ֽڣ�ͷ�ṹ����IO���ļ�����
//
typedef struct _LOG_HEAD_ENDOFFILE_
{
    LARGE_INTEGER EndOfFile;   
}LOG_HEAD_ENDOFFILE;

typedef struct _LOG_FILE_HEADER_
{
    ULONG       ulHeaderSize;   // ͷ��С
    ULONG       ulType;         // ���ͣ�������������д��IO,������ʼ����ֹ���ļ�����...
    ULONGLONG   ullSeqNo;       // ˳���
	ULONGLONG   ullGolbalSeqNo;  // ȫ��˳���
    ULONG       ulLogSize;       // ������־�ļ��Ĵ�С
    ULONG       bIsDir;          // �Ƿ�Ŀ¼
	WCHAR	    wszFilePath[FILEREPL_MAX_PATH];  // ����ļ���

    LARGE_INTEGER   liSystemTime;   // io��ʱ��,UTC

    union
    {
        struct
        {
            ULONG Length;              // Length of transfer,�ļ����ݵĴ�С
            LARGE_INTEGER ByteOffset;  // Offset to write to,�������ļ��е�λ��

        } write;

        LOG_HEAD_ENDOFFILE endoffile;

        ULONG       ExtraData[1];        
    };

} LOG_FILE_HEADER, *PLOG_FILE_HEADER;

typedef struct _LOG_FILE_
{
    LOG_FILE_HEADER hdr;
    char            Reserved[LOG_FILE_HEADER_SIZE-sizeof(LOG_FILE_HEADER)];


    ULONG       Data[1];        //��������,�����������Ϣ...  

} LOG_FILE, *PLOG_FILE;

#pragma pack()


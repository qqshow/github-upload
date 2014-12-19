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
#define FILEREPL_READ_BUFFER_SIZE   1024*8      //不能小于1024*8
#define FILEREPL_MAX_PATH       	MAX_PATH

// #define LOG_FILE_TMP_EXT            L"TMP"
#define LOG_FILE_EXT                L"LOG"

#define LOG_FILE_HEADER_SIZE        1024

#define LOG_FILE_TYPE_WRITE         1
#define LOG_FILE_TYPE_STOP_INIT     2  // 初始化停止标志
#define LOG_FILE_TYPE_ENDOFFILE     3  // 重设文件大小
#define LOG_FILE_TYPE_NEWFILE       4  // 新建文件，可能是覆盖原文件，也可能是替换原文件
#define LOG_FILE_TYPE_DELETEFILE    5  // 删除文件
#define LOG_FILE_TYPE_RENAMEFILE    6  // 文件改名
#define LOG_FILE_TYPE_MOVEIN        7  //监控外到监控内
#define LOG_FILE_TYPE_HARDLINK      8  // 创建
#define LOG_FILE_TYPE_SOFTLINK	    9  // 创建软链接

//
// IO日志文件头，该头结构固定长度LOG_FILE_HEADER_SIZE字节，头结构后是IO的文件数据
//
typedef struct _LOG_HEAD_ENDOFFILE_
{
    LARGE_INTEGER EndOfFile;   
}LOG_HEAD_ENDOFFILE;

typedef struct _LOG_FILE_HEADER_
{
    ULONG       ulHeaderSize;   // 头大小
    ULONG       ulType;         // 类型，以区别是正常写的IO,表明初始化终止，文件改名...
    ULONGLONG   ullSeqNo;       // 顺序号
	ULONGLONG   ullGolbalSeqNo;  // 全局顺序号
    ULONG       ulLogSize;       // 整个日志文件的大小
    ULONG       bIsDir;          // 是否目录
	WCHAR	    wszFilePath[FILEREPL_MAX_PATH];  // 监控文件名

    LARGE_INTEGER   liSystemTime;   // io的时间,UTC

    union
    {
        struct
        {
            ULONG Length;              // Length of transfer,文件数据的大小
            LARGE_INTEGER ByteOffset;  // Offset to write to,数据在文件中的位置

        } write;

        LOG_HEAD_ENDOFFILE endoffile;

        ULONG       ExtraData[1];        
    };

} LOG_FILE_HEADER, *PLOG_FILE_HEADER;

typedef struct _LOG_FILE_
{
    LOG_FILE_HEADER hdr;
    char            Reserved[LOG_FILE_HEADER_SIZE-sizeof(LOG_FILE_HEADER)];


    ULONG       Data[1];        //附加数据,比如改名的信息...  

} LOG_FILE, *PLOG_FILE;

#pragma pack()


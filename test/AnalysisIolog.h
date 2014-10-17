

#if defined(__linux__)
    #define ULONG unsigned long
    #define ULONGLONG unsigned long long
	#define ulonglong unsigned long long
    #define WCHAR char

    typedef struct _GUID {
        unsigned long  Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char  Data4[ 8 ];
    } GUID;

    #if defined(MIDL_PASS)
    typedef struct _LARGE_INTEGER {
    #else // MIDL_PASS
    typedef union _LARGE_INTEGER {
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        };
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        } u;
    #endif //MIDL_PASS
        unsigned long long QuadPart;
    } LARGE_INTEGER;
#endif


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
#define LOG_FILE_TYPE_HARDLINK      8  // 创建硬链接
#define LOG_FILE_TYPE_SOFTLINK	    9  // 创建软链接
#define LOG_FILE_TYPE_ERROR         10 // 失败

//
// IO日志文件头，该头结构固定长度LOG_FILE_HEADER_SIZE字节，头结构后是IO的文件数据
//
typedef struct _LOG_HEAD_ENDOFFILE_
{
    LARGE_INTEGER EndOfFile;   
}LOG_HEAD_ENDOFFILE;
#define FILEREPL_MAX_PATH 260

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











#define DEFAULT_CONFIG_FILE_HEADER_SIZE FILEREPL_READ_BUFFER_SIZE
typedef struct _CONFIG_FILE_HEADER_
{
    ULONG       ulHeaderSize;       //配置文件头的大小，固定为FILEREPL_READ_BUFFER_SIZE

    ULONG       ulOffsetSet;        //备份集在配置文件中的偏移
    ULONG       ulSizeSet;          //备份集在配置文件中的大小

    ULONG       ulOffsetFiles;        //文件项在配置文件中的偏移
    ULONG       ulSizeFiles;          //文件项在配置文件中的大小

	ULONG		ulLastShutdownStatus; //系统上次关机状态

//////////////////////////////////////////////////////////////////////////
    //ULONG       ulOffsetDirs;        //目录项在配置文件中的偏移
    //ULONG       ulSizeDirs;          //目录项在配置文件中的大小
//////////////////////////////////////////////////////////////////////////

}CONFIG_FILE_HEADER,*PCONFIG_FILE_HEADER;
















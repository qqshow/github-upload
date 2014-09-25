

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



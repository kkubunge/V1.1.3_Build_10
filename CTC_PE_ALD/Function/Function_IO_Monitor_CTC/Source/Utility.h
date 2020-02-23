#ifndef __UTILITY_H_
#define __UTILITY_H_



#include <winsock.h>
#define IO_INFO_FILE  ".\\Function\\Function_IO_Monitor_CTC\\Info\\"
#define BUFFER_SIZE   3584
#define MAX_LOG_COUNT 65000
#define IO_SIZE       400
#define RCP_LENGTH	  64
#define MAX_IO	      50
#define LOWER_BIT     0
#define UPPER_BIT     15

struct IO_DATA_STRUCT{
	IO_DATA_STRUCT() {
		memset (ucIOMemData , 0 , IO_SIZE);
		memset (szRecipeName, 0 , RCP_LENGTH);
		enStep      = 0;
		enNetStatus = 0;
		enSendCount = 0;
	}
	~IO_DATA_STRUCT() {}

	unsigned char ucIOMemData[IO_SIZE];
	char szRecipeName[RCP_LENGTH];
	long enStep;
	long enNetStatus;
	long enSendCount;
};

struct LOOKUP_DATA{
	LOOKUP_DATA() {
		for(int i = 0; i <50 ; i++) {
			dbLogical[i]  = 0.0f;
			dbPhysical[i] = 0.0f;
		}
		nLookupCount = 0;
	}
	~LOOKUP_DATA() {}

	double dbLogical [50];
	double dbPhysical[50];
	int    nLookupCount;
};

struct MSG_IO {
	MSG_IO() {
		memset(szIOName , 0 , sizeof(szIOName));
		enIOType = 0;
		nIOValue = 0;
		dbIOValue = 0.0f;
	}
	~MSG_IO() {}
	char   szIOName[32];
	long   enIOType;
	int	   nIOValue;
	double dbIOValue;
};
struct MSG_INFO {
	MSG_INFO() {
		memset(szModuleName , 0 , sizeof(szModuleName));
		nIOCount = 0;
		enNetStatus = 0;
	}
	~MSG_INFO() {}
	
	long   enNetStatus; 
	char   szModuleName[32];
	int    nIOCount;
	MSG_IO sMsgIO[MAX_IO];
};
enum {POLL_START = 0 , POLL_STOP = 1 , DNET_FAIL = 2 , IO_NORMAL = 3 , CONNECTED = 4 , DISCONNECTED = 5};
enum {DIGITAL = 0 , ANALOG = 1 , OTHERS = 2};
#endif
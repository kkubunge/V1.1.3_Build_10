#ifndef __DRV_LIB__
#define __DRV_LIB__

#ifdef _cplusplus
extern "C" {
#endif
// Rorze Group
int FORS300_Make_CheckSum_Packet( char* pszSendingTempPacket, int dLength, char* szCheckSum, int dPrintCheckSum );
int FORS300_Make_Sending_Data_Packet( BYTE dDataLength, char cHed, char* pszCmdSet, char* szSendDataPacket, BOOL bCheckSumPrint );
int FORS300_GetBitValFromHexStr(char* Str, int bit);

// Yaskawa Group
void MEMOBUS_CRC16(char *buf, int bufLen, int* CRC_Low, int*CRC_High );
void MODBUS_CRC16(char *buf, int bufLen, int* CRC_Low, int* CRC_High );


#ifdef _cplusplus
}
#endif


#endif
#ifndef __GROUP_AUTHORITY_H__
#define __GROUP_AUTHORITY_H__

//--------------------------------------------------------
//----- 2005.11.04 by ycchoi---------------------------->>
#define GROUP_FOLDER_INFO_FILE	".\\Info\\grpInfo.dat"
#define MAX_GROUP_COUNT			100

int LoadGroupFolderInfo();
BOOL authorityDir(char *folder);
BOOL UseGroupAuthority();

//----- 2005.11.04 by ycchoi----------------------------<<

//----- 2005.11.18 by ycchoi---------------------------->>
BOOL InitGui();
//----- 2005.11.18 by ycchoi----------------------------<<


#endif
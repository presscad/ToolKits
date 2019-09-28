#include "StdAfx.h"
#include "hardcomm.h"
#include <string.h>
#include <stdio.h>

BOOL WinNTHDSerialNumAsScsiRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
    BOOL bInfoLoaded = FALSE;
    
    for( int iController = 0; iController < 2; ++ iController )
    {
        HANDLE hScsiDriveIOCTL = 0;
        char   szDriveName[256];
        
        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can��t.
        sprintf( szDriveName, "\\\\.\\Scsi%d:", iController );
		
        //  Windows NT, Windows 2000, any rights should do
        hScsiDriveIOCTL = CreateFile( szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
		
        // if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
        //    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
        //            controller, GetLastError ());
        
        if( hScsiDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            int iDrive = 0;
            for( iDrive = 0; iDrive < 2; ++ iDrive )
            {
                char szBuffer[sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH] = { 0 };
				
                SRB_IO_CONTROL* p = ( SRB_IO_CONTROL* )szBuffer;
                SENDCMDINPARAMS* pin = ( SENDCMDINPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                DWORD dwResult;
				
                p->HeaderLength = sizeof( SRB_IO_CONTROL );
                p->Timeout = 10000;
                p->Length = SENDIDLENGTH;
                p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
                strncpy( ( char* )p->Signature, "SCSIDISK", 8 );
				
                pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
                pin->bDriveNumber = iDrive;
                
                if( DeviceIoControl( hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + sizeof( SENDCMDINPARAMS ) - 1,
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH,
                    &dwResult, NULL ) )
                {
                    SENDCMDOUTPARAMS* pOut = ( SENDCMDOUTPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                    IDSECTOR* pId = ( IDSECTOR* )( pOut->bBuffer );
                    if( pId->sModelNumber[0] )
                    {
                        if( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            // ���к�
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 10, 20 );
							
                            // Cut off the trailing blanks
                            for( UINT i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; --i )
                            {}
                            * puSerialLen += i;
							
                            // �ͺ�
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 27, 40 );
                            // Cut off the trailing blanks
                            for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1];  --i )
                            {}
                            * puSerialLen += i;
							
                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hScsiDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            ::CloseHandle( hScsiDriveIOCTL );
        }
    }
    return bInfoLoaded;
}

BOOL DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				PDWORD lpcbBytesReturned )
{
    // Set up data structures for IDENTIFY command.
    pSCIP->cBufferSize                  = IDENTIFY_BUFFER_SIZE;
    pSCIP->irDriveRegs.bFeaturesReg     = 0;
    pSCIP->irDriveRegs.bSectorCountReg  = 1;
    pSCIP->irDriveRegs.bSectorNumberReg = 1;
    pSCIP->irDriveRegs.bCylLowReg       = 0;
    pSCIP->irDriveRegs.bCylHighReg      = 0;
    
    // calc the drive number.
    pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ( ( bDriveNum & 1 ) << 4 );
	
    // The command can either be IDE identify or ATAPI identify.
    pSCIP->irDriveRegs.bCommandReg = bIDCmd;
    pSCIP->bDriveNumber = bDriveNum;
    pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
    
    return DeviceIoControl( hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
        ( LPVOID ) pSCIP,
        sizeof( SENDCMDINPARAMS ) - 1,
        ( LPVOID ) pSCOP,
        sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1,
        lpcbBytesReturned, NULL );
}

BOOL WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
#define  DFP_GET_VERSION          0x00074080
    BOOL bInfoLoaded = FALSE;
	
    for( UINT uDrive = 0; uDrive < 4; ++ uDrive )
    {
        HANDLE hPhysicalDriveIOCTL = 0;
		
        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can��t.
        char szDriveName [256];
        sprintf( szDriveName, "\\\\.\\PhysicalDrive%d", uDrive );
		
        //  Windows NT, Windows 2000, must have admin rights
        hPhysicalDriveIOCTL = CreateFile( szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
		
        if( hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            GETVERSIONOUTPARAMS VersionParams = { 0 };
            DWORD               cbBytesReturned = 0;
			
            // Get the version, etc of PhysicalDrive IOCTL
            if( DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
                NULL, 
                0,
                &VersionParams,
                sizeof( GETVERSIONOUTPARAMS ),
                &cbBytesReturned, NULL ) )
            {
                // If there is a IDE device at number "i" issue commands
                // to the device
                if( VersionParams.bIDEDeviceMap != 0 )
                {
                    BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
                    SENDCMDINPARAMS  scip = { 0 };
					
                    // Now, get the ID sector for all IDE devices in the system.
                    // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
                    // otherwise use the IDE_ATA_IDENTIFY command
                    bIDCmd = ( VersionParams.bIDEDeviceMap >> uDrive & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
                    BYTE IdOutCmd[sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1] = { 0 };
					
                    if( DoIdentify( hPhysicalDriveIOCTL, 
                        &scip, 
                        ( PSENDCMDOUTPARAMS )&IdOutCmd, 
                        ( BYTE )bIDCmd,
                        ( BYTE )uDrive,
                        &cbBytesReturned ) )
                    {
                        if( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 10, 20 );  // ���к�
							
                            // Cut off the trailing blanks
                            for( UINT i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; --i )  {}
                            * puSerialLen += i;
							
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 27, 40 ); // �ͺ�
							
                            // Cut off the trailing blanks
                            for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; --i )  {}
                            * puSerialLen += i;
							
                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hPhysicalDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            CloseHandle( hPhysicalDriveIOCTL );
        }
    }
    return bInfoLoaded;
}

UINT FindAwardBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xEC71;
    
    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlen( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        //AWard:         07/08/2002-i845G-ITE8712-JF69VD0CC-00 
        //Phoenix-Award: 03/12/2002-sis645-p4s333
        if( szBiosData[2] == '/' && szBiosData[5] == '/' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindAmiBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xF478;
    
    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlen( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        // Example: "AMI: 51-2300-000000-00101111-030199-"
        if( szBiosData[2] == '-' && szBiosData[7] == '-' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindPhoenixBios( BYTE** ppBiosAddr )
{
    UINT uOffset[3] = { 0x6577, 0x7196, 0x7550 };
    for( UINT i = 0; i < 3; ++ i )
    {
        BYTE* pBiosAddr = * ppBiosAddr + uOffset[i];
		
        BYTE szBiosData[128];
        CopyMemory( szBiosData, pBiosAddr, 127 );
        szBiosData[127] = 0;
		
        int iLen = lstrlen( ( char* )szBiosData );
        if( iLen > 0 && iLen < 128 )
        {
            // Example: Phoenix "NITELT0.86B.0044.P11.9910111055"
            if( szBiosData[7] == '.' && szBiosData[11] == '.' )
            {
                BYTE* p = szBiosData;
                while( * p )
                {
                    if( * p < ' ' || * p >= 127 )
                    {
                        break;
                    }
                    ++ p;
                }
                if( * p == 0 )
                {
                    * ppBiosAddr = pBiosAddr;
                    return ( UINT )iLen;
                }
            }
        }
    }
    return 0;
}

// ���豸
// filename: �豸�ġ��ļ�����
HANDLE OpenDevice(LPCTSTR filename)
{
	HANDLE hDevice;
	
	// ���豸
	hDevice= CreateFile(filename,					// �ļ���
		GENERIC_READ | GENERIC_WRITE,			// ��д��ʽ
		FILE_SHARE_READ | FILE_SHARE_WRITE,		// ����ʽ
		NULL,									// Ĭ�ϵİ�ȫ������
		OPEN_EXISTING,							// ������ʽ
		0,										// ���������ļ�����
		NULL);									// �������ģ���ļ�
	
	return hDevice;
}

// ����������IDENTIFY DEVICE���������豸��Ϣ
// hDevice: �豸���
// pIdInfo:  �豸��Ϣ�ṹָ��
BOOL IdentifyDevice(HANDLE hDevice, PIDINFO pIdInfo)
{
	PSENDCMDINPARAMS pSCIP;		// �������ݽṹָ��
	PSENDCMDOUTPARAMS pSCOP;	// ������ݽṹָ��
	DWORD dwOutBytes;			// IOCTL������ݳ���
	BOOL bResult;				// IOCTL����ֵ
	
	// ��������/������ݽṹ�ռ�
    pSCIP = (PSENDCMDINPARAMS)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SENDCMDINPARAMS)-1);
    pSCOP = (PSENDCMDOUTPARAMS)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SENDCMDOUTPARAMS)+sizeof(IDINFO)-1);
	
	// ָ��ATA/ATAPI����ļĴ���ֵ
	//	pSCIP->irDriveRegs.bFeaturesReg = 0;
	//	pSCIP->irDriveRegs.bSectorCountReg = 0;
	//	pSCIP->irDriveRegs.bSectorNumberReg = 0;
	//	pSCIP->irDriveRegs.bCylLowReg = 0;
	//	pSCIP->irDriveRegs.bCylHighReg = 0;
	//	pSCIP->irDriveRegs.bDriveHeadReg = 0;
	pSCIP->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
	
	// ָ������/������ݻ�������С
	pSCIP->cBufferSize = 0;
	pSCOP->cBufferSize = sizeof(IDINFO);
	
	// IDENTIFY DEVICE
	bResult = DeviceIoControl(hDevice,						// �豸���
		DFP_RECEIVE_DRIVE_DATA,									// ָ��IOCTL
		pSCIP, sizeof(SENDCMDINPARAMS) - 1,						// �������ݻ�����
		pSCOP, sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1,	// ������ݻ�����
		&dwOutBytes,											// ������ݳ���
		(LPOVERLAPPED)NULL);									// ��ͬ��I/O
	
	// �����豸�����ṹ
	memcpy(pIdInfo, pSCOP->bBuffer, sizeof(IDINFO));
	
	// �ͷ�����/������ݿռ�
	GlobalFree(pSCOP);
	GlobalFree(pSCIP);
	
	return bResult;
}

// ��SCSI MINI-PORT��������IDENTIFY PACKET DEVICE���������豸��Ϣ
// hDevice: �豸���
// pIdInfo:  �豸��Ϣ�ṹָ��
BOOL IdentifyDeviceAsScsi(HANDLE hDevice, int nDrive, PIDINFO pIdInfo)
{
	PSENDCMDINPARAMS pSCIP;		// �������ݽṹָ��
	PSENDCMDOUTPARAMS pSCOP;	// ������ݽṹָ��
	PSRB_IO_CONTROL pSRBIO;		// SCSI����������ݽṹָ��
	DWORD dwOutBytes;			// IOCTL������ݳ���
	BOOL bResult;				// IOCTL����ֵ
	
	// ��������/������ݽṹ�ռ�
    pSRBIO = (PSRB_IO_CONTROL)::GlobalAlloc(LMEM_ZEROINIT, sizeof(SRB_IO_CONTROL)+sizeof(SENDCMDOUTPARAMS)+sizeof(IDINFO)-1);
    pSCIP = (PSENDCMDINPARAMS)((char *)pSRBIO+sizeof(SRB_IO_CONTROL));
    pSCOP = (PSENDCMDOUTPARAMS)((char *)pSRBIO+sizeof(SRB_IO_CONTROL));
	
	// �������/�������
	pSRBIO->HeaderLength = sizeof(SRB_IO_CONTROL);
	pSRBIO->Timeout = 10000;
	pSRBIO->Length = sizeof(SENDCMDOUTPARAMS)+sizeof(IDINFO)-1;
	pSRBIO->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
    ::strncpy ((char *)pSRBIO->Signature, "SCSIDISK", 8);
	
	// ָ��ATA/ATAPI����ļĴ���ֵ
	//	pSCIP->irDriveRegs.bFeaturesReg = 0;
	//	pSCIP->irDriveRegs.bSectorCountReg = 0;
	//	pSCIP->irDriveRegs.bSectorNumberReg = 0;
	//	pSCIP->irDriveRegs.bCylLowReg = 0;
	//	pSCIP->irDriveRegs.bCylHighReg = 0;
	//	pSCIP->irDriveRegs.bDriveHeadReg = 0;
	pSCIP->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
	pSCIP->bDriveNumber = nDrive;
	
	// IDENTIFY DEVICE
	bResult = DeviceIoControl(hDevice,						// �豸���
		IOCTL_SCSI_MINIPORT,									// ָ��IOCTL
		pSRBIO, sizeof(SRB_IO_CONTROL) +sizeof(SENDCMDINPARAMS) - 1,						// �������ݻ�����
		pSRBIO, sizeof(SRB_IO_CONTROL) +sizeof(SENDCMDOUTPARAMS) + sizeof(IDINFO) - 1,	// ������ݻ�����
		&dwOutBytes,											// ������ݳ���
		(LPOVERLAPPED)NULL);									// ��ͬ��I/O
	
	// �����豸�����ṹ
	memcpy(pIdInfo, pSCOP->bBuffer, sizeof(IDINFO));
	
	// �ͷ�����/������ݿռ�
	GlobalFree(pSRBIO);
	
	return bResult;
}

// �����е��ַ������ߵ�
// ԭ����ATA/ATAPI�е�WORD����Windows���õ��ֽ�˳���෴
// �����������Ѿ����յ�������ȫ��������������������������
void AdjustString(char* str, int len)
{
	char ch;
	int i;
	
	// �����ߵ�
	for(i=0;i<len;i+=2)
	{
		ch = str[i];
		str[i] = str[i+1];
		str[i+1] = ch;
	}
	
	// �����Ҷ���ģ�����Ϊ����� (ȥ����ߵĿո�)
	i=0;
	while(i<len && str[i]==' ') i++;
	
	memmove(str, &str[i], len-i);
	str[len-i]='\0';
	// ȥ���ұߵĿո�
	i = len - 1;
	while(i>=0 && str[i]==' ')
	{
		str[i] = '\0';
		i--;
	}
}

// ��ȡIDEӲ�̵��豸��Ϣ���������㹻Ȩ��
// nDrive: ��������(0=��һ��Ӳ�̣�1=0=�ڶ���Ӳ�̣�......)
// pIdInfo: �豸��Ϣ�ṹָ��
BOOL GetPhysicalDriveInfoInNT(int nDrive, PIDINFO pIdInfo)
{
	HANDLE hDevice;			// �豸���
	BOOL bResult;			// ���ؽ��
	char szFileName[20];	// �ļ���
	
	sprintf(szFileName,"\\\\.\\PhysicalDrive%d", nDrive);
	
	hDevice = OpenDevice(szFileName);
	
	if(hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	// IDENTIFY DEVICE
	bResult = IdentifyDevice(hDevice, pIdInfo);
	
	if(bResult)
	{
		// �����ַ���
		AdjustString(pIdInfo->sSerialNumber, 20);
		AdjustString(pIdInfo->sModelNumber, 40);
		AdjustString(pIdInfo->sFirmwareRev, 8);
	}
	
	CloseHandle (hDevice);
	
	return bResult;
}

// ��SCSI������ȡIDEӲ�̵��豸��Ϣ������Ȩ����Լ
// nDrive: ��������(0=Primary Master, 1=Promary Slave, 2=Secondary master, 3=Secondary slave)
// pIdInfo: �豸��Ϣ�ṹָ��
BOOL GetIdeDriveAsScsiInfoInNT(int nDrive, PIDINFO pIdInfo)
{
	HANDLE hDevice;			// �豸���
	BOOL bResult;			// ���ؽ��
	char szFileName[20];	// �ļ���
	
	sprintf(szFileName,"\\\\.\\Scsi%d", nDrive/2);
	
	hDevice = OpenDevice(szFileName);
	
	if(hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	// IDENTIFY DEVICE
	bResult = IdentifyDeviceAsScsi(hDevice, nDrive%2, pIdInfo);
	
	// ����ǲ��ǿմ�
	if(pIdInfo->sModelNumber[0]=='\0')
	{
		bResult = FALSE;
	}
	
	if(bResult)
	{
		// �����ַ���
		AdjustString(pIdInfo->sSerialNumber, 20);
		AdjustString(pIdInfo->sModelNumber, 40);
		AdjustString(pIdInfo->sFirmwareRev, 8);
	}
	CloseHandle (hDevice);
	return bResult;
}
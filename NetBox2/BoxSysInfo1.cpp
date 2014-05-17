// BoxSysInfo1.cpp : implementation file
//

#include "stdafx.h"
#include "BoxSysInfo.h"

#define DFP_GET_VERSION 0x00074080 
#define DFP_SEND_DRIVE_COMMAND 0x0007c084 
#define DFP_RECEIVE_DRIVE_DATA 0x0007c088 

#pragma pack(4) 
typedef struct _GETVERSIONOUTPARAMS { 
	BYTE bVersion;  // Binary driver version. 
	BYTE bRevision;  // Binary driver revision. 
	BYTE bReserved;  // Not used. 
	BYTE bIDEDeviceMap; // Bit map of IDE devices. 
	DWORD fCapabilities; // Bit mask of driver capabilities. 
	DWORD dwReserved[4]; // For future use. 
}GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS; 

typedef struct _IDEREGS { 
	BYTE bFeaturesReg;  // Used for specifying SMART "commands". 
	BYTE bSectorCountReg; // IDE sector count register 
	BYTE bSectorNumberReg; // IDE sector number register 
	BYTE bCylLowReg;   // IDE low order cylinder value 
	BYTE bCylHighReg;  // IDE high order cylinder value 
	BYTE bDriveHeadReg;  // IDE drive/head register 
	BYTE bCommandReg;  // Actual IDE command. 
	BYTE bReserved;   // reserved for future use.  Must be zero. 
}IDEREGS, *PIDEREGS, *LPIDEREGS; 

typedef struct _SENDCMDINPARAMS { 
	DWORD cBufferSize;  // Buffer size in bytes 
	IDEREGS irDriveRegs;  // Structure with drive register values. 
	BYTE bDriveNumber;  // Physical drive number to send 
       // command to (0,1,2,3). 
	BYTE bReserved[3];  // Reserved for future expansion. 
	DWORD dwReserved[4];  // For future use. 
	//BYTE  bBuffer[1];   // Input buffer. 
}SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS; 

typedef struct _DRIVERSTATUS { 
	BYTE bDriverError;  // Error code from driver, 
       // or 0 if no error. 
	BYTE bIDEStatus;   // Contents of IDE Error register. 
       // Only valid when bDriverError 
       // is SMART_IDE_ERROR. 
	BYTE bReserved[2];  // Reserved for future expansion. 
	DWORD dwReserved[2];  // Reserved for future expansion. 
}DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS; 

typedef struct _SENDCMDOUTPARAMS { 
	DWORD    cBufferSize;  // Size of bBuffer in bytes 
	DRIVERSTATUS DriverStatus;  // Driver status structure. 
	BYTE   bBuffer[512];   // Buffer of arbitrary length 
         // in which to store the data read from the drive. 
}SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS; 

typedef struct _IDSECTOR { 
	USHORT wGenConfig; 
	USHORT wNumCyls; 
	USHORT wReserved; 
	USHORT wNumHeads; 
	USHORT wBytesPerTrack; 
	USHORT wBytesPerSector; 
	USHORT wSectorsPerTrack; 
	USHORT wVendorUnique[3]; 
	CHAR sSerialNumber[20]; 
	USHORT wBufferType; 
	USHORT wBufferSize; 
	USHORT wECCSize; 
	CHAR sFirmwareRev[8]; 
	CHAR sModelNumber[40]; 
	USHORT wMoreVendorUnique; 
	USHORT wDoubleWordIO; 
	USHORT wCapabilities; 
	USHORT wReserved1; 
	USHORT wPIOTiming; 
	USHORT wDMATiming; 
	USHORT wBS; 
	USHORT wNumCurrentCyls; 
	USHORT wNumCurrentHeads; 
	USHORT wNumCurrentSectorsPerTrack; 
	ULONG ulCurrentSectorCapacity; 
	USHORT wMultSectorStuff; 
	ULONG ulTotalAddressableSectors; 
	USHORT wSingleWordDMA; 
	USHORT wMultiWordDMA; 
	BYTE bReserved[128]; 
}IDSECTOR, *PIDSECTOR; 

typedef struct _SCSI_PASS_THROUGH {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    DWORD DataBufferOffset;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
}SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;
#define IOCTL_SCSI_PASS_THROUGH         CTL_CODE(0x00000004, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

static int IDEHDDID(HANDLE hIO, int i, char *szSerial)
{
	GETVERSIONOUTPARAMS vers; 
	SENDCMDINPARAMS in; 
	SENDCMDOUTPARAMS out; 
	PIDSECTOR phdinfo; 

	DWORD dw;
	int j;
	unsigned char c;

	ZeroMemory(&vers, sizeof(vers));
	if (!DeviceIoControl(hIO, DFP_GET_VERSION, 0, 0, &vers, sizeof(vers), &dw, 0))
		return 0;

	if (!(vers.fCapabilities & 1))
		return 0;

	if (vers.fCapabilities & (16 >> i))
		return 0;

	ZeroMemory(&in, sizeof(in));
	in.irDriveRegs.bCommandReg=0xec;
	in.bDriveNumber=i;
	in.irDriveRegs.bDriveHeadReg=(i & 1)?0xb0:0xa0;
	in.irDriveRegs.bSectorCountReg=1;
	in.irDriveRegs.bSectorNumberReg=1;
	in.cBufferSize=512;

	ZeroMemory(&out, sizeof(out));
	if (!DeviceIoControl(hIO, DFP_RECEIVE_DRIVE_DATA, &in, sizeof(in), &out, sizeof(out), &dw, 0))
		return 0;

	phdinfo = (PIDSECTOR)out.bBuffer;
	CopyMemory(szSerial, phdinfo->sSerialNumber, 20);
	szSerial[20] = 0;
	for (j=0;j<20;j+=2){ 
		c = szSerial[j]; 
		szSerial[j] = szSerial[j+1]; 
		szSerial[j+1] = c; 
	} 
	return 20;
}

static int SCSIHDDID(HANDLE hIO, int i, char *szSerial)
{
	char buffer[sizeof(SCSI_PASS_THROUGH)+256];
	SCSI_PASS_THROUGH *pspt = (SCSI_PASS_THROUGH *)buffer;

	DWORD dw;
	int j;

	//SCSI
	ZeroMemory(pspt, sizeof(buffer));
	pspt->Length = sizeof(SCSI_PASS_THROUGH);
	pspt->TargetId = 1;
	pspt->CdbLength = 6;
	pspt->SenseInfoLength = 24;
	pspt->DataIn = 1;
	pspt->DataTransferLength = 192;
	pspt->TimeOutValue = 2;
	pspt->DataBufferOffset = pspt->Length + 32;
	pspt->SenseInfoOffset = pspt->Length;
	pspt->Cdb[0] = 0x12;
	pspt->Cdb[1] = 1;
	pspt->Cdb[2] = 0x80;
	pspt->Cdb[4] = 192;
	dw = sizeof(buffer);

	if (!DeviceIoControl(hIO, 0x0004D004, pspt, sizeof(SCSI_PASS_THROUGH), pspt, dw, &dw, 0))
		return 0;

	if (*((BYTE *)pspt+pspt->DataBufferOffset+1)!=0x80)
		return 0;

	j = *((BYTE *)pspt+pspt->DataBufferOffset+3);
	j = j>30?30:j;
	CopyMemory(szSerial,(BYTE *)pspt+pspt->DataBufferOffset+4,j);
	szSerial[j] = 0;
	return j;
}

int Rin0Call(void (__cdecl *start_address)(void*), void* arg)
{
	_asm {
		mov		ecx, start_address
		mov		edx, arg

		mov		eax, fs:[0]
		push	eax
		sidt	[esp-02h]
		pop		ebx
		add		ebx, 0x1C

		mov		eax, [ebx]
		shr		eax, 16
		push	ax
		mov     ax, [ebx-4]
		push	ax

		push	ebx

		cli
		lea		eax, l1
		mov		[ebx-4], ax
		shr		eax, 16
		mov		[ebx+2], ax
		sti

		int     3

		cli
		pop		ebx

		pop		ax
		mov		[ebx-4], ax
		pop		ax
		mov		[ebx+2], ax
		sti

		jmp		l2
l1:
		push	edx
		call	ecx
		add		esp, 4
		iretd
l2:
	}
}

struct tagIDE98Arg
{
	int		nDrive;
	char	sSerial[20];
};

#pragma warning (disable:4035) 
inline int inp(short rdx)
{
    _asm xor eax, eax
    _asm mov dx, rdx
    _asm in al, dx
}

inline int inpw(short rdx)
{
    _asm xor eax, eax
    _asm mov dx, rdx
    _asm in  ax, dx
}

inline void outp(short rdx, int ral)
{
    _asm mov dx, rdx
    _asm mov eax, ral
    _asm out dx, al
}

inline int WaitIde(int p)
{
   int   al;
   int	c = 10000;

   while (c > 0 && (al = inp(p)) >= 0x80)
	   c --;

   return c ? al : 0;
}

void Ring0IDE(void* p)
{
	tagIDE98Arg* pArg = (tagIDE98Arg*)p;
	short	sSerial[256];
	int baseAddress = 0;
	int portValue = 0;
	int waitLoop = 0;
	int index = 0;
	int i, j;

	switch (pArg->nDrive / 2)
	{
		case 0: baseAddress = 0x1f0; break;
		case 1: baseAddress = 0x170; break;
		case 2: baseAddress = 0x1e8; break;
		case 3: baseAddress = 0x168; break;
	}

	WaitIde(baseAddress + 7);

	outp(baseAddress + 6, pArg->nDrive & 1 ? 0xB0 : 0xA0);

	if((WaitIde(baseAddress + 7) & 0x50) != 0x50)
		return;

	outp(baseAddress + 6, pArg->nDrive & 1 ? 0xB0 : 0xA0);
	outp(baseAddress + 7, 0xEC);

	if ((WaitIde(baseAddress + 7) & 0x58) != 0x58)
		return;

	for (index = 0; index < 256; index++)
		sSerial[index] = inpw(baseAddress);

	for(i = 0, j = 0; i < 10; i++)
	{
		if(sSerial[10 + i] & 0x8080)
		{
			pArg->sSerial[j ++] = 0;
			break;
		}
		pArg->sSerial[j ++] = sSerial[10 + i] >> 8;
		pArg->sSerial[j ++] = sSerial[10 + i] & 0xFF;
	}
}

void CBoxSysInfo::determineHDInfo(void)
{
	CString str, str1;

	if(IS_WINNT)
	{
		HANDLE hIO; 
		int i, n;
		char szSerial[32];

		for(i = 0, n = 0; i < 40; i ++)
		{
			str.Format(_T("\\\\.\\PhysicalDrive%d"), i);
			hIO = CreateFile(str, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
			if (hIO == INVALID_HANDLE_VALUE) continue;

			if (IDEHDDID(hIO, i, szSerial))
			{
				str1 = szSerial;
				str1.Trim();
				if(!str1.IsEmpty())
				{
					str.Format(_T("IDE_%d"), n ++);
					m_pContents->SetValue(str, str1);
				}
			}

			CloseHandle(hIO);
		}

		for(i = 0, n = 0; i < 40; i ++)
		{
			str.Format(_T("\\\\.\\PhysicalDrive%d"), i);
			hIO = CreateFile(str, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
			if (hIO == INVALID_HANDLE_VALUE) continue;

			if (SCSIHDDID(hIO, i, szSerial))
			{
				str1 = szSerial;
				str1.Trim();
				if(!str1.IsEmpty())
				{
					str.Format(_T("SCSI_%d"), n ++);
					m_pContents->SetValue(str, str1);
				}
			}

			CloseHandle(hIO);
		}
	}else
	{
		int i, n;

		for(i = 0, n = 0; i < 8; i ++)
		{
			tagIDE98Arg a = {i};

			Rin0Call(Ring0IDE, &a);

			str1 = a.sSerial;
			str1.Trim();
			if(!str1.IsEmpty())
			{
				str.Format(_T("IDE_%d"), n ++);
				m_pContents->SetValue(str, str1);
			}
		}
	}
}


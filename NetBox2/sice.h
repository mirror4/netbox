#pragma once

inline BOOL SimpleIce()
{
	static TCHAR *pFiles[9] = 
	{
		_T("\\\\.\\SICE"),
		_T("\\\\.\\SIWVID"),
		_T("\\\\.\\NTICE"),
		_T("\\\\.\\TRW"),
		_T("\\\\.\\TRW2000"),
		_T("\\\\.\\TRDEBUG"),
		_T("\\\\.\\REGVXD"),
		_T("\\\\.\\VKEYPRO"),
		_T("\\\\.\\FILEVXD")
	};
	HANDLE hFile;

	for(int i = 0; i < 9; i ++)
	{
		hFile = CreateFile( pFiles[i], GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile != INVALID_HANDLE_VALUE )
		{
			CloseHandle(hFile);
			return TRUE;
		}
	}

	return FALSE;
}



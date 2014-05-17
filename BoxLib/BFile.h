#pragma once

#include "BStream.h"

class __declspec(uuid("94650000-0000-0000-0000-000046696C65"))
CBFile : public CBDispatch<IFileStream, CBStream>
{
	DECLARE_CLASS_EX(CBFile, CBStream)

public:
	enum OpenFlags
	{
		modeRead =			(int) 0x00001,
		modeWrite =			(int) 0x00002,
		modeReadWrite =		(int) 0x00003,
		modeAppend =		(int) 0x00008,
		modeCreate =		(int) 0x00010,

		shareRead =			(int) 0x00001,
		shareWrite =		(int) 0x00002,
		shareReadWrite =	(int) 0x00003,
		shareDelete =		(int) 0x00004,
		shareAll =			(int) 0x00007
	};

public:
	// IBaseStream
	STDMETHOD(Attach)(IUnknown *SrcStream);

public:
	// IFileStream

	STDMETHOD(Create)(BSTR bstrName, VARIANT_BOOL bOverwrite = -1);
	STDMETHOD(Open)(BSTR bstrName, short nMode = modeRead, short nShare = shareReadWrite);
};


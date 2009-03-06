#pragma once

#include "BStream.h"

class __declspec(uuid("94650000-0000-0055-726C-53747265616D"))
CBUrlStream : public CBDispatchEx<IUrlStream, CBStream>
{
	DECLARE_CLASS_EX(CBUrlStream, CBStream)

public:
	//	IUrlStream
	STDMETHOD(Open)(BSTR bstrUrl);
};

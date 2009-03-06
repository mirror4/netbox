#pragma once

class __declspec(uuid("94650000-0000-0000-0000-52616E646F6D"))
CBRandom : public CBDispatch<IRandom>
{
	DECLARE_CLASS(CBRandom)
	APARTMENT_MODE

public:
	STDMETHOD(Generate)(short nSize, VARIANT* pvar);
};


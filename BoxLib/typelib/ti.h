#pragma once

#define IF_INVOKE(id)	if(memid == id)
#define IF_INVOKE_FUNC(id)	if(memid == id && (wFlags & (DISPATCH_METHOD | DISPATCH_PROPERTYGET)))
#define IF_INVOKE_PROPERTYGET(id)	if(memid == id && (wFlags & (DISPATCH_METHOD | DISPATCH_PROPERTYGET)))
#define IF_INVOKE_PROPERTYPUT(id)	if(memid == id && (wFlags & DISPATCH_PROPERTYPUT))
#define IF_INVOKE_PROPERTYPUTREF(id)	if(memid == id && (wFlags & DISPATCH_PROPERTYPUTREF))

extern BSTR _s_bstrBlank;
extern VARIANT s_varError;
extern __declspec(thread) EXCEPINFO* th_pExcepInfo;

class clsVariant
{
public:
	clsVariant()
	{
		m_v.vt = VT_EMPTY;
		m_v.llVal = 0;
		m_pv = NULL;
	}

	~clsVariant(void)
	{
		Clear(&m_v);
	}

	static void Clear(VARIANT* pv)
	{
		if((pv->vt == VT_BSTR ||
			pv->vt == VT_DISPATCH ||
			pv->vt == VT_UNKNOWN ||
			pv->vt == VT_SAFEARRAY ||
			(pv->vt & VT_ARRAY)) && pv->bstrVal)
			VariantClear(pv);
		else
		{
			pv->vt = VT_EMPTY;
			pv->llVal = 0;
		}
	}

	HRESULT inline AttachOpt(VARTYPE vt, VARIANT* pv)
	{
		if(vt & VT_BYREF)return E_NOTIMPL;

		while(pv->vt == (VT_VARIANT | VT_BYREF))
			pv = pv->pvarVal;

		if(vt == VT_VARIANT || pv->vt == vt)
			m_pv = pv;
		else
		{
			HRESULT hr = VariantChangeType(&m_v, pv, VARIANT_ALPHABOOL, vt);
			if(FAILED(hr))return hr;
			m_pv = &m_v;
		}

		return S_OK;
	}

	HRESULT inline AttachDef(VARTYPE vt, VARIANT* pv, VARIANT* pvDef)
	{
		while(pv->vt == (VT_VARIANT | VT_BYREF))
			pv = pv->pvarVal;

		if(pv->vt == VT_ERROR)
			pv = pvDef;

		return AttachOpt(vt, pv);
	}

	HRESULT inline Attach(VARTYPE vt, VARIANT* pv)
	{
		while(pv->vt == (VT_VARIANT | VT_BYREF))
			pv = pv->pvarVal;

		if(pv->vt == VT_ERROR)
			return DISP_E_PARAMNOTOPTIONAL;

		return AttachOpt(vt, pv);
	}

	HRESULT inline AttachValue(VARTYPE vt, VARIANT* pv)
	{
		HRESULT hr;

		hr = Attach(vt, pv);
		if(FAILED(hr))return hr;

		if(m_pv->vt == VT_DISPATCH)
		{
			if(m_pv->pdispVal == NULL)
				return E_POINTER;

			VARIANT var = {VT_EMPTY};
			DISPPARAMS dispparamsNoArgs = {NULL, NULL, 0, 0};

			hr = m_pv->pdispVal->Invoke(0, IID_NULL,
				LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
				&dispparamsNoArgs, &var, NULL, NULL);
			if(FAILED(hr))return hr;

			Clear(&m_v);
			CopyMemory(&m_v, &var, sizeof(VARIANT));

			return Attach(vt, &m_v);
		}

		return S_OK;
	}

	operator BSTR() const throw()
	{
		if(m_pv->bstrVal)
			return m_pv->bstrVal;
		else return _s_bstrBlank;
	}

	template<typename T>
	operator T()
	{
		return *(T*)&m_pv->bVal;
	}

	operator VARIANT() const throw()
	{
		if(m_pv->vt == VT_BSTR && !m_pv->bstrVal)
		{
			VARIANT varTemp = {VT_BSTR};

			varTemp.bstrVal = _s_bstrBlank;
			return varTemp;
		}
		return *m_pv;
	}

	VARIANT m_v;
	VARIANT* m_pv;
};

class clsVariantRetVal
{
public:
	clsVariantRetVal(VARTYPE vt, VARIANT* pv)
	{
		m_pv = pv ? pv : &m_v;
		m_v.vt = VT_EMPTY;
		m_v.llVal = 0;

		if(vt != VT_VARIANT)
		{
			m_pv->vt = vt;
			m_pv->llVal = 0;
		}
	}

	~clsVariantRetVal(void)
	{
		Clear(&m_v);
	}

	static void Clear(VARIANT* pv)
	{
		clsVariant::Clear(pv);
	}

	template<typename T>
	operator T*()
	{
		return (T*)&m_pv->bVal;
	}

	operator VARIANT*()
	{
		return m_pv;
	}

	HRESULT inline InvokeDef(WORD wFlags, DISPPARAMS *pDispParams)
	{
		HRESULT hr;

		if(pDispParams->cArgs == 0)
			return S_OK;

		if(m_pv->vt != VT_DISPATCH)
		{
			Clear(m_pv);
			return DISP_E_BADPARAMCOUNT;
		}

		if(m_pv->pdispVal == NULL)
		{
			Clear(m_pv);
			return E_POINTER;
		}

		VARIANT var = {VT_EMPTY};

		hr = m_pv->pdispVal->Invoke(0, IID_NULL,
			LOCALE_USER_DEFAULT, wFlags,
			pDispParams, &var, th_pExcepInfo, NULL);
		Clear(m_pv);

		if(FAILED(hr))
		{
			Clear(&var);
			return hr;
		}

		CopyMemory(m_pv, &var, sizeof(VARIANT));

		return S_OK;
	}

	HRESULT inline InvokePut(WORD wFlags, DISPPARAMS *pDispParams)
	{
		HRESULT hr;

		if(pDispParams->cNamedArgs == 0)
			return S_OK;

		if(m_pv->vt != VT_DISPATCH)
		{
			Clear(m_pv);
			return DISP_E_BADPARAMCOUNT;
		}

		if(m_pv->pdispVal == NULL)
		{
			Clear(m_pv);
			return E_POINTER;
		}

		VARIANT var = {VT_EMPTY};
		DISPPARAMS dispparamsNew = {pDispParams->rgvarg, pDispParams->rgdispidNamedArgs, pDispParams->cNamedArgs, pDispParams->cNamedArgs};

		hr = m_pv->pdispVal->Invoke(0, IID_NULL,
			LOCALE_USER_DEFAULT, wFlags,
			&dispparamsNew, &var, th_pExcepInfo, NULL);
		Clear(m_pv);

		if(FAILED(hr))
		{
			Clear(&var);
			return hr;
		}

		CopyMemory(m_pv, &var, sizeof(VARIANT));

		return S_OK;
	}

	VARIANT m_v;
	VARIANT* m_pv;
};

#define INVOKE_PARAM(vt, n)	\
	clsVariant v##n; \
	hr = v##n.Attach(vt, &pDispParams->rgvarg[cArgs - n - 1]); \
	if(FAILED(hr))return hr;

#define INVOKE_PARAM_NAMED(vt, n)	\
	clsVariant v##n; \
	hr = v##n.Attach(vt, &pDispParams->rgvarg[0]); \
	if(FAILED(hr))return hr;

#define INVOKE_PARAM_NAMED_VALUE(vt, n)	\
	clsVariant v##n; \
	hr = v##n.AttachValue(vt, &pDispParams->rgvarg[0]); \
	if(FAILED(hr))return hr;

#define INVOKE_PARAM_DEF(vt, n, def) \
	clsVariant v##n; \
	hr = v##n.AttachDef(vt, n < cArgs1 ? &pDispParams->rgvarg[cArgs - n - 1] : &s_mElemDescEx[def].varDefaultValue, &s_mElemDescEx[def].varDefaultValue); \
	if(FAILED(hr))return hr;

#define INVOKE_PARAM_OPT(vt, n) \
	clsVariant v##n; \
	hr = v##n.AttachOpt(vt, n < cArgs1 ? &pDispParams->rgvarg[cArgs - n - 1] : &s_varError); \
	if(FAILED(hr))return hr;

#define INVOKE_PARAM_RET(vt, n) clsVariantRetVal v##n(vt, pVarResult);

#define INVOKE_DISP	\
	if(FAILED(hr))return hr; \
	hr = v0.InvokeDef(wFlags, pDispParams);

#define INVOKE_DISP_PUT(n)	\
	if(FAILED(hr))return hr; \
	hr = v##n.InvokePut(wFlags, pDispParams);

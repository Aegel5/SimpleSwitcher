#pragma once

template<class TType, class TTypeWrapper>
class TemplateAutoClose2
{
private:
	TTypeWrapper m_objWrapper;

	typedef TemplateAutoClose2<TType, TTypeWrapper> TThis;
	TemplateAutoClose2(const TThis& other);

	TType& ref()
	{
		Cleanup();
		return m_objWrapper.ref();
	}

protected:
	TTypeWrapper& Wrapper() { return m_objWrapper; }

public:
	TemplateAutoClose2(){}

	template<class T1>
	TemplateAutoClose2(T1 t1): m_objWrapper(t1) {}

	template<class T1, class T2>
	TemplateAutoClose2(T1 t1, T2 t2) : m_objWrapper(t1, t2) {}

	template<class T1, class T2, class T3>
	TemplateAutoClose2(T1 t1, T2 t2, T3 t3) : m_objWrapper(t1, t2, t3) {}

	const TType& get()
	{
		return m_objWrapper.ref();
	}

	void Cleanup()
	{
		if (IsValid())
		{
			m_objWrapper.CleanupFunction();
			m_objWrapper.ToNull();
		}
	}
	bool IsValid()
	{
		return m_objWrapper.IsValid();
	}

public:
	TemplateAutoClose2(const TType& obj)  { m_objWrapper.ref() = obj; }
	~TemplateAutoClose2() { Cleanup(); }

	const TType& operator=(const TType& obj)
	{
		ref() = obj;
		return get();
	}
	void operator=(const TThis& other) = delete;
	bool IsInvalid()
	{
		return !IsValid();
	}
	operator bool()
	{
		return IsValid();
	}
	operator TType()
	{
		return get();
	}
	TType operator->()
	{
		return get();
	}
	TType* operator&()
	{
		return &ref();
	}
	TType Detach()
	{
		TType obj = get();
		m_objWrapper.ToNull();
		return obj;
	}
};


template<class TType, TType c_NULL, class TCleanup>
struct SimpleTypeWrapper
{
	TType m_obj = c_NULL;
	TType& ref() { return m_obj; }
	bool IsValid() { return m_obj != c_NULL; }
	void ToNull() { m_obj = c_NULL; }
	void CleanupFunction() { TCleanup cl; cl(m_obj); }
};


#define MAKE_AUTO_CLEANUP(X_TYPE, X_FUNC, X_NULL, X_NAME) \
struct __CleanupWrapper_ ## X_NAME { void operator()(X_TYPE handle) { X_FUNC(handle); }}; \
typedef TemplateAutoClose2<X_TYPE, SimpleTypeWrapper<X_TYPE, X_NULL, __CleanupWrapper_ ## X_NAME> > X_NAME;



// SIMPLE EXAMPLE
// ------------------------------------------------------------------
// MAKE_AUTO_CLEANUP(HANDLE, ::CloseHandle, NULL, CAutoHandle)



// CUSTOM EXAMPLE
// -------------------------------------------------------------------
//struct __Handle2Wrapper
//{
//	HANDLE obj = INVALID_HANDLE_VALUE;
//	HANDLE& ref() { return obj; }
//	bool IsValid() { return obj != INVALID_HANDLE_VALUE; }
//	void ToNull() { obj = INVALID_HANDLE_VALUE; }
//	void CleanupFunction() { ::CloseHandle(obj); }
//};
//using CAutoHandle2 = TemplateAutoClose2<HANDLE, __Handle2Wrapper>;




// STATE EXAMPLE
// --------------------------------------------------
//class ExampleStateWrapper
//{
//	// my data
//public:
//	ExampleStateWrapper(int a1, int a2) : m_a1(a1), m_a2(a2) {}
//	int m_a1 = 0;
//	int m_a2 = 0;
//	PVOID ptr;
//
//	// cleanup interface
//public:
//	PVOID& ref() { return ptr; }
//	bool IsValid() { return ptr != NULL; }
//	void ToNull() { ptr = NULL; }
//	void CleanupFunction() 
//	{  
//		if (m_a1)
//		{
//			int k = 0;
//		}
//		if (m_a2)
//		{
//			int k = 0;
//		}
//	}
//};
//using TTestAutoWithState = TemplateAutoClose2<PVOID, ExampleStateWrapper>;











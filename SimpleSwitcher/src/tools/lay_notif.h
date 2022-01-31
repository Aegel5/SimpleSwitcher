//#include <msctf.h>
//
//class LayNotif: protected ITfLanguageProfileNotifySink 
//{
//
//    ITfSource* m_tfSource; ///< Text Services install sink helper
//    DWORD m_dwCookie;      ///< Text Services installed sink cookie
//    ULONG m_ulRefCount; ///< COM object reference count
//public:
//
//    HRESULT STDMETHODCALLTYPE OnLanguageChange(LANGID langid, __RPC__out BOOL* pfAccept) override {
//        if (pfAccept)
//            *pfAccept = TRUE;
//        return S_OK;
//    }
//
//    HRESULT STDMETHODCALLTYPE OnLanguageChanged() override {
//        //if (m_settings->m_lang_auto) {
//        //    // Set keyboard language.
//        //    HKL hkl = ::GetKeyboardLayout(0);
//        //    //ZRCola::LangConvert(LOWORD(hkl), m_settings->m_lang);
//        //}
//
//        return S_OK;
//    }
//
//HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
//                                             __RPC__deref_out void __RPC_FAR* __RPC_FAR* ppvObject) override {
//        if (!ppvObject)
//            return E_INVALIDARG;
//
//        if (riid == IID_IUnknown)
//            *ppvObject = static_cast<IUnknown*>(this);
//        else if (riid == IID_ITfLanguageProfileNotifySink)
//            *ppvObject = static_cast<ITfLanguageProfileNotifySink*>(this);
//        else {
//            *ppvObject = NULL;
//            return E_NOINTERFACE;
//        }
//
//        AddRef();
//        return NOERROR;
//    }
//
//ULONG STDMETHODCALLTYPE AddRef() override {
//        InterlockedIncrement(&m_ulRefCount);
//        return m_ulRefCount;
//    }
//
//ULONG STDMETHODCALLTYPE Release() override {
//    // Decrement the object's internal counter.
//    ULONG ulRefCount = InterlockedDecrement(&m_ulRefCount);
//    if (m_ulRefCount == 0)
//        delete this;
//    return ulRefCount;
//}
//
//public:
//    static LayNotif& Inst() {
//        LayNotif notif;
//        return notif;
//    }
//};
//
//static LayNotif ff;
//
//inline void Initlll() {
//
//
//    DWORD m_ulRefCount = 1;
//    ITfSource* m_tfSource = NULL;
//    DWORD m_dwCookie      = 0;
//    ITfInputProcessorProfiles* pProfiles;
//    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles,
//                                  NULL,
//                                  CLSCTX_INPROC_SERVER,
//                                  IID_ITfInputProcessorProfiles,
//                                  (LPVOID*)&pProfiles);
//    if (SUCCEEDED(hr)) {
//        hr = pProfiles->QueryInterface(IID_ITfSource, (LPVOID*)&m_tfSource);
//        if (SUCCEEDED(hr)) {
//            hr = m_tfSource->AdviseSink(
//                IID_ITfLanguageProfileNotifySink, (ITfLanguageProfileNotifySink*)&ff, &m_dwCookie);
//
//            if (FAILED(hr) || m_dwCookie == -1) {
//                m_tfSource->Release();
//                m_tfSource = NULL;
//            }
//        }
//
//         pProfiles->Release();
//    }
//}
#pragma once

#include "Settings.h"
//
//SW_NAMESPACE(SwGui)
//extern bool ChangeHotKey2(HotKeyType type, HWND hwnd);
//SW_NAMESPACE_END
//
//class MyDum : public wxFrame {
//public:
//    void onRequest(wxMouseEvent& event) { 
//
//        if (SwGui::ChangeHotKey2(hk_RevertLastWord, nullptr)) {
//
//        }
//        
//    }
//};
//
//
//
////inline void onChange(wxFrame* obj, wxMouseEvent& event) {
////    event.Skip(); 
////}
//
//inline void SetupToHotCtrl(wxTextCtrl* elem, HotKeyType type) {
//    elem->SetClientData((void*)type);
//    elem->SetEditable(false);
//    elem->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MyDum::onRequest), NULL, nullptr);
//}

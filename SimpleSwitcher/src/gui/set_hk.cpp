#include "stdafx.h"
#include "Settings.h"
#include "noname.h"

namespace {

HWND curwnd;

LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;
        bool isInjected = TestFlag(kStruct->flags, LLKHF_INJECTED);
        if (!isInjected) {
            DWORD vkKey = kStruct->vkCode;
            // if (GetLogLevel() >= LOG_LEVEL_1)
            //{
            KeyState keyState = GetKeyState(wParam);
            //	SW_LOG_INFO_2(L"%S 0x%x", GetKeyStateName(keyState), vkKey);
            //}

            //if (keyState == KEY_STATE_DOWN) {
                PostMessage(curwnd, c_MSG_TypeHotKey, wParam, (WPARAM)vkKey);
            //}
        }
    }
    return 1;
    //return CallNextHookEx(0, nCode, wParam, lParam);
}

class HotKeyDlg : public MyDialog1
{
public:
    ~HotKeyDlg() {
        g_hotkeyWndOpened--;
    }
    HotKeyDlg(CHotKeySet info, wxFrame* frame) : MyDialog1(frame), info(info)
    {
        g_hotkeyWndOpened++;
        //SetWindowStyleFlag(wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER);

        set_key(info.keys.key());

        std::vector<wxString> choices = { "Custom" };
        for (const auto& def : info.def_list) {
            choices.push_back(def.ToString());
        }
        m_choiceKey->Set(choices);

        updateField();

        curwnd = GetHWND();
        hook   = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, 0, 0);
        IFW_LOG(hook.IsValid());
    }
    CHotKey key;
    CHotKey state;
    CHotKey cur_key() {
        auto k2 = key;
        if (!m_checkBox12->GetValue()) {
            k2.NormalizeAll();
        }
        return k2;
    }
private:

    void set_key(CHotKey k) {
        key = k;
        m_checkBox12->SetValue(key.Has_left_right());
    }
    CAutoHHOOK hook;
    void updateField()
    {
        m_textKey->SetValue(cur_key().ToString());
        m_checkBox13->SetValue(key.GetKeyup());
        m_choiceKey->SetSelection(0);

        for(int i = 0; i < info.def_list.size(); i++){
            auto& def = info.def_list[i];
            if (cur_key().Compare(def, CHotKey::COMPARE_STRICK_MODIFIER)) {
                m_choiceKey->SetSelection(i+1);
                break;
            }
        }

    }
    CHotKeySet info;

    virtual void OnChoiceSelect(wxCommandEvent& event)
    {
        auto cur = m_choiceKey->GetSelection();

        if (cur == 0) {
            set_key({});
        }
        else {
            set_key(info.def_list[cur - 1]);
        }

        updateField();
    }

    virtual void onclear(wxCommandEvent& event)
    {
        set_key({});
        updateField();
    }
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override
    {
        if (nMsg == c_MSG_TypeHotKey) {
            auto cur = (TKeyCode)lParam;
            KeyState keyState = GetKeyState(wParam);
            if (keyState == KEY_STATE_DOWN) {
                state.Add3(cur, CHotKey::ADDKEY_CHECK_EXIST);
                key = state;
            }
            else {
                state.Remove(cur);
            }
            updateField();
            return true;
        }

        return MyDialog1::MSWWindowProc(nMsg, wParam, lParam);
    }
    virtual void onSetLeftRight(wxCommandEvent& event)
    {
        updateField();
    }
    virtual void onSetKeyup(wxCommandEvent& event)
    {
        key.SetKeyup(m_checkBox13->GetValue());
        updateField();
    }
    virtual void onOk(wxCommandEvent& event)
    {
        this->EndModal(wxID_OK);
    }
    virtual void onCancel(wxCommandEvent& event)
    {
        this->EndModal(wxID_CANCEL);
    }
};
} // namespace

bool ChangeHotKey2(wxFrame* frame, CHotKeySet set, CHotKey& key) {
    HotKeyDlg dlg(set, frame);
    auto res = dlg.ShowModal();
    key = dlg.cur_key();
    auto res2 =  res == wxID_OK;
    if (res2) {
        if (key.Size() == 1 && CHotKey::IsKnownMods(key.ValueKey()) && !key.GetKeyup()) {
            wxMessageBox(_(L"Modifier must have #up flag"));
            res2 = false;
        }
    }
    return res2;
}



#include "stdafx.h"
#include "Settings.h"
#include "noname.h"

namespace {

HWND curwnd;

LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkKey              = kStruct->vkCode;
        // if (GetLogLevel() >= LOG_LEVEL_1)
        //{
        //	KeyState keyState = GetKeyState(wParam);
        //	SW_LOG_INFO_2(L"%S 0x%x", GetKeyStateName(keyState), vkKey);
        //}

        PostMessage(curwnd, c_MSG_TypeHotKey, wParam, (WPARAM)vkKey);
    }
    return CallNextHookEx(0, nCode, wParam, lParam);
}

class HotKeyDlg : public MyDialog1
{
public:
    HotKeyDlg(CHotKeySet& info, wxFrame* frame) : MyDialog1(frame), info(info)
    {
        m_radioBox1->SetString(0, info.def.ToString());
        m_radioBox1->SetString(1, info.def2.ToString());

        key = info.key;
        if (key == info.def2) {
            m_radioBox1->SetSelection(1);
        }
        updateField();

        curwnd = GetHWND();
        hook = WinApiInt::SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, 0, 0);
        IFW_LOG(hook.IsValid());
    }
    CHotKey key;

private:
    CAutoHHOOK hook;
    void updateField()
    {
        m_textKey->SetValue(key.ToString());
    }
    CHotKeySet info;

    virtual void onSelected(wxCommandEvent& event)
    {
        auto cur = m_radioBox1->GetSelection();
        if (cur == 0) {
            key = info.def;
        } else {
            key = info.def2;
        }
        updateField();
    }

    virtual void onclear(wxCommandEvent& event)
    {
        key.Clear();
        updateField();
    }
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override
    {
        if (nMsg == c_MSG_TypeHotKey) {
            key.Add((TKeyCode)lParam, CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
            updateField();
            return true;
        }

        return MyDialog1::MSWWindowProc(nMsg, wParam, lParam);
    }
    virtual void onSetLeftRight(wxCommandEvent& event)
    {
        key.SetLeftRightMode(m_checkBox12->GetValue());
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

bool ChangeHotKey(wxFrame* frame, HotKeyType type, CHotKey& key)
{
    HotKeyDlg dlg(setsgui.hotkeysList[type], frame);
    auto res = dlg.ShowModal();
    key      = dlg.key;
    return (res == wxID_OK);

}
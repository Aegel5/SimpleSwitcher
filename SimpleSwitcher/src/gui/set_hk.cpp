#include "stdafx.h"
#include "Settings.h"
#include "gen_ui/noname.h"

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

        PostMessage(g_hkdata->hwndEditRevert, c_MSG_TypeHotKey, wParam, (WPARAM)vkKey);
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
        updateField();

        RAWINPUTDEVICE Rid[1];

        Rid[0].usUsagePage = 0x01;           // HID_USAGE_PAGE_GENERIC
        Rid[0].usUsage     = 0x06;           // HID_USAGE_GENERIC_KEYBOARD
        Rid[0].dwFlags     = RIDEV_NOLEGACY; // adds keyboard and also ignores legacy keyboard messages
        Rid[0].hwndTarget  = this->GetHWND();

        if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
            int k = 0;
        }
    }

private:
    void updateField()
    {
        m_textKey->SetValue(key.ToString());
    }
    CHotKeySet info;
    CHotKey key;
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
    bool raw_idev_handler(LPARAM l_param, USHORT& key)
    {
        bool res = false;
        RAWINPUT* raw_buf;
        UINT cb_size;

        /* get the size of the RAWINPUT structure returned */
        GetRawInputData((HRAWINPUT)l_param, RID_INPUT, NULL, &cb_size, sizeof(RAWINPUTHEADER));

        /* allocate memory RAWINPUT structure */
        raw_buf = (PRAWINPUT)malloc(cb_size);
        if (!raw_buf)
            return false;

        /* finally, get the raw input */
        if (GetRawInputData((HRAWINPUT)l_param, RID_INPUT, raw_buf, &cb_size, sizeof(RAWINPUTHEADER))) {
            /* log key if the originating device is keyboard */
            if (raw_buf->header.dwType == RIM_TYPEKEYBOARD &&
                (raw_buf->data.keyboard.Message == WM_KEYDOWN || raw_buf->data.keyboard.Message == WM_SYSKEYDOWN)) {
                key = raw_buf->data.keyboard.VKey;
                res = key != 255;
                //SW_LOG_INFO(L"key: %s", HotKeyNames::Global().GetName(ff));
            }
        }

        free(raw_buf);
        return res;
    }
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override
    {
        if (nMsg == WM_INPUT) {
            auto tyep = GET_RAWINPUT_CODE_WPARAM(wParam);
            //SW_LOG_INFO(L"type: %d", tyep);
            //HRAWINPUT input = (HRAWINPUT)lParam;
            USHORT cur;
            if (raw_idev_handler(lParam, cur)) {
                key.Add(cur, CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
                updateField();
            }
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

bool ChangeHotKey(wxFrame* frame, HotKeyType type) {
    HotKeyDlg dlg(setsgui.hotkeysList[type], frame);


    return dlg.ShowModal() == wxID_OK;
}
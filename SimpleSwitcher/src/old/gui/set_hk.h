#pragma once

#include "noname.h"
#include "wxUtils.h"

class HotKeyDlg : public MyDialog1 {
    static inline HWND curwnd = 0;
    static LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
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
public: using Apply = std::function<void(const CHotKey&)>;
private:  Apply apply;
public: HotKeyDlg(wxFrame* frame) : MyDialog1(frame) {

        WxUtils::BindCheckbox(m_checkBoxDouble,
            [this]() {
                return false;
            },
            [this](bool val) {
                key.SetDouble(val).SetKeyup(false);
                updateField();
            });

        Bind(wxEVT_CLOSE_WINDOW, [this](auto& evt) {
            hook.Cleanup();
            evt.Skip();
            });

        Bind(wxEVT_SHOW, [this](wxShowEvent& evt) {
            if (evt.IsShown() && hook.IsInvalid()) {
                hook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, 0, 0);
                IFW_LOG(hook.IsValid());
            }
            evt.Skip();
            });

        //SetWindowStyleFlag(wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER);

        curwnd = GetHWND();

    }
      void ApplyKey() {
          apply(key_normalized());
      }
      std::vector<wxString> choices;
    void SetToChange(const CHotKeySet& info, Apply&& apply) {
        this->info = info;
        this->apply = apply;

        set_key(info.keys.key());

        choices.clear();
        for (const auto& def : info.def_list) {
            choices.push_back(def.ToString());
        }
        m_choiceKey->Set(choices);

        updateField(false);
    }
private:
    CHotKey key;
    CHotKey last_type_key;
    CHotKey state;
    CHotKey key_normalized() {
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
    void updateField(bool apply = true) {

        m_checkBoxDouble->SetValue(key.IsDouble());
        m_checkBox13->SetValue(key.GetKeyup());

        auto name = key_normalized().ToString();

        if (name.empty()) {
            if(m_choiceKey->GetCount() > info.def_list.size()) {
                choices.resize(info.def_list.size());
                m_choiceKey->Set(choices);
            }
            m_choiceKey->SetSelection(-1);
        }
        else {
            bool found = false;
            for (int i = 0; i < m_choiceKey->GetCount(); i++) {
                if (m_choiceKey->GetString(i) == name) {
                    m_choiceKey->SetSelection(i);
                    found = true;
                    break;
                }
            }

            if (!found) {
                choices.resize(info.def_list.size() + 1);
                choices[info.def_list.size()] = name;
                m_choiceKey->Set(choices);
                m_choiceKey->SetSelection(info.def_list.size());
            }
        }

        if(apply)
            ApplyKey();

    }
    CHotKeySet info;

    virtual void OnChoiceSelect(wxCommandEvent& event) {
        auto cur = m_choiceKey->GetSelection();
        if (cur < info.def_list.size()) {
            set_key(info.def_list[cur]);
        }
        else {
            key = last_type_key;
        }
        updateField();
    }

    virtual void onclear(wxCommandEvent& event) {
        set_key({});
        updateField();
    }
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override {
        if (nMsg == c_MSG_TypeHotKey) {
            auto cur = (TKeyCode)lParam;
            KeyState keyState = GetKeyState(wParam);
            if (keyState == KEY_STATE_DOWN) {
                state.Add(cur, CHotKey::ADDKEY_CHECK_EXIST);
                if (!state.Compare(key)) {
                    last_type_key = key = state;
                    updateField();
                }
            }
            else {
                state.Remove(cur);
            }

            return true;
        }

        return MyDialog1::MSWWindowProc(nMsg, wParam, lParam);
    }
    virtual void onSetLeftRight(wxCommandEvent& event) {
        updateField();
    }
    virtual void onSetKeyup(wxCommandEvent& event) {
        key.SetKeyup(m_checkBox13->GetValue()).SetDouble(false);
        updateField();
    }
};


//bool ChangeHotKey2(wxFrame* frame, CHotKeySet set, CHotKey& key) {
//    HotKeyDlg dlg(set, frame);
//    auto res = dlg.ShowModal();
//    key = dlg.cur_key();
//    auto res2 = res == wxID_OK;
//    if (!res2) return false;
//
//    if (key.Size() == 1 && CHotKey::IsKnownMods(key.ValueKey()) && !key.GetKeyup() && !key.IsDouble()) {
//        wxMessageBox(_(L"Modifier must have #up or #double flag"));
//        return false;
//    }
//    return res2;
//}



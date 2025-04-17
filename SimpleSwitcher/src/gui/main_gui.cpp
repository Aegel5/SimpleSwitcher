#include "sw-base.h"

#include "noname.h" 
#include "SwAutostart.h"
#include "FloatPanel.h"
#include "utils/accessibil.h"
#include <wx/taskbar.h>
#include <wx/custombgwin.h>
#include "wxUtils.h"
#include "Tray.h"
#include "IconManager.h"
#include "set_hk.h"

using namespace WxUtils;

namespace {
	void ShowNeedAdmin(const wxString& expl = L"") {
		wxString ms(_("Need admin rights"));
		if (expl != "") {
			ms += _(" for: ");
			ms += "\"";
			ms += expl;
			ms += "\"";
		}
		wxMessageBox(ms);
	}
	bool startOk() {
		return Utils::IsSelfElevated() || !conf_get_unsafe()->isMonitorAdmin;
	}
}
extern void StartGui2();
class TemporGuiHolder {
	std::thread thr;
public:
	void Start() {
		thr = std::thread(StartGui2);
	}
	~TemporGuiHolder() {
		g_exit = true;
		if (thr.joinable())
			thr.join();
	}
};

class MainWnd : public MyFrame4 {
public:TemporGuiHolder gui2;
private:
	IconManager iconsMan;
	Timers timers;
	CoreWorker coreWork;
	MyTray myTray;
	bool exitRequest = false;
	bool inited = false;
	CAutoHotKeyRegister enable_hk_register;
	HotKeyDlg* setHotKeyWnd = nullptr;

	static constexpr wxLanguage supportTranslations[] = { wxLANGUAGE_ENGLISH , wxLANGUAGE_RUSSIAN, wxLANGUAGE_HEBREW };

	std::generator<FloatPanel*> all_panels() {
		for (auto* it : this->GetChildren()) {
			auto cur = wxDynamicCast(it, FloatPanel);
			if (cur != 0)
				co_yield cur;
		}
	}
	void UpdateIcons() {
		if (conf_get_unsafe()->flagsSet == ProgramConfig::showAppIcon) {
            myTray.ResetIcon(iconsMan.Get_Standart(!g_enabled.IsEnabled()).bundle);
		}
		else {
			Worker()->PostMsg(Message_GetCurLay{ true });
		}
	}
public:

	MainWnd() : MyFrame4(nullptr) {
		try {

			g_guiHandle = GetHandle();

			if (!g_usenewgui) {
				if (startOk()) {
					if (Utils::IsDebug()) {
						if (!g_enabled.TryEnable()) {
							auto hk = conf_get_unsafe()->GetHk(hk_ToggleEnabled).keys.key();
							for (auto& it : hk) if (it == VKE_WIN) it = VK_LWIN;
							InputSender::SendHotKey(hk);
							Sleep(50);
						}
					}
					g_enabled.TryEnable();
				}
			}

			coreWork.Start();

			timers.StartCycle(200, [this]() { Worker()->PostMsg(Message_GetCurLay{});
			if (m_checkBoxEnable->GetValue() != g_enabled.IsEnabled()) {
				updateEnable();
			}
				});

			Bind(wxEVT_CLOSE_WINDOW, &MainWnd::onExitReqest, this);
			SetTitle(std::format(
				L"{} {}{}{}",
				GetTitle().t_str(),
				StrUtils::Convert(SW_VERSION),
				Utils::IsSelfElevated() ? L" Administrator" : L"",
				Utils::IsDebug() ? L" DEBUG" : L""));
			SetWindowStyleFlag(wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
				| wxRESIZE_BORDER
			);

			         auto font = m_gridHotKeys->GetDefaultCellFont();
					 auto font2 = font;
			         font2.SetPointSize(font2.GetPointSize() + 1);
			         m_gridHotKeys->SetDefaultCellFont(font2);
			         m_gridHotKeys->SetLabelFont(font);
			         m_gridLayouts->SetDefaultCellFont(font2);
			         m_gridLayouts->SetLabelFont(font);
            //m_gridHotKeys->SetLabelBackgroundColour(m_gridHotKeys->GetDefaultCellBackgroundColour());

					 m_gridHotKeys->Bind(wxEVT_SIZE, [&](auto& evt) {
					     m_gridHotKeys->SetColSize(0, m_gridHotKeys->GetSize().x - m_gridHotKeys->GetRowLabelSize());
					     });

            //m_gridHotKeys->SetDefaultEditor(new wxGridCellChoiceEditor({"test", "test"}));

					 m_staticTextBuildDate->SetLabelText(std::format("Built on '{}'", __DATE__));

					 m_hyperlink11->SetLabel(L"SimpleSwitcher.json");
					 m_hyperlink11->SetURL(std::format(L"file://{}", GetPath_Conf()));

					 BindButtom(m_buttonReloadConfig, []() {
					     auto conf = ConfPtr(new ProgramConfig());
					     auto errLoadConf = LoadConfig(*conf);
					     IFS_LOG(errLoadConf);
					     if (errLoadConf != SW_ERR_SUCCESS) {
					         wxMessageBox(_("Error reading config"));
					     }
					     else {
					         _conf_set(conf);
					     }
					     });

					 BindButtom(m_buttonAddPanel, [this]() {
					     return;
					     auto wnd = new FloatPanel(this);
					     wnd->Show(true);
					     });

					 BindButtom(m_buttonDelAllPanels, [this]() {
					     std::vector to_del{ std::from_range, all_panels() };
					     for (auto p : to_del) {
					         p->Destroy();
					     }
					     });

					 SyncLayouts();

					 m_notebook2->SetSelection(0);

					 BindCheckbox(m_checkDebuglog, []() {return GetLogLevel() > LOG_LEVEL_DISABLE; }, [](bool val) {
					     SetLogLevel_info(val? conf_get_unsafe()->logLevel : LOG_LEVEL_DISABLE);
					     });

					 BindCheckbox(m_checkBoxFixRAlt, 
					     []() {return conf_get_unsafe()->fixRAlt; }, 
					     [](bool val) {
					         SaveConfigWith([val](auto conf) {conf->fixRAlt = val; });
					     });
					 m_checkBoxFixRAlt->SetLabelText(m_checkBoxFixRAlt->GetLabelText() + L" \"" + Utils::GetNameForHKL(conf_get_unsafe()->fixRAlt_lay_)+"\"");

					 BindCheckbox(m_checkBoxPrevent, 
					     []() {return conf_get_unsafe()->EnableKeyLoggerDefence; }, 
					     [](bool val) {
					         SaveConfigWith([val](auto conf) {conf->EnableKeyLoggerDefence = val; });
					  });

					 BindChoice(m_choiceShowInTray, [&](wxChoice* elem) {
					     initChoiceFlags();
					     },
					     [&](wxChoice* elem) {
					         updateShowInTray(elem->GetStringSelection());
					         UpdateIcons();
					     });

					 BindButtom(m_buttonUpdateFlags, [this]() {
					     initChoiceFlags();
					     UpdateIcons();
					     });

					 BindCheckbox(m_checkBoxDisablAcc,
					     []() {
					         AllowAccessibilityShortcutKeys(!conf_get_unsafe()->disableAccessebility);
					         return conf_get_unsafe()->disableAccessebility; },
					     [](bool val) {
					         SaveConfigWith([val](auto cfg) {cfg->disableAccessebility = val; });
					         AllowAccessibilityShortcutKeys(!conf_get_unsafe()->disableAccessebility);
					     });

					 BindCheckbox(m_checkBoxSeparateExt,
					     []() {return conf_get_unsafe()->separate_ext_last_word && conf_get_unsafe()->separate_ext_several_words; },
					     [](bool val) {
					         SaveConfigWith([val](auto conf) {conf->separate_ext_last_word = conf->separate_ext_several_words = val; });
					     });

					 BindCheckbox(m_checkBoxAlterantiveLayoutChange, []() {return conf_get_unsafe()->AlternativeLayoutChange; }, [this](bool val) {
					     SaveConfigWith([val](auto cfg) {cfg->AlternativeLayoutChange = val; });
					     FillLayoutsInfo();
					     });

					 BindCheckbox(m_checkBoxClearForm, []() {return conf_get_unsafe()->fClipboardClearFormat; }, [](bool val) {
					     SaveConfigWith([val](auto cfg) {cfg->fClipboardClearFormat = val; });
					     });

					 BindCheckbox(m_checkBoxWorkInAdmin, []() {return conf_get_unsafe()->isMonitorAdmin; }, [this](bool val) {
					     SaveConfigWith([val](auto cfg) {cfg->isMonitorAdmin = val; });
					     updateAutoStart();
					     updateEnable();
					     });

					 BindChoice(m_comboUiLang, [this](wxChoice* elem) {
					         elem->Clear();

					         elem->SetSelection(-1);
					         for (int i = -1; auto lang : supportTranslations) {
					             i++;
					             elem->AppendString(wxLocale::GetLanguageName(lang));
					             if (conf_get_unsafe()->uiLang_ == lang) {
					                 elem->SetSelection(i);
					             }
					         }
					     }, [this](wxChoice* elem) {
					         SaveConfigWith(
					             [this, elem](auto cfg) {
					             cfg->uiLang_ = supportTranslations[elem->GetSelection()];
					             });
					         wxMessageBox(_("Need restart program"));
					         });

					     m_gridLayouts->GetGridWindow()->Bind(wxEVT_RIGHT_DOWN, [this](auto& evt) {
					         // Show popupmenu at position
					         wxMenu menu;
					         static const int update_id = 123;
					         menu.Append(update_id, wxT("&Update"));
					         menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [this](auto& e) {
					             if (e.GetId() == update_id) {
					                 SyncLayouts();
					                 FillLayoutsInfo();
					             }
					             });
					         PopupMenu(&menu);
					         });

					 updateAutoStart();

					 FillHotkeysInfo();
					 FillLayoutsInfo();

					 updateCapsTab();

			myTray.Init(this);
			myTray.ResetIcon(iconsMan.Get_Standart().bundle);
			this->SetIcons(iconsMan.GetAppIcons());
			myTray.Bind(wxEVT_MENU, &MainWnd::onExit, this, Minimal_Quit);
			myTray.Bind(wxEVT_MENU, [this](auto& evt) {
				show_wnd();
				}, Minimal_Show);
			myTray.Bind(wxEVT_MENU, [this](auto& evt) {
				g_enabled.TryToggle();
				}, Minimal_Activate);
			myTray.Bind(wxEVT_TASKBAR_LEFT_DCLICK, [this](auto& evt) {
				show_wnd();
				});



					BindCheckbox(m_checkBoxEnable, []() {return false; }, [this](auto val) {TryEnable(val); });

					updateEnable();

		}
		catch (std::exception& e) {
			wxMessageBox(_("Error while initing main wnd: ") + e.what());
		}
		inited = true;
	}

private:

	void show_wnd() {
		if (g_usenewgui) {
			g_show_gui = true;
		}
		else {
			ForceShow(this);
		}
	}

	void initChoiceFlags() {
		m_choiceShowInTray->Clear();
		m_choiceShowInTray->AppendString(ProgramConfig::showAppIcon);
		m_choiceShowInTray->AppendString(ProgramConfig::showOriginalFlags);
		for (const auto& it : iconsMan.ScanFlags()) {
			m_choiceShowInTray->AppendString(it);
		}
		updateShowInTray(conf_get_unsafe()->flagsSet);
	}

	void updateShowInTray(wxString newVal) {

		bool found = false;
		for (int i = 0; i < m_choiceShowInTray->GetCount(); i++) {
			auto cur = m_choiceShowInTray->GetString(i);
			if (cur == newVal) {
				m_choiceShowInTray->SetSelection(i);
				found = true;
				break;
			}
		}
		if (!found) {
			newVal = ProgramConfig::showOriginalFlags;
			m_choiceShowInTray->SetSelection(1);
		}
		if (conf_get_unsafe()->flagsSet != newVal) {
			SaveConfigWith([&](auto cfg) {cfg->flagsSet = newVal; });
		}
	}


	int getChildIndex(wxWindow* obj) {
		auto par = obj->GetParent();
		for (int i = 0; i < par->GetChildren().size(); i++) {
			if (par->GetChildren()[i] == obj)
				return i;
		}
		return -1;
	}


	void onExitReqest(wxCloseEvent& event) {

		if (event.CanVeto() && !exitRequest) {
			Hide();
			event.Veto();
			return;
		}

		LOG_ANY(L"destroy GUI");
		Destroy(); // you may also do:  event.Skip();
		// since the default event handler does call Destroy(), too
	}



	virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override {

		if (nMsg == WM_LayNotif) {

			if (!inited)
				return TRUE;

			if (conf_get_unsafe()->flagsSet == ProgramConfig::showAppIcon)
				return TRUE;

            auto info = iconsMan.Get((HKL)wParam, !g_enabled.IsEnabled());
			myTray.ResetIcon(info.bundle);
		}
		else if (nMsg == WM_HOTKEY) {
			if (wParam == 998) {
				TryEnable(!IsCoreEnabled());
			}
		}
		else if (nMsg == WM_ShowWindow) {
			show_wnd();
		}
		else {
			return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
		}

		return TRUE;
	}

	virtual void onHotDClick(wxGridEvent& event) override {

		bool isDouble = event.GetEventType() == wxEVT_GRID_CELL_LEFT_DCLICK;

		int col = event.GetCol();
		int row = event.GetRow();

		GETCONF;

		const auto& hotlist = cfg->hotkeysList;
		if (row >= hotlist.size()) return;
		const auto& data = hotlist[row];
		auto hk = data.hkId;


		if (col == 0) {
			CHotKey newkey;
			ChangeHotKey(data, [row, this, hk](auto key) {
				SaveConfigWith([&](auto cfg) {cfg->hotkeysList[row].keys.key() = key; });
				m_gridHotKeys->SetCellValue(row, 0, L" " + key.ToString());
				}, isDouble);
		}

		event.Skip();

	}

	virtual void on_grid_lay_double(wxGridEvent& event) override {

		event.Skip();

		bool isDouble = event.GetEventType() == wxEVT_GRID_CELL_LEFT_DCLICK;

		int col = event.GetCol();
		int row = event.GetRow();

		GETCONF;

		auto& lays = cfg->layouts_info.info;
		if (row >= lays.size()) return;
		auto& data = lays[row];

		if (isDouble && col == 0) {
			SaveConfigWith([&](auto conf) {  conf->layouts_info.info[row].enabled ^= 1; });
			FillLayoutsInfo(false);
		}

		CHotKeySet set;

		if (col == 1) {
			CHotKey newkey;
			set.def_list = {
				(CHotKey)CHotKey(VK_LCONTROL).SetKeyup(),
				(CHotKey)CHotKey(VK_RCONTROL).SetKeyup(),
				(CHotKey)CHotKey(VK_LSHIFT).SetDouble(),
				(CHotKey)CHotKey(VK_RSHIFT).SetDouble()
			};
			set.keys = data.hotkey;
		}
		if (col == 2) {
			CHotKey newkey;
			set.def_list = {
				CHotKey(VK_LMENU, VK_SHIFT, 0x31)
				,CHotKey(VK_LMENU, VK_SHIFT, 0x32)
				,CHotKey(VK_LMENU, VK_SHIFT, 0x33)
				,CHotKey(VK_CONTROL, VK_SHIFT, 0x31)
				,CHotKey(VK_CONTROL, VK_SHIFT, 0x32)
				,CHotKey(VK_CONTROL, VK_SHIFT, 0x33)
			};
			set.keys.key() = data.win_hotkey;
		}

		if (Utils::is_in(col, 1, 2)) {
			ChangeHotKey(set, [this, row, col](auto key) {
				SaveConfigWith([&](auto conf) {
					if (row >= conf->layouts_info.info.size()) return;
					auto& rec = conf->layouts_info.info[row];
					if (col == 2)
						rec.win_hotkey = key;
					else
						rec.hotkey.key() = key;
					});
				m_gridLayouts->SetCellValue(row, col, key.ToString());
				m_gridLayouts->AutoSizeColumn(col);
				}, isDouble);
		}
	}

	void ChangeHotKey(const CHotKeySet& set, HotKeyDlg::Apply&& apply, bool show = false) {

		if (setHotKeyWnd == nullptr) {
			setHotKeyWnd = new HotKeyDlg(this);
		}

		setHotKeyWnd->SetToChange(set, std::move(apply));

		if (show) {
			setHotKeyWnd->Show();
		}
	}

	void updateCapsTab() {
		BufScanMap remap;
		IFS_LOG(remap.FromRegistry());

		TKeyCode caps;

		remap.GetRemapedKey(VK_CAPITAL, caps);
		m_checkcapsrem->SetValue(caps == VK_F24);

		remap.GetRemapedKey(VK_SCROLL, caps);
		m_check_scrollremap->SetValue(caps == VK_F23);

		auto caption = [](DWORD sc, DWORD vc) {
			return std::format(L"sc: {}, vk: {}, '{}'", sc, vc, CHotKey::ToString(vc));
			};

		m_listBoxRemap->Clear();
		for (auto iter = remap.GetIter(); !iter.IsEnd(); ++iter) {
			auto [sc, vc] = iter.curElemSrc();
			auto [sc2, vc2] = iter.curElemDst();
			m_listBoxRemap->Append(std::format(L"{} -> {}", caption(sc, vc), caption(sc2, vc2)));
		}
	}
	virtual void onRemapCaps(wxCommandEvent& event) {
		auto obj = event.GetEventObject();
		auto check = wxDynamicCast(obj, wxCheckBox);
		if (check == nullptr)
			return;

		if (!Utils::IsSelfElevated()) {
			check->SetValue(!check->GetValue());
			ShowNeedAdmin("");
			return;
		}

		BufScanMap remap;
		IFS_LOG(remap.FromRegistry());

		bool showmsg = false;
		if (check->GetValue()) {
			if (check == m_checkcapsrem) {
				remap.PutRemapKey(VK_CAPITAL, VK_F24);
			}
			else {
				remap.PutRemapKey(VK_SCROLL, VK_F23);
			}
			showmsg = true;

		}
		else {
			remap.DelRemapKey(check == m_checkcapsrem ? VK_CAPITAL : VK_SCROLL);
		}
		IFS_LOG(remap.ToRegistry());

		updateCapsTab();

		if (showmsg)
			wxMessageBox(_("Will be applied after PC reboot"));
	}

	void updateEnable() {
		if (!startOk()) {
			g_enabled.TryEnable(false);
		}
		m_checkBoxEnable->SetValue(g_enabled.IsEnabled());
		UpdateIcons();
	}

	void FillHotkeysInfo(bool refit = true) {

		ClearGrid(m_gridHotKeys);

		for (int i = -1; const auto & it : conf_get_unsafe()->hotkeysList) {
			i++;
			m_gridHotKeys->AppendRows();
			m_gridHotKeys->SetRowLabelValue(i, wxGetTranslation(GetGuiTextForHk(it.hkId)));
			m_gridHotKeys->SetCellValue(i, 0, L" " + it.keys.ToString());
		}

		m_gridHotKeys->SetRowLabelSize(wxGRID_AUTOSIZE);

		if (refit) {
			FitHeight(m_gridHotKeys);
			FixProportions();
		}
		else {
			m_gridHotKeys->AutoSizeRows();
		}

	}
	void SyncLayouts() {

		HKL all_lays[50] = { 0 };
		int all_lay_size = GetKeyboardLayoutList(std::ssize(all_lays), all_lays);
		auto has_system_layout = [&](HKL lay) {
			for (int i = 0; i < all_lay_size; i++) {
				auto cur = all_lays[i];
				if (cur == lay) return true;
			}
			return false;
			};

		auto info_copy = conf_get_unsafe()->layouts_info;
		auto& info = info_copy.info;
		bool was_changes = false;

		// удалим те, которых сейчас нет в системе
		for (int i = (int)info.size() - 1; i >= 0; i--) {
			if (!has_system_layout(info[i].layout)) {
				Utils::RemoveAt(info, i);
				was_changes = true;
			}
		}

		// добавим все новые
		for (int i = 0; i < all_lay_size; i++) {
			auto cur = all_lays[i];
			if (!info_copy.HasLayout(cur)) {
				was_changes = true;
				//CHotKey winhk(VK_LMENU, VK_SHIFT, VKE_1 + info.size());
				info.push_back({ .layout = cur });
			}
		}

		if (was_changes) {
			// пересохраним если были изменения.
			SaveConfigWith([&](auto conf) {  conf->layouts_info = info_copy; });
		}

	}
	void FixProportions() {
		auto sz = GetSize();
		auto sz2 = sz;
		sz2.x = std::max((double)sz.x, sz.y * 1.16);
		sz2.y = std::max((double)sz.y, sz.x / 1.16);
		SetSize(sz2);

	}
	void FillLayoutsInfo(bool refit = true) {

		ClearGrid(m_gridLayouts);

		while (m_gridLayouts->GetNumberCols() >= 3) {
			m_gridLayouts->DeleteCols(2);
		}
		if (conf_get_unsafe()->AlternativeLayoutChange) {
			m_gridLayouts->AppendCols();
			m_gridLayouts->SetColLabelValue(2, L"Win hotkey");
		}

		// отобразим в gui
		for (int i = -1; const auto & it: conf_get_unsafe()->layouts_info.info) {
			i++;
			auto name = Utils::GetNameForHKL(it.layout);
			m_gridLayouts->AppendRows();
			m_gridLayouts->SetRowLabelValue(i, name);
			if (it.enabled) {
				m_gridLayouts->SetCellValue(i, 0, "X");
			}
			m_gridLayouts->SetCellValue(i, 1, it.hotkey.ToString());

			if (conf_get_unsafe()->AlternativeLayoutChange) {
				m_gridLayouts->SetCellValue(i, 2, it.win_hotkey.ToString());
				m_gridLayouts->SetCellBackgroundColour(i, 2, wxColor(0xE3, 0xF2, 0xFD));
			}
		}
		//m_gridLayouts->SetRowLabelSize(wxGRID_AUTOSIZE);
		m_gridLayouts->SetRowLabelSize(m_gridHotKeys->GetRowLabelSize());
		m_gridLayouts->AutoSizeColumns(false);
		if (refit && m_gridLayouts->GetNumberRows() <= 15) {
			FitHeight(m_gridLayouts);
			FixProportions();
		}
		else {
			m_gridLayouts->AutoSizeRows();
		}
	}

	void updateAutoStart() {
	    m_checkAddToAutoStart->SetValue(autostart_get());
	}

	//virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override
	//{
	//    // Pass the messages to the original WinAPI window procedure
	//    return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
	//}
public:

	void onExit(wxCommandEvent& event) override {
		exitRequest = true;
		LOG_ANY(L"exit request");
		Close(true);
	}

	void onCloseToTray(wxCommandEvent& event) {
		Hide();
	}

	void onAutocheck(wxCommandEvent& event) override {
	    if (conf_get_unsafe()->isMonitorAdmin && !Utils::IsSelfElevated()) {
	        m_checkAddToAutoStart->SetValue(!m_checkAddToAutoStart->GetValue());
	        ShowNeedAdmin("");
	        return;
	    }
		autostart_set(m_checkAddToAutoStart->GetValue());
	    updateAutoStart();
	}

	void TryEnable(bool val) {
		m_checkBoxEnable->SetValue(val);
		if (IsCoreEnabled()) {
			if (startOk()) {
				g_enabled.TryEnable();
			}
			else {
				ShowNeedAdmin("");
			}
		}
		else {
			g_enabled.TryEnable(false);
		}
		updateEnable();
	}

	bool IsCoreEnabled() { return m_checkBoxEnable->IsChecked(); }

};

wxMenu* MyTray::CreatePopupMenu() {

	auto menu = new wxMenu();

	for (int i = -1; const auto & it : conf_get_unsafe()->layouts_info.info) {
		i++;
		menu->Append(Minimal_SetLay_1 + i, Utils::GetNameForHKL(it.layout));
	}

	menu->AppendSeparator();
    menu->Append(Minimal_Activate, g_enabled.IsEnabled() ? _("Disable") : _("Enable"));
	menu->Append(Minimal_Show, _("Show"));
	menu->Append(Minimal_Quit, _("Exit"));


	return menu;
}



void StartMainGui(bool show, bool conf_err_msg) {

	MainWnd* frame = new MainWnd();

	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	frame->Show(show && !g_usenewgui);

	if (g_usenewgui) {
		g_show_gui = show;
		frame->gui2.Start();
	}
	else {

		if (conf_err_msg) {
			wxMessageBox(_("Error reading config"));
		}

		if (!startOk()) {
			ShowNeedAdmin();
		}
	}


}




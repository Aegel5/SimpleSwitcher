
class SetHotKeyCombo {

	std::string title;
	std::vector<std::string> defaults;
	std::string key_str; // cache
	std::string key_str_all; 
	bool popup_open = false;
	CHotKeyList* hotkeys;
	int iCurrentHk = 0;

	inline static bool left_right = false;
	//inline static std::atomic< std::pair<DWORD, KeyState>> last_type = {};
	inline static std::pair<DWORD, KeyState > last_type = {};
	inline static CHotKey state;
	inline static CAutoHHOOK hook;

	static LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
		if (nCode == HC_ACTION) {
			KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;
			bool isInjected = TestFlag(kStruct->flags, LLKHF_INJECTED);
			if (!isInjected) {
				last_type = { kStruct->vkCode, GetKeyState(wParam) };
			}
			return 1;
		}
		return CallNextHookEx(0, nCode, wParam, lParam);
	}
	const CHotKey& cur_key() {	
		return hotkeys->keys[iCurrentHk];
	}
	void Reinit(int newIndex = -1) {
		auto was_changes = hotkeys->DeleteEmpty();
		if (iCurrentHk >= hotkeys->keys.size()) iCurrentHk = 0;
		if (newIndex != -1) {
			iCurrentHk = newIndex;
			if (iCurrentHk >= hotkeys->keys.size()) {
				hotkeys->keys.resize(iCurrentHk + 1);
				was_changes = true;
			}
		}
		build_strings();
		state.Clear();
		if (was_changes) {
			SaveApplyGuiConfig();
		}
	}
	void build_strings() {
		key_str = StrUtils::Convert(cur_key().ToString());
		key_str_all = StrUtils::Convert(hotkeys->ToString());
	}
	void SetKey(CHotKey k) {
		if (cur_key().Compare(k, CHotKey::COMPARE_STRICK_MODIFIER)) return;
		hotkeys->keys[iCurrentHk] = k;
		SaveApplyGuiConfig();
		build_strings();

	}
	void InitHooks() {
		if (hook.IsInvalid()) {
			hook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, 0, 0);
		}
		state.Clear();
	}
public:
	SetHotKeyCombo(std::string title, auto&& def, CHotKeyList* hotkeys) : title(std::move(title)), hotkeys(hotkeys) {
		for (const CHotKey& it : def) {
			defaults.push_back(StrUtils::Convert(it.ToString()));
		}
		Reinit();
	}
	void Draw() {

		auto w = ImGui::CalcTextSize("Ctrl + Shift + CapsLock #double");

		ImGui::SetNextItemWidth(w.x + ImGui::GetStyle().FramePadding.x * 2.f);

		if (ImGui::BeginCombo(title.c_str(), key_str_all.c_str(), ImGuiComboFlags_HeightLarge | ImGuiComboFlags_NoArrowButton)) {

			ImWantFrameWithDelay(0.060f);

			popup_open = true;

			if (ImGui::IsWindowAppearing()) {
				InitHooks();
				ImGui::SetKeyboardFocusHere();
			}

			{
				auto [vk, oper] = last_type;
				if (vk != 0) {
					last_type = {};
					if (!left_right) {
						vk = CHotKey::Normalize(vk);
					}
					if (oper == KEY_STATE_DOWN) {
						state.Add(vk, CHotKey::ADDKEY_CHECK_EXIST | CHotKey::ADDKEY_NO_STRICK_MODS_CHECK);
						SetKey(state);
					}
					else {
						state.Clear();
					}
				}
			}

			bool found = false;
			for (const auto it : defaults) {
				bool is_current = it == key_str;
				if (is_current)
					found = true;
				if (ImGui::Selectable(it.c_str(), is_current)) {
					SetKey(CHotKey::FromString(StrUtils::Convert(it)));
				}

			}

			ImGui::Separator();

			ImGui::AlignTextToFramePadding();
			ImGui::Text(LOC("Press any key..."));
			ImGui::SameLine();
			ImGui::Checkbox(LOC("Left right difference"), &left_right);
			//ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(w.x*0.9f);
			static std::string empty;
			ImGui::InputText("##cur", found ? &empty:&key_str, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button(LOC("Clear"))) {
				SetKey({});
			}

			{
				
				bool val = cur_key().GetKeyup();
				if (ImGui::Checkbox("KEYUP", &val)) {
					auto k = cur_key();
					if (val) k.SetDouble(false);
					k.SetKeyup(val);
					SetKey(k);
				}
			}

			{
				bool val = cur_key().IsDouble();
				if (ImGui::Checkbox(LOC("Double-tapping"), &val)) {
					auto k = cur_key();
					if (val) k.SetKeyup(false);
					k.SetDouble(val);
					SetKey(k);
				}
			}

			{
				ImGui::SameLine();
				UStr t = iCurrentHk == 0 ? " 1 " : " 2 ";
				ImGuiUtils::SetCursorToRightForButton(t);
				if (ImGui::Button(t)) {
					Reinit(iCurrentHk ^ 1);
				}
			}

			if (!WinUtils::IsCurrentProcessActive()) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndCombo();
		}
		else {
			if (popup_open) {
				popup_open = false;
				hook.Cleanup();
				Reinit();
			}
		}
	}
};

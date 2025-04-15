
class SetHotKeyCombo {

	UStr title;
	std::vector<std::string> defaults;
	CHotKey key;
	std::string key_str; // cache
	using Apply = std::function<void(const CHotKey&)>;
	Apply apply;
	bool popup_open = false;

	inline static bool left_right = false;
	inline static std::atomic< std::pair<DWORD, KeyState>> last_type = {};
	inline static CHotKey state;
	inline static CAutoHHOOK hook;

	static LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
		if (nCode == HC_ACTION) {
			KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;
			bool isInjected = TestFlag(kStruct->flags, LLKHF_INJECTED);
			if (!isInjected) {
				last_type = { kStruct->vkCode, GetKeyState(wParam) };
			}
		}
		return 1;
	}
	void SetKey(CHotKey k, bool apl = true) {
		if (key.Compare(k, CHotKey::COMPARE_STRICK_MODIFIER)) return;
		key = k;
		key_str = Str_Utils::Convert(key.ToString());
		left_right = key.Has_left_right();
		if(apl)
			apply(key);
	}
	void Init() {
		if (hook.IsInvalid()) {
			hook = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, 0, 0);
		}
		state.Clear();
	}
public:
	SetHotKeyCombo(UStr title, CHotKey key, const std::vector<CHotKey>& def, auto&& apply) : title(title), apply(apply) {
		for (const auto& it : def) {
			defaults.push_back(Str_Utils::Convert(it.ToString()));
		}
		SetKey(key,false);
	}
	void Draw() {

		ImGui::SetNextItemWidth(180);

		if (ImGui::BeginCombo(title, key_str.c_str(), ImGuiComboFlags_HeightLarge)) {

			popup_open = true;

			if (ImGui::IsWindowAppearing()) {
				Init();
				ImGui::SetKeyboardFocusHere();
			}

			{
				auto cur = last_type.load();
				auto [vk, oper] = cur;
				if (vk != 0) {
					if (!left_right) {
						vk = CHotKey::Normalize(vk);
					}
					last_type.exchange({});
					if (oper == KEY_STATE_DOWN) {
						state.Add(vk, CHotKey::ADDKEY_CHECK_EXIST | CHotKey::ADDKEY_NO_STRICK_MODS_CHECK);
						SetKey(state);
					}
					else {
						state.Remove(vk, false);
					}
				}
			}

			bool found = false;
			for (const auto it : defaults) {
				bool is_current = it == key_str;
				if (is_current)
					found = true;
				if (ImGui::Selectable(it.c_str(), is_current)) {
					SetKey(CHotKey::FromString(Str_Utils::Convert(it)));
				}

			}

			ImGui::Separator();

			ImGui::AlignTextToFramePadding();
			ImGui::Text(LOC("Press any key..."));
			ImGui::SameLine();
			ImGui::Checkbox(LOC("Left right difference"), &left_right);
			//ImGui::AlignTextToFramePadding();
			ImGui::SetNextItemWidth(160);
			static std::string empty;
			ImGui::InputText("##cur", found ? &empty:&key_str, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button(LOC("Clear"))) {
				SetKey({});
			}

			{
				bool val = key.GetKeyup();
				if (ImGui::Checkbox(LOC("Triggers on KEYUP"), &val)) {
					if (val) key.SetDouble(false);
					auto k = key;
					k.SetKeyup(val);
					SetKey(k);
				}
			}

			{
				bool val = key.IsDouble();
				if (ImGui::Checkbox(LOC("Triggers on double press"), &val)) {
					if (val) key.SetKeyup(false);
					auto k = key;
					k.SetDouble(val);
					SetKey(k);
				}
			}


			ImGui::EndCombo();
		}
		else {
			if (popup_open) {
				popup_open = false;
				hook.Cleanup();
			}
		}
	}
};

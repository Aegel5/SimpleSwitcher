#pragma once

// Пока без поддержки Alternative Mode

class FixRAltHandler {

    void Handle_up(CurStateWrapper& state) {

		if (state.Size() != 0) {
			return;

		if (original_lay != 0) {
			auto info = Utils::GetFocusedWndInfo();
			LOG_ANY(L"return origin layout");
			Utils::SetLayPost(info.hwnd_default, original_lay);
			original_lay = 0;
		}
		if (temp_lay != 0) {
			LOG_ANY(L"unload temp layout");
			Sleep(15);
			IFW_LOG(UnloadKeyboardLayout(temp_lay));
			temp_lay = 0;
		}
    }
    void Handle_down(CurStateWrapper& state) {

		const auto& curk = state.state;

		if (curk.Size() == 3
			&& curk.HasKey(VK_LMENU, true)
			&& curk.HasKey(VK_CONTROL, false)
			&& !curk.IsKnownMods(vkCode)
			&& vkCode == curk.ValueKey()
			&& conf_get()->fixRAlt
			&& conf_get()->fixRAlt_lay_ != 0
			) {

			auto info = Utils::GetFocusedWndInfo();
			auto need_lay = conf_get()->fixRAlt_lay_;
			auto cur_lay = info.lay;

			if (cur_lay == need_lay) return 0;

			LOG_ANY(L"fix ralt");

			original_lay = cur_lay;

			if (conf_get()->layouts_info.GetLayoutInfo(need_lay) == nullptr) {
				if (temp_lay == 0) {
					auto str = std::format(L"{:x}", (int)need_lay);
					LOG_ANY(L"load temp layout {}", str);
					temp_lay = LoadKeyboardLayout(str.c_str(), KLF_ACTIVATE);
					IFW_LOG(temp_lay != NULL);
					Sleep(15);
				}
			}
			Utils::SetLayPost(info.hwnd_default, temp_lay);
		}
    }
};
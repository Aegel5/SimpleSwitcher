#pragma once

inline void SyncLayouts() {

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



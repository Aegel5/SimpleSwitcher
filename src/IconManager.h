#pragma once
#include "utils/Images.h"

class IconMgr {
	std::filesystem::path flagFold;
	using Bundle = std::vector<Images::ImageIcon>;
	std::map<wstring, Bundle> icons;

	const Bundle& GetBundle(TStr local_id_, bool is_gray = false) {

		wstring local_id = local_id_;
		StrUtils::ToLower(local_id);

		namespace fs = std::filesystem;

		GETCONF;

		auto folder_name = StrUtils::Convert(cfg->flagsSet);
		wstring key = std::format(L"{}$&{}{}", local_id, folder_name, is_gray ? L"$%^&!" : L"");

		auto it = icons.find(key);
		if (it != icons.end()) {
			return it->second;
		}

		std::vector<Images::Image> bndl;

		auto dir = flagFold / folder_name;
		if (fs::is_directory(dir)) {

			auto search = [&](const auto& id) {
				for (const auto& entry : fs::directory_iterator(dir)) {

					if (!entry.is_directory()) 
						continue;

					auto name = entry.path().filename().wstring();
					StrUtils::ToLower(name);

					if (name != id) continue;

					for (const auto& entry2 : fs::directory_iterator(entry)) {
						if (entry2.is_regular_file()) {
							auto cur = Images::LoadImageFromFile(entry2.path().string().c_str());
							if (cur->IsOk()) {
								bndl.push_back(cur);
							}
						}
					}
				}
				};

			if (cfg->useBritishFlag && (local_id == L"en" || local_id.starts_with(L"en-"))) {
				search(L"en-gb"s);
			}
			else {
				search(local_id);

				if (bndl.empty()) {
					// ничего не нашли, попробуем поискать только по языку.
					auto pos = local_id.find(L'-');
					if (pos != std::wstring::npos) {
						SView id_lang{ local_id.data(), pos };
						search(id_lang);
					}
				}
			}

			// todo поиск только по региону


			if (is_gray) {
				for (auto& it : bndl) {
					Images::SetBrightness(it, 0.8f);
				}
			}
		}

		Bundle res;

		for (auto& it : bndl) {
			res.push_back(Images::ImageToIconConsume(it));
		}

		return icons.emplace(key, res).first->second;
	}
public:
	UStr folder() {
		return 0;
	}
	IconMgr() {
		flagFold = PathUtils::GetPath_folder_noLower2() / L"Flags";
	}
	static IconMgr& Inst() {
		static IconMgr inst;
		return inst;
	}

	Images::ImageIcon GetIcon(TStr contry_id, Vec_i2 size, bool is_gray = false) {

		// приоритет: 1) все границы равны. 2) 1 граница равна, другая меньше 3) самый большой размер
		const auto& bndl = GetBundle(contry_id, is_gray);
		Images::ImageIcon pr2;
		Images::ImageIcon pr3;
		for (const auto it : bndl) {

			auto w = it->img.width;
			auto h = it->img.height;

			if (w == size.x && h == size.y) 
				return it;

			if ((w == size.x && h < size.y) || (w < size.x && h == size.y)) {
				pr2 = it;
			}
			auto sum = w + h;
			if (!pr3 || pr3->img.width + pr3->img.height < sum) {
				pr3 = it;
			}
		}

		if (pr2) return pr2;
		if (pr3) return pr3;

		return std::make_shared<Images::ImageIcon::element_type>(); // empty
	}

	void ClearCache() {
		icons.clear();
	}

	std::generator<string> ScanFlags() {
		namespace fs = std::filesystem;
		if (fs::is_directory(flagFold)) {
			for (const auto& entry : fs::directory_iterator(flagFold)) {
				if (entry.is_directory()) {
					co_yield entry.path().filename().string();
				}
			}
		}
	}
};

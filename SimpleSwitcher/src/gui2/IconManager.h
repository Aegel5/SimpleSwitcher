#pragma once
#include "utils/Images.h"


class Test {
	string flagFold;
	using Bundle = std::vector<Images::ImageIcon>;
	std::map<string, Bundle> icons;

	const Bundle& GetBundle(UStr contry_id, bool is_gray = false) {

		namespace fs = std::filesystem;

		GETCONF;

		const auto& folder_name = cfg->flagsSet2;
		string key = std::format("{}$&{}{}", contry_id, folder_name, is_gray ? "$%^&!" : "");

		auto it = icons.find(key);
		if (it != icons.end()) {
			return it->second;
		}

		std::vector<Images::Image> bndl;

		string dir = flagFold + L"\\" + folder_name;
		if (fs::is_directory(dir)) {
			for (const auto& entry : fs::directory_iterator(dir)) {
				if (entry.is_regular_file()) {
					fs::path p{ entry.path() };
					auto name = p.filename().string();
					StrUtils::ToUpper(name);
					if (name.starts_with(contry_id)) {
						auto cur = Images::LoadImageFromFile(p.string().c_str());
						if (cur->IsOk()) {
							bndl.push_back(cur);
						}
					}
				}
			}
		}

		if (is_gray) {
			for (auto& it : bndl) {
				Images::SetBrightness(it, 0.8f);
			}
		}

		Bundle res;

		for (auto& it : bndl) {
			res.push_back(Images::ImageToIconConsume(it));
		}

		return icons.emplace(key, res).first->second;
	}
public:
	Test() {
		flagFold = StrUtils::Convert(Utils::GetPath_folder_noLower() + L"\\Flags");
	}
	static Test& Inst() {
		static Test inst;
		return inst;
	}

	Images::ImageIcon GetIcon(UStr contry_id, int size = 16, bool is_gray = false) {

		const auto& bndl = GetBundle(contry_id, is_gray);
		for (const auto it : bndl) {
			return it;
		}

		return std::make_shared<Images::ImageIcon::element_type>();
	}

	void ClearCache() {
		icons.clear();
	}

	std::generator<string> ScanFlags() {
		namespace fs = std::filesystem;
		if (fs::is_directory(flagFold)) {
			for (const auto& entry : fs::directory_iterator(flagFold)) {
				if (entry.is_directory()) {
					fs::path p{ entry.path() };
					co_yield p.filename().string();
				}
			}
		}
	}
};

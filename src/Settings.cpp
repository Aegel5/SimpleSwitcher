#include "sw-base.h"
#include "Settings.h"
#include "json.hpp"

using json = nlohmann::json;

namespace nlohmann {

	void to_json(json& j, const HKL& p) {
		char sBuf[30] = { 0 };
		sprintf(sBuf, "0x%I64X", (uint64_t)p);
		j = sBuf;
	}

	void from_json(const json& j, HKL& p) {
		if (j.is_string()) {
			StrUtils::ToInt(j.get_ref<const std::string&>(), p);
		}
	}

	template <>
	struct adl_serializer<std::wstring> {
		static void to_json(json& j, const std::wstring& str) {
			j = StrUtils::Convert(str);
		}

		static void from_json(const json& j, std::wstring& str) {
			str = StrUtils::Convert(j.get_ref<const std::string&>());
		}
	};


	void to_json(json& j, const CHotKey& p) {
		j = p.ToString();
	}


	void from_json(const json& j, CHotKey& p) {
		if (j.is_string()) {
			p = CHotKey::FromString(StrUtils::Convert(j.get_ref<const std::string&>()));
		}
	}
	void to_json(json& j, const CHotKeyList& p) {
		if (p.keys.size() == 1) {
			j = p.key();
		}
		else {
			j = p.keys;
		}
	}

	void from_json(const json& j, CHotKeyList& p) {
		if (j.is_array()) {
			p.keys = j;
			if (p.keys.empty()) {
				p.keys.resize(1);
			}
		}
		else if (j.is_string()) {
			p.key() = j;
		}
	}


	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		LayoutInfo,
		layout,
		enabled,
		win_hotkey,
		hotkey
	)

		void to_json(json& j, const LayoutInfoList& p) {
		j = p.info;
	}

	void from_json(const json& j, LayoutInfoList& p) {
		j.get_to(p.info);
	}




	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		RunProgramInfo,
		path,
		args,
		elevated,
		hotkey
	)


		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
			ProgramConfig,
			isMonitorAdmin,
			force_DbgMode,
			fClipboardClearFormat,
			disableAccessebility,
			flagsSet,
			disableInPrograms,
			logLevel,
			SkipAllInjectKeys,
			SkipLowLevelInjectKeys,
			AlternativeLayoutChange,
			config_version,
			layouts_info,
			fixRAlt,
			fixRAlt_lay_,
			run_programs,
			separate_ext_mode,
			quick_press_ms,
			win_hotkey_cycle_lang,
			theme,
			background,
			gui_lang,
			ShowLangsInTrayMenu
		)
}

namespace cfg_details {

	TStatus LoadConfig(ProgramConfig& cfg) {
		try {

			auto p = PathUtils::GetPath_folder_noLower2() / "SimpleSwitcher.json";
			if (!std::filesystem::is_regular_file(p)) {
				// конфиг файла еще нет - считаем что загружены конфигом по-умолчанию.
				RETURN_SUCCESS;
			}

			std::ifstream ifs(p, std::ios::binary);

			json data = json::parse(ifs, nullptr, true, true);
			data.get_to(cfg);

			const auto& arr = data["hotkeys"];

			if (arr.is_object()) {
				for (auto& elem : cfg.hotkeysList) {
					auto key = HotKeyTypeName(elem.hkId);
					if (key.empty()) continue;
					auto it = arr.find(key);
					if (it != arr.end()) {
						elem.keys = it.value();
					}
				}
			}

			cfg.NormalizePaths();

			if (cfg.force_DbgMode) {
				SetLogLevel_info(cfg.logLevel);
			}


		}
		catch (std::exception& e) {
			LOG_ANY("error load={}", e.what());
			return SW_ERR_JSON;
		}


		RETURN_SUCCESS;
	}


	TStatus Save_conf(const ProgramConfig& gui) {
		try {
			auto path = ProgramConfig::GetPath_Conf();

			json data = gui;
			json hk_json;

			for (auto& elem : gui.hotkeysList) {
				auto hk = elem.hkId;
				auto key = HotKeyTypeName(hk);
				if (key.empty()) continue;
				hk_json[key] = elem.keys;
			}

			data["hotkeys"] = hk_json;

			std::ofstream outp(path, std::ios::binary);
			outp << std::setw(2) << data << std::endl;

		}
		catch (std::exception& e) {
			return SW_ERR_JSON;
		}

		RETURN_SUCCESS;
	}
}

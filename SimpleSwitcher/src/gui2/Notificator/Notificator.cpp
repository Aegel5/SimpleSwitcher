#include "Notificator.h"
#include "json.hpp"

using json = nlohmann::json;

namespace nlohmann {

	void to_json(json& j, const Notific::DateTime& p) {
		j = Notific::Serialize(p);
	}

	void from_json(const json& j, Notific::DateTime& p) {
		if (j.is_string()) {
			p = Notific::Desiarelize(j.get_ref<const string&>());
		}
	}

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		Notific::Entry,
		name,
		period,
		point,
		nextActivate,
		enabled
	)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		Notific::Settings,
		list
	)
}


namespace Notific {


	void Notificator::Save() {
		json data = settings;
		auto path = PathUtils::GetPath_folder_noLower2() / "Notificator.json";
		std::ofstream outp(path);
		outp << std::setw(1) << data << std::endl;
	}
	void Notificator::Load() {
		auto path = PathUtils::GetPath_folder_noLower2() / "Notificator.json";
		std::ifstream ifs(path);
		try {
			json data = json::parse(ifs, nullptr, true, true);
			data.get_to(settings);
		}
		catch (std::exception& e) {
			IFS_LOG(SW_ERR_JSON);
		}
		for (auto& it : entries) {
			it.AdjastActiveString();
		}
	}
}

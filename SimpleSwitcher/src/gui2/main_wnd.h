#pragma once



#include "gui_utils.h"

#include "SwAutostart.h"
#include "SetHotKeyCombo.h"
#include "Notificator/Notificator.h"

class MainWindow : public ImGuiUtils::WindowHelper {
	string config_path;
	std::string title;
	bool check_add_to_auto = false;
	bool show_demo_window = false;
	bool ShowStyleEditor = false;
	UStr show_message = 0;
	std::vector<SetHotKeyCombo> hotbox;
	std::vector<SetHotKeyCombo> layout_hotkeys;
	std::vector<SetHotKeyCombo> layout_win_hotkeys;
	COM::CAutoCOMInitialize autoCom;
	std::vector<string> flagsSets;
	std::vector<string> backgrounds;
	std::vector<std::pair<string, HKL>> menu_lays;
	ImVec2 startsize{ 544.0, 544.0 / 1.12 };
	Images::ShaderResource background = MAKE_SHARED(background);
public:
	bool IsVisible() { return show_wnd; }
private:

	void update_backg() {
		backgrounds.clear();
		namespace fs = std::filesystem;
		auto dir = PathUtils::GetPath_folder_noLower2() / "Background";
		if (fs::is_directory(dir)) {
			for (const auto& entry : fs::directory_iterator(dir)) {
				if (entry.is_regular_file()) {
					backgrounds.push_back(entry.path().filename().string());
				}
			}
		}
		else {
		}
	}
	void apply_background() {
		namespace fs = std::filesystem;
		auto name = conf_get_unsafe()->background;
		auto p = PathUtils::GetPath_folder_noLower2() / "Background" / name;
		auto img = Images::LoadImageFromFile(p.string().c_str());
		if (!img->IsOk()) { background->clear();  return; }
		auto parts = StrUtils::Split(name, '@');
		int alp = -1;
		if (parts.size() > 1 && StrUtils::ToInt(parts[0], alp) && alp >= 0 && alp <= 100) {
			Images::SetAlphaFactor(img, alp * 0.01f);
		}
		background = Images::ImageToShaderConsume(img);
	}
	void update_flags() { flagsSets = { std::from_range, IconMgr::Inst().ScanFlags() }; IconMgr::Inst().ClearCache(); }
	void ShowMessage(UStr msg) {
		show_message = msg;
		ImGui::OpenPopup(LOC("Message"));
	}
	void ShowMessageAdmin() { ShowMessage(LOC("Need admin right")); }
	void DrawMessage() {
		ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(LOC("Message"), 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
			ImGui::Text(show_message);
			//ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(ImGui::CalcTextSize("OK").x * 4, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}
	void SyncLays() {
		SyncLayouts();
		GETCONF;
		std::vector<CHotKey> def_list = {
			CHotKey(VK_LCONTROL).SetKeyup(),
			CHotKey(VK_RCONTROL).SetKeyup(),
			CHotKey(VK_LSHIFT).SetDouble(),
			CHotKey(VK_RSHIFT).SetDouble()
		};
		layout_hotkeys.clear();
		layout_win_hotkeys.clear();
		for (int i = -1; const auto & it : cfg->layouts_info.info) {
			i++;
			layout_hotkeys.emplace_back(StrUtils::Convert(Utils::GetNameForHKL(it.layout)), it.hotkey.key(), def_list,
				[i](auto key) {
					SaveConfigWith([&](auto conf) {
						if (i >= conf->layouts_info.info.size()) return;
						auto& rec = conf->layouts_info.info[i];
						rec.hotkey.key() = key;
						});
				}
			);

		}
	}
	void DrawFrameActual();
public:
	MainWindow(bool show) {
		show_wnd = show;
		update_backg();
		apply_background();
		IFS_LOG(autoCom.Init());
		auto scale = ImGui::GetPlatformIO().Monitors[0].DpiScale * 1.2f;
		//auto scale = WinUtils::GetDpiMainMonScale2() * 1.2f;
		InitImGui(scale);
		title = std::format(
			"SimpleSwitcher {}{}{}###main_wnd", SW_VERSION,
			Utils::IsSelfElevated() ? " Administrator" : "",
			Utils::IsDebug() ? " DEBUG" : "");
		GETCONF;
		for (int i = -1; const auto & it : cfg->hotkeysList) {
			i++;
			hotbox.emplace_back(GetGuiTextForHk(it.hkId), it.keys.key(), it.def_list,
				[i](auto k) {
					SaveConfigWith([i, k](auto p) {p->hotkeysList[i].keys.key() = k; });
				});
		}
		check_add_to_auto = autostart_get();
		SyncLays();
		config_path = StrUtils::Convert(std::format(L"file://{}", ProgramConfig::GetPath_Conf().wstring()));
		update_flags();
		startsize.x *= scale;
		startsize.y *= scale;
	}

	void DrawFrame() {

		if (!show_wnd) {
			return;
		}

		DrawFrameActual();

		if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}
		//if (ShowStyleEditor) {
		//	ImGui::Begin(LOC("Style editor"), &ShowStyleEditor);
		//	ImGui::ShowStyleEditor();
		//	ImGui::End();
		//}
	}
};

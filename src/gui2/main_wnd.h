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
	//bool ShowStyleEditor = false;
	UStr show_message = 0;
	int need_show_message = 0;
	std::vector<SetHotKeyCombo> hotbox;
	std::vector<SetHotKeyCombo> layout_hotkeys;
	std::vector<SetHotKeyCombo> layout_win_hotkeys;
	COM::CAutoCOMInitialize autoCom;
	std::vector<string> flagsSets;
	std::vector<string> backgrounds;
	std::vector<std::pair<string, HKL>> menu_lays;
	ImVec2 startsize{ 544.0, 544.0 / 1.12 };
	Images::ShaderResource background = MAKE_SHARED(background);
	bool bg_fill = false;
	BufScanMap remap;
	bool remap_open = false;
public:
	bool IsVisible() { return show_wnd; }
private:

	void update_backg() {
		backgrounds.clear();
		namespace fs = std::filesystem;
		auto dir = PathUtils::GetPath_folder_noLower2() / "UI_Skins";
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
		auto name = conf_gui()->ui_skin;
		auto p = PathUtils::GetPath_folder_noLower2() / "UI_Skins" / name;
		auto img = Images::LoadImageFromFile(p.string().c_str());
		if (!img->IsOk()) { background->clear();  return; }
		int alp = 70;
		std::smatch match;
		if (std::regex_search(name, match, std::regex(R"(@ALPHA(\d+))"))) {
			int alp_ = std::stoi(match[1].str());
			if (alp_ >= 0 && alp_ <= 100) {
				alp = alp_;
			}
		}
		Images::SetAlphaFactor(img, alp * 0.01f);
		background = Images::ImageToShaderConsume(img);
		bg_fill = name.contains("@STRETCH");
	}
	void update_flags() { 
		flagsSets = { std::from_range, IconMgr::Inst().ScanFlags() }; 
		IconMgr::Inst().ClearCache(); 
	}
	void ShowMessage(UStr msg) {
		show_message = msg;
		need_show_message = 1;
	}
	void ShowMessageAdmin() { ShowMessage(LOC("Need admin rights")); }
	void ShowMessageConfError() { ShowMessage(LOC("Error loading config file")); }
	void DrawMessage() {
		if (!need_show_message) return;
		if (need_show_message == 1) {
			auto title = LOC("Message");
			if(!ImGui::IsPopupOpen(title)) ImGui::OpenPopup(title);
			need_show_message = 2;
		}
		ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(LOC("Message"), 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
			ImGui::Text(show_message);
			//ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(ImGui::CalcTextSize("OK").x * 4, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		else {
			need_show_message = 0;
		}
	}
	void SyncLays() {

		SyncLayouts();

		std::vector<CHotKey> def_list = {
			CHotKey(VK_LCONTROL).SetKeyup(),
			CHotKey(VK_RCONTROL).SetKeyup(),
			CHotKey(VK_LSHIFT).SetDouble(),
			CHotKey(VK_RSHIFT).SetDouble()
		};

		layout_hotkeys.clear();
		layout_win_hotkeys.clear();
		for (auto & it : conf_gui()->layouts_info.info) {
			layout_hotkeys.emplace_back(
				StrUtils::Convert(Utils::GetNameForHKL(it.layout)),
				def_list,
				&it.hotkey);
		}
	}
	void DrawFrameActual();
public:
	MainWindow(bool show, bool conf_err) {
		show_wnd = show;
		update_backg();
		apply_background();
		IFS_LOG(autoCom.Init());
		auto scale = ImGui::GetPlatformIO().Monitors[0].DpiScale * 1.2f;
		title = std::format(
			"SimpleSwitcher {}{}{}###main_wnd", GET_SW_VERSION(),
			Utils::IsSelfElevated() ? " Administrator" : "",
			Utils::IsDebug() ? " DEBUG" : "");
		check_add_to_auto = autostart_get();
		config_path = StrUtils::Convert(std::format(L"file://{}", ProgramConfig::GetPath_Conf().wstring()));
		update_flags();
		startsize.x *= scale;
		startsize.y *= scale;
		Reinit(conf_err);
	}
	void ReinitHk() {
		hotbox.clear();
		for (auto& it : conf_gui()->hotkeysList) {
			hotbox.emplace_back(GetGuiTextForHk(it.hkId), GetHk_Defaults(it.hkId), &it.keys);
		}
	}
	void Reinit(bool conf_err) {
		if (conf_err) {
			ShowMessageConfError();
			return;
		}
		ReinitHk();
		SyncLays();
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

	void DrawSkin() {

		if (!background->IsOk())
			return;

		auto size = ImGui::GetWindowSize();
		auto h = size.y;
		auto w = background->img.width * size.y / background->img.height;
		auto x = size.x - w;
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 min = window_pos;
		min.x += x;
		ImVec2 max = min;
		max.x += w;
		max.y += h;
		if (bg_fill) {
			min = max = window_pos;
			max.x += size.x;
			max.y += size.y;
		}
		ImGui::GetForegroundDrawList()->AddImage((ImTextureID)(intptr_t)background->pTexture, min, max);

	}
};

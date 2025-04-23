#pragma once


#include "utils/win_utils.h"

#include "misc/fonts/Play-Regular.cpp"

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

inline void SetStyle() {
	GETCONF;
	const auto& theme = cfg->theme;
	auto& style = ImGui::GetStyle();

	style.FrameRounding = 3.0f;
	style.TabRounding = 5.0f;

	if (cfg->theme == "Light") { ImGui::StyleColorsLight(); }
	if (cfg->theme == "Dark") { ImGui::StyleColorsDark(); }
	else if (cfg->theme == "Classic") { ImGui::StyleColorsClassic(); }
	else if (cfg->theme == "Ocean") { 
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.74f, 0.86f, 0.88f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.22f, 0.34f, 0.94f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.28f, 0.43f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.24f, 0.37f, 0.48f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_InputTextCursor] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_TreeLines] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	}
	else if (cfg->theme == "Dark Relax") {

		// Corners
		style.FrameRounding = 6.0f;
		style.TabRounding = 6.0f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.93f, 0.94f, 0.95f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.93f, 0.94f, 0.95f, 0.58f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.17f, 0.24f, 0.31f, 0.95f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.22f, 0.31f, 0.41f, 0.58f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.18f, 0.24f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.20f, 0.37f, 0.46f, 0.98f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.17f, 0.24f, 0.31f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.23f, 0.29f, 0.36f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.37f, 0.45f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.22f, 0.31f, 0.41f, 0.75f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.22f, 0.31f, 0.41f, 0.47f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.31f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.16f, 0.50f, 0.73f, 0.21f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.16f, 0.50f, 0.73f, 0.80f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.16f, 0.50f, 0.73f, 0.50f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.16f, 0.50f, 0.73f, 0.50f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.86f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.16f, 0.50f, 0.73f, 0.76f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.86f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.16f, 0.50f, 0.73f, 0.15f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16f, 0.50f, 0.73f, 0.78f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_InputTextCursor] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.36f, 0.51f, 0.92f);
		colors[ImGuiCol_Tab] = ImVec4(0.23f, 0.31f, 0.41f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.21f, 0.31f, 0.44f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.93f, 0.94f, 0.95f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.93f, 0.94f, 0.95f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.16f, 0.50f, 0.73f, 0.43f);
		colors[ImGuiCol_TreeLines] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);






	}
	else  { 
		ImGui::StyleColorsLight(); }
}

inline void InitImGui() {

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	auto scale = WinUtils::GetDpiMainMonScale2();
	style.ScaleAllSizes(scale);
	static ImVector<ImWchar> ranges;
	{
		ImFontGlyphRangesBuilder builder;
		builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic()); // Add one of the default ranges
		static const ImWchar ranges_additional[] =
		{
			//0x1, 0xFFFF,
			0x0590, 0x05FF, 0xFB1D, 0xFB4F, // hebrew
			0,
		};
		builder.AddRanges(ranges_additional);
		builder.BuildRanges(&ranges);


		float size = std::floorf(16.0f * scale);
		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 20.0f, 0, ranges.Data);
		io.Fonts->AddFontFromMemoryCompressedTTF(DefaultFont_compressed_data, std::ssize(DefaultFont_compressed_data), size, 0, ranges.Data);
	}


	style.FrameRounding = 3;
	style.FrameBorderSize = 1;

	SetStyle();
}







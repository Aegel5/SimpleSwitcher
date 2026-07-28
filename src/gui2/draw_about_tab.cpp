#include "main_wnd.h"

void MainWindow::Draw_about_tab() {

	with_TabItem(LOC("About")) {

		ImGui::Text("SimpleSwitcher %s", GET_SW_VERSION());

		static string sha = GIT_COMMIT_HASH;
		if (!sha.empty()) {
			static string sha_link = "https://github.com/Aegel5/SimpleSwitcher/commit/" + sha;
			if (sha.size() > 7) sha.resize(7);
			ImGui::TextUnformatted("Commit SHA:");
			ImGui::SameLine(); ImGui::TextLinkOpenURL(sha.c_str(), sha_link.c_str());
		}

		ImGui::TextUnformatted(LOC("More info:"));

		ImGui::SameLine(); ImGui::TextLinkOpenURL("https://github.com/Aegel5/SimpleSwitcher");

		if (ImGui::Button(LOC("❤ Support ❤"))) {
			ImGui::GetPlatformIO().Platform_OpenInShellFn(GImGui, "https://github.com/Aegel5/SimpleSwitcher?tab=readme-ov-file#%EF%B8%8F-donate");
		}

		ImGui::TextUnformatted(LOC("The program develops only thanks to your support! 🤝"));

#ifndef IMGUI_DISABLE_DEMO_WINDOWS
		if (ImGui::Button("Show demo"))
			show_demo_window = true;
#endif

#ifndef IMGUI_DISABLE_DEBUG_TOOLS
		if (ImGui::Button("Show metrics")) {
			show_metrics = true;
		}
#endif

		//ImGui::TextLinkOpenURL("Telegram", "https://t.me/simple_switcher");


	}
}

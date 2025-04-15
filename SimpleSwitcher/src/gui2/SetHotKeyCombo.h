
class SetHotKeyCombo {
	UStr title;
	bool is_up = false;
	bool left_right = false;
	bool is_double = false;
public:
	SetHotKeyCombo(UStr title, const CHotKey& key, auto&& apply) : title(title) {
	}
	void Draw() {

		ImGui::SetNextItemWidth(180);

		if (ImGui::BeginCombo(title, 0)) {

			if (ImGui::IsWindowAppearing()) {
				ImGui::SetKeyboardFocusHere();
			}

			ImGui::Selectable("aa", true);
			ImGui::Selectable("bb", false);
			ImGui::Text(LOC("Press any key..."));
			ImGui::Selectable("cc", false);
			ImGui::Checkbox(LOC("Distinguish between left and right modifiers"), &left_right);
			ImGui::Checkbox(LOC("Triggers on KEYUP"), &is_up);
			ImGui::Checkbox(LOC("Triggers on double press"), &is_double);


			ImGui::EndCombo();
		}
		else {

		}
	}
};

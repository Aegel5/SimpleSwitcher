#pragma once

namespace ImGuiUtils {

	inline void ToCenter(bool always = false) {
		auto rec = ImGui::GetPlatformIO().Monitors[0].MainSize;
		rec.x *= 0.5f;
		rec.y *= 0.5f;
		ImGui::SetNextWindowPos(rec, always ? ImGuiCond_Appearing: ImGuiCond_FirstUseEver, { 0.5f,0.5f });
	}

	class WindowHelper {
	protected:
		bool to_top = true;
		bool show_wnd = false;
		void process_helper() {
			if (to_top) {
				auto hwnd = (HWND)ImGui::GetWindowViewport()->PlatformHandle;
				if (hwnd) {
					to_top = false;
					SetForegroundWindow(hwnd);
				}
			}
		}
	public:
		//void bring_to_top() {
		//	to_top = true;
		//}
		void ShowHide() {
			if (show_wnd) show_wnd = false;
			else {
				show_wnd = true; to_top = true;
			}
		}
	};

	inline bool PlusButton() {
		return (ImGui::Button("+", { ImGui::GetFrameHeight(),0 }));
	}

	// todo 1: to return bool instead apply 2: invoke on client side 3: new version int -> string.
	inline void Combo(UStr name, string& data, auto&& choices, auto&& apply, ImGuiComboFlags flags = 0) {
		if (ImGui::BeginCombo(name, data.c_str(), flags)) {
			if constexpr (std::is_invocable_v<TYPE_OF(choices)>) {
				for (UStr s : choices()) {
					if (ImGui::Selectable(s, data == s)) {
						data = s;
						apply();
					}
				}
			}
			else {
				for (UStr s : choices) {
					if (ImGui::Selectable(s, data == s)) {
						data = s;
						apply();
					}
				}
			}

			ImGui::EndCombo();
		}
	}

	inline void SetCursorToRightForButton(UStr text) {
		float w = ImGui::CalcTextSize(text).x + ImGui::GetStyle().FramePadding.x * 2.f + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - w);
		//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()); // todo use child_wnd
	}


	//inline float ButtonRightFrom(UStr text, int& new_offset, int w =0, float offset = 0) { // temporary solution
	//	auto cur = ImGui::GetCursorPos();
	//	w = ImGui::CalcTextSize(text).x + ImGui::GetStyle().FramePadding.x * 2.f + ImGui::GetStyle().ItemSpacing.x;
	//	auto r = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - w - offset;
	//	ImGui::SetCursorPosX(r);
	//	auto res = false;
	//	if (ImGui::Button(text)) { res = true; }
	//	ImGui::SetCursorPos(cur);
	//	return res;
	//}

	//inline void WindowCustomHeader(const auto* p, auto* b, int f, auto&& func) {

	//	// 1. Создаем окно с пустым заголовком (через ##)
	//	ImGui::Begin(p, b, f);

	//	ImGuiWindow* window = ImGui::GetCurrentWindow();
	//	ImGuiStyle& style = ImGui::GetStyle();

	//	// 2. Получаем точные границы заголовка
	//	ImRect titleBarRect = window->TitleBarRect();

	//	// 3. Вычисляем позицию "1 к 1"
	//	// Стандартный отступ текста слева = WindowPadding.x
	//	// Центрирование по вертикали = (ВысотаБара - ВысотаТекста) / 2
	//	float textHeight = ImGui::GetTextLineHeight();
	//	float verticalOffset = (titleBarRect.GetHeight() - textHeight) * 0.5f;

	//	// Устанавливаем курсор в экранные координаты заголовка
	//	ImGui::SetCursorScreenPos(ImVec2(
	//		titleBarRect.Min.x + style.WindowPadding.x,
	//		titleBarRect.Min.y + verticalOffset
	//	));

	//	// 4. РАЗРЕШАЕМ рисование в области заголовка
	//	// По умолчанию ImGui запрещает рисовать выше основной рабочей области окна.
	//	ImGui::PushClipRect(titleBarRect.Min, titleBarRect.Max, false);



	//	// 5. СТАНДАРТНЫЙ ВЫВОД (теперь он попадет в заголовок)
	//	func();

	//	// Можно добавить и другие стандартные виджеты рядом:
	//	// ImGui::SameLine(); 
	//	// if (ImGui::SmallButton("OK")) { ... }

	//	ImGui::PopClipRect();

	//	// 6. ВОЗВРАЩАЕМ курсор в начало контентной области окна
	//	// Чтобы последующий текст не накладывался на заголовок
	//	ImGui::SetCursorScreenPos(window->DC.CursorStartPos);

	//	// Обычное содержимое окна
	//}
}

inline void LoadFonts() {

	ImGuiIO& io = ImGui::GetIO();

	std::wstring font_path(MAX_PATH, L'\0');
	{
		UINT size = GetWindowsDirectoryW(font_path.data(), MAX_PATH);
		if (size > 0 && size < MAX_PATH) {
			font_path.resize(size); // Обрезаем до реальной длины
			font_path += L"\\Fonts\\";
		}
	}
	ImFont* font = nullptr;

	if (!font) {
		static MappedFile font_mapped{ (font_path + L"segoeui.ttf").c_str() };
		if (font_mapped.is_valid()) {
			ImFontConfig cfg;
			cfg.FontDataOwnedByAtlas = false;
			font = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(font_mapped.data()), font_mapped.size(), 24, &cfg);
		}
	}

	if (!font) {
		static MappedFile font_mapped{ (font_path + L"tahoma.ttf").c_str() };
		if (font_mapped.is_valid()) {
			ImFontConfig cfg;
			cfg.FontDataOwnedByAtlas = false;
			font = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(font_mapped.data()), font_mapped.size(), 20, &cfg);
		}
	}

	// fallback
	if (!font) {
		font = io.Fonts->AddFontDefault();
	}

	// эмодзи
	{
		static MappedFile font_mapped{ (font_path + L"seguiemj.ttf").c_str() };
		if (font_mapped.is_valid()) {
			ImFontConfig cfg;
			cfg.FontDataOwnedByAtlas = false;
			cfg.MergeMode = true;
			cfg.FontLoaderFlags |= ImGuiFreeTypeLoaderFlags_LoadColor;
			font = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(font_mapped.data()), font_mapped.size(), 18, &cfg);
		}
	}

}

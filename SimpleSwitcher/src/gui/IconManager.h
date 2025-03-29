#pragma once

class IconManager {

    std::map<wxString, wxBitmapBundle> flags_map;
    wxBitmapBundle icon;
    wxBitmapBundle gray_icon;
    bool gray_init = false;
    wxVector<wxBitmap> bitmaps;
    std::vector<std::wstring> externFlags;
    std::wstring flagFold;
    wxIconBundle app_icons{ L"appicon", 0 };
public: 
    const wxIconBundle& GetAppIcons() { return app_icons; }
    IconManager() {
        

        for (int i = 0; i < app_icons.GetIconCount(); i++) {
            auto cur = app_icons.GetIconByIndex(i);
            if (cur.GetWidth() > 32) continue;
            wxBitmap b;
            b.CopyFromIcon(cur);
            bitmaps.push_back(std::move(b));
        }
        icon = wxBitmapBundle::FromBitmaps(bitmaps);
        flagFold = Utils::GetPath_folder_noLower() + L"\\Flags";
    }
private:
      void ToGray(wxBitmap& bt) {
          wxImage img = bt.ConvertToImage();
          img.ChangeBrightness(-0.2);
          bt = img;
      }

      wxBitmapBundle GetBundl(const wxString& name, bool isGray) {

          auto folder_name = conf_get_unsafe()->flagsSet;

          auto name16 = name + L"16";
          auto name32 = name + L"32";
          auto key = name16;
          bool is_original = true;

          if (folder_name != SettingsGui::showOriginalFlags) {
              is_original = false;
              key += L"$$";
              key += folder_name;
          }

          if (isGray) {
              key += "#g$";
          }

          static bool inited = false;

          if (!inited) {
              ::wxInitAllImageHandlers();
              inited = true;
          }

          auto it = flags_map.find(key);
          if (it != flags_map.end())
              return it->second;

          wxVector<wxBitmap> bitmaps;
          wxBitmapBundle bundleResult;

          if (!is_original) {
              is_original = true;
              // попытаемся найти флаги из папки
              namespace fs = std::filesystem;
              std::wstring dir = flagFold + L"\\" + folder_name;
              for (const auto& entry : fs::directory_iterator(dir)) {
                  if (entry.is_regular_file()) {
                      fs::path p{ entry.path() };
                      wxString fname = p.filename().wstring();
                      fname.MakeUpper();
                      if (fname.starts_with(name)) {
                          wxString ext = p.extension().wstring();
                          ext.MakeLower();
                          auto type = wxBITMAP_TYPE_INVALID;

                          if (ext == L".ico") type = wxBITMAP_TYPE_ICO;
                          if (ext == L".png") type = wxBITMAP_TYPE_PNG;
                          else if (ext == L".bmp") type = wxBITMAP_TYPE_BMP;
                          else if (ext == L".jpg") type = wxBITMAP_TYPE_JPEG;

                          if (ext == L".svg") {
                              bundleResult = wxBitmapBundle::FromSVGFile(p.wstring(), { 32,32 });
                          }
                          else if (type != wxBITMAP_TYPE_INVALID) { // не понятно что пропускаем.
                              wxBitmap bitmap(p.wstring(), type);
                              if (bitmap.IsOk()) {
                                  is_original = false;
                                  bitmaps.push_back(bitmap);
                              }
                          }
                      }
                  }
              }
          }

          if (bundleResult.IsOk()) { // svg
              is_original = false;
              if (isGray) { // для gray пока сделаем хак, но по хорошему нужен отдельный кеш на gray, но для этого нужно знать destination size а для этого нужно патчить widgets
                  bitmaps.clear();
                  bitmaps.push_back(bundleResult.GetBitmap({ 16,16 }));
                  bitmaps.push_back(bundleResult.GetBitmap({ 32,32 }));
                  bundleResult.Clear();
              }
          }

          if (!bundleResult.IsOk()) {
              if (is_original && FindResource(0, name16.wc_str(), RT_RCDATA) != nullptr && FindResource(0, name32.wc_str(), RT_RCDATA) != nullptr) {
                  bitmaps.clear();
                  bitmaps.push_back(wxBitmap(name16, wxBITMAP_TYPE_PNG_RESOURCE));
                  bitmaps.push_back(wxBitmap(name32, wxBITMAP_TYPE_PNG_RESOURCE));
              }
              if (isGray) {
                  for (auto& it : bitmaps) { ToGray(it); }
              }
              bundleResult = wxBitmapBundle::FromBitmaps(bitmaps);
          }

          flags_map.emplace(key, bundleResult);

          return flags_map[key];
      }

public:
    struct IconInfo {
        wxSize sizeForPanel;
        wxBitmapBundle bundle;
    };
    IconInfo Get_Standart(bool is_gray = false) {
        IconInfo info;
        if (is_gray) {
            if (!gray_init) {
                gray_init = true;
                auto btms = bitmaps;
                for (auto& it : btms) { ToGray(it); }
                gray_icon = wxBitmapBundle::FromBitmaps(btms);
            }
        }
        info.bundle = is_gray ? gray_icon : icon;
        return info;
    }
    IconInfo Get(HKL lay, bool is_gray = false) {

        IconInfo info;

        wxString wname;

        {
            WORD langid = LOWORD(lay);

            TCHAR buf[512];
            buf[0] = 0;

            int flag = LOCALE_SNAME;
            int len = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, std::ssize(buf));
            IFW_LOG(len != 0);

            auto len_str = wcslen(buf);
            if (len_str < 2)
                return info;
            TStr name = buf + len_str - 2;
            wname = name;
            wname.MakeUpper();
            LOG_ANY(L"mainguid new layout: {}, name={}", (void*)lay, name);
        }


        info.bundle = GetBundl(wname, is_gray);

        if (!info.bundle.IsOk()) {
            LOG_ANY(L"ERR. can't find flag for ");
            info = Get_Standart(is_gray);
        }

        return info;
    }
    const std::vector<std::wstring>& ScanFlags() {
        externFlags.clear();
        flags_map.clear(); // почистим кеш, чтобы снова попытаться найти флаг в папке.
        namespace fs = std::filesystem;
        if (!fs::is_directory(flagFold)) 
            return externFlags;
        for (const auto& entry : fs::directory_iterator(flagFold)) {
            if (entry.is_directory()) {
                fs::path p{ entry.path() };
                externFlags.emplace_back(p.filename());
            }
        }
        return externFlags;
    }
};
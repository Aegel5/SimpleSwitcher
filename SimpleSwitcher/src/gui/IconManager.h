#pragma once

class IconManager {
    std::map<wxString, wxBitmapBundle> flags_map;
    wxBitmapBundle icon;
    wxBitmapBundle gray_icon;
    bool gray_init = false;
    wxVector<wxBitmap> bitmaps;
public: 
    IconManager() {
        wxIconBundle all_icons(L"appicon", 0);

        for (int i = 0; i < all_icons.GetIconCount(); i++) {
            auto cur = all_icons.GetIconByIndex(i);
            if (cur.GetWidth() > 32) continue;
            wxBitmap b;
            b.CopyFromIcon(cur);
            bitmaps.push_back(std::move(b));
        }
        icon = wxBitmapBundle::FromBitmaps(bitmaps);
    }
private:
      wxBitmap ToGray(const wxBitmap& bt) {
          wxImage img = bt.ConvertToImage();
          img.ChangeBrightness(-0.2);
          return img;
      }
      wxBitmap ToDebug(const wxBitmap& bt) {
          wxImage img = bt.ConvertToImage();
          img.ChangeHSV(0.1,0,0);
          return img;
      }

      wxBitmapBundle GetBundl(const wxString& name, bool isGray) {
          auto name16 = name + L"16";
          auto name32 = name + L"32";
          auto key = name16;
          if (isGray) {
              key += "g";
          }

          static bool inited = false;

          if (!inited) {
              ::wxInitAllImageHandlers();
              inited = true;
          }

          auto it = flags_map.find(key);
          if (it != flags_map.end()) 
              return it->second;

          if (FindResource(0, name16.wc_str(), RT_RCDATA) == nullptr || FindResource(0, name32.wc_str(), RT_RCDATA) == nullptr) {
          }
          else {
              wxVector<wxBitmap> bitmaps;
              bitmaps.push_back(wxBitmap(name16, wxBITMAP_TYPE_PNG_RESOURCE));
              bitmaps.push_back(wxBitmap(name32, wxBITMAP_TYPE_PNG_RESOURCE));
              if (Utils::IsDebug()) {
                  for (auto& it : bitmaps) {
                      it = ToDebug(it);
                  }
              }
              if (isGray) {
                  for (auto& it : bitmaps) { it = ToGray(it); }
              }
              flags_map.emplace(key, wxBitmapBundle::FromBitmaps(bitmaps));
          }

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
                for (auto& it : btms) { it = ToGray(it); }
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
            LOG_ANY(L"mainguid new layout: {}, name={}", (void*)lay, name);
        }


        info.bundle = GetBundl(wname, is_gray);

        if (!info.bundle.IsOk()) {
            LOG_ANY(L"ERR. can't find flag for ");
            info = Get_Standart(is_gray);
        }

        return info;
    }
};
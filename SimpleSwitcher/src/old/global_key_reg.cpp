//#include "Settings.h"
//
//namespace { 
//    
//    struct RegData {
//
//        bool registered = false;
//        int id = 0;
//
//        void reg(int id, CHotKey key) {
//            unreg();
//            if(key.IsEmpty()) return;
//
//            if (key.IsKnownMods(key.ValueKey())) {
//                LOG_WARN(L"key {} NOT registered because it MOD", key.ToString().c_str());
//                return;
//            }
//
//            UINT mods = MOD_NOREPEAT;
//            UINT vk = key.ValueKey();
//            for (TKeyCode* k = key.ModsBegin(); k != key.ModsEnd(); ++k)
//            {
//                auto cur = *k;
//                if (cur == VK_SHIFT || cur == VK_LSHIFT || cur == VK_RSHIFT) {
//                    mods |= MOD_SHIFT;
//                }
//                else if (cur == VK_CONTROL || cur == VK_LCONTROL || cur == VK_RCONTROL) {
//                    mods |= MOD_CONTROL;
//                }
//                else if (cur == VK_MENU || cur == VK_LMENU || cur == VK_RMENU) {
//                    mods |= MOD_ALT;
//                }
//                else if (cur == VK_LWIN || cur == VK_RWIN ) {
//                    mods |= MOD_WIN;
//                }
//                else {
//                    LOG_WARN(L"key {} NOT registered because unknown mod", key.ToString().c_str());
//                    return;
//                }
//            }
//
//            registered = RegisterHotKey(NULL, id, mods, vk);
//            if (!registered) {
//                LOG_WARN(L"key {} NOT registered", key.ToString().c_str());
//                IFW_LOG(registered);
//            }
//            else {
//                LOG_INFO_2(L"key %S registered OK", key.ToString2().c_str());
//            }
//            this->id = id;
//            
//        }
//        void unreg() {
//            if (registered) {
//                registered = false;
//                IFW_LOG(UnregisterHotKey(NULL, id));
//            }
//
//        }
//        ~RegData() {
//            unreg();
//        }
//    };
//
//    std::map<int, RegData> g_global_reg_data;
//
//    void GlobRegHotKey(int id, CHotKey key) {
//        g_global_reg_data[id].reg(id, key);
//    }
//
//}
//
//
////void Rereg_all() {
////
////    return; // отключаем за ненадобностью...
////
////    //g_global_reg_data.clear();
////    auto conf = conf_get_unsafe();
////    for (auto& hk : conf->hotkeysList) {
////        int cur = 1;
////        for (auto& kk : hk.second.keys) {
////            int id = hk.first*10 + cur++;
////            GlobRegHotKey(id, kk);
////        }
////    }
////}
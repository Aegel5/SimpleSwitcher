#include <atomic>



class LayNotif {
public:
    std::atomic<HKL> g_curLay = 0;
    std::atomic<bool> wasErr  = false;

};

inline LayNotif g_laynotif;
#pragma once 

class FloatPanel : public FloatPanelBase {
    using FloatPanelBase::FloatPanelBase;
public:
    void SetFlag(const wxBitmapBundle& flag) {
        auto cur = flag.GetBitmap({ 32,32 });
        m_bitmap1->SetBitmap(cur);
    }
};


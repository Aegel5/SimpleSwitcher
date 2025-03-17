#pragma once 

class FloatPanel : public FloatPanelBase {
    using FloatPanelBase::FloatPanelBase;
public:
    void SetFlag(const wxBitmap& flag) {
        if (!flag.IsOk()) return;
        auto size = flag.GetSize();
        this->SetSize(size);
        m_bitmap1->SetPosition({0, 0});
        m_bitmap1->SetSize(size);
        m_bitmap1->SetBitmap(flag);
    }
};


namespace WxUtils {
    void BindCheckbox(wxCheckBox* elem, auto get, auto set) {
        elem->SetValue(get());
        elem->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED,
            [elem, set](wxCommandEvent& ev) {
                set(elem->GetValue());
            }
        );
    }

    void BindChoice(wxChoice* elem, auto init, auto onSel) {
        init(elem);
        elem->Bind(wxEVT_COMMAND_CHOICE_SELECTED,
            [elem, onSel](wxCommandEvent& ev) {
                onSel(elem);
            }
        );
    }

    void BindButtom(wxButton* elem, auto click) {
        elem->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
            [click](wxCommandEvent& ev) {
                click();
            }
        );
    }

    inline void ClearGrid(wxGrid* grid) {
        if (grid->GetNumberRows() > 0)
            grid->DeleteRows(0, grid->GetNumberRows());
    }

    inline void ForceShow(wxFrame* f) {
        f->Show(true);
        f->Restore();
        f->Raise();
    }
}

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/grid.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/hyperlink.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame4
///////////////////////////////////////////////////////////////////////////////
class MyFrame4 : public wxFrame
{
	private:

	protected:
		wxNotebook* m_notebook2;
		wxPanel* m_panelMain;
		wxCheckBox* m_checkBoxEnable;
		wxCheckBox* m_checkAddToAutoStart;
		wxStaticText* m_staticTextExplain;
		wxCheckBox* m_checkBoxWorkInAdmin;
		wxCheckBox* m_checkBoxShowFlags;
		wxCheckBox* m_checkBoxAlterantiveLayoutChange;
		wxStaticText* m_staticText9;
		wxChoice* m_comboUiLang;
		wxCheckBox* m_checkDebuglog;
		wxCheckBox* m_checkBoxKeyDef;
		wxCheckBox* m_checkBoxDisablAcc;
		wxCheckBox* m_checkBoxClearForm;
		wxCheckBox* m_checkBoxAllowInjected;
		wxCheckBox* m_checkBoxFixRAlt;
		wxChoice* m_choiceFixRalt;
		wxPanel* m_panel141;
		wxGrid* m_gridHotKeys;
		wxGrid* m_gridLayouts;
		wxPanel* m_panel5;
		wxCheckBox* m_checkcapsrem;
		wxCheckBox* m_check_scrollremap;
		wxButton* m_button7;
		wxButton* m_button8;
		wxListBox* m_listBoxRemap;
		wxPanel* m_panel6;
		wxStaticText* m_staticTextBuildDate;
		wxHyperlinkCtrl* m_hyperlink1;
		wxButton* m_button1;
		wxButton* m_button2;

		// Virtual event handlers, override them in your derived class
		virtual void onEnable( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAutocheck( wxCommandEvent& event ) { event.Skip(); }
		virtual void onShowFlags( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEnableLog( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPrevent( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDisableAccessebl( wxCommandEvent& event ) { event.Skip(); }
		virtual void onHotDClick( wxGridEvent& event ) { event.Skip(); }
		virtual void on_grid_lay_double( wxGridEvent& event ) { event.Skip(); }
		virtual void onRemapCaps( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUpdateRemap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDeleteRemap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCloseToTray( wxCommandEvent& event ) { event.Skip(); }


	public:

		MyFrame4( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("SimpleSwitcher"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MyFrame4();

};

///////////////////////////////////////////////////////////////////////////////
/// Class MyDialog1
///////////////////////////////////////////////////////////////////////////////
class MyDialog1 : public wxDialog
{
	private:

	protected:
		wxChoice* m_choiceKey;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textKey;
		wxButton* m_button6;
		wxCheckBox* m_checkBox12;
		wxCheckBox* m_checkBox13;
		wxButton* m_button4;
		wxButton* m_button5;

		// Virtual event handlers, override them in your derived class
		virtual void OnChoiceSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onclear( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSetLeftRight( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSetKeyup( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOk( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }


	public:

		MyDialog1( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Set hotkey"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~MyDialog1();

};


///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/panel.h>
#include <wx/listbox.h>
#include <wx/hyperlink.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/radiobox.h>
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
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textLastword;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textSeveralWords;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textSelected;
		wxChoice* m_choiceLayFilter;
		wxTextCtrl* m_textFilterLay;
		wxButton* m_button3;
		wxPanel* m_panel14;
		wxStaticText* m_staticText32;
		wxTextCtrl* m_textCycleLay;
		wxChoice* m_choiceset1;
		wxTextCtrl* m_textSetlay1;
		wxChoice* m_choiceset2;
		wxTextCtrl* m_textSetlay2;
		wxChoice* m_choiceset3;
		wxTextCtrl* m_textSetlay3;
		wxPanel* m_panel5;
		wxStaticText* m_staticText8;
		wxCheckBox* m_checkcapsrem;
		wxStaticText* m_staticText321;
		wxTextCtrl* m_textcapsgen;
		wxButton* m_button7;
		wxButton* m_button8;
		wxListBox* m_listBoxRemap;
		wxPanel* m_panel16;
		wxCheckBox* m_checkDebuglog;
		wxCheckBox* m_checkBoxKeyDef;
		wxCheckBox* m_checkBoxDisablAcc;
		wxCheckBox* m_checkBoxClearForm;
		wxPanel* m_panel6;
		wxStaticText* m_staticTextBuildDate;
		wxHyperlinkCtrl* m_hyperlink1;
		wxButton* m_button1;
		wxButton* m_button2;

		// Virtual event handlers, override them in your derived class
		virtual void onEnable( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAutocheck( wxCommandEvent& event ) { event.Skip(); }
		virtual void onWorkInAdminCheck( wxCommandEvent& event ) { event.Skip(); }
		virtual void onLayChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClearFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRemapCaps( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUpdateRemap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDeleteRemap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEnableLog( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPrevent( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDisableAccessebl( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClearFormat( wxCommandEvent& event ) { event.Skip(); }
		virtual void onExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCloseToTray( wxCommandEvent& event ) { event.Skip(); }


	public:

		MyFrame4( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SimpleSwitcher"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 733,450 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MyFrame4();

};

///////////////////////////////////////////////////////////////////////////////
/// Class MyDialog1
///////////////////////////////////////////////////////////////////////////////
class MyDialog1 : public wxDialog
{
	private:

	protected:
		wxRadioBox* m_radioBox1;
		wxCheckBox* m_checkBox12;
		wxCheckBox* m_checkBox13;
		wxTextCtrl* m_textKey;
		wxButton* m_button6;
		wxButton* m_button4;
		wxButton* m_button5;

		// Virtual event handlers, override them in your derived class
		virtual void onSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSetLeftRight( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSetKeyup( wxCommandEvent& event ) { event.Skip(); }
		virtual void onclear( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOk( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }


	public:

		MyDialog1( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Set hotkey"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 454,270 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~MyDialog1();

};


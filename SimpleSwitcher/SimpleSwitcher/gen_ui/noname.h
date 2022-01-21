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
#include <wx/notebook.h>
#include <wx/frame.h>

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
		wxStaticText* m_staticTextRegisr;
		wxStaticText* m_staticTextScheduler;
		wxCheckBox* m_checkBoxWorkInAdmin;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textLastword;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textSeveralWords;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textSelected;
		wxChoice* m_choice1;
		wxTextCtrl* m_textCtrl4;
		wxButton* m_button3;
		wxPanel* m_panel14;
		wxPanel* m_panel15;
		wxPanel* m_panel5;
		wxPanel* m_panel16;
		wxPanel* m_panel6;
		wxButton* m_button1;
		wxButton* m_button2;

		// Virtual event handlers, override them in your derived class
		virtual void onEnable( wxCommandEvent& event ) { event.Skip(); }
		virtual void onExit( wxCommandEvent& event ) { event.Skip(); }


	public:

		MyFrame4( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SimpleSwitcher"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 733,450 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MyFrame4();

};


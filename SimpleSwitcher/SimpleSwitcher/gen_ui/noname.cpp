///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "noname.h"

///////////////////////////////////////////////////////////////////////////

MyFrame4::MyFrame4( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelMain = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("Base") ), wxVERTICAL );

	m_checkBoxEnable = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxEnable, 0, wxALL, 5 );

	m_checkAddToAutoStart = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Add to autostart"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkAddToAutoStart, 0, wxALL, 5 );

	m_staticTextRegisr = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("          Registry:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	m_staticTextRegisr->Wrap( -1 );
	sbSizer1->Add( m_staticTextRegisr, 0, wxALL, 2 );

	m_staticTextScheduler = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("          Scheduler:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	m_staticTextScheduler->Wrap( -1 );
	sbSizer1->Add( m_staticTextScheduler, 0, wxALL, 2 );

	m_checkBoxWorkInAdmin = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Work in programs running by admin"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxWorkInAdmin, 0, wxALL, 5 );


	bSizer3->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("Fix layout") ), wxVERTICAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Last word"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText3->Wrap( -1 );
	bSizer5->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl1 = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_textCtrl1, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sbSizer2->Add( bSizer5, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText7 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Several words"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText7->Wrap( -1 );
	bSizer51->Add( m_staticText7, 0, wxALL, 5 );

	m_textCtrl2 = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Ctrl + F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_textCtrl2, 1, wxALL|wxEXPAND, 5 );


	sbSizer2->Add( bSizer51, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText31 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Selected text"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText31->Wrap( -1 );
	bSizer52->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl11 = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_textCtrl11, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	sbSizer2->Add( bSizer52, 1, wxEXPAND, 5 );


	bSizer3->Add( sbSizer2, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("Layout filter and switch order") ), wxHORIZONTAL );

	wxString m_choice1Choices[] = { wxT("us-US"), wxT("ru") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( sbSizer3->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), m_choice1NChoices, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	sbSizer3->Add( m_choice1, 0, wxALL, 5 );

	m_textCtrl4 = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_textCtrl4, 1, wxALL, 5 );

	m_button3 = new wxButton( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_button3, 0, wxALL, 5 );


	bSizer3->Add( sbSizer3, 0, wxALL|wxEXPAND, 5 );


	m_panelMain->SetSizer( bSizer3 );
	m_panelMain->Layout();
	bSizer3->Fit( m_panelMain );
	m_notebook2->AddPage( m_panelMain, wxT("Main"), true );
	m_panel14 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );


	m_panel14->SetSizer( bSizer4 );
	m_panel14->Layout();
	bSizer4->Fit( m_panel14 );
	m_notebook2->AddPage( m_panel14, wxT("Change Layout"), false );
	m_panel15 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_panel15, wxT("Change Rules"), false );
	m_panel5 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_panel5, wxT("Key Remap"), false );
	m_panel16 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_panel16, wxT("Settings"), false );
	m_panel6 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_panel6, wxT("About"), false );

	bSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 0 );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );

	m_button1 = new wxButton( this, wxID_ANY, wxT("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_button2, 0, wxALL, 5 );


	bSizer1->Add( bSizer15, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );
}

MyFrame4::~MyFrame4()
{
}

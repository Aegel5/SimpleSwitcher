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
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelMain = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("label") ), wxVERTICAL );

	m_checkBoxEnable = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxEnable, 0, wxALL, 5 );

	m_checkAddToAutoStart = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Add to autostart"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkAddToAutoStart, 0, wxALL, 5 );

	m_staticTextRegisr = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Registry:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	m_staticTextRegisr->Wrap( -1 );
	sbSizer1->Add( m_staticTextRegisr, 0, wxALL, 2 );

	m_staticTextScheduler = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Registry:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	m_staticTextScheduler->Wrap( -1 );
	sbSizer1->Add( m_staticTextScheduler, 0, wxALL, 2 );

	m_checkBoxWorkInAdmin = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Work in program launched by admin"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxWorkInAdmin, 0, wxALL, 5 );


	bSizer3->Add( sbSizer1, 1, wxEXPAND, 5 );


	m_panelMain->SetSizer( bSizer3 );
	m_panelMain->Layout();
	bSizer3->Fit( m_panelMain );
	m_notebook2->AddPage( m_panelMain, wxT("Main"), false );
	m_panel14 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );


	m_panel14->SetSizer( bSizer4 );
	m_panel14->Layout();
	bSizer4->Fit( m_panel14 );
	m_notebook2->AddPage( m_panel14, wxT("a page"), true );
	m_panel15 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_panel15, wxT("a page"), false );
	m_panel16 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebook2->AddPage( m_panel16, wxT("a page"), false );

	bSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );
}

MyFrame4::~MyFrame4()
{
}

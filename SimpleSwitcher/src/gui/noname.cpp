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
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelMain = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("Base") ), wxVERTICAL );

	m_checkBoxEnable = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxEnable->SetValue(true);
	sbSizer1->Add( m_checkBoxEnable, 0, wxALL, 5 );

	m_checkAddToAutoStart = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Add to autostart"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkAddToAutoStart, 0, wxALL, 5 );

	m_staticTextExplain = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("          Registry: djkfjdkfjdkjf\n          Scheduler: fasfdasdf"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	m_staticTextExplain->Wrap( -1 );
	m_staticTextExplain->SetMaxSize( wxSize( 700,-1 ) );

	sbSizer1->Add( m_staticTextExplain, 0, wxALL, 2 );

	m_checkBoxWorkInAdmin = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Work in programs running by admin"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxWorkInAdmin, 0, wxALL, 5 );

	m_checkBoxShowFlags = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Show flags in tray"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxShowFlags, 0, wxALL, 5 );


	bSizer3->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("Fix layout") ), wxVERTICAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Last word"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText3->Wrap( -1 );
	bSizer5->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textLastword = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textLastword->SetMinSize( wxSize( 400,-1 ) );

	bSizer5->Add( m_textLastword, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button9 = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_button9, 0, wxALL, 5 );


	sbSizer2->Add( bSizer5, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText7 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Several words"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText7->Wrap( -1 );
	bSizer51->Add( m_staticText7, 0, wxALL, 5 );

	m_textSeveralWords = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Ctrl + F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_textSeveralWords, 1, wxALL|wxEXPAND, 5 );

	m_button91 = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_button91, 0, wxALL, 5 );


	sbSizer2->Add( bSizer51, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText31 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Selected text"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText31->Wrap( -1 );
	bSizer52->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textSelected = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_textSelected, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button92 = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_button92, 0, wxALL, 5 );


	sbSizer2->Add( bSizer52, 1, wxEXPAND, 5 );


	bSizer3->Add( sbSizer2, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, wxT("Work only with this layouts (leave empty for all):") ), wxHORIZONTAL );

	wxString m_choiceLayFilterChoices[] = { wxT("us-US"), wxT("ru") };
	int m_choiceLayFilterNChoices = sizeof( m_choiceLayFilterChoices ) / sizeof( wxString );
	m_choiceLayFilter = new wxChoice( sbSizer3->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), m_choiceLayFilterNChoices, m_choiceLayFilterChoices, 0 );
	m_choiceLayFilter->SetSelection( 0 );
	sbSizer3->Add( m_choiceLayFilter, 0, wxALL, 5 );

	m_textFilterLay = new wxTextCtrl( sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	sbSizer3->Add( m_textFilterLay, 1, wxALL, 5 );

	m_button3 = new wxButton( sbSizer3->GetStaticBox(), wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_button3, 0, wxALL, 5 );


	bSizer3->Add( sbSizer3, 1, wxALL|wxEXPAND, 5 );


	m_panelMain->SetSizer( bSizer3 );
	m_panelMain->Layout();
	bSizer3->Fit( m_panelMain );
	m_notebook2->AddPage( m_panelMain, wxT("Main"), true );
	m_panel14 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel14, wxID_ANY, wxT("Cycle change") ), wxVERTICAL );

	wxBoxSizer* bSizer53;
	bSizer53 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText32 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Next layout"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText32->Wrap( -1 );
	bSizer53->Add( m_staticText32, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCycleLay = new wxTextCtrl( sbSizer4->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer53->Add( m_textCycleLay, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button93 = new wxButton( sbSizer4->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer53->Add( m_button93, 0, wxALL, 5 );


	sbSizer4->Add( bSizer53, 0, wxEXPAND, 5 );


	bSizer4->Add( sbSizer4, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_panel14, wxID_ANY, wxT("Set layout") ), wxVERTICAL );

	wxBoxSizer* bSizer531;
	bSizer531 = new wxBoxSizer( wxHORIZONTAL );

	wxString m_choiceset1Choices[] = { wxT("us-US"), wxT("ru") };
	int m_choiceset1NChoices = sizeof( m_choiceset1Choices ) / sizeof( wxString );
	m_choiceset1 = new wxChoice( sbSizer5->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), m_choiceset1NChoices, m_choiceset1Choices, 0 );
	m_choiceset1->SetSelection( 0 );
	bSizer531->Add( m_choiceset1, 0, wxALL, 5 );

	m_textSetlay1 = new wxTextCtrl( sbSizer5->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer531->Add( m_textSetlay1, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button94 = new wxButton( sbSizer5->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer531->Add( m_button94, 0, wxALL, 5 );


	sbSizer5->Add( bSizer531, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5311;
	bSizer5311 = new wxBoxSizer( wxHORIZONTAL );

	wxString m_choiceset2Choices[] = { wxT("us-US"), wxT("ru") };
	int m_choiceset2NChoices = sizeof( m_choiceset2Choices ) / sizeof( wxString );
	m_choiceset2 = new wxChoice( sbSizer5->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), m_choiceset2NChoices, m_choiceset2Choices, 0 );
	m_choiceset2->SetSelection( 0 );
	bSizer5311->Add( m_choiceset2, 0, wxALL, 5 );

	m_textSetlay2 = new wxTextCtrl( sbSizer5->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5311->Add( m_textSetlay2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button95 = new wxButton( sbSizer5->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5311->Add( m_button95, 0, wxALL, 5 );


	sbSizer5->Add( bSizer5311, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5312;
	bSizer5312 = new wxBoxSizer( wxHORIZONTAL );

	wxString m_choiceset3Choices[] = { wxT("us-US"), wxT("ru") };
	int m_choiceset3NChoices = sizeof( m_choiceset3Choices ) / sizeof( wxString );
	m_choiceset3 = new wxChoice( sbSizer5->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), m_choiceset3NChoices, m_choiceset3Choices, 0 );
	m_choiceset3->SetSelection( 0 );
	bSizer5312->Add( m_choiceset3, 0, wxALL, 5 );

	m_textSetlay3 = new wxTextCtrl( sbSizer5->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5312->Add( m_textSetlay3, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button96 = new wxButton( sbSizer5->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5312->Add( m_button96, 0, wxALL, 5 );


	sbSizer5->Add( bSizer5312, 0, wxEXPAND, 5 );


	bSizer4->Add( sbSizer5, 0, wxEXPAND, 5 );


	m_panel14->SetSizer( bSizer4 );
	m_panel14->Layout();
	bSizer4->Fit( m_panel14 );
	m_notebook2->AddPage( m_panel14, wxT("Change Layout"), false );
	m_panel5 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxT("Capslock") ), wxVERTICAL );

	m_checkcapsrem = new wxCheckBox( sbSizer6->GetStaticBox(), wxID_ANY, wxT("Remap Capslock as F24"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer6->Add( m_checkcapsrem, 0, wxALL, 5 );

	wxBoxSizer* bSizer532;
	bSizer532 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText321 = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, wxT("Generate CapsLock"), wxDefaultPosition, wxSize( 150,-1 ), 0 );
	m_staticText321->Wrap( -1 );
	bSizer532->Add( m_staticText321, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textcapsgen = new wxTextCtrl( sbSizer6->GetStaticBox(), wxID_ANY, wxT("F24"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer532->Add( m_textcapsgen, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_button97 = new wxButton( sbSizer6->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer532->Add( m_button97, 0, wxALL, 5 );


	sbSizer6->Add( bSizer532, 0, wxEXPAND, 5 );


	bSizer20->Add( sbSizer6, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );

	m_button7 = new wxButton( m_panel5, wxID_ANY, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( m_button7, 0, wxALL, 5 );

	m_button8 = new wxButton( m_panel5, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( m_button8, 0, wxALL, 5 );


	bSizer20->Add( bSizer19, 0, wxEXPAND, 5 );

	m_listBoxRemap = new wxListBox( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_listBoxRemap->Append( wxT("123") );
	m_listBoxRemap->Append( wxT("1222") );
	bSizer20->Add( m_listBoxRemap, 1, wxALL|wxEXPAND, 5 );


	m_panel5->SetSizer( bSizer20 );
	m_panel5->Layout();
	bSizer20->Fit( m_panel5 );
	m_notebook2->AddPage( m_panel5, wxT("Key Remap"), false );
	m_panel16 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );

	m_checkDebuglog = new wxCheckBox( m_panel16, wxID_ANY, wxT("Enable debug log"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_checkDebuglog, 0, wxALL, 5 );

	m_checkBoxKeyDef = new wxCheckBox( m_panel16, wxID_ANY, wxT("Prevent other programs to hook keyboard (may cause problems in games and other apps)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxKeyDef->SetValue(true);
	bSizer24->Add( m_checkBoxKeyDef, 0, wxALL, 5 );

	m_checkBoxDisablAcc = new wxCheckBox( m_panel16, wxID_ANY, wxT("Disable the accessibility shortcut keys (5 SHIFT and others)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_checkBoxDisablAcc, 0, wxALL, 5 );

	m_checkBoxClearForm = new wxCheckBox( m_panel16, wxID_ANY, wxT("Clear text format on Ctrl-C"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_checkBoxClearForm, 0, wxALL, 5 );


	m_panel16->SetSizer( bSizer24 );
	m_panel16->Layout();
	bSizer24->Fit( m_panel16 );
	m_notebook2->AddPage( m_panel16, wxT("Settings"), false );
	m_panel6 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_staticTextBuildDate = new wxStaticText( m_panel6, wxID_ANY, wxT("Build date:"), wxDefaultPosition, wxDefaultSize, 0|wxBORDER_RAISED );
	m_staticTextBuildDate->Wrap( -1 );
	bSizer191->Add( m_staticTextBuildDate, 0, wxALL, 5 );

	m_hyperlink1 = new wxHyperlinkCtrl( m_panel6, wxID_ANY, wxEmptyString, wxT("https://github.com/Aegel5/SimpleSwitcher"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer191->Add( m_hyperlink1, 0, wxALL, 5 );

	m_hyperlink11 = new wxHyperlinkCtrl( m_panel6, wxID_ANY, wxEmptyString, wxT("https://vk.com/SimpleSwitcher"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer191->Add( m_hyperlink11, 0, wxALL, 5 );


	m_panel6->SetSizer( bSizer191 );
	m_panel6->Layout();
	bSizer191->Fit( m_panel6 );
	m_notebook2->AddPage( m_panel6, wxT("About"), false );

	bSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 0 );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );

	m_button1 = new wxButton( this, wxID_ANY, wxT("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Close to tray"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_button2, 0, wxALL, 5 );


	bSizer1->Add( bSizer15, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_checkBoxEnable->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onEnable ), NULL, this );
	m_checkAddToAutoStart->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onAutocheck ), NULL, this );
	m_checkBoxWorkInAdmin->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onWorkInAdminCheck ), NULL, this );
	m_checkBoxShowFlags->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onShowFlags ), NULL, this );
	m_choiceLayFilter->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onClearFilter ), NULL, this );
	m_choiceset1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_choiceset2->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_choiceset3->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_checkcapsrem->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onRemapCaps ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onUpdateRemap ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onDeleteRemap ), NULL, this );
	m_checkDebuglog->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onEnableLog ), NULL, this );
	m_checkBoxKeyDef->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onPrevent ), NULL, this );
	m_checkBoxDisablAcc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onDisableAccessebl ), NULL, this );
	m_checkBoxClearForm->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onClearFormat ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onExit ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onCloseToTray ), NULL, this );
}

MyFrame4::~MyFrame4()
{
	// Disconnect Events
	m_checkBoxEnable->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onEnable ), NULL, this );
	m_checkAddToAutoStart->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onAutocheck ), NULL, this );
	m_checkBoxWorkInAdmin->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onWorkInAdminCheck ), NULL, this );
	m_checkBoxShowFlags->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onShowFlags ), NULL, this );
	m_choiceLayFilter->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onClearFilter ), NULL, this );
	m_choiceset1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_choiceset2->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_choiceset3->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onLayChoice ), NULL, this );
	m_checkcapsrem->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onRemapCaps ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onUpdateRemap ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onDeleteRemap ), NULL, this );
	m_checkDebuglog->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onEnableLog ), NULL, this );
	m_checkBoxKeyDef->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onPrevent ), NULL, this );
	m_checkBoxDisablAcc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onDisableAccessebl ), NULL, this );
	m_checkBoxClearForm->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onClearFormat ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onExit ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onCloseToTray ), NULL, this );

}

MyDialog1::MyDialog1( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );

	wxString m_radioBox1Choices[] = { wxT("Custom"), wxT("123"), wxT("333") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( this, wxID_ANY, wxT("Hotkey"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 0 );
	m_radioBox1->SetMinSize( wxSize( 400,-1 ) );

	bSizer16->Add( m_radioBox1, 0, wxALL|wxEXPAND, 5 );

	m_checkBox12 = new wxCheckBox( this, wxID_ANY, wxT("Distinguish between left and right modifiers"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_checkBox12, 0, wxALL, 5 );

	m_checkBox13 = new wxCheckBox( this, wxID_ANY, wxT("Triggers on KEYUP"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_checkBox13, 0, wxALL, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_textKey = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer20->Add( m_textKey, 1, wxALL, 5 );

	m_button6 = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button6, 0, wxALL, 5 );


	bSizer16->Add( bSizer20, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_button4 = new wxButton( this, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button4, 0, wxALL, 5 );

	m_button5 = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button5, 0, wxALL, 5 );


	bSizer16->Add( bSizer18, 0, wxALIGN_RIGHT, 5 );


	bSizer21->Add( bSizer16, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer21 );
	this->Layout();
	bSizer21->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_radioBox1->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyDialog1::onSelected ), NULL, this );
	m_checkBox12->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetLeftRight ), NULL, this );
	m_checkBox13->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetKeyup ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onclear ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onOk ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onCancel ), NULL, this );
}

MyDialog1::~MyDialog1()
{
	// Disconnect Events
	m_radioBox1->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyDialog1::onSelected ), NULL, this );
	m_checkBox12->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetLeftRight ), NULL, this );
	m_checkBox13->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetKeyup ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onclear ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onOk ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onCancel ), NULL, this );

}

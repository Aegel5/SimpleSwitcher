///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "noname.h"

///////////////////////////////////////////////////////////////////////////

MyFrame4::MyFrame4( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 650,482 ), wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelMain = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, _("Base") ), wxVERTICAL );

	m_checkBoxEnable = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxEnable->SetValue(true);
	sbSizer1->Add( m_checkBoxEnable, 0, wxALL, 5 );

	m_checkAddToAutoStart = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Add to autostart"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkAddToAutoStart, 0, wxALL, 5 );

	m_staticTextExplain = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("          Registry: djkfjdkfjdkjf\n          Scheduler: fasfdasdf"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	m_staticTextExplain->Wrap( -1 );
	m_staticTextExplain->SetMaxSize( wxSize( 700,-1 ) );

	sbSizer1->Add( m_staticTextExplain, 0, wxALL, 2 );

	m_checkBoxWorkInAdmin = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Work in programs running by admin"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxWorkInAdmin, 0, wxALL, 5 );

	m_checkBoxShowFlags = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Show flags in tray"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxShowFlags, 0, wxALL, 5 );

	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText9 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("GUI Language"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer23->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );

	wxArrayString m_comboUiLangChoices;
	m_comboUiLang = new wxChoice( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_comboUiLangChoices, 0 );
	m_comboUiLang->SetSelection( 0 );
	bSizer23->Add( m_comboUiLang, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );


	sbSizer1->Add( bSizer23, 0, wxALL|wxEXPAND, 5 );


	bSizer3->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_panelMain, wxID_ANY, _("Advanced") ), wxVERTICAL );

	m_checkDebuglog = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, _("Enable debug log"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer10->Add( m_checkDebuglog, 0, wxALL, 5 );

	m_checkBoxKeyDef = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, _("Prevent other programs to hook keyboard (may cause problems in games and other apps)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxKeyDef->SetValue(true);
	sbSizer10->Add( m_checkBoxKeyDef, 0, wxALL, 5 );

	m_checkBoxDisablAcc = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, _("Disable the accessibility shortcut keys (5 SHIFT and others)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer10->Add( m_checkBoxDisablAcc, 0, wxALL, 5 );

	m_checkBoxClearForm = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, _("Clear text format on Ctrl-C"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer10->Add( m_checkBoxClearForm, 0, wxALL, 5 );

	m_checkBoxAllowInjected = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, _("Allow remote keys"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer10->Add( m_checkBoxAllowInjected, 0, wxALL, 5 );

	m_checkBoxAlterantiveLayoutChange = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, _("Alternative mode layout change (emulate Windows hotkeys)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer10->Add( m_checkBoxAlterantiveLayoutChange, 0, wxALL, 5 );


	bSizer3->Add( sbSizer10, 1, wxALL|wxEXPAND, 5 );


	m_panelMain->SetSizer( bSizer3 );
	m_panelMain->Layout();
	bSizer3->Fit( m_panelMain );
	m_notebook2->AddPage( m_panelMain, _("Settings"), false );
	m_panel141 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxVERTICAL );

	m_gridHotKeys = new wxGrid( m_panel141, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_gridHotKeys->CreateGrid( 5, 1 );
	m_gridHotKeys->EnableEditing( false );
	m_gridHotKeys->EnableGridLines( true );
	m_gridHotKeys->EnableDragGridSize( false );
	m_gridHotKeys->SetMargins( 0, 0 );

	// Columns
	m_gridHotKeys->SetColSize( 0, 500 );
	m_gridHotKeys->EnableDragColMove( false );
	m_gridHotKeys->EnableDragColSize( true );
	m_gridHotKeys->SetColLabelValue( 0, _("Hotkey") );
	m_gridHotKeys->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_gridHotKeys->AutoSizeRows();
	m_gridHotKeys->EnableDragRowSize( true );
	m_gridHotKeys->SetRowLabelValue( 0, _("Selected text to upper/lower") );
	m_gridHotKeys->SetRowLabelAlignment( wxALIGN_LEFT, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_gridHotKeys->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_CENTER );
	bSizer41->Add( m_gridHotKeys, 1, wxALL|wxEXPAND, 5 );

	m_gridLayouts = new wxGrid( m_panel141, wxID_ANY, wxPoint( -1,-1 ), wxDefaultSize, 0 );

	// Grid
	m_gridLayouts->CreateGrid( 0, 3 );
	m_gridLayouts->EnableEditing( false );
	m_gridLayouts->EnableGridLines( true );
	m_gridLayouts->EnableDragGridSize( false );
	m_gridLayouts->SetMargins( 0, 0 );

	// Columns
	m_gridLayouts->EnableDragColMove( false );
	m_gridLayouts->EnableDragColSize( true );
	m_gridLayouts->SetColLabelValue( 0, _("Use in cycle") );
	m_gridLayouts->SetColLabelValue( 1, _("Hotkey") );
	m_gridLayouts->SetColLabelValue( 2, _("Win hotkey") );
	m_gridLayouts->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_gridLayouts->EnableDragRowSize( true );
	m_gridLayouts->SetRowLabelSize( 50 );
	m_gridLayouts->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_gridLayouts->SetDefaultCellAlignment( wxALIGN_CENTER, wxALIGN_TOP );
	bSizer41->Add( m_gridLayouts, 1, wxALL|wxEXPAND, 5 );


	m_panel141->SetSizer( bSizer41 );
	m_panel141->Layout();
	bSizer41->Fit( m_panel141 );
	m_notebook2->AddPage( m_panel141, _("Hotkeys"), false );
	m_panel5 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, _("CapsLock / ScrollLock") ), wxVERTICAL );

	m_checkcapsrem = new wxCheckBox( sbSizer6->GetStaticBox(), wxID_ANY, _("Remap CapsLock as F24"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer6->Add( m_checkcapsrem, 0, wxALL, 5 );

	m_check_scrollremap = new wxCheckBox( sbSizer6->GetStaticBox(), wxID_ANY, _("Remap ScrollLock as F23"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer6->Add( m_check_scrollremap, 0, wxALL, 5 );


	bSizer20->Add( sbSizer6, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );

	m_button7 = new wxButton( m_panel5, wxID_ANY, _("Update"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( m_button7, 0, wxALL, 5 );

	m_button8 = new wxButton( m_panel5, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( m_button8, 0, wxALL, 5 );


	bSizer20->Add( bSizer19, 0, wxALL|wxEXPAND, 5 );

	m_listBoxRemap = new wxListBox( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer20->Add( m_listBoxRemap, 1, wxALL|wxEXPAND, 5 );


	m_panel5->SetSizer( bSizer20 );
	m_panel5->Layout();
	bSizer20->Fit( m_panel5 );
	m_notebook2->AddPage( m_panel5, _("Keys Remap"), false );
	m_panel6 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );

	m_staticTextBuildDate = new wxStaticText( m_panel6, wxID_ANY, _("Build date:"), wxDefaultPosition, wxDefaultSize, 0|wxBORDER_RAISED );
	m_staticTextBuildDate->Wrap( -1 );
	bSizer191->Add( m_staticTextBuildDate, 0, wxALL, 5 );

	m_hyperlink1 = new wxHyperlinkCtrl( m_panel6, wxID_ANY, wxEmptyString, wxT("https://github.com/Aegel5/SimpleSwitcher"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer191->Add( m_hyperlink1, 0, wxALL, 5 );


	m_panel6->SetSizer( bSizer191 );
	m_panel6->Layout();
	bSizer191->Fit( m_panel6 );
	m_notebook2->AddPage( m_panel6, _("About"), false );

	bSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 0 );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );

	m_button1 = new wxButton( this, wxID_ANY, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( this, wxID_ANY, _("Close to tray"), wxDefaultPosition, wxDefaultSize, 0 );
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
	m_comboUiLang->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onUiSelect ), NULL, this );
	m_checkDebuglog->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onEnableLog ), NULL, this );
	m_checkBoxKeyDef->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onPrevent ), NULL, this );
	m_checkBoxDisablAcc->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onDisableAccessebl ), NULL, this );
	m_checkBoxClearForm->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onClearFormat ), NULL, this );
	m_checkBoxAllowInjected->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onAllowInject ), NULL, this );
	m_checkBoxAlterantiveLayoutChange->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onAlternative ), NULL, this );
	m_gridHotKeys->Connect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( MyFrame4::onHotDClick ), NULL, this );
	m_gridLayouts->Connect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( MyFrame4::on_grid_lay_double ), NULL, this );
	m_checkcapsrem->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onRemapCaps ), NULL, this );
	m_check_scrollremap->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onRemapCaps ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onUpdateRemap ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onDeleteRemap ), NULL, this );
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
	m_comboUiLang->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame4::onUiSelect ), NULL, this );
	m_checkDebuglog->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onEnableLog ), NULL, this );
	m_checkBoxKeyDef->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onPrevent ), NULL, this );
	m_checkBoxDisablAcc->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onDisableAccessebl ), NULL, this );
	m_checkBoxClearForm->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onClearFormat ), NULL, this );
	m_checkBoxAllowInjected->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onAllowInject ), NULL, this );
	m_checkBoxAlterantiveLayoutChange->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onAlternative ), NULL, this );
	m_gridHotKeys->Disconnect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( MyFrame4::onHotDClick ), NULL, this );
	m_gridLayouts->Disconnect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( MyFrame4::on_grid_lay_double ), NULL, this );
	m_checkcapsrem->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onRemapCaps ), NULL, this );
	m_check_scrollremap->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyFrame4::onRemapCaps ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onUpdateRemap ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame4::onDeleteRemap ), NULL, this );
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

	wxString m_choiceKeyChoices[] = { _("Custom"), _("123") };
	int m_choiceKeyNChoices = sizeof( m_choiceKeyChoices ) / sizeof( wxString );
	m_choiceKey = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceKeyNChoices, m_choiceKeyChoices, 0 );
	m_choiceKey->SetSelection( 0 );
	bSizer16->Add( m_choiceKey, 0, wxALL, 5 );

	m_staticText10 = new wxStaticText( this, wxID_ANY, _("Press any key..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer16->Add( m_staticText10, 0, wxALL, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_textKey = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer20->Add( m_textKey, 1, wxALL, 5 );

	m_button6 = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button6, 0, wxALL, 5 );


	bSizer16->Add( bSizer20, 1, wxEXPAND, 0 );

	m_checkBox12 = new wxCheckBox( this, wxID_ANY, _("Distinguish between left and right modifiers"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_checkBox12, 0, wxALL, 5 );

	m_checkBox13 = new wxCheckBox( this, wxID_ANY, _("Triggers on KEYUP"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_checkBox13, 0, wxALL, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_button4 = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button4, 0, wxALL, 5 );

	m_button5 = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button5, 0, wxALL, 5 );


	bSizer16->Add( bSizer18, 0, wxALIGN_RIGHT, 5 );


	bSizer21->Add( bSizer16, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer21 );
	this->Layout();
	bSizer21->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_choiceKey->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyDialog1::OnChoiceSelect ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onclear ), NULL, this );
	m_checkBox12->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetLeftRight ), NULL, this );
	m_checkBox13->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetKeyup ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onOk ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onCancel ), NULL, this );
}

MyDialog1::~MyDialog1()
{
	// Disconnect Events
	m_choiceKey->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyDialog1::OnChoiceSelect ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onclear ), NULL, this );
	m_checkBox12->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetLeftRight ), NULL, this );
	m_checkBox13->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MyDialog1::onSetKeyup ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onOk ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog1::onCancel ), NULL, this );

}

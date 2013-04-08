///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Form.h"

///////////////////////////////////////////////////////////////////////////

TeacherMainFrame::TeacherMainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxGridSizer* gSizer21;
	gSizer21 = new wxGridSizer( 1, 9, 0, 0 );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, wxT("Win rate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	gSizer21->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText24 = new wxStaticText( this, wxID_ANY, wxT("Position"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	gSizer21->Add( m_staticText24, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, wxT("Pot ratio"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	gSizer21->Add( m_staticText25, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText26 = new wxStaticText( this, wxID_ANY, wxT("Stack ratio"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	gSizer21->Add( m_staticText26, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, wxT("Players"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	gSizer21->Add( m_staticText27, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText28 = new wxStaticText( this, wxID_ANY, wxT("Danger"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	gSizer21->Add( m_staticText28, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText29 = new wxStaticText( this, wxID_ANY, wxT("Bot average style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	gSizer21->Add( m_staticText29, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText30 = new wxStaticText( this, wxID_ANY, wxT("Bot style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	gSizer21->Add( m_staticText30, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText301 = new wxStaticText( this, wxID_ANY, wxT("Bot stack size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText301->Wrap( -1 );
	gSizer21->Add( m_staticText301, 0, wxALL, 5 );
	
	bSizer4->Add( gSizer21, 0, wxEXPAND, 5 );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 1, 9, 0, 0 );
	
	wxString m_WinRateChoiceChoices[] = { wxT("Very low"), wxT("Low"), wxT("Normal"), wxT("Good"), wxT("Very good"), wxT("Best") };
	int m_WinRateChoiceNChoices = sizeof( m_WinRateChoiceChoices ) / sizeof( wxString );
	m_WinRateChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_WinRateChoiceNChoices, m_WinRateChoiceChoices, 0 );
	m_WinRateChoice->SetSelection( 0 );
	gSizer2->Add( m_WinRateChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_PositionChoiceChoices[] = { wxT("Early"), wxT("Middle"), wxT("Later") };
	int m_PositionChoiceNChoices = sizeof( m_PositionChoiceChoices ) / sizeof( wxString );
	m_PositionChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_PositionChoiceNChoices, m_PositionChoiceChoices, 0 );
	m_PositionChoice->SetSelection( 0 );
	gSizer2->Add( m_PositionChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_PotRateChoiceChoices[] = { wxT("Very low"), wxT("Low"), wxT("Normal"), wxT("High"), wxT("Huge") };
	int m_PotRateChoiceNChoices = sizeof( m_PotRateChoiceChoices ) / sizeof( wxString );
	m_PotRateChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_PotRateChoiceNChoices, m_PotRateChoiceChoices, 0 );
	m_PotRateChoice->SetSelection( 0 );
	gSizer2->Add( m_PotRateChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_StackRateChoiceChoices[] = { wxT("Very low"), wxT("Low"), wxT("Normal"), wxT("High"), wxT("Huge") };
	int m_StackRateChoiceNChoices = sizeof( m_StackRateChoiceChoices ) / sizeof( wxString );
	m_StackRateChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_StackRateChoiceNChoices, m_StackRateChoiceChoices, 0 );
	m_StackRateChoice->SetSelection( 0 );
	gSizer2->Add( m_StackRateChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_PlayersChoiceChoices[] = { wxT("One"), wxT("Two"), wxT("Three or more") };
	int m_PlayersChoiceNChoices = sizeof( m_PlayersChoiceChoices ) / sizeof( wxString );
	m_PlayersChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_PlayersChoiceNChoices, m_PlayersChoiceChoices, 0 );
	m_PlayersChoice->SetSelection( 0 );
	gSizer2->Add( m_PlayersChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_DangerChoiceChoices[] = { wxT("Low"), wxT("Normal"), wxT("High") };
	int m_DangerChoiceNChoices = sizeof( m_DangerChoiceChoices ) / sizeof( wxString );
	m_DangerChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DangerChoiceNChoices, m_DangerChoiceChoices, 0 );
	m_DangerChoice->SetSelection( 0 );
	gSizer2->Add( m_DangerChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_BotAverageStyleChoiceChoices[] = { wxT("Passive"), wxT("Normal"), wxT("Agressive") };
	int m_BotAverageStyleChoiceNChoices = sizeof( m_BotAverageStyleChoiceChoices ) / sizeof( wxString );
	m_BotAverageStyleChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_BotAverageStyleChoiceNChoices, m_BotAverageStyleChoiceChoices, 0 );
	m_BotAverageStyleChoice->SetSelection( 0 );
	gSizer2->Add( m_BotAverageStyleChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_BotStyleChoiceChoices[] = { wxT("Passive"), wxT("Normal"), wxT("Agressive") };
	int m_BotStyleChoiceNChoices = sizeof( m_BotStyleChoiceChoices ) / sizeof( wxString );
	m_BotStyleChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_BotStyleChoiceNChoices, m_BotStyleChoiceChoices, 0 );
	m_BotStyleChoice->SetSelection( 0 );
	gSizer2->Add( m_BotStyleChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_BostStackSizeChoiceChoices[] = { wxT("Small"), wxT("Normal"), wxT("Big") };
	int m_BostStackSizeChoiceNChoices = sizeof( m_BostStackSizeChoiceChoices ) / sizeof( wxString );
	m_BostStackSizeChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_BostStackSizeChoiceNChoices, m_BostStackSizeChoiceChoices, 0 );
	m_BostStackSizeChoice->SetSelection( 0 );
	gSizer2->Add( m_BostStackSizeChoice, 0, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( gSizer2, 0, 0, 5 );
	
	wxGridSizer* gSizer211;
	gSizer211 = new wxGridSizer( 1, 9, 0, 0 );
	
	m_WinRateCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_WinRateCheck, 0, wxALL, 5 );
	
	m_PositionCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_PositionCheck, 0, wxALL, 5 );
	
	m_PotRatioCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_PotRatioCheck, 0, wxALL, 5 );
	
	m_StackRatioCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_StackRatioCheck, 0, wxALL, 5 );
	
	m_PlayersCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_PlayersCheck, 0, wxALL, 5 );
	
	m_DangerCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_DangerCheck, 0, wxALL, 5 );
	
	m_BotAverageStyleCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_BotAverageStyleCheck, 0, wxALL, 5 );
	
	m_BotStyleCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_BotStyleCheck, 0, wxALL, 5 );
	
	m_BotStackCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_BotStackCheck, 0, wxALL, 5 );
	
	bSizer4->Add( gSizer211, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	wxGridSizer* gSizer12;
	gSizer12 = new wxGridSizer( 1, 3, 0, 0 );
	
	wxString m_ActionRadioChoices[] = { wxT("Check/Fold"), wxT("Check/Call"), wxT("Bet/Raise") };
	int m_ActionRadioNChoices = sizeof( m_ActionRadioChoices ) / sizeof( wxString );
	m_ActionRadio = new wxRadioBox( this, wxID_ANY, wxT("Action"), wxDefaultPosition, wxDefaultSize, m_ActionRadioNChoices, m_ActionRadioChoices, 1, wxRA_SPECIFY_COLS );
	m_ActionRadio->SetSelection( 0 );
	gSizer12->Add( m_ActionRadio, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_NextButton = new wxButton( this, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_NextButton, 0, wxALL, 5 );
	
	m_SaveButton = new wxButton( this, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_SaveButton, 0, wxALL, 5 );
	
	m_LoadButton = new wxButton( this, wxID_ANY, wxT("Load"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_LoadButton, 0, wxALL, 5 );
	
	m_TeachButton = new wxButton( this, wxID_ANY, wxT("Teach"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_TeachButton, 0, wxALL, 5 );
	
	m_TestButton = new wxButton( this, wxID_ANY, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_TestButton, 0, wxALL, 5 );
	
	m_GenerateRange = new wxButton( this, wxID_ANY, wxT("Range"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_GenerateRange, 0, wxALL, 5 );
	
	m_ReadButton = new wxButton( this, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_ReadButton, 0, wxALL, 5 );
	
	m_WriteButton = new wxButton( this, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_WriteButton, 0, wxALL, 5 );
	
	m_ExecuteButton = new wxButton( this, wxID_ANY, wxT("Execute"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_ExecuteButton, 0, wxALL, 5 );
	
	gSizer12->Add( fgSizer6, 1, wxEXPAND|wxALL, 5 );
	
	m_QueryText = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	gSizer12->Add( m_QueryText, 0, wxEXPAND|wxALL, 5 );
	
	bSizer4->Add( gSizer12, 0, wxEXPAND, 5 );
	
	m_Grid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_Grid->CreateGrid( 10, 10 );
	m_Grid->EnableEditing( false );
	m_Grid->EnableGridLines( true );
	m_Grid->EnableDragGridSize( false );
	m_Grid->SetMargins( 0, 0 );
	
	// Columns
	m_Grid->EnableDragColMove( false );
	m_Grid->EnableDragColSize( true );
	m_Grid->SetColLabelSize( 30 );
	m_Grid->SetColLabelValue( 0, wxT("Win rate") );
	m_Grid->SetColLabelValue( 1, wxT("Position") );
	m_Grid->SetColLabelValue( 2, wxT("Pot ratio") );
	m_Grid->SetColLabelValue( 3, wxT("Stack ratio") );
	m_Grid->SetColLabelValue( 4, wxT("Players") );
	m_Grid->SetColLabelValue( 5, wxT("Danger") );
	m_Grid->SetColLabelValue( 6, wxT("Bot avg style") );
	m_Grid->SetColLabelValue( 7, wxT("Bot style") );
	m_Grid->SetColLabelValue( 8, wxT("Bot stack size") );
	m_Grid->SetColLabelValue( 9, wxT("Result") );
	m_Grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_Grid->EnableDragRowSize( true );
	m_Grid->SetRowLabelSize( 80 );
	m_Grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_Grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer4->Add( m_Grid, 1, wxALL|wxEXPAND, 5 );
	
	m_Gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	bSizer4->Add( m_Gauge, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
	m_StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	m_WinRateChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnWinRate ), NULL, this );
	m_PositionChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPosition ), NULL, this );
	m_PotRateChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPotRatio ), NULL, this );
	m_StackRateChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStackRatio ), NULL, this );
	m_PlayersChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayers ), NULL, this );
	m_DangerChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayersStyle ), NULL, this );
	m_BotAverageStyleChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStyleChanges ), NULL, this );
	m_BotStyleChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStyle ), NULL, this );
	m_BostStackSizeChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStackSize ), NULL, this );
	m_ActionRadio->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnAction ), NULL, this );
	m_NextButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnNext ), NULL, this );
	m_SaveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnSave ), NULL, this );
	m_LoadButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnLoad ), NULL, this );
	m_TeachButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTeach ), NULL, this );
	m_TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTest ), NULL, this );
	m_GenerateRange->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnRange ), NULL, this );
	m_ReadButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnRead ), NULL, this );
	m_WriteButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnWrite ), NULL, this );
	m_ExecuteButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnExecute ), NULL, this );
	m_Grid->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( TeacherMainFrame::OnKeyDown ), NULL, this );
	m_Grid->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( TeacherMainFrame::OnGridScroll ), NULL, this );
}

TeacherMainFrame::~TeacherMainFrame()
{
	// Disconnect Events
	m_WinRateChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnWinRate ), NULL, this );
	m_PositionChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPosition ), NULL, this );
	m_PotRateChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPotRatio ), NULL, this );
	m_StackRateChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStackRatio ), NULL, this );
	m_PlayersChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayers ), NULL, this );
	m_DangerChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayersStyle ), NULL, this );
	m_BotAverageStyleChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStyleChanges ), NULL, this );
	m_BotStyleChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStyle ), NULL, this );
	m_BostStackSizeChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStackSize ), NULL, this );
	m_ActionRadio->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnAction ), NULL, this );
	m_NextButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnNext ), NULL, this );
	m_SaveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnSave ), NULL, this );
	m_LoadButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnLoad ), NULL, this );
	m_TeachButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTeach ), NULL, this );
	m_TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTest ), NULL, this );
	m_GenerateRange->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnRange ), NULL, this );
	m_ReadButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnRead ), NULL, this );
	m_WriteButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnWrite ), NULL, this );
	m_ExecuteButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnExecute ), NULL, this );
	m_Grid->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( TeacherMainFrame::OnKeyDown ), NULL, this );
	m_Grid->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( TeacherMainFrame::OnGridScroll ), NULL, this );
}

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
	
	m_staticText28 = new wxStaticText( this, wxID_ANY, wxT("Players style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	gSizer21->Add( m_staticText28, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText29 = new wxStaticText( this, wxID_ANY, wxT("Style change"), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	wxString m_PlayersStyleChoiceChoices[] = { wxT("Passive"), wxT("Normal"), wxT("Agressive") };
	int m_PlayersStyleChoiceNChoices = sizeof( m_PlayersStyleChoiceChoices ) / sizeof( wxString );
	m_PlayersStyleChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_PlayersStyleChoiceNChoices, m_PlayersStyleChoiceChoices, 0 );
	m_PlayersStyleChoice->SetSelection( 0 );
	gSizer2->Add( m_PlayersStyleChoice, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_StyleChangeChoiceChoices[] = { wxT("Passive"), wxT("Normal"), wxT("Agressive") };
	int m_StyleChangeChoiceNChoices = sizeof( m_StyleChangeChoiceChoices ) / sizeof( wxString );
	m_StyleChangeChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_StyleChangeChoiceNChoices, m_StyleChangeChoiceChoices, 0 );
	m_StyleChangeChoice->SetSelection( 0 );
	gSizer2->Add( m_StyleChangeChoice, 0, wxALL|wxEXPAND, 5 );
	
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
	
	m_PlayersStyleCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_PlayersStyleCheck, 0, wxALL, 5 );
	
	m_StyleChengeCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_StyleChengeCheck, 0, wxALL, 5 );
	
	m_BotStyleCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_BotStyleCheck, 0, wxALL, 5 );
	
	m_BotStackCheck = new wxCheckBox( this, wxID_ANY, wxT("autoincrement"), wxDefaultPosition, wxDefaultSize, 0 );
	
	gSizer211->Add( m_BotStackCheck, 0, wxALL, 5 );
	
	bSizer4->Add( gSizer211, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	wxGridSizer* gSizer12;
	gSizer12 = new wxGridSizer( 1, 4, 0, 0 );
	
	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 1, 1, 0, 0 );
	
	wxString m_ActionRadioChoices[] = { wxT("Check/Fold"), wxT("Bet/Call"), wxT("Raise/Reraise") };
	int m_ActionRadioNChoices = sizeof( m_ActionRadioChoices ) / sizeof( wxString );
	m_ActionRadio = new wxRadioBox( this, wxID_ANY, wxT("Action"), wxDefaultPosition, wxDefaultSize, m_ActionRadioNChoices, m_ActionRadioChoices, 1, wxRA_SPECIFY_COLS );
	m_ActionRadio->SetSelection( 0 );
	gSizer4->Add( m_ActionRadio, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	gSizer12->Add( gSizer4, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 3, 0, 0 );
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
	
	gSizer12->Add( fgSizer6, 1, wxEXPAND|wxALL, 5 );
	
	bSizer4->Add( gSizer12, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
	m_StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	m_WinRateChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnWinRate ), NULL, this );
	m_PositionChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPosition ), NULL, this );
	m_PotRateChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPotRatio ), NULL, this );
	m_StackRateChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStackRatio ), NULL, this );
	m_PlayersChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayers ), NULL, this );
	m_PlayersStyleChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayersStyle ), NULL, this );
	m_StyleChangeChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStyleChanges ), NULL, this );
	m_BotStyleChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStyle ), NULL, this );
	m_BostStackSizeChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStackSize ), NULL, this );
	m_ActionRadio->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnAction ), NULL, this );
	m_NextButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnNext ), NULL, this );
	m_SaveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnSave ), NULL, this );
	m_LoadButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnLoad ), NULL, this );
	m_TeachButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTeach ), NULL, this );
	m_TestButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTest ), NULL, this );
}

TeacherMainFrame::~TeacherMainFrame()
{
	// Disconnect Events
	m_WinRateChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnWinRate ), NULL, this );
	m_PositionChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPosition ), NULL, this );
	m_PotRateChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPotRatio ), NULL, this );
	m_StackRateChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStackRatio ), NULL, this );
	m_PlayersChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayers ), NULL, this );
	m_PlayersStyleChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnPlayersStyle ), NULL, this );
	m_StyleChangeChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnStyleChanges ), NULL, this );
	m_BotStyleChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStyle ), NULL, this );
	m_BostStackSizeChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnBotStackSize ), NULL, this );
	m_ActionRadio->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( TeacherMainFrame::OnAction ), NULL, this );
	m_NextButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnNext ), NULL, this );
	m_SaveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnSave ), NULL, this );
	m_LoadButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnLoad ), NULL, this );
	m_TeachButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTeach ), NULL, this );
	m_TestButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TeacherMainFrame::OnTest ), NULL, this );
}

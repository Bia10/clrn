///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Form.h"

///////////////////////////////////////////////////////////////////////////

MyFrame3::MyFrame3( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 875,187 ), wxSize( 875,187 ) );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer21;
	gSizer21 = new wxGridSizer( 1, 8, 0, 0 );

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

	bSizer4->Add( gSizer21, 0, wxEXPAND, 5 );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 1, 8, 0, 0 );

	wxString m_choice10Choices[] = { wxT("Very low"), wxT("Low"), wxT("Normal"), wxT("Good"), wxT("Very good"), wxT("Best") };
	int m_choice10NChoices = sizeof( m_choice10Choices ) / sizeof( wxString );
	m_choice10 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice10NChoices, m_choice10Choices, 0 );
	m_choice10->SetSelection( 0 );
	gSizer2->Add( m_choice10, 0, wxALL|wxEXPAND, 5 );

	wxString m_choice11Choices[] = { wxT("Early"), wxT("Middle"), wxT("Later") };
	int m_choice11NChoices = sizeof( m_choice11Choices ) / sizeof( wxString );
	m_choice11 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice11NChoices, m_choice11Choices, 0 );
	m_choice11->SetSelection( 0 );
	gSizer2->Add( m_choice11, 0, wxALL|wxEXPAND, 5 );

	wxString m_choice12Choices[] = { wxT("Very low"), wxT("Low"), wxT("Normal"), wxT("High"), wxT("Huge") };
	int m_choice12NChoices = sizeof( m_choice12Choices ) / sizeof( wxString );
	m_choice12 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice12NChoices, m_choice12Choices, 0 );
	m_choice12->SetSelection( 0 );
	gSizer2->Add( m_choice12, 0, wxALL|wxEXPAND, 5 );

	wxString m_choice121Choices[] = { wxT("Very low"), wxT("Low"), wxT("Normal"), wxT("High"), wxT("Huge") };
	int m_choice121NChoices = sizeof( m_choice121Choices ) / sizeof( wxString );
	m_choice121 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice121NChoices, m_choice121Choices, 0 );
	m_choice121->SetSelection( 0 );
	gSizer2->Add( m_choice121, 0, wxALL|wxEXPAND, 5 );

	wxString m_choice14Choices[] = { wxT("One"), wxT("Two"), wxT("Three or more") };
	int m_choice14NChoices = sizeof( m_choice14Choices ) / sizeof( wxString );
	m_choice14 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice14NChoices, m_choice14Choices, 0 );
	m_choice14->SetSelection( 0 );
	gSizer2->Add( m_choice14, 0, wxALL|wxEXPAND, 5 );

	wxString m_choice15Choices[] = { wxT("Passive"), wxT("Normal"), wxT("Agressive") };
	int m_choice15NChoices = sizeof( m_choice15Choices ) / sizeof( wxString );
	m_choice15 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice15NChoices, m_choice15Choices, 0 );
	m_choice15->SetSelection( 0 );
	gSizer2->Add( m_choice15, 0, wxALL|wxEXPAND, 5 );

	wxString m_choice151Choices[] = { wxT("Passive"), wxT("Normal"), wxT("Agressive") };
	int m_choice151NChoices = sizeof( m_choice151Choices ) / sizeof( wxString );
	m_choice151 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice151NChoices, m_choice151Choices, 0 );
	m_choice151->SetSelection( 0 );
	gSizer2->Add( m_choice151, 0, wxALL|wxEXPAND, 5 );

	wxString m_choice152Choices[] = { wxT("Passive"), wxT("Normal"), wxT("Agressive") };
	int m_choice152NChoices = sizeof( m_choice152Choices ) / sizeof( wxString );
	m_choice152 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice152NChoices, m_choice152Choices, 0 );
	m_choice152->SetSelection( 0 );
	gSizer2->Add( m_choice152, 0, wxALL|wxEXPAND, 5 );

	bSizer4->Add( gSizer2, 0, 0, 5 );

	wxGridSizer* gSizer12;
	gSizer12 = new wxGridSizer( 1, 2, 0, 0 );

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 1, 3, 0, 0 );

	wxString m_radioBox8Choices[] = { wxT("Yes"), wxT("No") };
	int m_radioBox8NChoices = sizeof( m_radioBox8Choices ) / sizeof( wxString );
	m_radioBox8 = new wxRadioBox( this, wxID_ANY, wxT("FOLD"), wxDefaultPosition, wxDefaultSize, m_radioBox8NChoices, m_radioBox8Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox8->SetSelection( 0 );
	gSizer4->Add( m_radioBox8, 0, wxALL|wxEXPAND, 5 );

	wxString m_radioBox81Choices[] = { wxT("Yes"), wxT("No") };
	int m_radioBox81NChoices = sizeof( m_radioBox81Choices ) / sizeof( wxString );
	m_radioBox81 = new wxRadioBox( this, wxID_ANY, wxT("CHECK/CALL"), wxDefaultPosition, wxDefaultSize, m_radioBox81NChoices, m_radioBox81Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox81->SetSelection( 0 );
	gSizer4->Add( m_radioBox81, 0, wxALL|wxEXPAND, 5 );

	wxString m_radioBox82Choices[] = { wxT("Yes"), wxT("No") };
	int m_radioBox82NChoices = sizeof( m_radioBox82Choices ) / sizeof( wxString );
	m_radioBox82 = new wxRadioBox( this, wxID_ANY, wxT("RAISE"), wxDefaultPosition, wxDefaultSize, m_radioBox82NChoices, m_radioBox82Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox82->SetSelection( 0 );
	gSizer4->Add( m_radioBox82, 0, wxALL|wxEXPAND, 5 );

	gSizer12->Add( gSizer4, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 1, 4, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_button10 = new wxButton( this, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_button10, 0, wxALL, 5 );

	m_button12 = new wxButton( this, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_button12, 0, wxALL, 5 );

	m_button13 = new wxButton( this, wxID_ANY, wxT("Load"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_button13, 0, wxALL, 5 );

	m_button14 = new wxButton( this, wxID_ANY, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_button14, 0, wxALL, 5 );

	gSizer12->Add( fgSizer6, 1, wxEXPAND, 5 );

	bSizer4->Add( gSizer12, 0, 0, 5 );

	this->SetSizer( bSizer4 );
	this->Layout();

	// Connect Events
	m_choice10->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnWinRate ), NULL, this );
	m_choice11->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPosition ), NULL, this );
	m_choice12->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPotRatio ), NULL, this );
	m_choice121->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnStackRatio ), NULL, this );
	m_choice14->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPlayers ), NULL, this );
	m_choice15->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPlayersStyle ), NULL, this );
	m_choice151->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnStyleChanges ), NULL, this );
	m_choice152->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnBotStyle ), NULL, this );
	m_radioBox8->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyFrame3::OnFold ), NULL, this );
	m_radioBox81->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyFrame3::OnCheckCall ), NULL, this );
	m_radioBox82->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyFrame3::OnRaise ), NULL, this );
	m_button10->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnSave ), NULL, this );
	m_button12->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnNext ), NULL, this );
	m_button13->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnLoad ), NULL, this );
	m_button14->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnTest ), NULL, this );
}

MyFrame3::~MyFrame3()
{
	// Disconnect Events
	m_choice10->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnWinRate ), NULL, this );
	m_choice11->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPosition ), NULL, this );
	m_choice12->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPotRatio ), NULL, this );
	m_choice121->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnStackRatio ), NULL, this );
	m_choice14->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPlayers ), NULL, this );
	m_choice15->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnPlayersStyle ), NULL, this );
	m_choice151->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnStyleChanges ), NULL, this );
	m_choice152->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MyFrame3::OnBotStyle ), NULL, this );
	m_radioBox8->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyFrame3::OnFold ), NULL, this );
	m_radioBox81->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyFrame3::OnCheckCall ), NULL, this );
	m_radioBox82->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( MyFrame3::OnRaise ), NULL, this );
	m_button10->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnSave ), NULL, this );
	m_button12->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnNext ), NULL, this );
	m_button13->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnLoad ), NULL, this );
	m_button14->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame3::OnTest ), NULL, this );
}

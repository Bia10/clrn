///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __noname__
#define __noname__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame3
///////////////////////////////////////////////////////////////////////////////
class MyFrame3 : public wxFrame 
{
private:

protected:
	wxStaticText* m_staticText23;
	wxStaticText* m_staticText24;
	wxStaticText* m_staticText25;
	wxStaticText* m_staticText26;
	wxStaticText* m_staticText27;
	wxStaticText* m_staticText28;
	wxStaticText* m_staticText29;
	wxStaticText* m_staticText30;
	wxChoice* m_choice10;
	wxChoice* m_choice11;
	wxChoice* m_choice12;
	wxChoice* m_choice121;
	wxChoice* m_choice14;
	wxChoice* m_choice15;
	wxChoice* m_choice151;
	wxChoice* m_choice152;
	wxRadioBox* m_radioBox8;
	wxRadioBox* m_radioBox81;
	wxRadioBox* m_radioBox82;
	wxButton* m_button10;
	wxButton* m_button12;
	wxButton* m_button13;
	wxButton* m_button14;

	// Virtual event handlers, override them in your derived class
	virtual void OnWinRate( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPosition( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPotRatio( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnStackRatio( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPlayers( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPlayersStyle( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnStyleChanges( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnBotStyle( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnFold( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnCheckCall( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnRaise( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnSave( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnNext( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnLoad( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnTest( wxCommandEvent& event ){ event.Skip(); }


public:
	MyFrame3( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 875,187 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	~MyFrame3();

};

#endif //__noname__

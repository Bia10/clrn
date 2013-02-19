///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __Form__
#define __Form__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class TeacherMainFrame
///////////////////////////////////////////////////////////////////////////////
class TeacherMainFrame : public wxFrame 
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
		wxStaticText* m_staticText301;
		wxChoice* m_WinRateChoice;
		wxChoice* m_PositionChoice;
		wxChoice* m_PotRateChoice;
		wxChoice* m_StackRateChoice;
		wxChoice* m_PlayersChoice;
		wxChoice* m_PlayersStyleChoice;
		wxChoice* m_StyleChangeChoice;
		wxChoice* m_BotStyleChoice;
		wxChoice* m_BostStackSizeChoice;
		wxCheckBox* m_WinRateCheck;
		wxCheckBox* m_PositionCheck;
		wxCheckBox* m_PotRatioCheck;
		wxCheckBox* m_StackRatioCheck;
		wxCheckBox* m_PlayersCheck;
		wxCheckBox* m_PlayersStyleCheck;
		wxCheckBox* m_StyleChengeCheck;
		wxCheckBox* m_BotStyleCheck;
		wxCheckBox* m_BotStackCheck;
		wxRadioBox* m_FoldRadio;
		wxRadioBox* m_CheckCallRadio;
		wxRadioBox* m_BetRaiseradio;
		wxButton* m_SaveButton;
		wxButton* m_NextButton;
		wxButton* m_LoadButton;
		wxButton* m_TeachButton;
		wxButton* m_TestButton;
		wxStatusBar* m_StatusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnWinRate( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPosition( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPotRatio( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnStackRatio( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPlayers( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPlayersStyle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnStyleChanges( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBotStyle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBotStackSize( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFold( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckCall( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRaise( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNext( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnLoad( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTeach( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTest( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		TeacherMainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Neuro network teacher"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,239 ), long style = wxCAPTION|wxCLOSE_BOX|wxTAB_TRAVERSAL );
		~TeacherMainFrame();
	
};

#endif //__Form__

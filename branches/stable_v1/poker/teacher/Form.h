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
#include <wx/richtext/richtextctrl.h>
#include <wx/grid.h>
#include <wx/gauge.h>
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
		wxStaticText* m_staticText27;
		wxStaticText* m_staticText28;
		wxStaticText* m_staticText29;
		wxStaticText* m_staticText30;
		wxStaticText* m_staticText301;
		wxChoice* m_WinRateChoice;
		wxChoice* m_PositionChoice;
		wxChoice* m_BetSizeChoice;
		wxChoice* m_PlayersChoice;
		wxChoice* m_DangerChoice;
		wxChoice* m_BotAverageStyleChoice;
		wxChoice* m_BotStyleChoice;
		wxChoice* m_BostStackSizeChoice;
		wxCheckBox* m_WinRateCheck;
		wxCheckBox* m_PositionCheck;
		wxCheckBox* m_BetSizeCheck;
		wxCheckBox* m_PlayersCheck;
		wxCheckBox* m_DangerCheck;
		wxCheckBox* m_BotAverageStyleCheck;
		wxCheckBox* m_BotStyleCheck;
		wxCheckBox* m_BotStackCheck;
		wxRadioBox* m_ActionRadio;
		wxButton* m_NextButton;
		wxButton* m_SaveButton;
		wxButton* m_LoadButton;
		wxButton* m_TeachButton;
		wxButton* m_TestButton;
		wxButton* m_GenerateRange;
		wxButton* m_ReadButton;
		wxButton* m_WriteButton;
		wxButton* m_ExecuteButton;
		wxRichTextCtrl* m_QueryText;
		wxGrid* m_Grid;
		wxGauge* m_Gauge;
		wxStatusBar* m_StatusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnWinRate( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPosition( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPotRatio( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPlayers( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPlayersStyle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnStyleChanges( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBotStyle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBotStackSize( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAction( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNext( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnLoad( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTeach( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTest( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRange( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRead( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnWrite( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExecute( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnGridScroll( wxMouseEvent& event ){ event.Skip(); }
		
	
	public:
		TeacherMainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Neuro network teacher"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1004,510 ), long style = wxCAPTION|wxCLOSE_BOX|wxTAB_TRAVERSAL );
		~TeacherMainFrame();
	
};

#endif //__Form__
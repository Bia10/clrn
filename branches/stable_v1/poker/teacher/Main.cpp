#include "Teacher.h"

#include <wx/wx.h>

class wxMyApp : public wxApp
{
public:
	virtual bool OnInit();
};

IMPLEMENT_APP(wxMyApp)

bool wxMyApp::OnInit()
{
	wxFrame* frame=new tchr::Teacher();
	frame->Show(true);
	return true;
}

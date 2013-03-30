#include "Form.h"
#include "../neuro/Params.h"

namespace tchr
{
class Teacher : public TeacherMainFrame
{
public:
	Teacher();

private:

	virtual void OnWinRate( wxCommandEvent& event ) override;
	virtual void OnPosition( wxCommandEvent& event ) override;
	virtual void OnPotRatio( wxCommandEvent& event ) override;
	virtual void OnStackRatio( wxCommandEvent& event ) override;
	virtual void OnPlayers( wxCommandEvent& event ) override;
	virtual void OnPlayersStyle( wxCommandEvent& event ) override;
	virtual void OnStyleChanges( wxCommandEvent& event ) override;
	virtual void OnBotStyle( wxCommandEvent& event ) override;
	virtual void OnBotStackSize( wxCommandEvent& event ) override;
	virtual void OnAction( wxCommandEvent& event ) override;
	virtual void OnSave( wxCommandEvent& event ) override;
	virtual void OnNext( wxCommandEvent& event ) override;
	virtual void OnLoad( wxCommandEvent& event ) override;
	virtual void OnTeach( wxCommandEvent& event ) override;
	virtual void OnTest( wxCommandEvent& event ) override;
	virtual void OnRange( wxCommandEvent& event ) override;

private:

	void AddParameters();
	void SetGuiParams(const neuro::Params& params);
	void IncrementChecked();
	void LoadParams(neuro::Params::Set& params, const std::string& path);
	void MergeParams(neuro::Params::Set& dst, const neuro::Params::Set& src);
	void RangeThread(int count);
    void TeachThread();

private:

	//! Current parameters
	neuro::Params m_CurrentParams;

	//! Parameters
	neuro::Params::Set m_Parameters;

    //! All selected
    bool m_All;

};
}
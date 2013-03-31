#include "Form.h"
#include "../neuro/Params.h"

#include <boost/date_time/posix_time/ptime.hpp>

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
    virtual void OnGridScroll( wxMouseEvent& event ) override;
    virtual void OnKeyDown( wxKeyEvent& event ) override;

private:

	void AddParameters();
	void SetGuiParams(const neuro::Params& params);
	void IncrementChecked();
	void LoadParams(neuro::Params::List& params, const std::string& path);
	void MergeParams(neuro::Params::List& dst, const neuro::Params::List& src);
	void RangeThread(int count);
    void TeachThread();
    void UpdateGrid(unsigned row);
    void SetPramsValues();
    void SetRowViewToCurrentParams();
    void GridUp(unsigned amount = 1);
    void GridDown(unsigned amount = 1);
    void TrainCallback(unsigned epoch, unsigned epochCount, float error, float desiredError, const boost::posix_time::ptime& start);

private:

	//! Current parameters
	neuro::Params m_CurrentParams;

	//! Parameters
	neuro::Params::List m_Parameters;

    //! Current grid row
    unsigned m_CurrentRow;

    //! All selected
    bool m_All;

};
}
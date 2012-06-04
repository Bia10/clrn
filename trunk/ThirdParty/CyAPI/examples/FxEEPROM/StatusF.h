//---------------------------------------------------------------------------

#ifndef StatusFH
#define StatusFH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TStatusForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *MsgLabel;
private:	// User declarations
public:		// User declarations
        __fastcall TStatusForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TStatusForm *StatusForm;
//---------------------------------------------------------------------------
#endif

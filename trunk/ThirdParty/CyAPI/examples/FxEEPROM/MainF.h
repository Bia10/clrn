//---------------------------------------------------------------------------

#ifndef MainFH
#define MainFH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CyAPI.h"
#include <Buttons.hpp>

#define MAX_CTLXFER_SIZE 0x1000 // 4  KB

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TBitBtn *LgEEBtn;
        TBitBtn *SmEEBtn;
        TSpeedButton *DevPresentBtn;
        TSpeedButton *LEDOffBtn;
        TSpeedButton *LEDOnBtn;
        void __fastcall LgEEBtnClick(TObject *Sender);
        
private:	// User declarations
        bool bArrival;
        String FileSpec;
        String AppDir;

        void __fastcall FindFX2(void);
        void __fastcall WndProc(TMessage &Message);
        bool __fastcall LoadHexToRAM(String fName, bool Low = true);
        void __fastcall GetVendAX(TStringList *sL);
        void __fastcall ResetFX2(UCHAR ucStop);
        int  __fastcall Hex2Bytes(String byteChars, PUCHAR buf);
        String FOpenDialog(String caption, String filter, String initDir, HWND handle);
        bool PerformCtlFileTransfer(void);


public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);

        CCyUSBDevice *FX2Device;
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif

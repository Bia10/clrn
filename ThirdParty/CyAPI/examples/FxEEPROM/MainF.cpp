//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainF.h"
#include "StatusF.h"
#include "dbt.h"


// {DD57F00D-F1D4-4b88-AC34-28D73328F836}
static GUID GUID_ADAPT_DEVICE_REQUEST_NOTIFICATION = {0xdd57f00d, 0xf1d4, 0x4b88, 0xac, 0x34, 0x28, 0xd7, 0x33, 0x28, 0xf8, 0x36};

typedef struct _CUSTOM_EVENT {
    ULONG  Version;
    USHORT MajorFunction;
    USHORT MinorFunction;
} CUSTOM_EVENT, *PCUSTOM_EVENT;


//______________________________________________________________________________
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//______________________________________________________________________________
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
   bArrival  = false;
   AppDir    = ExtractFileDir(Application->ExeName);

   FX2Device = new CCyUSBDevice(Handle);
   FindFX2();
}


//______________________________________________________________________________
//
// This method allows detection of hot-plugged devices
//

void __fastcall TMainForm::WndProc(TMessage &Message)
{
  if (Message.Msg == WM_DEVICECHANGE) {

    // Executes on the DBT_DEVNODECHANGED WParam following the DBT_DEVICEARRIVAL
    // Gives the device time to get ready, otherwise FindListener won't succeed
    // in openning a device.

    if (bArrival) FindFX2();

    bArrival = (Message.WParam == DBT_DEVICEARRIVAL);

    if (Message.WParam == DBT_DEVICEREMOVECOMPLETE) {

      PDEV_BROADCAST_HDR bcastHdr = (PDEV_BROADCAST_HDR) Message.LParam;
      if (bcastHdr->dbch_devicetype == DBT_DEVTYP_HANDLE) {

        PDEV_BROADCAST_HANDLE pDev = (PDEV_BROADCAST_HANDLE) Message.LParam;
        if (pDev->dbch_handle == FX2Device->DeviceHandle()) {
          FX2Device->Close();
          DevPresentBtn->Glyph = LEDOffBtn->Glyph;
          LgEEBtn->Enabled = false;
          SmEEBtn->Enabled = false;
        }

      }
    }


    // The CyUSB.sys driver automatically sends this DBT_CUSTOMEVENT
    // when any of the below 5 major IRP codes are processed.
    if (Message.WParam == DBT_CUSTOMEVENT) {

      PDEV_BROADCAST_HDR bcastHdr = (PDEV_BROADCAST_HDR) Message.LParam;
      if (bcastHdr->dbch_devicetype == DBT_DEVTYP_HANDLE) {

        PDEV_BROADCAST_HANDLE pDev = (PDEV_BROADCAST_HANDLE) Message.LParam;
        if (pDev->dbch_eventguid == GUID_ADAPT_DEVICE_REQUEST_NOTIFICATION) {
          PCUSTOM_EVENT pEvent = (PCUSTOM_EVENT)pDev->dbch_data;

          switch (pEvent->MajorFunction) {
          case 0x00: //IRP_MJ_CREATE
            break;
          case 0x02: //IRP_MJ_CLOSE
            break;
          case 0x16: //IRP_MJ_POWER
            break;
          case 0x17: //IRP_MJ_SYSTEM_CONTROL
            break;
          case 0x1b: //IRP_MJ_PNP
            break;
          }
        }   
      }
    }


  }

  TForm::WndProc(Message);

}

//______________________________________________________________________________
//
// This routine just checks to see if the device has VendorID == 0x04B4
//

void __fastcall TMainForm::FindFX2(void)
{
    // Disable as if no device found
    DevPresentBtn->Glyph = LEDOffBtn->Glyph;
    LgEEBtn->Enabled = false;
    SmEEBtn->Enabled = false;

    int devices = FX2Device->DeviceCount();

    if (devices == 0) return;
    
    int d = 0;
    int VID = 0;

    while ((d < devices) && (VID != 0x04B4)) {
      FX2Device->Open(d);

      if (!FX2Device->IsOpen()) {  // Try a reset
        FX2Device->Reset();
        Sleep(1000);
        FX2Device->Open(d);
      }

      VID = FX2Device->VendorID;
      d++;
    }

    if (FX2Device->IsOpen()) {
      if (VID == 0x04B4) {
        DevPresentBtn->Glyph = LEDOnBtn->Glyph;
        LgEEBtn->Enabled = true;
        SmEEBtn->Enabled = true;

        // All the control transfers use REQ_VENDOR and TGT_DEVICE
        FX2Device->ControlEndPt->ReqType = REQ_VENDOR;
        FX2Device->ControlEndPt->Target = TGT_DEVICE;

      }
    }
}

//______________________________________________________________________________
//
// This routine called when either LgEEBtn or SmEEBtn is clicked
//

void __fastcall TMainForm::LgEEBtnClick(TObject *Sender)
{
  if (!FX2Device->IsOpen()) throw Exception("No FX2 Devices connected.");

  bool bBigEE = (Sender == LgEEBtn);
  String Caption = bBigEE ? "Select file to download into large (512 - 64K byte) EEPROM" : "Select file to download into small (16 - 256 byte) EEPROM";

  FileSpec = FOpenDialog(Caption,"i2c EEPROM Files (*.iic)|*.iic|All Files (*.*)|*.*|",MainForm->AppDir, Handle);
  if (FileSpec.Length() == 0) return;

  StatusForm->Caption = bBigEE ? "Large EEPROM" : "Small EEPROM";
  StatusForm->MsgLabel->Caption = "Downloading " + ExtractFileName(FileSpec) + " . . .";
  StatusForm->Show();
  Refresh();
  StatusForm->Refresh();



  // Download VendAX.hex code
  LoadHexToRAM("VendAX");

  // Set the ReqCode to be used for the EEPROM check and programming
  FX2Device->ControlEndPt->ReqCode = bBigEE ? 0xA9 : 0xA2;

  // See if there's an EEPROM attached
  LONG len = 2;
  WORD wCDCD;
  PUCHAR buf = (PUCHAR) &wCDCD;
  FX2Device->ControlEndPt->Value = 0;
  FX2Device->ControlEndPt->Index = 0;

  FX2Device->ControlEndPt->Read(buf,len);

  if (wCDCD == 0xCDCD) {
    StatusForm->Hide();
    MessageBox(NULL, "EEPROM not enabled.", "Error", MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  // 0xA9 and 0xA2 are also the request codes used in the EEPROM download
  PerformCtlFileTransfer();

  StatusForm->Hide();

}

//______________________________________________________________________________
//
// LoadHexToRAM loads a .hex file into RAM.
//
// If the file to be loaded is larger than 8KB, this method should be called twice:
// first with bLow = false and again with bLow = true.
//
// When bLow = false, all lines of code above the 8KB boundary are loaded.
// When bLow = true, all lines of code below the 8KB boundary are loaded.
//
// This application only calls this method once, passing "VendAX".  But, it can
// be used to download any valid .hex file

bool __fastcall TMainForm::LoadHexToRAM(String fName, bool bLow)
{
  // Load all the text lines from the .hex file into a String List for easy
  // manipulation

  TStringList *sList = new TStringList;

  if (fName.Pos("VendAX")) {
    GetVendAX(sList);
  } else {
    TFileStream *srcStream = new TFileStream(fName,fmOpenRead | fmShareDenyWrite);
    if (srcStream == NULL) return false;
    sList->LoadFromStream(srcStream);
    delete srcStream;
  }


  String tmp;
  int v;

  // Delete non-data records
  for (int i=sList->Count - 1; i>=0; i--) {
    if (sList->Strings[i].Length()) {
      tmp = sList->Strings[i].SubString(8,2);   // Get the Record Type into v
      v = 0;  HexToBin(tmp.c_str(),(char *)&v,1); v *= 2;
      if (v) sList->Delete(i);                  // Data records are type == 0
    }
  }

  // Re-construct the strings to only contain the offset followed by the data
  for (int i=0; i<sList->Count; i++) {
    // Remove comments
    v = sList->Strings[i].Pos("//");
    if (v) sList->Strings[i].SetLength(v-1);

    // Build string that just contains the offset followed by the data bytes
    if (sList->Strings[i].Length()) {
      // Get the offset
      String sOffset = sList->Strings[i].SubString(4,4);

      // Get the string of data chars
      tmp = sList->Strings[i].SubString(2,2);
      v = 0;  HexToBin(tmp.c_str(),(char *)&v,1); v *= 2;
      String s = sList->Strings[i].SubString(10,v);

      // Replace the string in the list
      sList->Strings[i] = sOffset + s;
    }

  }

  if (bLow) ResetFX2(1);      // Stop the processor

  FX2Device->ControlEndPt->ReqCode = bLow ? 0xA0 : 0xA3;
  FX2Device->ControlEndPt->Index = 0;
  FX2Device->ControlEndPt->Value = 0;

  // Go through the list, loading data into RAM
  String DataString = "";
  WORD nxtoffset = 0;
  LONG xferLen = 0;

  WORD offset;
  int RamSize = 0x2000;  // 8KB

  UCHAR buf[MAX_CTLXFER_SIZE];

  char  c;
  PCHAR pc;

  for (int i=0; i<sList->Count; i++) {
    // Get the offset
    tmp = sList->Strings[i].SubString(1,4);
    HexToBin(tmp.c_str(),(char *)&offset,2);
    pc = (char *) &offset; c = pc[0]; pc[0]=pc[1]; pc[1]=c;  // Swap the bytes

    int sLen = sList->Strings[i].Length();

    // Handle a line that straddles the 8KB boundary
    int bytes = (sLen - 4)/2;
    LONG lastAddr = offset + bytes;

    // This case is the last segment to be sent to low memory
    if (bLow && (offset < RamSize) && (lastAddr > RamSize))
      bytes = RamSize - offset;

    // In this case, we found the first segment to be sent to the high memory
    if (!bLow && (offset < RamSize) && (lastAddr > RamSize)) {
      bytes = lastAddr - RamSize;
      String s = "xxxx"+sList->Strings[i].SubString(sLen - (bytes*2)+1,bytes*2);
      sList->Strings[i] = s;
      offset = RamSize;
    }

    if ((bLow &&  (offset < RamSize)) || // Below 8KB - internal RAM
       (!bLow && (offset >= RamSize)) ) {

      xferLen += bytes;

      if ((offset == nxtoffset) && (xferLen < MAX_CTLXFER_SIZE)) {
        DataString += sList->Strings[i].SubString(5,bytes*2);
      } else {
        LONG len = DataString.Length() / 2;
        if (len) {
          Hex2Bytes(DataString,buf);
          FX2Device->ControlEndPt->Write(buf, len);
        }

        FX2Device->ControlEndPt->Value = offset;  // The destination address

        DataString = sList->Strings[i].SubString(5,bytes*2);

        xferLen = bytes;

      }

      nxtoffset = offset + bytes;  // Where next contiguous data would sit
    }
  }

  // Send the last segment of bytes
  LONG len = DataString.Length() / 2;
  if (len) {
     Hex2Bytes(DataString,buf);
     FX2Device->ControlEndPt->Write(buf, len);
  }

  if (bLow) ResetFX2(0);      // Start running this new code

  return true;

}

//______________________________________________________________________________


bool TMainForm::PerformCtlFileTransfer(void)
{
   PCHAR buf;
   TFileStream *fStream;
   bool success;

   LONG bufLen;
   String tmp;

   fStream = new TFileStream(FileSpec,fmOpenRead | fmShareDenyWrite);

   if (fStream) {
     LONG fSize = fStream->Size;

     FX2Device->ControlEndPt->TimeOut = 25000;
     FX2Device->ControlEndPt->Index = 0;

     if (fSize <= MAX_CTLXFER_SIZE) {   // Small file

       FX2Device->ControlEndPt->Value = 0;  // The destination address

       buf = new UCHAR[fSize];
       fStream->Read(buf, fSize);
       success = FX2Device->ControlEndPt->Write(buf, fSize);

       if (!success)
         Application->MessageBox("File transfer failed.","Write to Device");

       delete[] buf;

     } else {                           // Big file
       LONG bufLen;

       buf = new UCHAR[MAX_CTLXFER_SIZE];

       success = true;
       do {
         FX2Device->ControlEndPt->Value = fStream->Position;  // Dest address

         bufLen = fSize - fStream->Position;
         if (bufLen > MAX_CTLXFER_SIZE) bufLen = MAX_CTLXFER_SIZE;

         fStream->Read(buf, bufLen);
         success &= FX2Device->ControlEndPt->Write(buf, bufLen);

       } while (success && (fStream->Position < fSize));

       if (!success)
         Application->MessageBox("File transfer failed.","Write to Device");

       delete[] buf;

     } // if (fSize <= 4096)

     FX2Device->ControlEndPt->TimeOut = 5000;
     delete fStream;

   } else // if (fStream)
     Application->MessageBox("Could not read from file.", "File Xfer Error");


   return success;

}

//______________________________________________________________________________
//
// ucStop = 1 = Reset and Hold
// ucStop = 0 = Reset and Run
//

void __fastcall TMainForm::ResetFX2(UCHAR ucStop)
{
  if (!FX2Device->IsOpen()) throw Exception("No USB Devices connected.");

  FX2Device->ControlEndPt->ReqCode = 0xA0;
  FX2Device->ControlEndPt->Value = 0xE600;
  FX2Device->ControlEndPt->Index = 0;

  long len = 1;
  FX2Device->ControlEndPt->Write(&ucStop,len);

}

//______________________________________________________________________________

int __fastcall TMainForm::Hex2Bytes(String byteChars, PUCHAR buf) {

  // Remove non-hex chars
  String tmpStr = byteChars.UpperCase().Trim();
  int bytes = tmpStr.Length() / 2;

  // Stuff the output buffer with the byte values
  if (bytes)
    for (int i=0; i<bytes; i++) {
      String s = tmpStr.SubString(1+ i*2, 2);
      HexToBin(s.c_str(), &buf[i], 1);
    }

  return bytes;
}

//______________________________________________________________________________


String TMainForm::FOpenDialog(String caption, String filter, String initDir, HWND handle)
{
  OPENFILENAME ofName;
  ZeroMemory(&ofName,sizeof(OPENFILENAME));

  char fileName[256];
  ZeroMemory(fileName,256);

  int len = filter.Length();
  char *filters = filter.c_str();
  for (int i=0; i<len; i++)
    if (filters[i] == '|') filters[i] = 0;

  ofName.lStructSize = sizeof(OPENFILENAME);
  ofName.hwndOwner = handle;
  ofName.lpstrFilter = filters;
  ofName.nFilterIndex = 0;
  ofName.lpstrFile = fileName;
  ofName.nMaxFile = 256;
  //ofName.lpstrFileTitle = NULL;
  //ofName.nMaxFileTitle = 0;
  //ofName.lpstrInitialDir = initDir.c_str();
  ofName.lpstrTitle = caption.c_str();
  ofName.Flags = OFN_EXPLORER;

  //ofName.nFileOffset = 0;
  //ofName.nFileExtension = 0;
  //ofName.lpstrDefExt = NULL;

  if (GetOpenFileName(&ofName))
    return String(ofName.lpstrFile);
  else
    return "";


}

//______________________________________________________________________________
//
// This loads the VendAX.hex file into the string list, *sL
// VendAX.hex is hard-coded, below.
//

void __fastcall TMainForm::GetVendAX(TStringList *sL) {

  // This is the VendAX.hex file, version 3.  Put it in the string list
  sL->Add(":0A0DA8000001020203030404050524");
	sL->Add(":1005E600E4F52CF52BF52AF529C203C200C202C296");
	sL->Add(":1005F60001120DCA7E0A7F008E0A8F0B75120A75CC");
	sL->Add(":10060600131275080A75091C75100A75114A7514B6");
	sL->Add(":100616000A751578EE54C070030206EB752D007549");
	sL->Add(":100626002E808E2F8F30C3749A9FFF740A9ECF241C");
	sL->Add(":1006360002CF3400FEE48F288E27F526F525F52413");
	sL->Add(":10064600F523F522F521AF28AE27AD26AC25AB2440");
	sL->Add(":10065600AA23A922A821C312089D5037E5302524D4");
	sL->Add(":10066600F582E52F3523F583E0FFE52E2524F58277");
	sL->Add(":10067600E52D3523F583EFF0E4FAF9F8E5242401B6");
	sL->Add(":10068600F524EA3523F523E93522F522E83521F567");
	sL->Add(":100696002180B3852D0A852E0B74002480FF740AF1");
	sL->Add(":1006A60034FFFEC3E5139FF513E5129EF512C3E56D");
	sL->Add(":1006B6000D9FF50DE50C9EF50CC3E50F9FF50FE5B7");
	sL->Add(":1006C6000E9EF50EC3E5099FF509E5089EF508C3DC");
	sL->Add(":1006D600E5119FF511E5109EF510C3E5159FF5157B");
	sL->Add(":1006E600E5149EF514D2E843D82090E668E0440964");
	sL->Add(":1006F600F090E65CE0443DF0D2AF90E680E054F73F");
	sL->Add(":10070600F0538EF8C2031208FF300105120080C2B2");
	sL->Add(":10071600013003F21208FD50EDC203120CAD2000A9");
	sL->Add(":100726001690E682E030E704E020E1EF90E682E012");
	sL->Add(":0F07360030E604E020E0E4120C561209FE80C702");
	sL->Add(":010745002291");
	sL->Add(":0B0D9D0090E50DE030E402C322D322F9");
	sL->Add(":1000800090E6B9E0700302016B14700302021424BD");
	sL->Add(":10009000FE70030202AA24FB7003020165147003C0");
	sL->Add(":1000A00002015F147003020153147003020159240A");
	sL->Add(":1000B00005600302031F120DCE400302032B90E6DE");
	sL->Add(":1000C000BBE024FE603B14605624FD6016146040C3");
	sL->Add(":1000D00024067075E50A90E6B3F0E50B90E6B4F0FF");
	sL->Add(":1000E00002032B120D9D500FE51290E6B3F0E513BD");
	sL->Add(":1000F00090E6B4F002032B90E6A0E04401F0020386");
	sL->Add(":100100002BE50C90E6B3F0E50D90E6B4F002032B7E");
	sL->Add(":10011000E50E90E6B3F0E50F90E6B4F002032B9005");
	sL->Add(":10012000E6BAE0FF120C82AA06A9077B01EA494B56");
	sL->Add(":10013000600DEE90E6B3F0EF90E6B4F002032B9082");
	sL->Add(":10014000E6A0E04401F002032B90E6A0E04401F0B9");
	sL->Add(":1001500002032B120D7F02032B120DBA02032B1286");
	sL->Add(":100160000DB202032B120D6D02032B120DD04003B2");
	sL->Add(":1001700002032B90E6B8E0247F602B14603C24023D");
	sL->Add(":10018000600302020AA200E433FF25E0FFA202E4BA");
	sL->Add(":10019000334F90E740F0E4A3F090E68AF090E68BCE");
	sL->Add(":1001A0007402F002032BE490E740F0A3F090E68A9B");
	sL->Add(":1001B000F090E68B7402F002032B90E6BCE0547ED4");
	sL->Add(":1001C000FF7E00E0D3948040067C007D0180047CAB");
	sL->Add(":1001D000007D00EC4EFEED4F24A8F582740D3EF537");
	sL->Add(":1001E00083E493FF3395E0FEEF24A1FFEE34E68F26");
	sL->Add(":1001F00082F583E0540190E740F0E4A3F090E68AB2");
	sL->Add(":10020000F090E68B7402F002032B90E6A0E044012C");
	sL->Add(":10021000F002032B120DD2400302032B90E6B8E04C");
	sL->Add(":1002200024FE601D2402600302032B90E6BAE0B4B2");
	sL->Add(":100230000105C20002032B90E6A0E04401F0020396");
	sL->Add(":100240002B90E6BAE0705990E6BCE0547EFF7E0049");
	sL->Add(":10025000E0D3948040067C007D0180047C007D001A");
	sL->Add(":10026000EC4EFEED4F24A8F582740D3EF583E49329");
	sL->Add(":10027000FF3395E0FEEF24A1FFEE34E68F82F58395");
	sL->Add(":10028000E054FEF090E6BCE05480FF131313541FBB");
	sL->Add(":10029000FFE0540F2F90E683F0E04420F002032BA0");
	sL->Add(":1002A00090E6A0E04401F002032B120DD4507C90A4");
	sL->Add(":1002B000E6B8E024FE60202402705B90E6BAE0B469");
	sL->Add(":1002C0000104D200806590E6BAE06402605D90E6C9");
	sL->Add(":1002D000A0E04401F0805490E6BCE0547EFF7E0034");
	sL->Add(":1002E000E0D3948040067C007D0180047C007D008A");
	sL->Add(":1002F000EC4EFEED4F24A8F582740D3EF583E49399");
	sL->Add(":10030000FF3395E0FEEF24A1FFEE34E68F82F58304");
	sL->Add(":10031000E04401F0801590E6A0E04401F0800C126A");
	sL->Add(":100320000333500790E6A0E04401F090E6A0E044DB");
	sL->Add(":0203300080F05B");
	sL->Add(":0103320022A8");
	sL->Add(":030033000208F9C7");
	sL->Add(":0408F90053D8EF32AF");
	sL->Add(":040DCA00120DC22222");
	sL->Add(":0108FF0022D6");
	sL->Add(":0208FD00D32204");
	sL->Add(":0209FE00D32202");
	sL->Add(":020DCE00D3222E");
	sL->Add(":080DB20090E6BAE0F51ED32221");
	sL->Add(":100D6D0090E740E51EF0E490E68AF090E68B04F003");
	sL->Add(":020D7D00D3227F");
	sL->Add(":080DBA0090E6BAE0F51CD3221B");
	sL->Add(":100D7F0090E740E51CF0E490E68AF090E68B04F0F3");
	sL->Add(":020D8F00D3226D");
	sL->Add(":020DD000D3222C");
	sL->Add(":020DD200D3222A");
	sL->Add(":020DD400D32228");
	sL->Add(":1003330090E678E05410FFC4540F4450F51D13E4C5");
	sL->Add(":1003430033F51F90E6B9E0245EB40B0040030205C9");
	sL->Add(":10035300E490036275F003A4C58325F0C58373029B");
	sL->Add(":10036300045C02045C0203D702038302039D0203BD");
	sL->Add(":10037300BD0204060204560203E00203F302042C46");
	sL->Add(":1003830090E740E51FF0E490E68AF090E68B04F0F6");
	sL->Add(":1003930090E6A0E04480F00205E490E600E0FFC4AC");
	sL->Add(":1003A300540F90E740F0E490E68AF090E68B04F077");
	sL->Add(":1003B30090E6A0E04480F00205E490E740740FF07B");
	sL->Add(":1003C300E490E68AF090E68B04F090E6A0E04480A7");
	sL->Add(":1003D300F00205E490E6BAE0F51D0205E490E67A42");
	sL->Add(":1003E300E054FEF0E490E68AF090E68BF00205E438");
	sL->Add(":1003F30090E67AE04401F0E490E68AF090E68BF030");
	sL->Add(":100403000205E490E7407407F0E490E68AF090E692");
	sL->Add(":100413008B04F090E6A0E04480F07FE87E031209AD");
	sL->Add(":10042300B8D204120C270205E490E6B5E054FEF0BE");
	sL->Add(":1004330090E6BFE090E68AF090E6BEE090E68BF0AF");
	sL->Add(":1004430090E6BBE090E6B3F090E6BAE090E6B4F055");
	sL->Add(":100453000205E4751F01431D0190E6BAE075310002");
	sL->Add(":10046300F532A3E0FEE4EE423190E6BEE0753300E0");
	sL->Add(":10047300F534A3E0FEE4EE423390E6B8E064C060F6");
	sL->Add(":100483000302054CE534453370030205E490E6A00E");
	sL->Add(":10049300E020E1F9C3E5349440E533940050088546");
	sL->Add(":1004A3003335853436800675350075364090E6B9A8");
	sL->Add(":1004B300E0B4A335E4F537F538C3E5389536E537C9");
	sL->Add(":1004C30095355060E5322538F582E5313537F583CA");
	sL->Add(":1004D300E0FF74402538F582E434E7F583EFF00557");
	sL->Add(":1004E30038E5387002053780D0E4F537F538C3E5D1");
	sL->Add(":1004F300389536E5379535501874402538F582E43C");
	sL->Add(":1005030034E7F58374CDF00538E53870020537809C");
	sL->Add(":10051300DDAD367AE779407EE77F40AB07AF32AE99");
	sL->Add(":10052300311208AEE490E68AF090E68BE536F025CA");
	sL->Add(":1005330032F532E5353531F531C3E5349536F534E9");
	sL->Add(":10054300E5339535F53302048790E6B8E0644060FF");
	sL->Add(":10055300030205E4E534453370030205E4E490E661");
	sL->Add(":100563008AF090E68BF090E6A0E020E1F990E68B2C");
	sL->Add(":10057300E0753500F53690E6B9E0B4A335E4F53718");
	sL->Add(":10058300F538C3E5389536E5379535503874402549");
	sL->Add(":1005930038F582E434E7F583E0FFE5322538F58268");
	sL->Add(":1005A300E5313537F583EFF00538E5387002053767");
	sL->Add(":1005B30080D0AD367AE779407EE77F40AB07AF3234");
	sL->Add(":1005C300AE31120B8CE5362532F532E5353531F592");
	sL->Add(":1005D30031C3E5349536F534E5339535F533020506");
	sL->Add(":0205E30057C3FC");
	sL->Add(":0105E50022F3");
	sL->Add(":100CD200C0E0C083C08290E6B5E04401F0D2015387");
	sL->Add(":0F0CE20091EF90E65D7401F0D082D083D0E032C4");
	sL->Add(":100D4100C0E0C083C0825391EF90E65D7404F0D09F");
	sL->Add(":060D510082D083D0E032E5");
	sL->Add(":100D5700C0E0C083C0825391EF90E65D7402F0D08B");
	sL->Add(":060D670082D083D0E032CF");
	sL->Add(":100B1E00C0E0C083C08290E680E030E70E85080C0E");
	sL->Add(":100B2E0085090D85100E85110F800C85100C851111");
	sL->Add(":100B3E000D85080E85090F5391EF90E65D7410F048");
	sL->Add(":070B4E00D082D083D0E03219");
	sL->Add(":100D2900C0E0C083C082D2035391EF90E65D74089E");
	sL->Add(":080D3900F0D082D083D0E0323B");
	sL->Add(":100B5500C0E0C083C08290E680E030E70E85080CD7");
	sL->Add(":100B650085090D85100E85110F800C85100C8511DA");
	sL->Add(":100B75000D85080E85090F5391EF90E65D7420F001");
	sL->Add(":070B8500D082D083D0E032E2");
	sL->Add(":010DD60032EA");
	sL->Add(":010DD70032E9");
	sL->Add(":010DD80032E8");
	sL->Add(":010DD90032E7");
	sL->Add(":010DDA0032E6");
	sL->Add(":010DDB0032E5");
	sL->Add(":010DDC0032E4");
	sL->Add(":010DDD0032E3");
	sL->Add(":010DDE0032E2");
	sL->Add(":010DDF0032E1");
	sL->Add(":010DE00032E0");
	sL->Add(":010DE10032DF");
	sL->Add(":010DE20032DE");
	sL->Add(":010DE30032DD");
	sL->Add(":010DE40032DC");
	sL->Add(":010DE50032DB");
	sL->Add(":010DE60032DA");
	sL->Add(":010DE70032D9");
	sL->Add(":010DE80032D8");
	sL->Add(":010DE90032D7");
	sL->Add(":010DEA0032D6");
	sL->Add(":010DEB0032D5");
	sL->Add(":010DEC0032D4");
	sL->Add(":010DED0032D3");
	sL->Add(":010DEE0032D2");
	sL->Add(":010DEF0032D1");
	sL->Add(":010DF00032D0");
	sL->Add(":010DF10032CF");
	sL->Add(":010DF20032CE");
	sL->Add(":010DF30032CD");
	sL->Add(":010DF40032CC");
	sL->Add(":010DF50032CB");
	sL->Add(":010DF60032CA");
	sL->Add(":010DF70032C9");
	sL->Add(":010DF80032C8");
	sL->Add(":010DF90032C7");
	sL->Add(":060A9C00AB07AA06AC0541");
	sL->Add(":100AA200E4FDE51F6010EA7E000DEE2400F582E40D");
	sL->Add(":100AB2003410F583EAF0EBAE050D74002EF582E4F6");
	sL->Add(":100AC2003410F583EBF0AF050D74002FF582E4349A");
	sL->Add(":100AD20010F583ECF07A107B00AF1D120D0DAF1DE7");
	sL->Add(":030AE200120AE60F");
	sL->Add(":010AE50022EE");
	sL->Add(":0A0B8C008E398F3A8D3B8A3C8B3D79");
	sL->Add(":100B9600E4F53EE53EC3953B5020053AE53AAE39CD");
	sL->Add(":100BA6007002053914FFE53D253EF582E4353CF536");
	sL->Add(":0A0BB60083E0FD120A9C053E80D981");
	sL->Add(":010BC0002212");
	sL->Add(":0A08AE008E398F3A8D3B8A3C8B3D5A");
	sL->Add(":1008B800E4FDF53EE51F6012E539FF7E000DEE24EC");
	sL->Add(":1008C80000F582E43410F583EFF0E53AAE050D74D7");
	sL->Add(":1008D800002EF582E43410F583E53AF07A107B00B7");
	sL->Add(":1008E800AF1D120D0DAB3DAA3CAD3BAF1D120CF177");
	sL->Add(":0108F80022DD");
	sL->Add(":100A00001201000200000040B404041000000102C2");
	sL->Add(":100A100000010A06000200000040010009022E0049");
	sL->Add(":100A200001010080320904000004FF0000000705F6");
	sL->Add(":100A30000202000200070504020002000705860208");
	sL->Add(":100A40000002000705880200020009022E000101D1");
	sL->Add(":100A50000080320904000004FF00000007050202C4");
	sL->Add(":100A60004000000705040240000007058602400020");
	sL->Add(":100A70000007058802400000040309041003430036");
	sL->Add(":100A80007900700072006500730073000E0345006A");
	sL->Add(":0C0A90005A002D005500530042000000E9");
	sL->Add(":100C560090E682E030E004E020E60B90E682E030A9");
	sL->Add(":100C6600E119E030E71590E680E04401F07F147E5C");
	sL->Add(":0C0C7600001209B890E680E054FEF02265");
	sL->Add(":100C270030040990E680E0440AF0800790E680E00F");
	sL->Add(":100C37004408F07FDC7E051209B890E65D74FFF08A");
	sL->Add(":0F0C470090E65FF05391EF90E680E054F7F022D3");
	sL->Add(":1009B8008E398F3A90E600E054187012E53A240117");
	sL->Add(":1009C800FFE43539C313F539EF13F53A801590E68E");
	sL->Add(":1009D80000E05418FFBF100BE53A25E0F53AE53979");
	sL->Add(":1009E80033F539E53A153AAE39700215394E6005D6");
	sL->Add(":0609F800120CC180EE228A");
	sL->Add(":080DC200E4F51BD2E9D2AF22D7");
	sL->Add(":100AE600AD0790E678E020E6F9C2E990E678E044C2");
	sL->Add(":100AF60080F0ED25E090E679F090E678E030E0F9D8");
	sL->Add(":100B060090E678E04440F090E678E020E6F990E65A");
	sL->Add(":080B160078E030E1D6D2E922BB");
	sL->Add(":100BF500AC0790E678E020E6F9E51B702390E678EF");
	sL->Add(":100C0500E04480F0EC25E090E679F08D16AF03A97D");
	sL->Add(":100C1500077517018A188919E4F51A751B01D3227E");
	sL->Add(":020C2500C322E8");
	sL->Add(":100BC100AC0790E678E020E6F9E51B702590E67821");
	sL->Add(":100BD100E04480F0EC25E0440190E679F08D16AF19");
	sL->Add(":100BE10003A9077517018A188919E4F51A751B03FA");
	sL->Add(":040BF100D322C32226");
	sL->Add(":03004B0002074663");
	sL->Add(":10074600C0E0C083C082C085C084C086758600C0F4");
	sL->Add(":10075600D075D000C000C001C002C003C006C007EB");
	sL->Add(":1007660090E678E030E206751B0602083090E678DF");
	sL->Add(":10077600E020E10CE51B64026006751B07020830E9");
	sL->Add(":10078600E51B24FE605F14603624FE700302082118");
	sL->Add(":1007960024FC700302082D24086003020830AB17FE");
	sL->Add(":1007A600AA18A919AF1A051A8F8275830012084E66");
	sL->Add(":1007B60090E679F0E51A65167070751B05806B90EA");
	sL->Add(":1007C600E679E0AB17AA18A919AE1A8E82758300CE");
	sL->Add(":1007D60012087B751B02E5166401704E90E678E000");
	sL->Add(":1007E6004420F08045E51624FEB51A0790E678E029");
	sL->Add(":1007F6004420F0E51614B51A0A90E678E04440F075");
	sL->Add(":10080600751B0090E679E0AB17AA18A919AE1A8EE7");
	sL->Add(":100816008275830012087B051A800F90E678E04403");
	sL->Add(":1008260040F0751B008003751B005391DFD007D085");
	sL->Add(":1008360006D003D002D001D000D0D0D086D084D04C");
	sL->Add(":0808460085D082D083D0E0329E");
	sL->Add(":020C8200A907C0");
	sL->Add(":100C8400AE14AF158F828E83A3E064037017AD0199");
	sL->Add(":100C940019ED7001228F828E83E07C002FFDEC3EE3");
	sL->Add(":080CA400FEAF0580DFE4FEFF56");
	sL->Add(":010CAC002225");
	sL->Add(":100CF100120BC1E51B24FA600E146006240770F381");
	sL->Add(":0C0D0100D322E4F51BD322E4F51BD3221F");
	sL->Add(":100D0D00120BF5E51B24FA600E146006240770F330");
	sL->Add(":0C0D1D00D322E4F51BD322E4F51BD32203");
	sL->Add(":100CAD0090E682E044C0F090E681F04387010000B9");
	sL->Add(":040CBD000000002211");
	sL->Add(":100CC1007400F58690FDA57C05A3E582458370F946");
	sL->Add(":010CD1002200");
	sL->Add(":03004300020900AF");
	sL->Add(":030053000209009F");
	sL->Add(":10090000020CD200020D5700020D4100020D290019");
	sL->Add(":10091000020B1E00020B5500020DD600020DD7007F");
	sL->Add(":10092000020DD800020DD900020DDA00020DDB0025");
	sL->Add(":10093000020DDC00020DDD00020DDE00020DDF0005");
	sL->Add(":10094000020DE000020DD700020DE100020DE200F1");
	sL->Add(":10095000020DE300020DE400020DE500020DE600C9");
	sL->Add(":10096000020DE700020DD700020DD700020DD700DF");
	sL->Add(":10097000020DE800020DE900020DEA00020DEB0095");
	sL->Add(":10098000020DEC00020DED00020DEE00020DEF0075");
	sL->Add(":10099000020DF000020DF100020DF200020DF30055");
	sL->Add(":1009A000020DF400020DF500020DF600020DF70035");
	sL->Add(":0809B000020DF800020DF90030");
	sL->Add(":03000000020D915D");
	sL->Add(":0C0D9100787FE4F6D8FD75813E0205E68F");
	sL->Add(":10084E00BB010CE58229F582E5833AF583E022505F");
	sL->Add(":10085E0006E92582F8E622BBFE06E92582F8E222A9");
	sL->Add(":0D086E00E58229F582E5833AF583E49322C3");
	sL->Add(":10087B00F8BB010DE58229F582E5833AF583E8F0B3");
	sL->Add(":10088B00225006E92582C8F622BBFE05E92582C85F");
	sL->Add(":02089B00F22247");
	sL->Add(":10089D00EB9FF5F0EA9E42F0E99D42F0E89C45F0B1");
	sL->Add(":0108AD002228");
	sL->Add(":00000001FF");

}






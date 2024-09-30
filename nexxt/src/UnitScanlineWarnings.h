//---------------------------------------------------------------------------

#ifndef UnitScanlineWarningsH
#define UnitScanlineWarningsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormScanlineWarnings : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TSpeedButton *btnCyan;
	TLabel *Label1;
	TSpeedButton *btnRed;
	TSpeedButton *btnYellow;
	TSpeedButton *btnOrange;
	TUpDown *UpDown1;
	void __fastcall btnYellowMouseEnter(TObject *Sender);
	void __fastcall btnOrangeMouseEnter(TObject *Sender);
	void __fastcall btnRedMouseEnter(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall UpDown1Changing(TObject *Sender, bool &AllowChange);
	void __fastcall btnCyanMouseEnter(TObject *Sender);
	void __fastcall btnCyanClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFormScanlineWarnings(TComponent* Owner);
    void __fastcall RunUpdate(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormScanlineWarnings *FormScanlineWarnings;
//---------------------------------------------------------------------------
#endif

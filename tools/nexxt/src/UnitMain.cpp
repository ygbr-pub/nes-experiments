//------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <dir.h>

#include <Clipbrd.hpp>
#include <windows.h>
#include <vector>
#pragma hdrstop

#include "UnitMain.h"
#include "UnitCHREditor.h"
#include "UnitSwapColors.h"
#include "UnitNametableOffset.h"
#include "UnitNESBank.h"
#include "UnitSetSize.h"
#include "UnitName.h"
#include "UnitManageMetasprites.h"
#include "UnitInputNumber.h"
#include "UnitMetaspriteOffset.h"
#include "UnitPreferences.h"
#include "UnitBrush.h"
#include "About.h"
#include "UnitNavigator.h"
#include "UnitNavThread.h"
#include "UnitImportBitmap.h"
#include "UnitLossyDetails.h"
#include "UnitSwapAttributes.h"
#include "UnitLineDetails.h"
#include "UnitBankCHR.h"
#include "UnitCHRbit.h"
#include "UnitPropertyConditions.h"
#include "UnitSwapPatternColour.h"
#include "UnitMetatileEditor.h"
#include "UnitMTprops.h"
#include "UnitSelect2subpalSets.h"
#include "UnitEmphasisPalette.h"
#include "UnitAttributeCheckerOptions.h"
#include "UnitPipelineHelper.h"
#include "UnitPropID.h"
#include "UnitScanlineWarnings.h"
#include "UnitBucketToolbox.h"
#include "UnitSwapBanks.h"
#include "UnitSpecialPasteCHR.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;


#include "palette.h"
#include "smallnums.h"


CRITICAL_SECTION CriticalSection;

AnsiString strStat;
AnsiString tmpStat;

//patterns overlaid when using attribute checker.
Graphics::TBitmap *BufCheckerStripes;  //8x8px pattern
Graphics::TBitmap *BufCheckerClassic;  //4x4px pattern, 1 col per pix

Graphics::TBitmap *BufBmpName;
Graphics::TBitmap *BufBmpTiles;
extern Graphics::TBitmap *bufBmp;


bool openByFileDone=false;
bool bProcessDrawNavOn=false;
bool bLinkedCHRmode=false;
bool bIgnoreCheckBoxOnClick_LinkedCHR=false;

int iGlobalAlpha;
int iRadioOpenSave;
int iShowFilepath;
int iRotateCheckerSeparation;
bool bSnapToScreen;
int iPropIDHoveredItem=0;
int  iWarnMsprCyan=2;
bool bWarnMsprCyan;
bool bWarnMsprYellow;
bool bWarnMsprOrange;
bool bWarnMsprRed;

bool bImportCHRWrap;
bool bImportCHRCarry;
bool bImportCHRSkip;

bool bNavDoSnap=true;
bool bNavDoAlign=true;
bool bNavDoZoomchange=false;


bool bFillFirstIteration=false;

bool bExportPalFilename;
bool bExportPalSet;

bool bNotify_linkedmodewarning=true;

//these are used specifically for when opening by file association.
//they buffer when reading file, but aren't used elseplace.
bool bMT_Reserve1st;
bool bMT_AlignScr;
bool bMT_UseAttr;
bool bMT_Clonestamp;
bool bMT_CloneSnap;
bool bMT_Tiles;
bool bMT_Attr;
bool bMT_Props;
bool bMT_Map;

bool bBS_InclLabel;
bool bBS_InclProps;

//--
bool bKeyEscape;
bool bIgnoreKey=true;
bool bDrawMarqSwitch=false;

AnsiString ansiFileNameAssociatedMetatiles;
AnsiString ansiFileNameAssociatedTileset;
bool bAssociateMetatiles=false;
bool bAssociateTileset=false;

AnsiString dialogTitle_OpenMetatiles;

 int mxDown=0;  //metaset mouse down
int myDown=0;


int basePalette[64];
int fullPaletteGenerated[64*8];  //specific to the full palette generator.
int outPalette[64];
int attrCheckerSpecialPalette[64];
int ppuMask;

int ppuMaskSet[4];
unsigned char palBuf[4*16];
unsigned char palImportBuf[4*16]; //used by bmp import specifically.
unsigned char bgPal[4*16];
unsigned char bgPalUndo[4*16];
unsigned char bgPalCheckpoint[4*16];
unsigned char bgPalUndoCheckPoint[4*16];

bool bSmudge=0;
int iSmudge=0;
int bgPalCur;
int bgPalCurOld;
int palActive;
int palActiveOld;
int mtPropsActive;


//line tool presets
int iLinePresetIndex=0;

bool bLinePreset_styleDots[4]   		={false,false,false,false};
bool bLinePreset_styleDashes[4] 		={false,false,false,false};
bool iLinePreset_styleRepeat[4] 		={1,1,1,1};

bool bLinePreset_taperIn[4]				={false,false,false,false};
bool bLinePreset_taperOut[4]			={false,false,false,false};
bool bLinePreset_taperFromMid[4]		={false,false,false,false};
bool bLinePreset_taperEnabled[4]		={true, true, true, true};


bool bLinePreset_modeQuick[4]			={false,false,false,false};
bool bLinePreset_modeCoat[4]			={false,false,false,false};

bool bLinePreset_rosterReset[4]			={false,false,false,false};
bool bLinePreset_rosterAutosize[4]		={false,false,false,false};

bool bLinePreset_rosterEnableLine[4]		={true, true, true, true};
bool bLinePreset_rosterEnableCurve[4]		={true, true, true, true};
bool bLinePreset_rosterEnableKnee[4]		={true, true, true, true};
bool bLinePreset_rosterEnableRight[4]		={true, true, true, true};

bool bLinePreset_rosterEnableRect[4]		={true, true, true, true};
bool bLinePreset_rosterEnableEllipse[4]		={true, true, true, true};

bool bLinePreset_rosterEnableHyperbola[4]	={true, true, true, true};
bool bLinePreset_rosterEnableHyperline[4]	={true, true, true, true};
bool bLinePreset_rosterEnableHypercave[4]	={true, true, true, true};
int iLinePreset_rosterIndex[4]				={0, 0, 0, 0};

bool bLinePreset_adjustAutoreset[4]			={true, true, true, true};

//pipeline helper setup bools
bool bAutoExport_metatiles=false;
bool bAutoExportRadio1=false;
bool bAutoExportRadio2=false;
bool bAutoExportRadio3=false;


//line tool state
bool bTrkLengthManual=false;
int lineToolRoster=0; //chooses line tool
int lineToolAlt=0;
int lineToolX=0;
int lineToolY=0;
int lineToolY_toggleperm=0;
bool lineRosterEnable[9]; //whether to include in roster
int lineDashLen=1;
int globalLineSenderMode=0;  //used to communicate with async line keys which mode to refresh.
int penActive; //chooses drawing mode
int tileActive;
int bankActive=0;
int nameXC;
int nameYC;
int tileXC;
int tileYC;
int palHover;
int palColHover;
int colHover;
int palDragColor;
int palBank;

//int tileViewTagCur=0;
int tileViewTagBuf=0;
unsigned char tileViewTable[256]; //used to indirect tile arrangements wihtout changing physical memory.
unsigned int bankViewTable[256*4]; //used to offset which tiles are mapped from which banks. Set to 4 tables to futureproof an expansion of viewed sets without further modificiation.
unsigned int undo_bankViewTable[256*4];
unsigned int checkpoint_bankViewTable[256*4];
unsigned int undo_checkpoint_bankViewTable[256*4];



//unsigned char sprModeTableReverse[256];
int lineDownX;
int lineDownY;
int lineUpX= -1;
int lineUpY= -1;


extern int mvOriginX;
extern int mvOriginY;
extern int CHR_mmX;
extern int CHR_mmY;

int main_tilesetScrollX=0;
int main_tilesetScrollY=0;

bool lineDrawing=false;
bool isMovingLine;
bool isNudgingLine;
bool bOldLine;
extern unsigned char arr_linePlotCHR[128*128];
extern unsigned char arr_linePlotCHR2[128*128];
extern bool bForbidPaint;
extern int lineOffX; //x0y0 adjust nudge offset
extern int lineOffY;

unsigned char collisionID_R[256];
unsigned char collisionID_G[256];
unsigned char collisionID_B[256];

AnsiString collisionIDlabel[256];
AnsiString undo_collisionIDlabel[256];
AnsiString checkpoint_collisionIDlabel[256];
AnsiString undo_checkpoint_collisionIDlabel[256];

AnsiString propCHRlabel[8];
bool bClearUint32Array=false;



unsigned char iBucketDirectionA=0x10;
unsigned char iBucketDirectionB=0x94;

int iBrushPresetIndex=0;
int iBrushCursorAlignment[14]={
		4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4
};

int iBrushSize[14]={
		2, 3, 4, 5, 6, 7, 8,
	   16, 3, 4, 5, 6, 7, 8

};
int iBrushSnapSize_x[14]={
		2, 3, 4, 5, 6, 7, 8,
		8, 3, 4, 5, 6, 7, 8

};
int iBrushSnapSize_y[14]={
		2, 3, 4, 5, 6, 7, 8,
		8, 3, 4, 5, 6, 7, 8

};


 bool bBrushMask[4];
// int bBrushMaskBuffer[25];
float brush_x_anchor=0;
float brush_y_anchor=0;
int tableBrush_2a[16][16]= {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}

	};
int tableBrush_3a[16][16]= {
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0}
	};
int tableBrush_4a[16][16]= {
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0}
	};
int tableBrush_5a[16][16]= {
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0}
	};
int tableBrush_6a[16][16]= {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};
int tableBrush_7a[16][16]= {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};
int tableBrush_8a[16][16]= {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}

	};



//---

	int tableBrush_2b[16][16]= {
		{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
		{0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
	};
int tableBrush_3b[16][16]= {
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
int tableBrush_4b[16][16]= {
		{0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int tableBrush_5b[16][16]= {
		{0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int tableBrush_6b[16][16]= {
		{0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
int tableBrush_7b[16][16]= {
		{0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};
int tableBrush_8b[16][16]= {
		{0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};

int undo_tableBrush_2a[16][16];
int undo_tableBrush_3a[16][16];
int undo_tableBrush_4a[16][16];
int undo_tableBrush_5a[16][16];
int undo_tableBrush_6a[16][16];
int undo_tableBrush_7a[16][16];
int undo_tableBrush_8a[16][16];

int undo_tableBrush_2b[16][16];
int undo_tableBrush_3b[16][16];
int undo_tableBrush_4b[16][16];
int undo_tableBrush_5b[16][16];
int undo_tableBrush_6b[16][16];
int undo_tableBrush_7b[16][16];
int undo_tableBrush_8b[16][16];

int checkpoint_tableBrush_2a[16][16];
int checkpoint_tableBrush_3a[16][16];
int checkpoint_tableBrush_4a[16][16];
int checkpoint_tableBrush_5a[16][16];
int checkpoint_tableBrush_6a[16][16];
int checkpoint_tableBrush_7a[16][16];
int checkpoint_tableBrush_8a[16][16];

int checkpoint_tableBrush_2b[16][16];
int checkpoint_tableBrush_3b[16][16];
int checkpoint_tableBrush_4b[16][16];
int checkpoint_tableBrush_5b[16][16];
int checkpoint_tableBrush_6b[16][16];
int checkpoint_tableBrush_7b[16][16];
int checkpoint_tableBrush_8b[16][16];

int undo_checkpoint_tableBrush_2a[16][16];
int undo_checkpoint_tableBrush_3a[16][16];
int undo_checkpoint_tableBrush_4a[16][16];
int undo_checkpoint_tableBrush_5a[16][16];
int undo_checkpoint_tableBrush_6a[16][16];
int undo_checkpoint_tableBrush_7a[16][16];
int undo_checkpoint_tableBrush_8a[16][16];

int undo_checkpoint_tableBrush_2b[16][16];
int undo_checkpoint_tableBrush_3b[16][16];
int undo_checkpoint_tableBrush_4b[16][16];
int undo_checkpoint_tableBrush_5b[16][16];
int undo_checkpoint_tableBrush_6b[16][16];
int undo_checkpoint_tableBrush_7b[16][16];
int undo_checkpoint_tableBrush_8b[16][16];

int (*ptr_tableBrush[14])[16][16] = {
		&tableBrush_2a,
		&tableBrush_3a,
		&tableBrush_4a,
		&tableBrush_5a,
		&tableBrush_6a,
		&tableBrush_7a,
		&tableBrush_8a,

		&tableBrush_2b,
		&tableBrush_3b,
		&tableBrush_4b,
		&tableBrush_5b,
		&tableBrush_6b,
		&tableBrush_7b,
		&tableBrush_8b,

};
int (*ptr_undo_tableBrush[14])[16][16] = {
		&undo_tableBrush_2a,
		&undo_tableBrush_3a,
		&undo_tableBrush_4a,
		&undo_tableBrush_5a,
		&undo_tableBrush_6a,
		&undo_tableBrush_7a,
		&undo_tableBrush_8a,

		&undo_tableBrush_2b,
		&undo_tableBrush_3b,
		&undo_tableBrush_4b,
		&undo_tableBrush_5b,
		&undo_tableBrush_6b,
		&undo_tableBrush_7b,
		&undo_tableBrush_8b,

};


int (*ptr_checkpoint_tableBrush[14])[16][16] = {
		&checkpoint_tableBrush_2a,
		&checkpoint_tableBrush_3a,
		&checkpoint_tableBrush_4a,
		&checkpoint_tableBrush_5a,
		&checkpoint_tableBrush_6a,
		&checkpoint_tableBrush_7a,
		&checkpoint_tableBrush_8a,

		&checkpoint_tableBrush_2b,
		&checkpoint_tableBrush_3b,
		&checkpoint_tableBrush_4b,
		&checkpoint_tableBrush_5b,
		&checkpoint_tableBrush_6b,
		&checkpoint_tableBrush_7b,
		&checkpoint_tableBrush_8b,

};

int (*ptr_undo_checkpoint_tableBrush[14])[16][16] = {
	&undo_checkpoint_tableBrush_2a,
	&undo_checkpoint_tableBrush_3a,
	&undo_checkpoint_tableBrush_4a,
	&undo_checkpoint_tableBrush_5a,
	&undo_checkpoint_tableBrush_6a,
	&undo_checkpoint_tableBrush_7a,
	&undo_checkpoint_tableBrush_8a,

	&undo_checkpoint_tableBrush_2b,
	&undo_checkpoint_tableBrush_3b,
	&undo_checkpoint_tableBrush_4b,
	&undo_checkpoint_tableBrush_5b,
	&undo_checkpoint_tableBrush_6b,
	&undo_checkpoint_tableBrush_7b,
	&undo_checkpoint_tableBrush_8b,

};

uint32_t propCopy[256];
bool btnStateProps[8];

char propConditional[8]={0,0,0,0,0,0,0,2};   //0=no, 1=yes, 2=all
char propConditionalUndo[8]={0,0,0,0,0,0,0,2};
char checkpointPropConditional[8]={0,0,0,0,0,0,0,2};
char checkpointPropConditionalUndo[8]={0,0,0,0,0,0,0,2};

bool propCondition[4][8];
bool propConditionUndo[4][8];
bool checkpointPropCondition[4][8];
bool checkpointPropConditionUndo[4][8];

bool propShowCondMap=true;
bool propShowCondTile=false;
extern int hoverBtn;
extern int mtHoverBtn;
bool collision_specific=false;  //used to tell if collision is a specific 8bit ID, rather than individual bits. Used by highlight overlay code.

unsigned int chrBanks = 4; //4 is default starting banks.
unsigned int undo_chrBanks = 4;
unsigned int checkpoint_chrBanks = 4;
unsigned int undo_checkpoint_chrBanks = 4;

unsigned int curViewCHRbankCanvas2=0;

unsigned int 	chrBanks_LinkBuf = 4;
unsigned int 	undo_chrBanks_LinkBuf = 4;
unsigned int 	checkpoint_chrBanks_LinkBuf = 4;
unsigned int 	undo_checkpoint_chrBanks_LinkBuf = 4;


unsigned char *chr = (char*)calloc((chrBanks*4096), sizeof(char));
unsigned char *undoChr = (char*)calloc((undo_chrBanks*4096), sizeof(char));
unsigned char *checkpointChr = (char*)calloc((checkpoint_chrBanks*4096), sizeof(char));
unsigned char *undoCheckpointChr = (char*)calloc((undo_checkpoint_chrBanks*4096), sizeof(char));
unsigned char *tmp_chr = (char*)calloc((chrBanks*4096), sizeof(char));

unsigned char *chr_LinkBuf = (char*)calloc((chrBanks_LinkBuf*4096), sizeof(char));
unsigned char *undoChr_LinkBuf = (char*)calloc((undo_chrBanks_LinkBuf*4096), sizeof(char));
unsigned char *undoCheckpointChr_LinkBuf = (char*)calloc((checkpoint_chrBanks_LinkBuf*4096), sizeof(char));
unsigned char *checkpointChr_LinkBuf = (char*)calloc((undo_checkpoint_chrBanks_LinkBuf*4096), sizeof(char));

//used to hold legacy format tile properties.
uint32_t tileProperties32[512];
uint32_t tilePropertiesUndo32[512];
uint32_t checkpointTileProperties32[512];
uint32_t checkpointTilePropertiesUndo32[512];

uint32_t tileProperties_LinkBuf32[512];
uint32_t tilePropertiesUndo_LinkBuf32[512];
uint32_t checkpointTileProperties_LinkBuf32[512];
uint32_t checkpointTilePropertiesUndo_LinkBuf32[512];

//new format tile properties.
unsigned char *tileProperties						=	(char*)calloc((chrBanks*256), sizeof(char));
unsigned char *tilePropertiesUndo 					= (char*)calloc((undo_chrBanks*256), sizeof(char));
unsigned char *checkpointTileProperties 			= (char*)calloc((checkpoint_chrBanks*256), sizeof(char));
unsigned char *checkpointTilePropertiesUndo 		= (char*)calloc((undo_checkpoint_chrBanks*256), sizeof(char));

unsigned char *tileProperties_LinkBuf 				= (char*)calloc((chrBanks_LinkBuf*256), sizeof(char));
unsigned char *tilePropertiesUndo_LinkBuf 			= (char*)calloc((undo_chrBanks_LinkBuf*256), sizeof(char));
unsigned char *checkpointTileProperties_LinkBuf 	= (char*)calloc((checkpoint_chrBanks_LinkBuf*256), sizeof(char));
unsigned char *checkpointTilePropertiesUndo_LinkBuf = (char*)calloc((undo_checkpoint_chrBanks_LinkBuf*256), sizeof(char));

std::vector<std::string> chrBankLabels;
std::vector<std::string> undo_chrBankLabels;
std::vector<std::string> checkpoint_chrBankLabels;
std::vector<std::string> undo_checkpoint_chrBankLabels;
std::vector<std::string> tmp_chrBankLabels;

std::vector<std::string> chrBankLabels_LinkBuf;
std::vector<std::string> undo_chrBankLabels_LinkBuf;
std::vector<std::string> checkpoint_chrBankLabels_LinkBuf;
std::vector<std::string> undo_checkpoint_chrBankLabels_LinkBuf;
std::vector<std::string> tmp_chrBankLabels_LinkBuf;



//segmented chr bank windows



extern TRect curCursor;
extern TRect bnkCursor;
extern TRect curSelection;
extern TRect bnkSelection;
extern bool isBnkCursor;

unsigned char chrBuf[4096*4]; //used by various drawing tools
unsigned char chrImportBuf[4096*4]; //used by bitmap import specifically.
unsigned char chrBufColour[4096*4]; //used by colour swap
unsigned char chrBufFill[8192];
unsigned char chrBufLine[8192];
unsigned char chrQuickLine[8192];
unsigned char chrCopy[4096];





int CHRCollisionGranularityX=1;
int CHRCollisionGranularityY=1;
int MetaCollisionGranularityX=1;
int MetaCollisionGranularityY=1;


const int META_2x2=4*64;
const int META_4x4=16*64;
const int META_8x8=64*64;
//----metatile unpacking space for merging outside sets.
int buf_metatileSets_2x2;
int buf_metatileSets_4x4;
int buf_metatileSets_8x8;

unsigned char *metatileBuf_2x2_id = (char*)calloc((1*META_2x2),sizeof(char));
unsigned char *metatileBuf_2x2_pal = (char*)calloc((1*META_2x2),sizeof(char));
unsigned char *metatileBuf_2x2_props = (char*)calloc((1*META_2x2),sizeof(char));

unsigned char *metatileBuf_4x4_id = (char*)calloc((1*META_4x4),sizeof(char));
unsigned char *metatileBuf_4x4_pal = (char*)calloc((1*META_4x4),sizeof(char));
unsigned char *metatileBuf_4x4_props = (char*)calloc((1*META_4x4),sizeof(char));

unsigned char *metatileBuf_8x8_id = (char*)calloc((1*META_8x8),sizeof(char));
unsigned char *metatileBuf_8x8_pal = (char*)calloc((1*META_8x8),sizeof(char));
unsigned char *metatileBuf_8x8_props = (char*)calloc((1*META_8x8),sizeof(char));

//------metatile banks & lists

bool bMTpropHover=false;
bool bMTuseDirectTile=false;
bool bColissionIDHover=false;
unsigned int highlight_mt=0;

int32_t metatileSets_2x2 = 4;
int32_t undo_metatileSets_2x2 = 4;
int32_t checkpoint_metatileSets_2x2 = 4;
int32_t undo_checkpoint_metatileSets_2x2 = 4;

unsigned char *metatileSet_2x2_id = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *undo_metatileSet_2x2_id = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *checkpoint_metatileSet_2x2_id = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_2x2_id = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));

unsigned char *metatileSet_2x2_pal = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *undo_metatileSet_2x2_pal = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *checkpoint_metatileSet_2x2_pal = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_2x2_pal = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));

unsigned char *metatileSet_2x2_props = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *undo_metatileSet_2x2_props = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *checkpoint_metatileSet_2x2_props = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_2x2_props = (char*)calloc((metatileSets_2x2*META_2x2),sizeof(char));


std::vector<std::string> metatileSetLabels_2x2;
std::vector<std::string> undo_metatileSetLabels_2x2;
std::vector<std::string> checkpoint_metatileSetLabels_2x2;
std::vector<std::string> undo_checkpoint_metatileSetLabels_2x2;
std::vector<std::string> tmp_metatileSetLabels_2x2;   //?

int32_t metatileSets_4x4 = 4;
int32_t undo_metatileSets_4x4 = 4;
int32_t checkpoint_metatileSets_4x4 = 4;
int32_t undo_checkpoint_metatileSets_4x4 = 4;

unsigned char *metatileSet_4x4_id = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *undo_metatileSet_4x4_id = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *checkpoint_metatileSet_4x4_id = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_4x4_id = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));

unsigned char *metatileSet_4x4_pal = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *undo_metatileSet_4x4_pal = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *checkpoint_metatileSet_4x4_pal = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_4x4_pal = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));

unsigned char *metatileSet_4x4_props = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *undo_metatileSet_4x4_props = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *checkpoint_metatileSet_4x4_props = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_4x4_props = (char*)calloc((metatileSets_4x4*META_4x4),sizeof(char));



std::vector<std::string> metatileSetLabels_4x4;
std::vector<std::string> undo_metatileSetLabels_4x4;
std::vector<std::string> checkpoint_metatileSetLabels_4x4;
std::vector<std::string> undo_checkpoint_metatileSetLabels_4x4;
std::vector<std::string> tmp_metatileSetLabels_4x4;   //?

int32_t metatileSets_8x8 = 2;
int32_t undo_metatileSets_8x8 = 2;
int32_t checkpoint_metatileSets_8x8 = 2;
int32_t undo_checkpoint_metatileSets_8x8 = 2;

unsigned char *metatileSet_8x8_id = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *undo_metatileSet_8x8_id = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *checkpoint_metatileSet_8x8_id = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_8x8_id = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));

unsigned char *metatileSet_8x8_pal = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *undo_metatileSet_8x8_pal = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *checkpoint_metatileSet_8x8_pal = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_8x8_pal = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));

unsigned char *metatileSet_8x8_props = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *undo_metatileSet_8x8_props = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *checkpoint_metatileSet_8x8_props = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));
unsigned char *undo_checkpoint_metatileSet_8x8_props = (char*)calloc((metatileSets_8x8*META_8x8),sizeof(char));




std::vector<std::string> metatileSetLabels_8x8;
std::vector<std::string> undo_metatileSetLabels_8x8;
std::vector<std::string> checkpoint_metatileSetLabels_8x8;
std::vector<std::string> undo_checkpoint_metatileSetLabels_8x8;
std::vector<std::string> tmp_metatileSetLabels_8x8;   //?


//these are updated as needed and discarded on save.
//uint32_t *mtUsage_2x2 = (uint32_t*)calloc((metatileSets_2x2*64),sizeof(uint32_t));
//uint32_t *mtUsage_4x4 = (uint32_t*)calloc((metatileSets_4x4*64),sizeof(uint32_t));
//uint32_t *mtUsage_8x8 = (uint32_t*)calloc((metatileSets_8x8*64),sizeof(uint32_t));

char *mtContent_2x2 = (char*)calloc((metatileSets_2x2*64),sizeof(char));
char *mtContent_4x4 = (char*)calloc((metatileSets_4x4*64),sizeof(char));
char *mtContent_8x8 = (char*)calloc((metatileSets_8x8*64),sizeof(char));


int iSetView=64;
int iSetViewOff=0;
int iListViewOff=0;
int mtClickID=0;
int mtClickID_store_2x2=0;
int mtClickID_store_4x4=0;
int mtClickID_store_8x8=0;
bool bMtMultiSelectRemoveMode=false;

extern TRect metaSelection_2x2;
extern TRect metaSelection_4x4;
extern TRect metaSelection_8x8;

//metatile property stuff
bool btnStateMtProps[8];



unsigned char subpalCopy[4]; //for copying a single subpalette

unsigned char nameTable[NAME_MAX_SIZE];
unsigned char attrTable[ATTR_MAX_SIZE];
unsigned char tmpNameTable[NAME_MAX_SIZE];
unsigned char tmpAttrTable[ATTR_MAX_SIZE];

int importBMP_tile_count;
int importBMP_tile_count_reduced;

int nameTableViewX;
int nameTableViewY;

unsigned int iMapMatchCnt=0;
int prevmtClickID= -1;


TRect nameSelection;
int nameCopyWidth;
int nameCopyHeight;
int nameTableWidth;
int nameTableHeight;
int buf_nameWidth;      //used by bmp import to restore
int buf_nameHeight;

unsigned char nameCopy[NAME_MAX_SIZE];
unsigned char attrCopy[NAME_MAX_SIZE];

//unsigned char undoChr[8192];
unsigned char undoNameTable[NAME_MAX_SIZE];
unsigned char undoAttrTable[ATTR_MAX_SIZE];

//unsigned char checkpointChr[8192];
unsigned char checkpointNameTable[NAME_MAX_SIZE];
unsigned char checkpointAttrTable[ATTR_MAX_SIZE];

//unsigned char undoCheckpointChr[8192];
unsigned char undoCheckpointNameTable[NAME_MAX_SIZE];
unsigned char undoCheckpointAttrTable[ATTR_MAX_SIZE];

int undoNameTableWidth;
int undoNameTableHeight;

int checkpointNameTableWidth;
int checkpointNameTableHeight;

int undoCheckpointNameTableWidth;
int undoCheckpointNameTableHeight;



bool unsavedChanges;

TRect chrSelection;
TRect chrSelBuf;
TRect nameSelBuf;
int chrCopyWidth;
int chrCopyHeight;
bool chrCopyRect;
unsigned char chrSelected[256];


bool bMultiSelectRemoveMode;
bool chrSelectRect;

bool metaSelectMulti;

unsigned char metaSelected_2x2[4];
unsigned char metaSelected_4x4[16];
unsigned char metaSelected_8x8[64];

//global drag source state
bool bImageNameAccepted;
bool bImageTileAccepted;
//global mousedown state
int txDown;    //tileset
int tyDown;
int nxDown;   //nametable/screen/map
int nyDown;

bool bOutsideSel;
bool bDrawDestShadow;
bool bMouseOverNam;
bool bMouseOverNav;
bool bAllowLockMessage=true;

int typeInFontOffset;
int typeInASCIIOffset;

int prevMouseY;
int prevMouseX; //FG: reserved for potential features to come
int prevNullTile;
bool doUpdateMetatileNulls=false;

unsigned char metaSpritesBuf[256*64*4];
unsigned char metaSprites[256*64*4];
unsigned char undoMetaSprites[256*64*4];
unsigned char checkpointMetaSprites[256*64*4];
unsigned char undoCheckpointMetaSprites[256*64*4];

//BROKE STUDIO
AnsiString metaSpriteBankName;
AnsiString metaSpriteNames[256];
AnsiString metaSpriteNamesUndo[256];
AnsiString metaSpriteNamesCheckpoint[256];
AnsiString metaSpriteNamesCheckpointUndo[256];

AnsiString tmpMetaSprName;
//AnsiString metaSpriteNameCopy;
char metaSpriteNameCopy[256];
//

int spriteActive;
int metaSpriteActive;

int spriteGridX;
int spriteGridY;
int undoSpriteGridX;
int undoSpriteGridY;

int checkpointSpriteGridX;
int checkpointSpriteGridY;
int undoCheckpointSpriteGridX;
int undoCheckpointSpriteGridY;


int spriteHover;
int spriteDrag;
int spriteDragX;
int spriteDragY;

int CHREditorHover;
int bigTilesHover;

unsigned char metaSpriteCopy	[64*4];

//metasprite direct animation properties
char cMsprLabel[256][31];

int CHRLatestOffset;
int CHRLatestSize;
bool CHRLatestFromSet=true;

int uiScale;


int CF_CHR;
int CF_NAM;
int CF_META;    //nesst metasprite
int CF_PAL;
int CF_SUBPAL;
int CF_METATILE;  //nexxt metatile (2,4 and 8).




int tmpContinousIncDecTimer;
int continousIncDecDuration=3;

bool sharedCol0 = true;
bool safeColsOnly = true;


bool clickV;
bool clickC;

bool bBufCtrl;
bool bBufShift;
bool bBufAlt;
bool bBufVK_3; //grave accent

int cf_palRotation; //used to paste several subpalette entries in sequence from clipboard
int blendMode;

int oldNameXC;
int oldNameYC;
//int oldTileXC;
//int oldTileYC;
int oldCHRRectWdt;
int oldCHRRectHgt;

int returnCarriagePos;
int nullTile;

int fillPal;

int tileSelRectWdt=1;
int tileSelRectHgt=1;

bool holdStats;
bool cueStats;
bool cueUpdateMetasprite;
bool cueUpdateMetasprite_noListUpdate=false;
bool cueUpdateMM;
bool cueUpdateTiles;
bool cueUpdateNametable;
bool cueUpdateNametableNoRepaint;
bool cueUpdateNTstrip;
int  cntNTstrip=0;
bool cueRemoveDoubles;
bool cueCHRdraw;
bool cueCHRdrawAll;

bool cueNT;
bool throttleOK = true;
bool throttleOKselRect = false;
bool throttleOKtileSelRect = false;
bool mouseDraggedNTSel = false;
bool mouseDraggedTileSel = false;

bool bbigTileMouseDown = false;
bool bForceNameScale = false;

bool isLastClickedMetaSprite;
bool isLastClickedSpriteList;
bool isLastClickedNametable;
bool isLastClickedTiles;

bool byte1bool;
bool db1bool;

bool acceptPalDrag;
int palDragVal;




//these are "factory preset" startup settings. Some are chars for reusing an old nesst routine in a slightly hacky way.
//those are cast as temporary ints later when necessary (in the preferences dialogue).
unsigned char prefStartScale[1]={2};
unsigned char prefStartCol[1]={3};
unsigned char prefStartSubpal[1]={0};

bool prefStartGridShow=false;
bool prefStartGrid1=false;
bool prefStartGrid2=true;
bool prefStartGrid4=false;

bool prefStartGrid32x30=false;
bool prefStartGridPixelCHR=true;
bool prefStartGridTilesCHR=false;
bool prefStartGridMidpointsCHR=false;

bool prefStartShowCHR=false;
bool prefStartShowMM=false;

bool prefStartShowBrushes=false;
bool prefStartShowBucket=false;
bool prefStartShowLines=false;

bool prefStartScale2x=false;
bool prefStartScale3x=false;
bool prefStartScale4x=false;


bool prefStartCol0=false;
bool prefStartCol1=false;
bool prefStartCol2=false;
bool prefStartCol3=false;

bool prefStartSubpal0=false;
bool prefStartSubpal1=false;
bool prefStartSubpal2=false;
bool prefStartSubpal3=false;

RECT curMainWinPos;

TRect destRect;

int iSwap_WhichSubpal=0;
int iSwap_Pal0Subpal=0;

inline int min(int a, int b) {
	return (a < b) ? a : b;
}
inline int max(int a, int b) {
    return (a > b) ? a : b;
}
inline int ViewTablePos(int c) {
	int i;
	for (i = 0; i<256; i++) {
		if (tileViewTable[i] == c) {
			return i;
		}
	}
	return 0;
}

inline const char * const BoolToString(bool b)
{
	return b?"true":"false";
}

inline bool GetTileHasContents(char* tile)
{
	//Expects the reference to be passed from tileset or other chr data

	for(int i=0;i<16;i++)
	{
		if(tile[i]!=0) return true;
	}
	return false;
}


inline int count_diff_bits(unsigned char a) {
	int count = 0;
	//unsigned char first_bit = a & 1;
    int i = 0;
    
    //check first 7 bits against the more significant bit
    while (i<7) { 
        
        count += (a & 1) ^ ((a >> 1) & 1);
        a >>= 1;
        i++;
    }
	//a >>=1; //one last bit shift down to compare the last bit with 'first_bit'

	//count += (a & 1) ^ first_bit;
	return count;
}
inline int count_diff_bytes(char *ptr, int size, char mask) {
	int difference = 0;
	int i;
	for (i = 0; i < size-1; i++) {
		char xor_result = ptr[i] ^ ptr[i+1];
		for (int j = 0; j < 8; j++) {
			difference += ((xor_result >> j) & 1);
		}
	}
		/*
		i++;
			char xor_result2 = ptr[i] ^ ptr[0];
			for (int j = 0; j < 8; j++) {
					difference += ((xor_result2 >> j) & 1);
				}
	*/
	return difference;
}

int DrawTimerDuration=500;
const int masterPaletteColumns=14; //project: convert to 16

int iAttrCheckerMethod=2;
int faderAttrCheckerOpacity[5];
int faderAttrCheckerSeparation[5];
int upDownAttrCheckerRotation[5];
const unsigned char attrCheckerStripeCHR[16]={
	0x55,0xAA,0x55,0xAA,
	0x55,0xAA,0x55,0xAA,
	0x33,0x66,0xCC,0x99,
	0x33,0x66,0xCC,0x99
};
const unsigned char attrCheckerClassicCHR[16]={
		0x0f,0x0f,0x0f,0x0f,
		0x0f,0x0f,0x0f,0x0f,
		0x00,0x00,0x00,0x00,
		0xff,0xff,0xff,0xff
	};
const unsigned char attrCheckerIDCHR[16*4]={
		0x01,0x19,0x25,0x25,
		0x25,0x19,0x01,0xff,
		0x00,0x18,0x24,0x24,
		0x24,0x18,0x00,0x00,

		0x01,0x11,0x31,0x11,
		0x11,0x39,0x01,0xff,
		0x00,0x10,0x30,0x10,
		0x10,0x38,0x00,0x00,

		0x01,0x19,0x25,0x09,
		0x11,0x3D,0x01,0xFF,
		0x00,0x18,0x24,0x08,
		0x10,0x3C,0x00,0x00,

		0x01,0x19,0x25,0x09,
		0x25,0x19,0x01,0xff,
		0x00,0x18,0x24,0x08,
		0x24,0x18,0x00,0x00
};
const unsigned char bgPalDefault[5*16]={

	//nesst classic grayscale
	0x0f,0x00,0x10,0x30,
	0x0f,0x00,0x10,0x30,
	0x0f,0x00,0x10,0x30,
	0x0f,0x00,0x10,0x30,

	//nesst classic startup ("A")
	0x0f,0x00,0x10,0x30,
	0x0f,0x01,0x21,0x31,
	0x0f,0x06,0x16,0x26,
	0x0f,0x09,0x19,0x29,

	//nesst classic "B"
	0x0f,0x11,0x21,0x31,
	0x0f,0x12,0x22,0x32,
	0x0f,0x13,0x23,0x33,
	0x0f,0x14,0x24,0x34,

	//nesst classic "C"
	0x0f,0x15,0x25,0x35,
	0x0f,0x16,0x26,0x36,
	0x0f,0x17,0x27,0x37,
	0x0f,0x18,0x28,0x38,

	//nesst classic "D"
	0x0f,0x19,0x29,0x39,
	0x0f,0x1a,0x2a,0x3a,
	0x0f,0x1b,0x2b,0x3b,
	0x0f,0x1c,0x2c,0x3c

};
const unsigned char bgPalDefault_NEXXT2[5*16]={

	//nexxt 2.0 release default palette
    //aimed at giving more immediately useful/inspiring options.
	//nesst classic grayscale
	0x0f,0x00,0x10,0x30,
	0x0f,0x00,0x10,0x30,
	0x0f,0x00,0x10,0x30,
	0x0f,0x00,0x10,0x30,

	//startup ("A")
	0x0f,0x00,0x10,0x30,
	0x0f,0x0C,0x21,0x32,
	0x0f,0x05,0x16,0x27,
	0x0f,0x0B,0x1A,0x29,

	//startup "B"
	0x0f,0x1C,0x21,0x32,
	0x0f,0x11,0x22,0x33,
	0x0f,0x12,0x23,0x34,
	0x0f,0x13,0x24,0x35,

	//startup "C"
	0x0f,0x13,0x24,0x36,
	0x0f,0x15,0x26,0x37,
	0x0f,0x16,0x27,0x37,
	0x0f,0x19,0x28,0x38,

	//startup "D"
	0x0f,0x18,0x29,0x38,
	0x0f,0x1b,0x2a,0x39,
	0x0f,0x1b,0x2b,0x3c,
	0x0f,0x1c,0x2c,0x3b

};


bool AllValuesEqual(char* arr, int len, char target) {
	for (int i = 0; i < len; i++) {
		if (arr[i] != target) {
			return false; // Not all values equal the target
		}
	}
	return true;
}
         
AnsiString __fastcall TFormMain::MakePathLocal(const AnsiString& path1, const AnsiString& path2) {
	//path 1 - typically workdir
	//path 2 - to compare

	if (path2.Pos(path1) == 1) { //Does path2 start w. path1?
		AnsiString localPath = path2.SubString(path1.Length() + 1, path2.Length() - path1.Length());
		if (localPath[1] == '\\' || localPath[1] == '/') // Remove leading slash if present
			localPath.Delete(1, 1);
		return localPath;
	}
	else {
		return path2; // If path2 cannot be made local to path1, return it unaltered
	}
}

AnsiString CheckAndCreateFolder(AnsiString folderName, bool isExePath)
{
	//usage:
	//CheckAndCreateFolder("backups");


	// Get the path to the executable
	char sourcePath[MAXPATH];
	char targetPath[MAXPATH];
	if(isExePath){
		GetModuleFileName(NULL, sourcePath, MAXPATH);
		//remove executable from path string
		char* lastBackslash = strrchr(sourcePath, '\\');
		if (lastBackslash != NULL) {*lastBackslash = '\0';}
	}
	else{
		GetCurrentDirectory(MAX_PATH,sourcePath);
	}
	strncpy(targetPath, sourcePath, sizeof(sourcePath));
	strcat(targetPath, "\\");

	strcat(targetPath, folderName.c_str());


	// Make folder if not existing
	struct ffblk fileInfo;
	int result = findfirst(targetPath, &fileInfo, FA_DIREC);
	if (result == 0)	{findclose(&fileInfo); }
	else				mkdir(targetPath);
	strcat(targetPath, "\\");
	return AnsiString(targetPath);

}
AnsiString ShortenFileName(const AnsiString &fileName, int maxLength, int partsToKeep)
{
    AnsiString shortenedFileName = fileName;

    while (shortenedFileName.Length() > maxLength)
    {
        // Split the path into parts by '\'
        TStrings *pathParts = new TStringList();
        pathParts->Delimiter = '\\';
        pathParts->DelimitedText = shortenedFileName;

        // Calculate the # of parts needed to retain to fit within maxLength.
        int totalParts = pathParts->Count;
        int partsToRemove = totalParts - partsToKeep - 1; // Keep partsToKeep parts and replace the rest.

        if (partsToRemove > 0)
        {
            // Calculate the length of the removed parts.
            int removedPartsLength = 0;
            for (int i = totalParts - partsToRemove - 1; i >= 0; --i)
            {
                removedPartsLength += pathParts->Strings[i].Length() + 1; // Include the length of '\'
            }

            // Create the shortened path with '...' between the retained parts.
            AnsiString newPath;
            for (int i = 0; i < partsToKeep; ++i)
            {
                newPath = newPath + pathParts->Strings[i] + "\\";
            }
            newPath = newPath + "...\\" + pathParts->Strings[totalParts - 2]+"\\"+ pathParts->Strings[totalParts - 1]; // Preserve the last substring.

            delete pathParts;

            // If the new path is still too long, truncate the file name.
            if (newPath.Length() <= maxLength)
            {
                shortenedFileName = newPath;
            }
            else
            {
                // If the new path is still too long, truncate the file name.
				shortenedFileName = newPath.SubString(1, maxLength-4);
				shortenedFileName = shortenedFileName + "...";
			}
		}
		else
        {
            delete pathParts;
			break; // No more parts to remove, exit the loop
        }
    }

    return shortenedFileName;
}

void SetCheckMenuItem(TMenuItem *m, int tag)
{
	for (int i = 0; i < m->Count; i++) {
		TMenuItem *item = m->Items[i];
		if (item->Tag == tag) {
			item->Checked = true;
			break;
		}
	}
}

void SetCheckPopupMenuItem(TPopupMenu *PopupMenu, int tag)
{
  for (int i = 0; i < PopupMenu->Items->Count; i++)
  {
	TMenuItem *Item = PopupMenu->Items->Items[i];
	Item->Checked = (Item->Tag == tag);
  }
}

 void RemoveCharacter(char *str, char character)
{
    int i, j = 0;
    int n = strlen(str);
    char *newstr = (char *)malloc(n);
    for (i = 0; i < n; i++)
    {
        if (str[i] != character)
        {
            newstr[j] = str[i];
            j++;
        }
    }
    newstr[j] = '\0';
    strcpy(str, newstr);
    free(newstr);
}

AnsiString reg_load_str(const char *name,AnsiString def)
{
	HKEY key;
	DWORD type,size;
	char *str;
	AnsiString ret;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,REG_SECTION,0,KEY_READ,&key)!=ERROR_SUCCESS) return def;

	type=REG_SZ;
	size=1024;
	str=(char*)malloc(size);
	if(RegQueryValueEx(key,name,NULL,&type,(unsigned char*)str,&size)!=ERROR_SUCCESS) ret=def; else ret=str;
	free(str);
	RegCloseKey(key);

	return ret;
}



void reg_save_str(const char *name,AnsiString str)
{
	HKEY key;
	DWORD disp;

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,REG_SECTION,0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&key,&disp)!=ERROR_SUCCESS) return;

	RegSetValueEx(key,name,0,REG_SZ,str.c_str(),strlen(str.c_str())+1);
	RegCloseKey(key);
}



AnsiString num_to_3char(int i)
{
	AnsiString str;

	if(i<0){str="-";} else {str=" ";}
	i=abs(i);

	if(i>=100) str+=IntToStr((i/100)%10);
	if(i>=10 ) str+=IntToStr((i/10 )%10); else str+=" ";

	str+=IntToStr(i%10);

	return str;
}
AnsiString num_to_4char_ca65(int i)  //variant for ca65 assembler.
{
	AnsiString str;

	if(i<0){str="<-";} else {str="  ";}
	i=abs(i);

	if(i>=100) str+=IntToStr((i/100)%10);
	if(i>=10 ) str+=IntToStr((i/10 )%10); else str+=" ";

	str+=IntToStr(i%10);

	return str;
}


int get_file_size(FILE *file)
{
	int size;

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	return size;
}



int read_dword(unsigned char *data)
{
	return data[0]+(data[1]<<8)+(data[2]<<16)+(data[3]<<24);
}

bool __fastcall TFormMain::ConfirmNameSelection(void)
{
	return (nameSelection.left>=0&&nameSelection.top>=0);
}

int __fastcall TFormMain::AttrGet(int x,int y, bool doBuffer, bool returnByte)
{
	int pal;

	if(doBuffer) pal=attrCopy[y/4*((nameTableWidth+3)/4)+x/4];
	else pal=attrTable[y/4*((nameTableWidth+3)/4)+x/4];

	//return full byte
	if(returnByte) return pal;

	//return extracted value
	if(x&2) pal>>=2;
	if(y&2) pal>>=4;

	return pal&3;
}



void __fastcall TFormMain::AttrSet(int x,int y,int pal,bool doBuffer)
{
	int pp,mask;

	pp=y/4*((nameTableWidth+3)/4)+x/4;

	mask=3;
	pal=pal&3;

	if(x&2)
	{
		pal<<=2;
		mask<<=2;
	}
	if(y&2)
	{
		pal<<=4;
		mask<<=4;
	}

	if(pp<nameTableWidth*nameTableHeight)
	{

		if(doBuffer)attrCopy[pp]=(attrCopy[pp]&(mask^255))|pal;
		else attrTable[pp]=(attrTable[pp]&(mask^255))|pal;
	}
}



int encode_rle(unsigned char *src,int size,unsigned char *dst)
{
	int stat[256];
	int i,tag,sym,sym_prev,len,ptr;

	memset(stat,0,sizeof(stat));

	for(i=0;i<size;++i) ++stat[src[i]];

	tag=-1;

	for(i=0;i<256;++i)
	{
		if(!stat[i])
		{
			tag=i;
			break;
		}
	}

	if(tag<0) return -1;

	ptr=0;
	len=1;
	sym_prev=-1;

	dst[ptr++]=tag;

	for(i=0;i<size;++i)
	{
		sym=src[i];

		if(sym_prev!=sym||len>=255||i==size-1)
		{
			if(len>1)
			{
				if(len==2)
				{
					dst[ptr++]=sym_prev;
				}
				else
				{
					dst[ptr++]=tag;
					dst[ptr++]=len-1;
				}
			}

			dst[ptr++]=sym;

			sym_prev=sym;

			len=1;
		}
		else
		{
			++len;
		}
	}

	dst[ptr++]=tag;	//end of file marked with zero length rle
	dst[ptr++]=0;

	return ptr;
}



int name_size(void)
{
	return nameTableWidth*nameTableHeight;
}



int attr_size(void)
{
	return (nameTableWidth+3)/4*((nameTableHeight+3)/4);
}



AnsiString save_data(const char *name,unsigned char *src,int size,int format,bool rle)
{
	char arrname[256],temp[256];
	FILE *file;
	int i,tag,pp,len,sym,sym_prev,type;
	unsigned char *dst;

	if(rle)
	{
		dst=(unsigned char*)malloc(size*2);

		size=encode_rle(src,size,dst);

		if(size<0)
		{
			free(dst);
			return "No unused values found (tiles and/or attributes), can't be saved as RLE due to limitations of the format.";
		}
	}
	else
	{
		dst=src;
	}

	i=strlen(name);

	while(i)
	{
		if(name[i]=='\\'||name[i]=='/')
		{
			i++;
			break;
		}
		i--;
	}

	strcpy(arrname,&name[i]);

	for(i=strlen(arrname)-1;i>=0;i--)
	{
		if(arrname[i]=='.')
		{
			arrname[i]=0;
			break;
		}
	}

	if(arrname[0]!='_'&&!(arrname[0]>='a'&&arrname[0]<='z')&&!(arrname[0]>='A'&&arrname[0]<='Z'))
	{
		strcpy(temp,arrname);
		strcpy(arrname,"n");
		strcat(arrname,temp);
	}

	switch(format)
	{
	case SAVE_FORMAT_H:
		{
			file=fopen(name,"wt");

			if(!file) break;

			fprintf(file,"const unsigned char %s[%i]={\n",arrname,size);

			for(i=0;i<size;i++)
			{
				fprintf(file,"0x%2.2x",dst[i]);
				if(i<size-1) fprintf(file,",");
				if((i&15)==15||i==(size-1)) fprintf(file,"\n");
			}

			fprintf(file,"};\n");
			fclose(file);
		}
		break;

	case SAVE_FORMAT_ASM:
		{
			file=fopen(name,"wt");

			if(!file) break;

			fprintf(file,"%s:\n",arrname);

			for(i=0;i<size;i++)
			{
				if(!(i&15))
				{
				if(byte1bool) fprintf(file,"\t.byte ");      //quick workaround.
				if(db1bool) fprintf(file,"\t.db ");
				}
				fprintf(file,"$%2.2x",dst[i]);
				if(i<size-1&&(i&15)<15) fprintf(file,",");
				if((i&15)==15||i==(size-1)) fprintf(file,"\n");
			}

			fprintf(file,"\n");
			fclose(file);
		}
		break;

	case SAVE_FORMAT_BIN:
		{
			file=fopen(name,"wb");

			if(!file) break;

			fwrite(dst,size,1,file);
			fclose(file);
		}
		break;
	}

	if(rle) free(dst);

	return "";
}



inline void mem_exchange(unsigned char *src,unsigned char *dst,int len)
{
	int temp;

	while(len)
	{
		temp=*src;
		*src++=*dst;
		*dst++=temp;
		len--;
	}
}
inline void mem_32exchange(uint32_t *src,uint32_t *dst,int len)
{
	int temp;
    len = len/sizeof(uint32_t);
	while(len)
	{
		temp=*src;
		*src++=*dst;
		*dst++=temp;
		len--;
	}
}
 inline void swap_bool_2d(bool arr1[][8], bool arr2[][8], size_t rows, size_t cols)
{
    const size_t max_rows = 4; // maximum possible rows
    const size_t max_cols = 8; // maximum possible columns
    bool temp[max_rows][max_cols];

    if (rows > max_rows || cols > max_cols) {
        // handle error: input arrays are too large
        return;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            temp[i][j] = arr1[i][j];
            arr1[i][j] = arr2[i][j];
            arr2[i][j] = temp[i][j];
		}
	}
}
bool CheckTag(const char* text, size_t size, const char* tag) {
	size_t tag_size = strlen(tag);
	if (size < tag_size) {
		// The string is smaller than the tag, so the tag cannot be present.
		return false;
	}

    for (size_t i = 0; i <= size - tag_size; ++i) {
		if (memcmp(text + i, tag, tag_size) == 0) {
			// The tag was found.
			return true;
		}
	}

	// The tag was not found.
	return false;
}



void transpose_bitmask(char* data, int d) {
	//assumes the full bitmask is likesided and max of w & h.
	//change this if requirements are otherwise.
	if (data == NULL || d <= 0) return;

	for (int i=0; i<d; i++) {
		for (int j=i+1; j<d; j++) {
			char tmp = data[i*d+j];
			data[i*d+j] = data[j*d+i];
			data[j*d+i] = tmp;
		}
	}
}

void rotate180_bitmask(char* data, int w, int h) {
	if (data == NULL || w <= 0 || h <= 0) return;
	for (int i=0; i<h/2; i++) {
		for (int j=0; j<w; j++) {
			char tmp 				= data[i*w+j];
			data[i*w+j] 			= data[(h-1-i)*w+(w-1-j)];
			data[(h-1-i)*w+(w-1-j)] = tmp;
		}
	}
}

void shear_bitmask(char* data, float shear, int w, int h, bool bRot180, bool bTranspose, bool bRounding){
  /*
  ---------------------------------------------------
  usage:
  ======

  -to shear on y axis, set bTranspose
  -to rotate,
	1)shear x: -tan(angle/2)
	2)shear y: sin(angle)
	3)shear x: -tan(angle/2) again
  -to rotate more than +/- 90 degrees, set bRot180.

  -with bRounding set, the shear factor is rounding corrected by 0.5

  You may want to transfer the mask between buffer and workspace.
  ---------------------------------------------------
  */

 	if (data == NULL || w <= 0 || h <= 0) return;
	
	int canvas_d=32;
	int bufOff=16; //used to center placement in the internal buffer
  	float round=bRounding? 0.5f:0.0f; 
	float center=canvas_d/2; //used to calc
	
	//in case control allows more than -90...90 degree turns, flip 
	if(bRot180) rotate180_bitmask(data,canvas_d,canvas_d);
	
  	if(bTranspose){   //transpose 90 degrees, used for y shear.
		//int d = (w>h)? w:h;
	  	transpose_bitmask(data, canvas_d);
	}

	//init internal buffer canvas; large enough. 
	char temp[64][64];
	for 	(int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) { 
			temp[i][j]=0;
		}
	}


	for (int i = 0; i < 32; i++) {
		float shearedIndexFloat = (shear * (i -center+round)) ;
		int skewi = (int)shearedIndexFloat;
		for (int j = 0; j < 32; j++) {
			
				temp[i+bufOff][skewi+j+bufOff] = data[(i)*canvas_d + j];
		}
	}

	// Copy the sheared field back to the param field
	for (int i = 0; i < canvas_d; i++) {
		for (int j = 0; j < canvas_d; j++) {
			data[(i)*canvas_d + j] = temp[i+bufOff][j+bufOff];
		}
	}

  if(bTranspose){   //2nd pass to get it back.
	  //int d = (w>h)? w:h;
	  transpose_bitmask(data, canvas_d);
  }

}


void Bresenham_line(int x0, int y0, int x1, int y1, int w, unsigned char *array,bool isRightAngle) {
	int x,y; //for counters

	int dash = lineDashLen +1;
	bool bDots = FormLineDetails->btnDots->Down;
	bool bDashes  = FormLineDetails->btnDashes->Down;
	if (bDots==false && bDashes==false) {bDots=true; bDashes=true; }

	int dx = (abs(x1 - x0)+1)<<1;   //edit. "classic" is dx = abs((x1 - x0)
	int dy = (abs(y1 - y0)+1)<<1;   //edit. "classic" is abs((y1 - y0)
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err;
	int max_iterations = max(abs(x1 - x0), abs(y1 - y0)); //may be used as a reference to abort plot prematurely somewhere along the line.

	int maxi_subtract = isRightAngle? abs(lineToolAlt) : 0;

	max_iterations=max_iterations-maxi_subtract;

	int cnt=0;

	if(dx > dy)  //x is fast axis
	{
		err = (dy - dx) >>1;
		for (x=x0,y=y0; sx<0 ? x>=x1:x<=x1; x+=sx) {

			int plOff = max(min(y*w + x,128*128-1),0);
			if(x>=0 && y>=0 && x<128 && y<128){
				if(x%dash == 0){ 	if(bDots)    *(array + plOff) = 1; }//plot
				else           {    if(bDashes)  *(array + plOff) = 1; }
			}

			err += dy;
			if(err<<1 >= dx) {y+=sy; err-=dx;}

			//used for user controlled early abort.
			cnt++;
			if(cnt>max_iterations) return;

		}
	}
	else  //y is fast axis
	{
		err = (dx - dy) >>1;                           //this reverses the offset introduced at init
		for (x=x0,y=y0; sy< 0 ? y>=y1:y<=y1; y+=sy) {

			if(x>=0 && y>=0 && x<128 && y<128){   //prohibits over/underflow
				if(y%dash == 0){ 	if(bDots)    *(array + x +y*w) = 1; }//plot
				else           {    if(bDashes)  *(array + x +y*w) = 1; }
			}

			err += dx;
			if (err<<1 >= dy) {x+=sx; err-=dy;}

            //used for user controlled early abort.
			cnt++;
			if(cnt>max_iterations) return;

		}
	}
}
void Bresenham_2line(float x0, float y0, float x1, float y1, float x2, float y2, int pw, unsigned char* ptr, int midpoint_ix, int midpoint_iy) {
	//this method is primarily used when it's required of a straight line
	//to have a midpoint calculated.
    //That's currently some cases related to taper in/out. 

	float mx1, mx2, my1, my2;
	float corr = 0.5f;
    float div = 2.0f;
	//float ix=0.0f, iy=0.0f;
	//float f=0.99f;
	float rc=0.01f; //rounding correction
	float fix=(float)midpoint_ix, fiy=(float)midpoint_iy;
	if(fix>0) fix-=rc;
	if(fiy>0) fiy-=rc;
	if(fix<0) fix+=rc;
	if(fiy<0) fiy+=rc;

	//double taper midpoint offsets
	/*
	if(bi==0){ ix=0.0f; iy=0.0f;}
	if(bi==1){ ix=f; 	iy=0.0f;}
	if(bi==2){ ix=0.0f; iy=f;}
	if(bi==3){ ix=f; 	iy=f;}

	if(bi==-1){ix=-f; 	iy=0.0f;}
	if(bi==-2){ix=0.0f; iy=-f;}
	if(bi==-3){ix=-f; 	iy=-f;}
	*/

	if(y0 < y2 && x0 < x2) {
		mx1 = (x0 + x2) / div;
		my1 = (y0 + y2) / div;

		mx2 = (x0 + x2) / div + corr;
		my2 = (y0 + y2) / div + corr;
	}
	else if(y0 >= y2 && x0 < x2) {
		mx1 = (x0 + x2) / div;
		my1 = (y0 + y2) / div + corr;

		mx2 = (x0 + x2) / div + corr;
		my2 = (y0 + y2) / div;
	}
	else if(y0 < y2 && x0 >= x2) {
		mx1 = (x0 + x2) / div + corr;
		my1 = (y0 + y2) / div;

		mx2 = (x0 + x2) / div;
		my2 = (y0 + y2) / div + corr;
	}
	else {
		mx1 = (x0 + x2 ) / div + corr;
		my1 = (y0 + y2 ) / div + corr;

		mx2 = (x0 + x2 ) / div;
		my2 = (y0 + y2 ) / div;
    }

	//apply center taper if valid
	bool bTaperEnable = FormLineDetails->CheckEnableBrush->Checked;
	bool bTaperFromMid= ((FormLineDetails->btnTaperFromMid->Down) && (bTaperEnable));
	bool bTaperIn		= ((FormLineDetails->btnTaperIn->Down) && (bTaperEnable));
	bool bTaperOut	= ((FormLineDetails->btnTaperOut->Down) && (bTaperEnable));
	bool bTaper2		= (bTaperIn && bTaperOut);

	if((bTaperFromMid) && (bTaper2)){
		mx1=x1;
		mx2=x1;
		my1=y1;
		my2=y1;
	}
	else{
		if (lineToolRoster<0) {mx1=mx1-fix; my1=my1-fiy;
						   mx2=mx2-fix; my2=my2-fiy;}
		//only this clause matters, currently. Full condition kept
		//in case i decide to make to make the middle roster choice polar.
		else                  {mx1=mx1+fix; my1=my1+fiy;
							   mx2=mx2+fix; my2=my2+fiy;}
	}
	Bresenham_line(x0, y0, (int)mx1+lineToolAlt, (int)my1+lineToolAlt, pw, ptr, false);
	Bresenham_line((int)mx2+lineToolAlt, (int)my2+lineToolAlt, x2, y2, pw, ptr, false);
	//Bresenham_line(x2, y2, (int)mx2+lineToolAlt, (int)my2+lineToolAlt, pw, ptr);
}
void Bresenham_angle(int x0, int y0, int x1, int y1, int x2, int y2, int w, unsigned char *array, bool knee, bool join, int midpoint_ix, int midpoint_iy) {

	bool bIsRightAngle = (!knee) && join;
	//double taper midpoint offsets
	float rc=0.01f; //rounding correction
	float fix=(float)midpoint_ix, fiy=(float)midpoint_iy;

	if(fix>0) fix-=rc;
	if(fiy>0) fiy-=rc;
	if(fix<0) fix-=rc;
	if(fiy<0) fiy-=rc;

	float f;
	if(knee) //simulates hard knee curve
	{
		f=0.66;        //the rounding error *down* is important.


		int mx = (x0 + x2) / 2;   			//dirty midpoint calc
		int my = (y0 + y2) / 2;

		int dix = (int)((x1 - mx) * f );     //midpoint displacement
		int diy = (int)((y1 - my) * f );
		x1 = mx + dix;
		y1 = my + diy;

	//further operations to soften the knee can be done here
	//if softened, add a bezier to connect.
	}
	else
	{
		x1=(int)x1;
		y1=(int)y1;
		//x1=(int)x1+fix;
		//y1=(int)y1+fiy;

	}

	Bresenham_line(x0,y0, x1,y1,  w, array,bIsRightAngle);
	//Bresenham_line(x1,y1, x2,y2,  w, array);

	if(join) Bresenham_line(x2,y2, x1,y1,  w, array,bIsRightAngle);
	else 	 Bresenham_line(x1,y1, x2,y2,  w, array,bIsRightAngle);
}
int calculateMidpoint(int p1, int p2) {
	return ((p1 + p2 ) / 2) +0.5;
}
void Bresenham_rect(int x0, int y0, int x2, int y2, int w, unsigned char *array) {


	bool bShift		= (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	bool bCtrl		= (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	bool bAlt		= (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

	if(bAlt && (!bShift || !bCtrl)){
		int width = abs(x2 - x0);
		int height = abs(y2 - y0);

		// Find the side length of the square
		int side = (width + height)/2;

		if (x2 > x0) {
			if (y2 > y0) {
				x2 = x0 + side;
				y2 = y0 + side;
			} else {
				x2 = x0 + side;
				y2 = y0 - side;
			}
		} else {
			if (y2 > y0) {
				x2 = x0 - side;
				y2 = y0 + side;
			} else {
				x2 = x0 - side;
				y2 = y0 - side;
			}
		}
	}


	int midpoints[4];
	midpoints[0] = calculateMidpoint(x0, x2); // Midpoint of top side (x-coordinates)
	midpoints[1] = calculateMidpoint(x2, x0); // Midpoint of bottom side (x-coordinates)
	midpoints[2] = calculateMidpoint(y0, y2); // Midpoint of left side (y-coordinates)
	midpoints[3] = calculateMidpoint(y2, y0); // Midpoint of right side (y-coordinates)

	//top
	Bresenham_line(midpoints[0],y0,x0-lineToolX,y0-lineToolY,w,array,true);
	Bresenham_line(midpoints[0],y0,x2+lineToolX,y0-lineToolY,w,array,true);

	//bottom
	Bresenham_line(midpoints[1],y2,x0-lineToolX,y2+lineToolY,w,array,true);
	Bresenham_line(midpoints[1],y2,x2+lineToolX,y2+lineToolY,w,array,true);

	//left
	Bresenham_line(x0,midpoints[2],x0-lineToolX,y0-lineToolY,w,array,true);
	Bresenham_line(x0,midpoints[2],x0-lineToolX,y2+lineToolY,w,array,true);

	//right
	Bresenham_line(x2,midpoints[3],x2+lineToolX,y0-lineToolY,w,array,true);
	Bresenham_line(x2,midpoints[3],x2+lineToolX,y2+lineToolY,w,array,true);

}




void plotQuadBezierSeg(int x0, int y0, int x1, int y1, int x2, int y2, int w, unsigned char *array)
{ /* plot a limited quadratic Bezier segment */
    int cnt=0;
	int dash = lineDashLen +1;
	bool bDots = FormLineDetails->btnDots->Down;
	bool bDashes  = FormLineDetails->btnDashes->Down;
	if (bDots==false && bDashes==false) {bDots=true; bDashes=true; }


	int sx = x2-x1, sy = y2-y1;
	long xx = x0-x1, yy = y0-y1, xy; /* relative values for checks */
	double dx, dy, err, cur = xx*sy-yy*sx; /* curvature */


	//assert(xx*sx <= 0 && yy*sy <= 0); /* sign of gradient must not change */
	if(!(xx*sx <= 0 && yy*sy <= 0)) return;

	if (sx*(long)sx+sy*(long)sy > xx*xx+yy*yy) { /* begin with longer part */
		x2 = x0; x0 = sx+x1; y2 = y0; y0 = sy+y1; cur = -cur; /* swap P0 P2 */
	}
	if (cur != 0) { /* no straight line */
		xx += sx; xx *= sx = x0 < x2 ? 1 : -1; /* x step direction */
		yy += sy; yy *= sy = y0 < y2 ? 1 : -1; /* y step direction */
		xy = 2*xx*yy; xx *= xx; yy *= yy; /* differences 2nd degree */
		if (cur*sx*sy < 0) { /* negated curvature? */
			xx = -xx; yy = -yy; xy = -xy; cur = -cur;
		}
		dx = 4.0*sy*cur*(x1-x0)+xx-xy; /* differences 1st degree */
		dy = 4.0*sx*cur*(y0-y1)+yy-xy;
		xx += xx; yy += yy; err = dx+dy+xy; /* error 1st step */

		do {
			// plot curve
			int plOff = max(min(y0*w + x0,128*128-1),0);

			if(x0>=0 && y0>=0 && x0<128 && y0<128){
				if(cnt%dash == 0){ 	   if(bDots)    *(array+plOff) = 1; }
				else              {    if(bDashes)  *(array+plOff) = 1; }
			}

			cnt++;
			if (x0 == x2 && y0 == y2) return; // last pixel -> curve finished
			y1 = 2*err < dx; // save value for test of y step
			if (2*err > dy) { x0 += sx; dx -= xy; err += dy += yy; } // x step
			if ( y1 ) { y0 += sy; dy -= xy; err += dx += xx; } // y step
			if (cnt>256) break;
		} //while ((dy < 0 && dx > 0) || (dx < 0 && abs(dx) > abs(x2-x0)) || (dy > 0 && abs(dy) > abs(y2-y0)));
		while (dy < 0 && dx > 0); // gradient negates -> algorithm fails

	}
 Bresenham_line(x0,y0, x2,y2,  w, array,false); /* plot remaining part to end */
}

void plotFineQuadBezier(int x0, int y0, int x1, int y1, int x2, int y2, int w, unsigned char *array)
{
	int cnt=0;
	int dash = lineDashLen +1;
	bool bDots = FormLineDetails->btnDots->Down;
	bool bDashes  = FormLineDetails->btnDashes->Down;
	if (bDots==false && bDashes==false) {bDots=true; bDashes=true; }


	int sx = x0<x2 ? 1 : -1, sy = y0<y2 ? 1 : -1; /* step direction */
	long f = 1, fx = x0-2*x1+x2, fy = y0-2*y1+y2;
	long long x = 2*fx*fx, y = 2*fy*fy, xy = 2*fx*fy*sx*sy;
	long long cur = sx*sy*(fx*(y2-y0)-fy*(x2-x0)); /* curvature */
	/* compute error increments of P0 */
	long long dx = abs(y0-y1)*xy-abs(x0-x1)*y-cur*abs(y0-y2);
	long long dy = abs(x0-x1)*xy-abs(y0-y1)*x+cur*abs(x0-x2);
	/* compute error increments of P2 */
	long long ex = abs(y2-y1)*xy-abs(x2-x1)*y+cur*abs(y0-y2);
	long long ey = abs(x2-x1)*xy-abs(y2-y1)*x-cur*abs(x0-x2);

	/* sign of gradient must not change */
	//assert((x0-x1)*(x2-x1) <= 0 && (y0-y1)*(y2-y1) <= 0);
	if(!((x0-x1)*(x2-x1) <= 0 && (y0-y1)*(y2-y1) <= 0)) return;

	if (cur == 0) { Bresenham_line(x0,y0, x2,y2,  w, array,false); return; } /* straight line */
	/* compute required minimum resolution factor */
	if (dx == 0 || dy == 0 || ex == 0 || ey == 0)
		f = abs(xy/cur)/2+1; /* division by zero: use curvature */
	else {
		fx = 2*y/dx; if (fx > f) f = fx; /* increase resolution */
		fx = 2*x/dy; if (fx > f) f = fx;
		fx = 2*y/ex; if (fx > f) f = fx;
		fx = 2*x/ey; if (fx > f) f = fx;
	} /* negated curvature? */
	if (cur < 0) { x = -x; y = -y; dx = -dx; dy = -dy; xy = -xy; }
	dx = f*dx+y/2-xy; dy = f*dy+x/2-xy; ex = dx+dy+xy; /* error 1.step */

	for (fx = fy = f; ; ) { /* plot curve */
		/* plot curve */

		int plOff = max(min(y0*w + x0,128*128-1),0);

		if(x>=0 && y>=0 && x<128 && y<128){
			if(cnt%dash == 0){ 	   if(bDots)    *(array+plOff) = 1; }
			else              {    if(bDashes)  *(array+plOff) = 1; }
		}
		cnt++;


		if (x0 == x2 && y0 == y2) break;
		int iter=0;
		do { /* move f sub-pixel */

			ey = 2*ex-dy; /* save value for test of y step */
			if (2*ex >= dx) { fx--; dy -= xy; ex += dx += y; } /* x step */
			if (ey <= 0) { fy--; dx -= xy; ex += dy += x; } /* y step */
			iter++;
			if(iter>256) break;
		} while (fx > 0 && fy > 0); /* pixel complete? */
		if (2*fx <= f) { x0 += sx; fx += f; } /* sufficient sub-steps.. */
		if (2*fy <= f) { y0 += sy; fy += f; } /* .. for a pixel? */
	}
}
void plotQuadBezier(int x0, int y0, int x1, int y1, int x2, int y2, int w, unsigned char *array, int midpoint_ix, int midpoint_iy)
{
	//int dash = lineDashLen +1;
	//bool bDots = FormLineDetails->btnDots->Down;
	//bool bDashes  = FormLineDetails->btnDashes->Down;
	//if (bDots==false && bDashes==false) {bDots=true; bDashes=true; }

	/*
	int ix, iy;
	//double taper midpoint offsets
	if(bi==0){ ix=0; iy=0;}

	if(bi==1){ ix=1; iy=0;}
	if(bi==2){ ix=0; iy=1;}
	if(bi==3){ ix=1; iy=1;}

	if(bi==-1){ ix=-1; iy=0;}
	if(bi==-2){ ix=0; iy=-1;}
	if(bi==-3){ ix=-1; iy=-1;}

	//secret sauce
	//tips the curve in aesthetic favour
	float f=0.99;

	/*
	float mpOff=lineToolMid;
	if (mpOff != 0.0) {
	// Calculate the displacement vector
	int dx = (int)(mpOff / 16.0 * (x2 - x0));
	int dy = (int)(mpOff / 16.0 * (y2 - y0));

	// Add the displacement to the midpoint
	mx += dx;
	my += dy;
	}
	*/

    /*
	int mx = (x0 + x2) / 2;   			//midpoint calc
	int my = (y0 + y2) / 2;
	int dix = (int)((x1 - mx) * f);     //midpoint displacement
	int diy = (int)((y1 - my) * f);
	x1 = mx + dix + ix;
	y1 = my + diy + iy;
	*/


	//algo begin

	/* plot any quadratic Bezier curve */
    int x = x0-x1, y = y0-y1;
	double t = x0-2*x1+x2, r;
    if ((long)x*(x2-x1) > 0) { /* horizontal cut at P4? */
        if ((long)y*(y2-y1) > 0) /* vertical cut at P6 too? */
        if (fabs((y0-2*y1+y2)/t*x) > abs(y)) { /* which first? */
            x0 = x2; x2 = x+x1; y0 = y2; y2 = y+y1; /* swap points */
        } /* now horizontal cut at P4 comes first */
		t = (x0-x1)/t;
		r = (1-t)*((1-t)*y0+2.0*t*y1)+t*t*y2; /* By(t=P4) */
		t = (x0*x2-x1*x1)*t/(x0-x1); /* gradient dP4/dx=0 */
		x = floor(t+0.5); y = floor(r+0.5);
		r = (y1-y0)*(t-x0)/(x1-x0)+y0; /* intersect P3 | P0 P1 */
		plotQuadBezierSeg(x0,y0, x,floor(r+0.5), x,y,  w, array);
		r = (y1-y2)*(t-x2)/(x1-x2)+y2; /* intersect P4 | P1 P2 */
		x0 = x1 = x; y0 = y; y1 = floor(r+0.5); /* P0 = P4, P1 = P8 */
	}
	if ((long)(y0-y1)*(y2-y1) > 0) { /* vertical cut at P6? */
		t = y0-2*y1+y2; t = (y0-y1)/t;
		r = (1-t)*((1-t)*x0+2.0*t*x1)+t*t*x2; /* Bx(t=P6) */
		t = (y0*y2-y1*y1)*t/(y0-y1); /* gradient dP6/dy=0 */
		x = floor(r+0.5); y = floor(t+0.5);
		r = (x1-x0)*(t-y0)/(y1-y0)+x0; /* intersect P6 | P0 P1 */
		plotQuadBezierSeg(x0,y0, floor(r+0.5),y, x,y,  w, array);
		r = (x1-x2)*(t-y2)/(y1-y2)+x2; /* intersect P7 | P1 P2 */
		x0 = x; x1 = floor(r+0.5); y0 = y1 = y; /* P0 = P6, P1 = P7 */
	}
 plotQuadBezierSeg(x0,y0, x1,y1, x2,y2,  w, array); /* remaining part */
}

void Bezier_curve(int x0, int y0, int x1, int y1, int x2, int y2, int w, unsigned char *array, int midpoint_ix, int midpoint_iy) {

	int dash = lineDashLen +1;
	bool bDots = FormLineDetails->btnDots->Down;
	bool bDashes  = FormLineDetails->btnDashes->Down;
	if (bDots==false && bDashes==false) {bDots=true; bDashes=true; }

	int ix=midpoint_ix;
	int iy=midpoint_iy;
	//double taper midpoint offsets
	/*
	if(bi==0){ ix=0; iy=0;}

	if(bi==1){ ix=1; iy=0;}
	if(bi==2){ ix=0; iy=1;}
	if(bi==3){ ix=1; iy=1;}

	if(bi==-1){ ix=-1; iy=0;}
	if(bi==-2){ ix=0; iy=-1;}
	if(bi==-3){ ix=-1; iy=-1;}
	*/


	//secret sauce
	//tips the curve in aesthetic favour
	float f=0.99;




	int mx = (x0 + x2) / 2;   			//midpoint calc
	int my = (y0 + y2) / 2;
	int dix = (int)((x1 - mx) * f);     //midpoint displacement
	int diy = (int)((y1 - my) * f);
	//
	/*
	float mpOff=lineToolMid;
	if (mpOff != 0.0) {
	// Calculate the displacement vector
	int dx = (int)(mpOff / 16.0 * (x2 - x0));
	int dy = (int)(mpOff / 16.0 * (y2 - y0));

	// Add the displacement to the midpoint
	mx += dx;
	my += dy;
	}
	*/
	int tmp_x1 = x1 +ix;  //this is for quad bezier when regular fails.
	int tmp_y1 = y1 +iy;

	x1 = mx + dix + ix;
	y1 = my + diy + iy;
	//the following is based on Alois Zingl's line drawing Bezier.

	int sx = x0<x2 ? 1 : -1;
	int sy = y0<y2 ? 1 : -1; 				// step direction

	double x = x0-2*x1+x2;
	double y = y0-2*y1+y2;

	double xy = 2*x*y*sx*sy;
	double cur = sx*sy*(x*(y2-y0)-y*(x2-x0))/2; 	// curvature

	/* compute error increments of P0 */
	double dx = (1-2*abs(x0-x1))*y*y+abs(y0-y1)*xy-2*cur*abs(y0-y2);
	double dy = (1-2*abs(y0-y1))*x*x+abs(x0-x1)*xy+2*cur*abs(x0-x2);

	/* compute error increments of P2 */
	double ex = (1-2*abs(x2-x1))*y*y+abs(y2-y1)*xy+2*cur*abs(y0-y2);
	double ey = (1-2*abs(y2-y1))*x*x+abs(x2-x1)*xy-2*cur*abs(x0-x2);

	/* sign of gradient must not change */
	assert((x0-x1)*(x2-x1) <= 0 && (y0-y1)*(y2-y1) <= 0);
	//if(!((x0-x1)*(x2-x1) <= 0 && (y0-y1)*(y2-y1) <= 0)) return;

	if (cur==0) { Bresenham_line(x0,y0, x2,y2, w, array,false); return; } // straight line

	x *= 2*x; y *= 2*y;
	if (cur < 0) { /* negated curvature */
		x = -x; dx = -dx; ex = -ex; xy = -xy;
		y = -y; dy = -dy; ey = -ey;
	}

	/* algorithm fails for almost straight line, check error values */


	if (dx >= -y || dy <= -x || ex <= -y || ey >= -x) {

		// approximation


		x1 = (x0+4*x1+x2)/6;
		y1 = (y0+4*y1+y2)/6;

		//Bresenham_line(x0,y0, x1,y1,  w, array);
		//Bresenham_line(x1,y1, x2,y2,  w, array);

		plotQuadBezier(x0,y0, tmp_x1,tmp_y1, x2,y2, w, array,midpoint_ix,midpoint_iy);
		//plotFineQuadBezier(x0,y0, x1,y1, x2,y2, w, array);  //can't handle sharp curve after all
		return;
	}


	dx -= xy; ex = dx+dy; dy -= xy; /* error of 1.step */
	int cnt=0;

	for(;;) { // plot curve

		//*(array + y0*w + x0) = 1;
		int plOff = max(min(y0*w + x0,128*128-1),0);

		if(x0>=0 && y0>=0 && x0<128 && y0<128){
			if(cnt%dash == 0){ 	if(bDots)    *(array + plOff) = 1; }//plot
			else              {    if(bDashes)  *(array + plOff) = 1; }
		}


		cnt++;
		ey = 2*ex-dy; /* save value for test of y step */
		if (2*ex >= dx) { /* x step */
			if (x0 == x2) break;
			x0 += sx; dy -= xy; ex += dx += y;
		}
		if (ey <= 0) { /* y step */
			if (y0 == y2) break;
			y0 += sy; dx -= xy; ex += dy += x;
		}
	if(cnt>256) break;
	}
}

void Hyperbola_curve(int x0, int y0, int x1, int y1, int x2, int y2, int w, unsigned char *array, int midpoint_ix, int midpoint_iy, int type) {


	int qx0,qx2,qy0,qy2;
	//hyperbole/s-curve

	if(type==abs(5)) {
		qx0 = (x0);
		qy0 = (y1);

		qx2 = (x2);
		qy2 = (y1);
	}


	//straight line behaviour -exploits that the bezier curve faults to bresen lines if angle is too shallow.
	else if(type==abs(6)) {
		qx0 = (x0 + x1) / 2;
		qy0 = (y0 + y1) / 2;

		qx2 = (x2 + x1) / 2;
		qy2 = (y2 + y1) / 2;
	}
	//hyperbolic concave (hypercave for short)
	else if(type==abs(7)) {
		qx0 = (x1);
		qy0 = (y0);

		qx2 = (x1);
		qy2 = (y2);
	}

	Bezier_curve(x0, y0, qx0, qy0, x1, y1, w, array, midpoint_ix, midpoint_iy);
	Bezier_curve(x1, y1, qx2, qy2, x2, y2, w, array, midpoint_ix, midpoint_iy);
}
void Line_Circle(int x0, int y0, int x2, int y2, int w, unsigned char *array) {

	int ix=0;
	int iy=0;

	bool bShift		= (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	bool bCtrl		= (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	bool bAlt		= (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
	if(bAlt && !(bShift || bCtrl)){
		int width = abs(x2 - x0);
		int height = abs(y2 - y0);

		// Find the side length of the square
		int side = (width + height)/2;

		if (x2 > x0) {
			if (y2 > y0) {
				x2 = x0 + side;
				y2 = y0 + side;
			} else {
				x2 = x0 + side;
				y2 = y0 - side;
			}
		} else {
			if (y2 > y0) {
				x2 = x0 - side;
				y2 = y0 + side;
			} else {
				x2 = x0 - side;
				y2 = y0 - side;
			}
		}
	}



	int midpoints[4];
	midpoints[0] = calculateMidpoint(x0, x2); // Midpoint of top side (x-coordinates)
	midpoints[1] = calculateMidpoint(x2, x0); // Midpoint of bottom side (x-coordinates)
	midpoints[2] = calculateMidpoint(y0, y2); // Midpoint of left side (y-coordinates)
	midpoints[3] = calculateMidpoint(y2, y0); // Midpoint of right side (y-coordinates)

	//top
	Bezier_curve(midpoints[0], y0, x0-lineToolX, y0-lineToolY, x0, midpoints[2], w, array, ix, iy);
	Bezier_curve(midpoints[0], y0, x2+lineToolX, y0-lineToolY, x2, midpoints[3], w, array, ix, iy);

	//bottom
	Bezier_curve(midpoints[1], y2, x0-lineToolX, y2+lineToolY, x0, midpoints[2], w, array, ix, iy);
	Bezier_curve(midpoints[1], y2, x2+lineToolX, y2+lineToolY, x2, midpoints[3], w, array, ix, iy);


}
void __fastcall TFormMain::WorkCHRToBankCHR(void)
{
	/* legacied.
	int chunk=256;
	for(int i=0;i<16;i++) //transfer from bank to work
	{
		memcpy(chrBank+(chunk*chrA_id[i])  ,chr+(i*chunk)	,chunk);
		memcpy(chrBank+(chunk*chrB_id[i])  ,chr+4096+(i*chunk)	,chunk);
	}

	//verify
	//BankCHRToWorkCHR();

	if(FormBankCHR != NULL) if(FormBankCHR->Visible) FormBankCHR->DrawTimer->Enabled=true;
	*/
}
void __fastcall TFormMain::BankCHRToWorkCHR(void)
{
	/*
	int chunk=256;
	for(int i=0;i<16;i++) //transfer from bank to work
	{
		memcpy(chr+(i*chunk)		,chrBank+(chunk*chrA_id[i])	,chunk);
		memcpy(chr+4096+(i*chunk)	,chrBank+(chunk*chrB_id[i])	,chunk);
	}
	*/

}



void __fastcall TFormMain::pal_validate()
{
	int i,j,col;

	//sets the background colour to common
	if(sharedCol0)
	{
		col=bgPal[palBank*16+0*4+0];

		bgPal[palBank*16+1*4+0]=col;
		bgPal[palBank*16+2*4+0]=col;
		bgPal[palBank*16+3*4+0]=col;
	}

	//makes illegal or unsafe colours unavailable
	if(safeColsOnly)
	{
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				switch(bgPal[palBank*16+i*4+j])    //fg todo: make this optional
				{
					case 0x0d:
					case 0x1d:
					case 0x0e:
					case 0x1e:
					case 0x2e:
					case 0x3e:
					case 0x1f:
					case 0x2f:
					case 0x3f:
					bgPal[palBank*16+i*4+j]=colBlack;
					break;
				}
			}
		}
	}
}


void palette_calc(void)
{
	int i;
	float r,g,b;

		for(i=0;i<64;i++)
		{
			r=((float)((basePalette[i]>>16)&0xff))/255.0;
			g=((float)((basePalette[i]>>8)&0xff))/255.0;
			b=((float)(basePalette[i]&0xff))/255.0;
			r=r*emphasis[ppuMask>>5][0]/100.0;
			g=g*emphasis[ppuMask>>5][1]/100.0;
			b=b*emphasis[ppuMask>>5][2]/100.0;
			if(r>1.0) r=1.0;
			if(g>1.0) g=1.0;
			if(b>1.0) b=1.0;
			outPalette[i]=(((int)(255.0*r))<<16)|(((int)(255.0*g))<<8)|((int)(255.0*b));
		}

	if(ppuMask&0x01)
	{
	   for(i=0;i<64;i++)
		{
			outPalette[i]=outPalette[i&0xf0];

		}
	}

	/*
	the table found in the array 'emphasis'
	is the one from nintech.txt, which credits Chris Covell

	001        B: 074.3%        G: 091.5%        R: 123.9%
	010        B: 088.2%        G: 108.6%        R: 079.4%
	011        B: 065.3%        G: 098.0%        R: 101.9%
	100        B: 127.7%        G: 102.6%        R: 090.5%
	101        B: 097.9%        G: 090.8%        R: 102.3%
	110        B: 100.1%        G: 098.7%        R: 074.1%
	111        B: 075.0%        G: 075.0%        R: 075.0%


	*/


}



AnsiString RemoveExt(AnsiString name)
{
	return ChangeFileExt(name,"");
}



AnsiString GetExt(AnsiString name)
{
	name=ExtractFileName(name);

	return name.SubString(name.LastDelimiter(".")+1,name.Length()-name.LastDelimiter(".")).LowerCase();
}



void __fastcall TFormMain::ClearNametable(bool useNulltile)
{
	int tmp	= useNulltile ? nullTile : 0 ;

	memset(nameTable,tmp,sizeof(nameTable));
	memset(attrTable,0,sizeof(attrTable));
}



void __fastcall TFormMain::DrawCol(int x,int y,int w, int h,int c,bool sel)
{
	TRect r;

	r.left  =x;
	r.top   =y;
	r.right =x+w;
	r.Bottom=y+h;

	PaintBoxPal->Canvas->Brush->Color=TColor(outPalette[c]);
	PaintBoxPal->Canvas->FillRect(r);

	if(sel)
	{
		PaintBoxPal->Canvas->Pen->Color=TColor(0xffffff);
		PaintBoxPal->Canvas->Rectangle(r);
		PaintBoxPal->Canvas->Pen->Color=TColor(0);

		r.left  +=1;
		r.top   +=1;
		r.right -=1;
		r.bottom-=1;

		PaintBoxPal->Canvas->Rectangle(r);
	}
}



void __fastcall TFormMain::DrawBGPal(int x,int y,int pal)
{
    //if changing dimensions, remember to also change click areas. 
	int w=20 -24 +12*uiScale;
	int h=20 -24 +12*uiScale;



	DrawCol(x   ,y,w,h,bgPal[palBank*16+pal*4+0],pal==palActive&&bgPalCur==0?true:false);
	DrawCol(x+w,y,w,h,bgPal[palBank*16+pal*4+1],pal==palActive&&bgPalCur==1?true:false);
	DrawCol(x+w*2,y,w,h,bgPal[palBank*16+pal*4+2],pal==palActive&&bgPalCur==2?true:false);
	DrawCol(x+w*3,y,w,h,bgPal[palBank*16+pal*4+3],pal==palActive&&bgPalCur==3?true:false);
}

void __fastcall TFormMain::EnableDisableTypeConflictShortcuts(bool forceDisable)
{
    bool a = SpeedButtonTypeIn->Down;
    bool b = forceDisable;
	Renamecurrentmetasprite1->ShortCut=(a||b)?  TextToShortCut("(None)"):TextToShortCut("Shift+R");
	Renameallmetasprites1->ShortCut=(a||b)?  TextToShortCut("(None)"):TextToShortCut("Shift+Alt+R");

	Colour001->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("1");
	Colour011->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("2");
	Colour101->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("3");
	Colour111->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("4");

	Protectcolour01->ShortCut=(a||b)? TextToShortCut("(None)"):TextToShortCut("5");
	Protectcolour11->ShortCut=(a||b)? TextToShortCut("(None)"):TextToShortCut("6");
	Protectcolour21->ShortCut=(a||b)? TextToShortCut("(None)"):TextToShortCut("7");
	Protectcolour31->ShortCut=(a||b)? TextToShortCut("(None)"):TextToShortCut("8");

	PenMode1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("9");
	PenMode2->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("0");

	Pal0->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+1");
	Pal1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+2");
	Pal2->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+3");
	Pal3->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+4");

	Pal01->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+1");
	Pal11->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+2");
	Pal21->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+3");
	Pal31->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+4");

	MPalA->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+5");
    MPalB->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+6");
	MPalC->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+7");
	MPalD->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+8");

	PresetA1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Ctrl+Alt+1");
	PresetB1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Ctrl+Alt+2");
	PresetC1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Ctrl+Alt+3");
	PresetD1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Ctrl+Alt+4");

	Openascurrentset1->ShortCut=(a||b)?         TextToShortCut("(None)"):TextToShortCut("Shift+O");
	MPaletteOpen->ShortCut=(a||b)?              TextToShortCut("(None)"):TextToShortCut("Shift+O");

	Savecurrentset1->ShortCut=(a||b)?           TextToShortCut("(None)"):TextToShortCut("Shift+S");
	MPaletteSave->ShortCut=(a||b)?              TextToShortCut("(None)"):TextToShortCut("Shift+S");

	//MCHRSaveLatest->ShortCut=(a||b)?			TextToShortCut("(None)"):TextToShortCut("Shift+S");
	IncDecCap1->ShortCut=(a||b)?				TextToShortCut("(None)"):TextToShortCut("Shift+M");
	MCHRFreqSort->ShortCut=(a||b)?				TextToShortCut("(None)"):TextToShortCut("Shift+F");
	MCHRDensitySort->ShortCut=(a||b)?			TextToShortCut("(None)"):TextToShortCut("Shift+D");
	MPaletteResetGrayscale->ShortCut=(a||b)?	TextToShortCut("(None)"):TextToShortCut("Shift+G");
	MPaletteCopy->ShortCut=(a||b)?				TextToShortCut("(None)"):TextToShortCut("Shift+C");
	MPalettePaste->ShortCut=(a||b)?				TextToShortCut("(None)"):TextToShortCut("Shift+V");

	Fill1->ShortCut=(a||b)?	            TextToShortCut("(None)"):TextToShortCut("F"); 
    Delete1->ShortCut=(a||b)?	        TextToShortCut("(None)"):TextToShortCut("Del");
    N2x2tileeditmode1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+E");
    
    Drawontilesetmodeonoff1->ShortCut=(a||b)?	   TextToShortCut("(None)"):TextToShortCut("Shift+D");
    Reverttocheckpoint1->ShortCut=(a||b)?	       TextToShortCut("(None)"):TextToShortCut("Shift+Z");
    
	TogglePenBrush1->ShortCut=(a||b)?	 TextToShortCut("(None)"):TextToShortCut("Shift+B");
    Quantized1->ShortCut=(a||b)?	     TextToShortCut("(None)"):TextToShortCut("Shift+U");
	Linemode1->ShortCut=(a||b)?     TextToShortCut("(None)"):TextToShortCut("Shift+N");

	Recallcolour1->ShortCut=(a||b)?	     TextToShortCut("(None)"):TextToShortCut("Z");



}

void __fastcall TFormMain::DrawPalettes(void)
{
    //used to draw the Paintbox area for subpalettes, as well as the system palette.

	int i,j,x,y,pp,col;

	if(!Visible) return;
	int rc = 0 -96 + 48*uiScale;

	DrawBGPal( 16, 0,0);
	DrawBGPal(128+rc, 0,1);
	DrawBGPal( 18,32+rc/4,2);
	DrawBGPal(128+rc,32+rc/4,3);

	y=64;
	pp=0;

	col=bgPal[palBank*16+palActive*4+bgPalCur];

	//if(col==0x0f) col=0x0d;

	int w=16 -16 +8*uiScale;
	int h=16 -16 +8*uiScale;


	for(i=0;i<4;i++)
	{
		x=0;

		for(j=0;j<16;j++)
		{
			if (j<16)   //was 14;
			{
				DrawCol(x,y+rc/2,w,h,pp,pp==col);   //x,y,size,c,sel

				pp++;

			}
			else {DrawCol(x,y+rc/2,w,h,13,i*16+j==col);}
			x+=w;
		}

		//pp+=2;
		y+=h;
	}
}



void __fastcall TFormMain::DrawTile(TPicture *pic,int x,int y,unsigned int tile,unsigned int highlight_tile, int pal,int tx,int ty,bool sel, bool efficientTarget, int inputScale, bool bIsNav, bool isMeta, bool doubleWidth, bool banked, bool isBankSwap)
{


	//if(highlight_tile>255) ShowMessage(IntToStr(highlight_tile));
	//highlight_tile=highlight_tile%256;    //todo: this should not be needed anymore. test.

	int j,k,l,m,pp,col,r,g,b;
	unsigned char *dst;
	int rr[8],gg[8],bb[8];

	int chk_r[8],chk_g[8],chk_b[8];  //attr checker
	int avgr[4],avgg[4],avgb[4];  //^--avg checker
	int tpal; //separation subpal index.
	int sep=faderAttrCheckerSeparation[iAttrCheckerMethod];
	int tile_o=100 - faderAttrCheckerOpacity[iAttrCheckerMethod]; //opacity blender value for tile.
	int attr_o=faderAttrCheckerOpacity[iAttrCheckerMethod]; //opacity blender value for attr checker.

	int hgrid,vgrid,hcol,vcol;
	int scale=inputScale;
	int tyTmp;




	bool bOn = SpeedButtonGridAll->Down;
	bool bNamDrag = (ImageName->Dragging() || FormNavigator->Map1->Dragging());
	bool bSilhouette=btnMask->Down;
	bool bAuto = AutoViewDragMode1->Checked;
	bool btiletab = PageControlEditor->ActivePage==TabSheetTile?true:false;
	bool bEfficient = btiletab&&efficientTarget==true&&(uiScale==2);

	bool doHighlight = (hoverBtn>=0 || mtHoverBtn>=0 || bColissionIDHover>=0);
	bool checkerTarget = (tx>=0&&ty>=0); //assumed nametable.
	bool doChecker = ((SpeedButtonChecker->Down) && (faderAttrCheckerOpacity[iAttrCheckerMethod]>0) && checkerTarget);

	int set = bankActive/16;
	bool highlight;
	/*
	bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
	bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
	bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;
	*/

	if(doHighlight){

		if(bColissionIDHover)
		{ ///*
			int tmp_propID=0;
			if(FormPropID->btnHov->Down) tmp_propID = iPropIDHoveredItem;
			else if(FormPropID->btnSel) tmp_propID = FormPropID->ListBox1->ItemIndex;

			if(FormPropID->btnAll)

			{
				if(tx<0&&ty<0)  //target is Tileset canvas(es) - this assumption may be reevaluated with metatiles...
				{
					if(!banked) highlight= (   collisionID_R[tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile]] !=0xFF
											|| collisionID_G[tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile]] !=0xFF
											|| collisionID_B[tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile]] !=0xFF
											);


					else highlight= (   collisionID_R[tileProperties[curViewCHRbankCanvas2*256+highlight_tile]] != 0xFF
									 || collisionID_G[tileProperties[curViewCHRbankCanvas2*256+highlight_tile]] != 0xFF
									 || collisionID_B[tileProperties[curViewCHRbankCanvas2*256+highlight_tile]] != 0xFF
									);
				}
				else if (isMeta)
				{
				
				if(!banked){
						if(bMTuseDirectTile) highlight = (collisionID_R[tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile]] != 0xFF
														 || collisionID_G[tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile]] != 0xFF
														 || collisionID_B[tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile]] != 0xFF
														);


						else highlight = (collisionID_R[highlight_mt] != 0xFF
										||  collisionID_G[highlight_mt] != 0xFF
										||  collisionID_B[highlight_mt] != 0xFF
										);


					}
					else{
						if(bMTuseDirectTile) highlight = (collisionID_R[tileProperties[set+highlight_tile]] != 0xFF
											||			  collisionID_G[tileProperties[set+highlight_tile]] != 0xFF
											||			  collisionID_B[tileProperties[set+highlight_tile]] != 0xFF
											);


						else highlight = (collisionID_R[highlight_mt] != 0xFF
										  || collisionID_G[highlight_mt] != 0xFF
										  || collisionID_B[highlight_mt] != 0xFF
										);
					}
				}
			}
			//selected or hovered item .
			else{


				if(tx<0&&ty<0)  //target is Tileset canvas(es) - this assumption may be reevaluated with metatiles...
				{
					if(!banked) highlight= (tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile] == tmp_propID);
					else        highlight= (tileProperties[curViewCHRbankCanvas2*256+highlight_tile]		    == tmp_propID);
				}
				else if (isMeta)
				{
					if(!banked){
						if(bMTuseDirectTile) highlight = (tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile] == tmp_propID);
						else highlight = (highlight_mt == (unsigned) tmp_propID);
					}
					else{
						if(bMTuseDirectTile) highlight = (tileProperties[set+highlight_tile] == tmp_propID);
						else highlight = (highlight_mt == (unsigned) tmp_propID);
					}
				} }  //*/
		}
		else if(bMTpropHover)
		{

			 if(collision_specific){
				//highlights all tiles of this particular bit combination
				//fix: store the bit result in global var.
				//highlight= (tmpTable[set+highlight_mt]==tileProperties[set+tileActive]);
			}
			else  //Individual bits!
			{
				//highlights all tiles of this particular bit button

				//highlight = (tmpTable[highlight_mt] & (1 << (mtHoverBtn))) != 0;
				if(!banked){
					if(bMTuseDirectTile) highlight = (tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile] & (1 << (mtHoverBtn))) != 0;
					else highlight = (highlight_mt & (1 << (mtHoverBtn))) != 0;
				}
				else{
					if(bMTuseDirectTile) highlight = (tileProperties[set+highlight_tile] & (1 << (mtHoverBtn))) != 0;
					else highlight = (highlight_mt & (1 << (mtHoverBtn))) != 0;
				}

				if(tx<0&&ty<0)  //target is Tileset canvas(es) - this assumption may be reevaluated with metatiles...
				{
					//AND with current bits' condition, if any.
					if(FormPropConditions->chkTiles->Checked && (propConditional[mtHoverBtn]>0))
						highlight = highlight && propCondition[pal][mtHoverBtn];

					//AND with any conditionals set to "all"
					for(int bitcount=0;bitcount<8;bitcount++){
						if(FormPropConditions->chkTiles->Checked && (propConditional[bitcount]==2 && btnStateProps[bitcount]))
						highlight = highlight && propCondition[pal][bitcount];
				   }
				}
				else if (isMeta)
				{
					//AND with current bits' condition, if any.
					if(FormPropConditions->chkMap->Checked && (propConditional[mtHoverBtn]>0))
						highlight = highlight && propCondition[pal][mtHoverBtn];

					//AND with any conditionals set to "all"
					for(int bitcount=0;bitcount<8;bitcount++){
					if(FormPropConditions->chkMap->Checked && (propConditional[bitcount]==2 && btnStateProps[bitcount]))
					highlight = highlight && propCondition[pal][bitcount];
					}
				}
			}  //*/

		}
		else{   //direct tile collision
		//bug is in this clause
			if(collision_specific){
				//*
				//highlights all tiles of this particular bit combination
				if(!banked) highlight= (tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile]==tileProperties[bankViewTable[set+tileActive]/16+tileActive]);
				else        highlight= (tileProperties[curViewCHRbankCanvas2*256+highlight_tile]		   == tileProperties[curViewCHRbankCanvas2*256+tileActive]);

				if(isMeta) highlight = highlight && FormCHRbit->btnShowMeta->Down;
				else if(tx<0&&ty<0) highlight = highlight && FormCHRbit->btnShowCHR->Down;
				else       highlight = highlight && FormCHRbit->btnShowScreen->Down;
				//*/
			}

			else  //Individual bits!
			{

				//highlights all tiles of this particular bit button
				//possible issue.
				if(!banked) highlight = false;//(tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile] & (1 << (hoverBtn))) != 0;
				else 		highlight = (tileProperties[curViewCHRbankCanvas2*256+highlight_tile] 			 & (1 << (hoverBtn))) != 0;



				if(isMeta)
				{
					//*
					highlight = highlight && FormCHRbit->btnShowMeta->Down;
					//AND with current bits' condition, if any.
					if(FormPropConditions->chkMetas->Checked && (propConditional[hoverBtn]>0))
						highlight = highlight && propCondition[pal][hoverBtn];

					//AND with any conditionals set to "all"
					for(int bitcount=0;bitcount<8;bitcount++){
						if(FormPropConditions->chkMetas->Checked && (propConditional[bitcount]==2 && btnStateProps[bitcount]))
						highlight = highlight && propCondition[pal][bitcount];}
				  //*/
				}
				else if(tx<0&&ty<0)  //target is Tileset canvas(es)
				{
					//*
					highlight = highlight && FormCHRbit->btnShowCHR->Down;
					//AND with current bits' condition, if any.
					if(FormPropConditions->chkTiles->Checked && (propConditional[hoverBtn]>0))
						highlight = highlight && propCondition[pal][hoverBtn];

					//AND with any conditionals set to "all"
					for(int bitcount=0;bitcount<8;bitcount++){
						if(FormPropConditions->chkTiles->Checked && (propConditional[bitcount]==2 && btnStateProps[bitcount]))
						highlight = highlight && propCondition[pal][bitcount];
					}
				   //*/
				}
				else
				{
					//*
					//target is assumed map
					highlight = highlight && FormCHRbit->btnShowScreen->Down;
					//AND with current bits' condition, if any.
					if(FormPropConditions->chkMap->Checked && (propConditional[hoverBtn]>0))
						highlight = highlight && propCondition[pal][hoverBtn];

					//AND with any conditionals set to "all"
					for(int bitcount=0;bitcount<8;bitcount++){
						if(FormPropConditions->chkMap->Checked && (propConditional[bitcount]==2 && btnStateProps[bitcount]))
						highlight = highlight && propCondition[pal][bitcount];
					}
				   //	*/
				}  //is map enclosure
			}      //individual bits enclosure
		}          //is direct tile collision enclosure

	}              //do doHighlights enclosure

	if(bEfficient) scale = scale/2;

	if(!banked)  pp=tile*16+bankViewTable[tile+set]; //using A/B sets directly

	else{                               //mmc5 etc handling goes here
		if(isBankSwap)pp=tile*16;                     // chr bank swapper
		else pp=tile*16+(curViewCHRbankCanvas2*4096); // chr bank manager.
	}
	if(chrSelectRect) sel=false;

	for(j=0;j<8;++j)
	{
		for(k=0;k<8;++k)
		{
			//attribute checker stuff pt.1
			if(doChecker)
			{
				if(iAttrCheckerMethod==0 || iAttrCheckerMethod==1 || iAttrCheckerMethod==4){
					//checkerboard, stripes or ID pattern
					if(iAttrCheckerMethod==0) col=(((attrCheckerClassicCHR[j]<<k)&128)>>7)|(((attrCheckerClassicCHR[j+8]<<k)&128)>>6);
					if(iAttrCheckerMethod==1) col=(((attrCheckerStripeCHR[j]<<k)&128)>>7)|(((attrCheckerStripeCHR[j+8]<<k)&128)>>6);
					if(iAttrCheckerMethod==4) col=(((attrCheckerIDCHR[16*pal+j]<<k)&128)>>7)|(((attrCheckerIDCHR[16*pal+j+8]<<k)&128)>>6);
					//patterned
					if(iAttrCheckerMethod==4) {
						col=outPalette[bgPalDefault[col]];

					}

					else {
						col=outPalette[bgPal[palBank*16+pal*4+col]];
					}
					for(int avgi=0; avgi<4; avgi++){
						avgr[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]]>>16)&0xff;
						avgg[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]]>>8)&0xff;
						avgb[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]])&0xff;
					}
					if(pal==0) tpal=0;
					else tpal=(iRotateCheckerSeparation+pal -1) % 3 +1;

					chk_r[k]=(((col>>16)&0xff)*30+avgr[tpal]*sep)/(30+sep);
					chk_g[k]=(((col>>8)&0xff)*30+avgg[tpal]*sep)/(30+sep);
					chk_b[k]=((col&0xff)*30 +avgb[tpal]*sep)/(30+sep);
					



				}
				else{
				//averaged and then separated by staircased ID.
				for(int avgi=0; avgi<4; avgi++){
					avgr[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]]>>16)&0xff;
					avgg[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]]>>8)&0xff;
					avgb[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]])&0xff;
				}
				//tpal=(iRotateCheckerSeparation+pal)&3;
				if(pal==0) tpal=0;
				else tpal=(iRotateCheckerSeparation+pal -1) % 3 +1;

				chk_r[k]=(avgr[0]*10+avgr[1]+avgr[2]*10+avgr[3]*10+avgr[tpal]*sep)/(30+sep);
				chk_g[k]=(avgg[0]*10+avgg[1]+avgg[2]*10+avgg[3]*10+avgg[tpal]*sep)/(30+sep);
				chk_b[k]=(avgb[0]*10+avgb[1]+avgb[2]*10+avgb[3]*10+avgb[tpal]*sep)/(30+sep);
             	}
			 }



			//tile!
			col=(((chr[pp+j]<<k)&128)>>7)|(((chr[pp+j+8]<<k)&128)>>6);
			//else        col=(((chrBank[pp+j]<<k)&128)>>7)|(((chrBank[pp+j+8]<<k)&128)>>6);
			int tmpBgPalCur= (bgPalCur<1)? 3:bgPalCur;
				tmpBgPalCur= (col<1)		 ? col:tmpBgPalCur;

			if(!bSilhouette)col=outPalette[bgPal[palBank*16+pal*4+col]];
			else col=outPalette[bgPal[palBank*16+palActive*4+tmpBgPalCur]];
			rr[k]=(col>>16)&0xff;
			gg[k]=(col>>8)&0xff;
			bb[k]= col&0xff;

			if(doChecker) //pt 2 - opacity mixer.
			{
				rr[k]= ((rr[k]*tile_o) + (chk_r[k]*attr_o)) / 100;
				gg[k]= ((gg[k]*tile_o) + (chk_g[k]*attr_o)) / 100;
				bb[k]= ((bb[k]*tile_o) + (chk_b[k]*attr_o)) / 100;
			}

		}

		for(l=0;l<scale;++l)
		{
			dst=(unsigned char*)pic->Bitmap->ScanLine[y+j*scale+l]+x*3;


			for(k=0;k<8;++k)
			{
				int addcol=0;
				int add_r=0;
				int add_g=0;
				int add_b=0;


				if(doHighlight){
					if(mtHoverBtn>=0 && ((isMeta) || (bMTuseDirectTile&&tx<0&&ty<0))){
						if(highlight) addcol=  50;
						else 		  addcol= -25;
					}
					else if(hoverBtn>=0){
						if(isMeta && FormCHRbit->btnShowMeta->Down){
							if(highlight) addcol=  50;
							else 		  addcol= -25;
						}
						else if(bIsNav && FormCHRbit->btnShowNav->Down){
							if(highlight) addcol=  50;
							else 		  addcol= -25;
						}
						else if(tx<0&&ty<0 && FormCHRbit->btnShowCHR->Down){
							if(highlight) addcol=  50;
							else 		  addcol= -25;
						}
						else if(tx>=0&&ty>=0 && FormCHRbit->btnShowScreen->Down && !bIsNav){
							if(highlight) addcol=  50;
							else 		  addcol= -25;
						}
					}
					else if(bColissionIDHover){

						if(highlight){

							int tmpID=0;
							if(FormPropID->btnSel->Down) tmpID = FormPropID->ListBox1->ItemIndex;
							else if(FormPropID->btnHov->Down) tmpID = iPropIDHoveredItem;

							else if(FormPropID->btnAll->Down){
								if(tx<0&&ty<0){
									if(!banked) tmpID= tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile];
									else tmpID= tileProperties[curViewCHRbankCanvas2*256+highlight_tile];
								}
								else if (isMeta){
									if(!banked){
										if(bMTuseDirectTile) tmpID = tileProperties[bankViewTable[set+highlight_tile]/16+highlight_tile];
										else tmpID = highlight_mt;
									}
									else{
										if(bMTuseDirectTile) tmpID = tileProperties[set+highlight_tile];
										else tmpID = highlight_mt;
									}
								}
							}
							add_r=  collisionID_R[tmpID]/1.5;
							add_g=  collisionID_G[tmpID]/1.5;
							add_b=  collisionID_B[tmpID]/1.5;
							addcol= -25;
						}

						else{
							if(tx<0&&ty<0){  addcol= -33;}
							else if (isMeta){ addcol= -33;}
						}

					}

				}



				r=max(min(rr[k]+addcol+add_b,255),0);
				g=max(min(gg[k]+addcol+add_g,255),0);
				b=max(min(bb[k]+addcol+add_r,255),0);
				for(m=0;m<scale;++m)
				{
					*dst++=r;
					*dst++=g;
					*dst++=b;
				}
			}
		}
	}


	hgrid=0;
	vgrid=0;
	hcol=64; //originally 64
	vcol=64; //originally 64
	if (bOn || (bAuto && bNamDrag))
	{

	if(isMeta){
	   if(tx<0&&ty<0)  //target is single metatile
	   {
			hgrid=2; //spacing: 2 means every other dot.
			vgrid=2;
			//if(doubleWidth==true) {hgrid=0; vgrid=0;}
	   }
	   else            //target is metatile set
	   {
			if(doubleWidth){
				if(!(ty&7))
				{
					hcol=128;
					hgrid=1;
				}
				if(!(tx&7))
				{
					vcol=128;
					vgrid=1;
				}
			}

			else{
				if(!(ty&3))
				{
					hcol=128;
					hgrid=1;
				}
				if(!(tx&3))
				{
					vcol=128;
					vgrid=1;
				}
			}
	   }

	}
	else{
		if(tx<0&&ty<0)  //target is Tileset canvas(es)
		{
			if(SpeedButtonGridTile->Down||SpeedButtonGridAtr->Down||SpeedButtonGridBlock->Down)
			{
				hgrid=2; //spacing: 2 means every other dot.
				vgrid=2;
			}

		}
		else    //target is Screen canvas or otherwise
		{
			if(SpeedButtonGridTile->Down)
			{
				hgrid=2;
				vgrid=2;
				if(doubleWidth==true) {hgrid=0; vgrid=0;}
			}
			if(SpeedButtonGridAtr->Down)
			{
				if(!(ty&1))
				{
					hcol=96;  //originally 96
					//if(ImageName->Dragging()) hcol=80;
					//if(doubleWidth==true) {hcol=hcol-8;  vcol=vcol-8;}
					hgrid=1;
				}
				if(!(tx&1))
				{
					vcol=96; //originally 96
					//if(ImageName->Dragging()) hcol=80;
				   //	if(doubleWidth==true) {hcol=hcol-8;  vcol=vcol-8;}
					vgrid=1;
				}
				//Tilegrid2->Checked^=true;
			}
			if(SpeedButtonGridBlock->Down)
			{
				tyTmp = ty;
				if (((ty / 30) % 2 == 1) && (SpeedButtonGridScreen->Down)) tyTmp -= 2;
				if(!(tyTmp&3))

				{
					hcol=128;  //0riginally 128
					//if(ImageName->Dragging() ) hcol=112;
					//if(doubleWidth==true) {hcol=hcol-8;  vcol=vcol-8;}
					hgrid=1;
				}
				if(!(tx&3))
				{
					vcol=128;
					//if(ImageName->Dragging()) hcol=112;
					//if(doubleWidth==true) {hcol=hcol-8;  vcol=vcol-8;}
					vgrid=1;
				}
				//Tilegrid4->Checked^=true;  //FG bookmark
			}

		}
	}
	}

	if(!isMeta)if(
		(bAuto && bNamDrag )
		|| (AlwaysNavigator1->Checked 		&&   bIsNav)
		|| (AlwaysCanvas1->Checked 			&& !(bIsNav))

		|| (MouseNavigator1->Checked 		&&   bIsNav 	&& bMouseOverNav)
		|| (MouseCanvas1->Checked 			&& !(bIsNav) 	&& bMouseOverNam)

		|| (ButtonNavigator1->Checked 		&& (SpeedButtonGridScreen->Down && (bOn || (bAuto && bNamDrag))) 		&& bIsNav)
		|| (ButtonCanvas1->Checked  		&& (SpeedButtonGridScreen->Down && (bOn || (bAuto && bNamDrag)))		&& !(bIsNav))

		|| (MouseButtonNavigator1->Checked 	&& ((SpeedButtonGridScreen->Down && (bOn || (bAuto && bNamDrag))) || bMouseOverNav)	&& bIsNav)
		|| (MouseButtonCanvas1->Checked 	&& ((SpeedButtonGridScreen->Down && (bOn || (bAuto && bNamDrag))) || bMouseOverNam)	&& !(bIsNav))
		)
		{
			if(!(ty%30))
			{
				hcol=128;  //0riginally 128
				//if(ImageName->Dragging()) hcol=112;
				hgrid=1;
			}
			if(!(tx%32))
			{
				vcol=128;
				//if(ImageName->Dragging()) hcol=112;
				vgrid=1;
			}
		}


	if(hgrid)
	{
		dst=(unsigned char*)pic->Bitmap->ScanLine[y]+x*3;

		for(j=0;j<8*scale;j+=hgrid)
		{
			*dst+++=hcol;
			*dst+++=hcol;
			*dst+++=hcol;

			if(hgrid>1) dst+=(hgrid-1)*3;
		}

		if(doubleWidth==true && y>0)
		{
			dst=(unsigned char*)pic->Bitmap->ScanLine[y-1]+x*3;
			for(j=0;j<8*scale;j+=hgrid)
			{
				*dst+++=hcol;
				*dst+++=hcol;
				*dst+++=hcol;

				//if(hgrid>1) dst+=(hgrid-1)*3;
			}
		}
	}

	if(vgrid)
	{
		for(j=1;j<8*scale;j+=vgrid)
		{
			dst=(unsigned char*)pic->Bitmap->ScanLine[y+j]+x*3;

			*dst+++=vcol;
			*dst+++=vcol;
			*dst+++=vcol;

			  if(doubleWidth==true && x>0) //
			  {
				dst=(unsigned char*)pic->Bitmap->ScanLine[y+j]+(x-1)*3;

				*dst+++=vcol;
				*dst+++=vcol;
				*dst+++=vcol;
			  }


		}

		*dst=*dst;//to prevent warning
	}
}



void __fastcall TFormMain::DrawTileChecker(TPicture *pic,int x,int y,int tile,int pal,int tx,int ty,bool sel, bool efficientTarget, int inputScale)
{
	//this code is legacied.
	/*
	const unsigned char checker[16]={
		0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
		0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff
	};
	int j,k,l,m,col,r,g,b;
	int rr[8],gg[8],bb[8];
	int avgr[4],avgg[4],avgb[4];
	unsigned char *dst;
	int hgrid,vgrid,hcol,vcol;
	int scale=inputScale;
	bool btiletab = PageControlEditor->ActivePage==TabSheetTile?true:false;
	bool bEfficient = btiletab&&efficientTarget==true&&(uiScale==2);
	if(bEfficient) scale = scale/2;

	for(j=0;j<8;j++)
	{
		for(k=0;k<8;k++)
		{

			//checkerboar or stripes pattern
			col=(((checker[j]<<k)&128)>>7)|(((checker[j+8]<<k)&128)>>6);
			col=(((attrCheckerStripeCHR[j]<<k)&128)>>7)|(((attrCheckerStripeCHR[j+8]<<k)&128)>>6);

			//patterned
			col=outPalette[bgPal[palBank*16+pal*4+col]];

			rr[k]=(col>>16)&0xff;
			gg[k]=(col>>8)&0xff;
			bb[k]=col&0xff;

			//averaged and then separated by staircased ID.
			for(int avgi=0; avgi<4; avgi++){
				avgr[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]]>>16)&0xff;
				avgg[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]]>>8)&0xff;
				avgb[avgi]=(outPalette[bgPal[palBank*16+pal*4+avgi]])&0xff;
			}
			int tpal=(iRotateCheckerSeparation+pal)&3;
			int sep=faderAttrCheckerSeparation[iAttrCheckerMethod];

			rr[k]=(avgr[0]*10+avgr[1]+avgr[2]*10+avgr[3]*10+avgr[tpal]*sep)/(30+sep);
			gg[k]=(avgg[0]*10+avgg[1]+avgg[2]*10+avgg[3]*10+avgg[tpal]*sep)/(30+sep);
			bb[k]=(avgb[0]*10+avgb[1]+avgb[2]*10+avgb[3]*10+avgb[tpal]*sep)/(30+sep);


		}

		for(l=0;l<scale;++l)
		{
			dst=(unsigned char*)pic->Bitmap->ScanLine[y+j*scale+l]+x*3;

			for(k=0;k<8;++k)
			{
				r=rr[k];
				g=gg[k];
				b=bb[k];

				for(m=0;m<scale;++m)
				{
					*dst++=r;
					*dst++=g;
					*dst++=b;
				}
			}
		}
	} */
}



void __fastcall TFormMain::DrawEmptyTile(TPicture *pic,int x,int y,int pal,int tx,int ty,bool sel,bool efficientTarget, int inputScale)
{
	int j,k,l,m,pp,col;
	unsigned char *dst;
	int scale=inputScale;

	bool btiletab = PageControlEditor->ActivePage==TabSheetTile?true:false;
	bool bEfficient = btiletab&&efficientTarget==true&&(uiScale==2);
	if(bEfficient) scale = scale/2;

	for(j=0;j<8;++j)
	{
		for(l=0;l<scale;++l)
		{
			dst=(unsigned char*)pic->Bitmap->ScanLine[y+j*scale+l]+x*3;

			for(k=0;k<8;++k)
			{
				if((j^k)&4) col=0x80; else col=0xc0;

				for(m=0;m<scale;++m)
				{
					*dst++=col;
					*dst++=col;
					*dst++=col;
				}
			}
		}
	}
}



void __fastcall TFormMain::DrawSpriteDot(TPicture *pic,int x,int y,TColor color,int scale)
{
	x*=scale;
	y*=scale;

	pic->Bitmap->Canvas->Brush->Style=bsSolid;
	pic->Bitmap->Canvas->Brush->Color=color;
	pic->Bitmap->Canvas->Pen->Color=color;
	pic->Bitmap->Canvas->FillRect(TRect(x,y,x+scale,y+scale));
}



void __fastcall TFormMain::DrawSpriteTile(TPicture *pic,int x,int y,int tile,int attr,TColor frame,int scale)
{
	int set=bankActive/16;
	int tx,ty,j,k,l,m,pp,col,r,g,b;
	int rr[8],gg[8],bb[8];
	unsigned char *dst;
	unsigned char chrt[16][8];
	bool bSilhouette=btnMask->Down;
	x*=scale;
	y*=scale;

	if(!SpeedButtonSprite8x16->Down)
	{
		pp=(tile&255)*16+bankViewTable[set+tile];

		for(j=0;j<8;++j)
		{
			for(k=0;k<8;++k)
			{
				if(attr&128) ty=7-j; else ty=j;//vflip
				if(attr&64 ) tx=7-k; else tx=k;//hflip

				chrt[ty][tx]=(((chr[pp+j]<<k)&128)>>7)|(((chr[pp+j+8]<<k)&128)>>6);
			}
		}
	}
	else
	{
		int bankViewOff;
		if(set<2*256) bankViewOff=0;
		else          bankViewOff=512;

		for(j=0;j<16;++j)
		{
			//FIX tomorrow - bankview indirection.
			//(also fix saving set as vram buttons to file)
			// old
			/*
			if(j==0) pp=(tile&254)*16+((tile&1)*4096);
			if(j==8) pp=(tile&254)*16+16+((tile&1)*4096);
			*/



			if(j==0) pp=(tile&254)*16    +  bankViewTable[ ((tile&1)*256)+bankViewOff];
			if(j==8) pp=(tile&254)*16+16 +  bankViewTable[ ((tile&1)*256)+bankViewOff];


			/*
			int spr8x16_set0, spr8x16_set1;
			int spr8x16_use;
			if(set<2*256)         {spr8x16_set0=0;     spr8x16_set1 = 256;}
			else                  {spr8x16_set0=2*256; spr8x16_set1 = 3*256;}

			if(tile&1==0) spr8x16_use = spr8x16_set0;
			else          spr8x16_use = spr8x16_set1;

			if(j==0) pp=(tile&254)*16    + bankViewTable[spr8x16_use + (tile&254)];
			if(j==8) pp=(tile&254)*16+16 + bankViewTable[spr8x16_use + (tile&254)+1];
			*/

			for(k=0;k<8;++k)
			{
				if(attr&128) ty=15-j; else ty=j;//vflip
				if(attr&64 ) tx= 7-k; else tx=k;//hflip

				chrt[ty][tx]=(((chr[pp+(j&7)]<<k)&128)>>7)|(((chr[pp+(j&7)+8]<<k)&128)>>6);
			}
		}
	}
	int tmpBgPalCur= (bgPalCur<1)? 3:bgPalCur;
	for(j=0;j<(SpeedButtonSprite8x16->Down?16:8);++j)
	{
		for(k=0;k<8;++k)
		{
			col=chrt[j][k];

			if(col)
			{

				if(!bSilhouette) col=outPalette[bgPal[palBank*16+(attr&3)*4+col]];
				else col=outPalette[bgPal[palBank*16+palActive*4+tmpBgPalCur]];

				rr[k]=(col>>16)&0xff;
				gg[k]=(col>>8)&0xff;
				bb[k]= col&0xff;
			}
			else
			{
				rr[k]=-1;
			}
		}

		for(l=0;l<scale;++l)
		{
			if(y+j*scale+l>=pic->Bitmap->Height) break;

			dst=(unsigned char*)pic->Bitmap->ScanLine[y+j*scale+l]+x*3;

			for(k=0;k<8;++k)
			{
				r=rr[k];
				g=gg[k];
				b=bb[k];

				if(r>=0)
				{
					for(m=0;m<scale;++m)
					{
						*dst++=r;
						*dst++=g;
						*dst++=b;
					}
				}
				else
				{
					dst+=3*scale;
				}
			}
		}
	}

	if(frame!=clBlack)
	{
		pic->Bitmap->Canvas->Brush->Style=bsClear;
		pic->Bitmap->Canvas->Pen->Color=frame;
		pic->Bitmap->Canvas->Rectangle(x,y,x+8*scale,y+(SpeedButtonSprite8x16->Down?16*scale:8*scale));
	}
}



void __fastcall TFormMain::DrawExportTile16(TPicture *pic,int x,int y,int tile,int pal,bool direct)
{
	int set=bankActive/16;
	int j,k,pp;;
	unsigned char *dst;
	int col1,col2;

	if (direct)  pp=tile;
	if (!direct) pp=tile*16+bankViewTable[set+tile];

	for(j=0;j<8;j++)
	{
		dst=(unsigned char*)pic->Bitmap->ScanLine[y+j]+x/2;

		for(k=0;k<8;k+=2)
		{
			col1=(pal<<2)|(((chr[pp+j]<<k)&128)>>7)|(((chr[pp+j+8]<<k)&128)>>6);
			col2=(pal<<2)|(((chr[pp+j]<<(k+1))&128)>>7)|(((chr[pp+j+8]<<(k+1))&128)>>6);

			*dst++=col2|(col1<<4);
		}
	}
}



void __fastcall TFormMain::DrawSelection(TImage *image,TRect rect, int inputScale, bool bHalfsize, bool bTargetNav)
{
	int adjust = bTargetNav?8:0;

	TRect r;
	TRect l;
	TColor c;

	//if(&rect==&curCursor || &rect==&bnkCursor) c=TColor(0x888888);

	if(isBnkCursor) c=TColor(0xaaaaaa);
	else c=TColor(0xffffff);


	r.left  =rect.left  *(8*inputScale);
	r.top   =rect.top   *(8*inputScale);
	r.right =rect.right *(8*inputScale);
	r.bottom=rect.bottom*(8*inputScale);

	r.left  -=1;
	r.top   -=1;
	r.right +=1;
	r.bottom+=1;

	image->Canvas->Brush->Style=bsClear;
	image->Canvas->Pen->Color=c;
	image->Canvas->Rectangle(r);

	r.left  +=1;
	r.top   +=1;
	r.right -=1;
	r.bottom-=1;

	if (bHalfsize)image->Canvas->Pen->Color=c;
	else image->Canvas->Pen->Color=TColor(0x000000);
	image->Canvas->Rectangle(r);

	r.left  -=2;
	r.top   -=2;
	r.right +=2;
	r.bottom+=2;

	image->Canvas->Pen->Color=TColor(0x000000);
	image->Canvas->Rectangle(r);


		if (bDrawDestShadow==true)
		{


			r.left  =destRect.left;
			r.top   =destRect.top;
			r.right =destRect.right;
			r.bottom=destRect.bottom;

			if(bTargetNav) //correct destRect
			{
			   if(nameTableWidth<32)
			   {
				r.left=r.left -((32-nameTableWidth)/2);
				r.right=r.right -((32-nameTableWidth)/2);
			   }
			   if(nameTableHeight<32)
			   {
			   r.top=r.top - ((32-nameTableHeight)/2);
			   r.bottom=r.bottom -((32-nameTableHeight)/2);
			   }
			}
			r.left  *=(8*inputScale);
			r.top   *=(8*inputScale);
			r.right *=(8*inputScale);
			r.bottom*=(8*inputScale);

			r.left  -=1;
			r.top   -=1;
			r.right +=1;
			r.bottom+=1;

			image->Canvas->Brush->Style=bsClear;
			image->Canvas->Pen->Color=TColor(0x888888);
			image->Canvas->Rectangle(r);

			

			r.left  -=1;
			r.top   -=1;
			r.right +=1;
			r.bottom+=1;

			if (bHalfsize)image->Canvas->Pen->Color=TColor(0x888888);
			else image->Canvas->Pen->Color=TColor(0x000000);
			image->Canvas->Rectangle(r);

			//modifier symbols
			 image->Canvas->Brush->Style=bsSolid;
			//move
			if(bBufCtrl && !bBufShift && !bBufAlt){

				image->Canvas->Pen->Color=TColor(0x000000);
				l.left  =4 			+ destRect.left  *(8*inputScale);
				l.top   =4-adjust 	+ destRect.top   *(8*inputScale);
				l.right =l.left+2;	l.bottom=l.top+8;
				image->Canvas->Rectangle(l);

			   l.left+=2;	l.right+=2;	l.top+=0;	l.bottom=l.top+2; image->Canvas->Rectangle(l);
			   l.left+=2;	l.right+=2;	l.top+=2;	l.bottom=l.top+6; image->Canvas->Rectangle(l);
			   l.left+=2;	l.right+=2;	l.top-=2;	l.bottom=l.top+2; image->Canvas->Rectangle(l);
			   l.left+=2;	l.right+=2;	l.top+=2;	l.bottom=l.top+6; image->Canvas->Rectangle(l);

				image->Canvas->Pen->Color=TColor(0x888888);
				l.left  =3 			+destRect.left  *(8*inputScale);
				l.top   =3-adjust 	+destRect.top   *(8*inputScale);
				l.right =l.left+2;	l.bottom=l.top+8;
				image->Canvas->Rectangle(l);

			   l.left+=2;	l.right+=2;	l.top+=0;	l.bottom=l.top+2; image->Canvas->Rectangle(l);
			   l.left+=2;	l.right+=2; l.top+=2;	l.bottom=l.top+6; image->Canvas->Rectangle(l);
			   l.left+=2;	l.right+=2;	l.top-=2;	l.bottom=l.top+2; image->Canvas->Rectangle(l);
			   l.left+=2;	l.right+=2; l.top+=2;	l.bottom=l.top+6; image->Canvas->Rectangle(l);
			}
			//clone
			if(bBufCtrl && !bBufShift && bBufAlt)
			{

				image->Canvas->Pen->Color=TColor(0x000000);
				l.left  =4 					+destRect.left  *(8*inputScale);
				l.top   =(4+2)-adjust 		+destRect.top *(8*inputScale);
				l.right =l.left+2;	l.bottom=l.top+4;
				image->Canvas->Rectangle(l);
				l.left+=2;	l.right+=4;	l.top-=2;	l.bottom=l.top+2; image->Canvas->Rectangle(l);
				l.left+=0;	l.right+=0;	l.top+=6;	l.bottom=l.top+2; image->Canvas->Rectangle(l);


				image->Canvas->Pen->Color=TColor(0x888888);
				l.left  =3 					+destRect.left  *(8*inputScale);
				l.top   =(3+2)-adjust 		+destRect.top *(8*inputScale);
				l.right =l.left+2;	l.bottom=l.top+4;
				image->Canvas->Rectangle(l);
				l.left+=2;	l.right+=4;	l.top-=2;	l.bottom=l.top+2; image->Canvas->Rectangle(l);
				l.left+=0;	l.right+=0;	l.top+=6;	l.bottom=l.top+2; image->Canvas->Rectangle(l);
			}

		}

		

	
}



void __fastcall TFormMain::UpdateTiles(bool updchr)
{
	int i,j,x,y;
	int ui8=8*uiScale;
	int ui128=128*uiScale;
	bool bbigTile=(PageControlEditor->ActivePage==TabSheetTile?true:false)&&!bForceNameScale;
	TRect rect;
	if(bImageNameAccepted) return;  //this was a cosmetic fix to a performance issue i now believe is fixed regarding holding alt key. Kept for redundancy.
	if(!Visible) return;
	//if (mouseDraggedTileSel==true&&(Throttle->OnTimer==false)) return;
	//throttleOKtileSelRect = false;
	x=0;
	y=0;

	for(i=0;i<256;i++)
	{

		if(bbigTile)
			 DrawTile(ImageBigTiles->Picture,x*2,y*2,tileViewTable[i],tileViewTable[i],palActive,-1,-1,chrSelected[i],false,uiScale*2,false,false,false,false,false);
		else DrawTile(ImageTiles->Picture,x,y,tileViewTable[i],tileViewTable[i],palActive,-1,-1,chrSelected[i],false,uiScale,false,false,false,false,false);


		x+=ui8;


		if(x>=ui128)
		{
			x=0;

			y+=ui8;
		}
	}

	if(!chrSelectRect)

	{
		for(i=0;i<256;i++)
		{
			if(chrSelected[i])
			{
				rect.Left=i&15;
				rect.Right=rect.Left+1;
				rect.Top=i/16;
				rect.Bottom=rect.Top+1;


				if(bbigTile) DrawSelection(ImageBigTiles,rect,uiScale*2,false,false);
				else DrawSelection(ImageTiles,rect,uiScale,false,false);
			}
		}
	}
	else
	{

		if(bbigTile) DrawSelection(ImageBigTiles,chrSelection,uiScale*2,false,false);
		else DrawSelection(ImageTiles,chrSelection,uiScale,false,false);
	}


	if(bbigTile) ImageBigTiles->Repaint();
    else ImageTiles->Repaint();

	if((updchr) && (openByFileDone)) FormCHREditor->PaintBoxChr->Repaint();
}



void __fastcall TFormMain::UpdateNameTable(int tx,int ty,bool repaint)
{
	int i,j,x,y,nx,ny,tile;
	TRect r;
	int cgX=CHRCollisionGranularityX;
	int cgY=CHRCollisionGranularityY;
	int highlight_tile;

	bool bInvertSelected= btnSelectedOnlyInverted->Down;
	bool bBigTile = (PageControlEditor->ActivePage==TabSheetTile?true:false)&&!bForceNameScale;
	if(bBigTile&&(uiScale==2)) {ImageTiles->Picture->Bitmap->SetSize(ImageTiles->Width*2,ImageTiles->Height*2);
	}
	else if (bBigTile) {
		ImageTiles->Picture->Bitmap->SetSize(ImageTiles->Width*2,ImageTiles->Height*2);
		ImageTiles->Stretch = true;
	}
	else {
		ImageTiles->Picture->Bitmap->SetSize(ImageTiles     ->Width,ImageTiles     ->Height);
		ImageTiles->Stretch = false;
	}
	if(!Visible) return;

	if (mouseDraggedNTSel==true&&throttleOKselRect==false) {cueNT=true; return;}
	throttleOKselRect = false;


	if(tx<0||ty<0)
	{
		//redraw whole viewport

		y=0;
		unsigned char tmp_chrSelected[256];
		for (i=0;i<256;i++)
		tmp_chrSelected[tileViewTable[i]]= bInvertSelected ? !chrSelected[i] : chrSelected[i];

		for(i=0;i<32;i++)
		{
			x=0;

			for(j=0;j<32;j++)
			{
				nx=j+nameTableViewX;
				ny=i+nameTableViewY;

				if(nx>=0&&nx<nameTableWidth&&ny>=0&&ny<nameTableHeight)
				{
					tile=nameTable[ny*nameTableWidth+nx];
					//depends on tile associated collision filter
					highlight_tile=nameTable[((ny*nameTableWidth)/(nameTableWidth*cgY))*(nameTableWidth*cgY) + (nx/cgX)*cgX];

					/*
					if(SpeedButtonChecker->Down)
					{
						if(bBigTile)DrawTileChecker(ImageTiles->Picture,x,y,tile,AttrGet(nx,ny,false,false),nx,ny,false,true,uiScale);
						else DrawTileChecker(ImageName->Picture,x,y,tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);

					}
					else
					{*/
						if(!SpeedButtonSelTiles->Down||tmp_chrSelected[tile])
						{
							if(bBigTile)DrawTile(ImageTiles->Picture,x,y,tile,highlight_tile,AttrGet(nx,ny,false,false),nx,ny,false,true,uiScale,false,false,false,false,false);
							else DrawTile(ImageName->Picture,x,y,tile,highlight_tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale,false,false,false,false,false);
						}
						else
						{
							if(bBigTile) DrawEmptyTile(ImageTiles->Picture,x,y,AttrGet(nx,ny,false,false),nx,ny,false,true,uiScale);
							else DrawEmptyTile(ImageName->Picture,x,y,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
						}
					//}
				}
				else
				{
					if(bBigTile) DrawEmptyTile(ImageTiles->Picture,x,y,0,nx,ny,false,true,uiScale);
					else DrawEmptyTile(ImageName->Picture,x,y,0,nx,ny,false,false,uiScale);
				}

				if(bBigTile&&(uiScale==2)) x+=(8*uiScale)/2;
				else x+=8*uiScale;
			}

			if(bBigTile&&(uiScale==2)) y+=(8*uiScale)/2;
			else y+=8*uiScale;
		}
	}
	else
	{
		//redraw one attribute cell

		//delimit edges of drawing field (odd scroll positions otherwise draw outside memory)

		//quantize
		tx&=~1;
		ty&=~1;

		y=(ty-nameTableViewY)*8*uiScale;
		//if(!(nameTableViewY%2 && (ty==0||32)))


		for(i=0;i<2;i++)
		{
			x=(tx-nameTableViewX)*8*uiScale;

			for(j=0;j<2;j++)
			{
				nx=j+tx;
				ny=i+ty;

				if(y<0 || y>=32*8*uiScale) continue;   //protects against writing outside viewport bitmap


				if(nx>=0&&nx<nameTableWidth&&ny>=0&&ny<nameTableHeight)
				{
					tile=nameTable[ny*nameTableWidth+nx];
					highlight_tile=nameTable[((ny*nameTableWidth)/(nameTableWidth*cgY))*(nameTableWidth*cgY) + (nx/cgX)*cgX];
					/*
					if(SpeedButtonChecker->Down)
					{
						if(bBigTile) DrawTileChecker(ImageTiles->Picture,x,y,tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
						else DrawTileChecker(ImageName->Picture,x,y,tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
					}
					else
					{ */
						if(!SpeedButtonSelTiles->Down||chrSelected[tile])
						{
							if(bBigTile) DrawTileChecker(ImageTiles->Picture,x,y,tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
							else DrawTile(ImageName->Picture,x,y,tile,highlight_tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale,false,false,false,false,false);
						}
						else
						{
							if(bBigTile) (ImageTiles->Picture,x,y,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
							else DrawEmptyTile(ImageName->Picture,x,y,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
						}
					//}
				}
				else
				{
					if(bBigTile) DrawEmptyTile(ImageTiles->Picture,x,y,0,nx,ny,false,false,uiScale);
					else DrawEmptyTile(ImageName->Picture,x,y,0,nx,ny,false,false,uiScale);
				}

				x+=8*uiScale;
			}

			y+=8*uiScale;
		}
	}

	BufBmpName->Assign(ImageName->Picture->Bitmap);

	if(nameSelection.left>=0&&nameSelection.top>=0)
	{
		r.left  =nameSelection.left  -nameTableViewX;
		r.right =nameSelection.right -nameTableViewX;
		r.top   =nameSelection.top   -nameTableViewY;
		r.bottom=nameSelection.bottom-nameTableViewY;


		if(!bBigTile) DrawSelection(ImageName,r,uiScale,false,false);  //selection only meaningful on the big canvas.
	}

	if(repaint) {
		if(bBigTile) ImageTiles->Repaint();
		else ImageName->Repaint();
	}
	//update appropriate menu items
	SelectedOnly1->Checked=SpeedButtonSelTiles->Down;
}

void __fastcall TFormMain::UpdateNameStrip(bool repaint, int off)
{
	int i,j,x,y,nx,ny,tile;
	TRect r;
	bool bBigTile = (PageControlEditor->ActivePage==TabSheetTile?true:false)&&!bForceNameScale;
    int cgX=CHRCollisionGranularityX;
	int cgY=CHRCollisionGranularityY;
	int highlight_tile;
	/*
	if(bBigTile&&(uiScale==2)) {ImageTiles->Picture->Bitmap->SetSize(ImageTiles->Width*2,ImageTiles->Height*2);
	}
	else if (bBigTile) {
		ImageTiles->Picture->Bitmap->SetSize(ImageTiles->Width*2,ImageTiles->Height*2);
		ImageTiles->Stretch = true;
	}
	else {
		ImageTiles->Picture->Bitmap->SetSize(ImageTiles     ->Width,ImageTiles     ->Height);
		ImageTiles->Stretch = false;
	} */
	if(!Visible) return;

	if (mouseDraggedNTSel==true&&throttleOKselRect==false) {cueNT=true; return;}
	throttleOKselRect = false;

	i=cntNTstrip+off;

	//y=i*8;
	if(bBigTile&&(uiScale==2)) y=i*(8*uiScale)/2;
	else y=i*8*uiScale;
	x=0;

	for(j=0;j<32;j++)
	{
		nx=j+nameTableViewX;
		ny=i+nameTableViewY;

		if(nx>=0&&nx<nameTableWidth&&ny>=0&&ny<nameTableHeight)
		{
			tile=nameTable[ny*nameTableWidth+nx];
			highlight_tile=nameTable[((ny*nameTableWidth)/(nameTableWidth*cgY))*(nameTableWidth*cgY) + (nx/cgX)*cgX];
			if(SpeedButtonChecker->Down)
			{
				if(bBigTile)DrawTileChecker(ImageTiles->Picture,x,y,tile,AttrGet(nx,ny,false,false),nx,ny,false,true,uiScale);
				else DrawTileChecker(ImageName->Picture,x,y,tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
			}
			else
			{
				if(!SpeedButtonSelTiles->Down||chrSelected[tile])
				{
					if(bBigTile)DrawTile(ImageTiles->Picture,x,y,tile,highlight_tile,AttrGet(nx,ny,false,false),nx,ny,false,true,uiScale,false,false,false,false,false);
					else 		DrawTile(ImageName->Picture,x,y,tile,highlight_tile,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale,false,false,false,false,false);
				}
				else
				{
					if(bBigTile) DrawEmptyTile(ImageTiles->Picture,x,y,AttrGet(nx,ny,false,false),nx,ny,false,true,uiScale);
					else DrawEmptyTile(ImageName->Picture,x,y,AttrGet(nx,ny,false,false),nx,ny,false,false,uiScale);
				}
			}
		}
		else
		{
			if(bBigTile) DrawEmptyTile(ImageTiles->Picture,x,y,0,nx,ny,false,true,uiScale);
			else DrawEmptyTile(ImageName->Picture,x,y,0,nx,ny,false,false,uiScale);
		}
		if(bBigTile&&(uiScale==2)) x+=(8*uiScale)/2;
		else x+=8*uiScale;
	}

	if(nameSelection.left>=0&&nameSelection.top>=0)
	{
		r.left  =nameSelection.left  -nameTableViewX;
		r.right =nameSelection.right -nameTableViewX;
		r.top   =nameSelection.top   -nameTableViewY;
		r.bottom=nameSelection.bottom-nameTableViewY;

		if(!bBigTile) DrawSelection(ImageName,r,uiScale,false,false);
	}

	if(repaint) {

		if(bBigTile) ImageTiles->Repaint();
		else ImageName->Repaint();
	}
	//update appropriate menu items
	SelectedOnly1->Checked=SpeedButtonSelTiles->Down;
}

void ExportBMPToClipboard(char *bitmapBuffer, size_t len, bool cbDoClear)
{
	//non-vcl dependent way of copying to clip. currently not in use.
	HGLOBAL hResult;
	OpenClipboard(NULL);
	if (cbDoClear) EmptyClipboard();

	len -= sizeof(BITMAPFILEHEADER);
	hResult = GlobalAlloc(GMEM_MOVEABLE, len);

	memcpy(GlobalLock(hResult), bitmapBuffer + sizeof(BITMAPFILEHEADER), len);
	GlobalUnlock(hResult);
	SetClipboardData(CF_DIB, hResult);
	CloseClipboard();
	GlobalFree(hResult);
}

void __fastcall TFormMain::CopyCHR(bool copy,bool cut)
{
	int set=bankActive/16;
	int i,j,k,pp,ps,x,y,w,h;
	int chrmax = chrBanks*4096;
	int propmax = chrBanks*256;
	//FormMain->SetUndo();
	int prop_p;

	if(chrSelectRect)
	{
		GetSelection(chrSelection,x,y,w,h);
		if(w&&h&&cut) SetUndo();

		pp=0;
		prop_p=0;
		for(i=0;i<h;i++)
		{
			for(j=0;j<w;j++)
			{

				for(k=0;k<16;k++)
				{
					ps=(bankViewTable[set+(x+j) + (y+i)*16]%chrmax) + tileViewTable[(x+j) + (y+i)*16]*16 + k;

					if(copy) chrCopy[pp++]=chr[ps];
					if(cut)  chr[ps]=0;
				}
				if(copy) 	propCopy[prop_p++]=tileProperties[((bankViewTable[set+(x+j) + (y+i)*16]/16)%propmax) + (x+j) + (y+i)*16];
				if(cut)     tileProperties[((bankViewTable[set+(x+j) + (y+i)*16]/16)%propmax) + (x+j) + (y+i)*16]=0;
			}
		}

		if(copy)
		{
			chrCopyWidth=w;
			chrCopyHeight=h;
			chrCopyRect=true;
		}
	}
	else
	{
		if(cut) SetUndo();

		pp=0;
		w=0;
		h=-1;
		prop_p=0;

		for(i=0;i<256;i++)
		{
			if(chrSelected[i])
			{
				for(j=0;j<16;j++)
				{
					ps=(bankViewTable[set+i*16+j]%chrmax)+i*16+j;
					if(copy) chrCopy[pp++]=chr[ps];
					if(cut) chr[ps]=0;
				}
				if(copy) propCopy[prop_p++]=tileProperties[((bankViewTable[set+i]/16)%propmax)+i];
				if(cut) tileProperties[((bankViewTable[set+i]/16)%propmax)+i]=0;

				w++;
			}
		}

		if(copy)
		{
			chrCopyWidth=w;
			chrCopyHeight=h;
			chrCopyRect=false;
		}
	}

	DWORD Len=3*sizeof(int)+sizeof(chrCopy)+sizeof(propCopy);

	HGLOBAL hDst=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,Len);

	if (hDst)
	{
		LPSTR gDst=(LPSTR)GlobalLock(hDst);

		if (gDst)
		{
			if (OpenClipboard(Handle))
			{
				EmptyClipboard();

				*((int*)&gDst[0*sizeof(int)])=chrCopyWidth;
				*((int*)&gDst[1*sizeof(int)])=chrCopyHeight;
				*((int*)&gDst[2*sizeof(int)])=chrCopyRect;
				memcpy(&gDst[12],chrCopy,sizeof(chrCopy));
				memcpy(&gDst[12+sizeof(chrCopy)],propCopy,sizeof(propCopy));


				GlobalUnlock(hDst);

				SetClipboardData(CF_CHR,hDst);
				CloseClipboard();
			}
		}
	}

	if(cut)
	{
		UpdateAll();
		/*
		UpdateTiles(true);

		cueUpdateMetasprite=true;

		UpdateNameTable(-1,-1,true);
		if (FormNavigator->Visible){FormNavigator->Draw(false,false);}
		if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
		if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
    	*/
	}

	if(cut){     //sanitize line drawing buffers
		memset (arr_linePlotCHR,0,sizeof(arr_linePlotCHR2));
		memset (arr_linePlotCHR2,0,sizeof(arr_linePlotCHR2));
		for(int tile=0;tile<256;tile++){
			memcpy(chrBuf+tile*16,chr+tileViewTable[tile]*16 + bankViewTable[set*256+tile],16);
		}

		//memcpy (chrBuf, chr, 4*4096);

		lineUpX= -1;
		lineUpY= -1;
	}
}



void __fastcall TFormMain::PasteCHR(int mode)
{
	//mode bools
	bool bKeepMask= ((mode==2) && (FormSpecialPasteCHR->RadioKeepMask->Checked==true)) ? true:false;
	bool bRemoveMask= ((mode==3) && (FormSpecialPasteCHR->RadioRemoveMask->Checked==true)) ? true:false;

	bool bChkB = FormSpecialPasteCHR->CheckUseMaskB->Checked || bKeepMask;
	bool bChkC = FormSpecialPasteCHR->CheckUseMaskC->Checked || bRemoveMask;


	bool bPasteMaskSolids    =  ((mode==1 || bChkB || bChkC ) && (FormSpecialPasteCHR->RadioSolids->Checked==true)) ? true:false;
	bool bPasteMaskCol0      =  ((mode==1 || bChkB || bChkC) && (FormSpecialPasteCHR->RadioCol0->Checked==true)) ? true:false;
	bool bPasteMaskCustom    =  ((mode==1 || bChkB || bChkC) && (FormSpecialPasteCHR->RadioCustom->Checked==true)) ? true:false;

	bool bAddPaste= ((mode==2) && (
									FormSpecialPasteCHR->RadioAdd->Checked==true
									|| FormSpecialPasteCHR->RadioAdd1->Checked==true
									|| FormSpecialPasteCHR->RadioAdd2->Checked==true
									|| FormSpecialPasteCHR->RadioAdd3->Checked==true
									)) ? true:false;
	bool bSubPaste= ((mode==3) && (
									FormSpecialPasteCHR->RadioSub->Checked==true
									|| FormSpecialPasteCHR->RadioSub1->Checked==true
									|| FormSpecialPasteCHR->RadioSub2->Checked==true
									|| FormSpecialPasteCHR->RadioSub3->Checked==true
									)) ? true:false;

	int add_offset=0;
	int sub_offset=0;

	if(FormSpecialPasteCHR->RadioAdd1->Checked==true) add_offset=1;
	if(FormSpecialPasteCHR->RadioAdd2->Checked==true) add_offset=2;
	if(FormSpecialPasteCHR->RadioAdd3->Checked==true) add_offset=3;

	if(FormSpecialPasteCHR->RadioSub1->Checked==true) sub_offset=1;
	if(FormSpecialPasteCHR->RadioSub2->Checked==true) sub_offset=2;
	if(FormSpecialPasteCHR->RadioSub3->Checked==true) sub_offset=3;

	bool bSubMask=   ((mode==3) && (FormSpecialPasteCHR->CheckSubmask->Checked==true)) ? true:false;


	bool bBitOR=	((mode==2) && (FormSpecialPasteCHR->RadioBitOR->Checked==true)) ? true:false;
	bool bBitAND=   ((mode==2) && (FormSpecialPasteCHR->RadioBitAND->Checked==true)) ? true:false;
	bool bBitNAND=  ((mode==3) && (FormSpecialPasteCHR->RadioBitNAND->Checked==true)) ? true:false;


	bool bKeepSim = ((mode==2) && (FormSpecialPasteCHR->RadioKeepSim->Checked==true)) ? true:false;
	bool bKeepDiff= ((mode==3) && (FormSpecialPasteCHR->RadioKeepDiff->Checked==true)) ? true:false;

	bool bOnTop 	=((mode==2)&& (FormSpecialPasteCHR->RadioOnTop->Checked==true)) ? true:false;
	bool bBehind 	=((mode==3)&& (FormSpecialPasteCHR->RadioBehind->Checked==true)) ? true:false;


	unsigned char pasteMask[16];
	unsigned char compMask;          //used by comparative paste modes
	unsigned char subMask[16]; //used by subtractive paste with offset only.

	unsigned char tmpTile[16];

	bool pm0=true;
	bool pm1=true;
	bool pm2=true;
	bool pm3=true;

	if (bPasteMaskCustom){
		pm0 = FormSpecialPasteCHR->btn0->Down;
		pm1 = FormSpecialPasteCHR->btn1->Down;
		pm2 = FormSpecialPasteCHR->btn2->Down;
		pm3 = FormSpecialPasteCHR->btn3->Down;
	}

	if (bPasteMaskCol0){
		pm0 = true;
		pm1 = false;
		pm2 = false;
		pm3 = false;
	}

	if (bPasteMaskSolids || bOnTop){
		pm0 = false;
		pm1 = true;
		pm2 = true;
		pm3 = true;
	}


	//general
	int i,j,k,pp,pd,x,y,w,h;
	int set=bankActive/16;

	//used by flex-paste
	bool b=FormCHREditor->btn2x2mode->Down;
	int selectThres=b?2:1;
	bool bIsFlex;

	int xlen;   //width of selection
	int ylen;   //height of selection
	int chrmax = chrBanks*4096;
	int propmax = chrBanks*256;
	int maxlen; //the longest a paste run can be; based on clipboard contents
	int countlen = 0; //manual counter for the above.

	int psColID[8];
	int pdColID[8];

	int propOff; //used by rect selection sources for properties.
	//used by bitplane masks
	int fLen=16;
	int fOff=0;

	//used by colour masks
	unsigned char mask[16];
	bool b0 = FormCHREditor->Protect0->Down;
	bool b1 = FormCHREditor->Protect1->Down;
	bool b2 = FormCHREditor->Protect2->Down;
	bool b3 = FormCHREditor->Protect3->Down;

	if(bBehind) {b0=false; b1=true; b2=true; b3=true;}


	//used bt PasteAsMaskSilhouette.
	unsigned char tmpChrCol[16];
	//int coloff=0;
	//int collen=16;


	//if (bgPalCur==0) {collen=0; coloff=0;}
	//if (bgPalCur==1) {collen=8; coloff=0;}
	//if (bgPalCur==2) {collen=8; coloff=8;}

	for (int i=0; i<16; i++){
			tmpChrCol[i]=0;
		}
	/*
	for (int i=coloff; i<collen; i++){
			tmpChrCol[i]=0xff;
	}
	*/
	if (bgPalCur==1) {	for (int i=0; i<8; i++){tmpChrCol[i]=0xff;}}
	if (bgPalCur==2) {for (int i=8; i<16; i++){tmpChrCol[i]=0xff;}}
	if (bgPalCur==3) {for (int i=0; i<16; i++){tmpChrCol[i]=0xff;}}

	bool bAsSilhouette= (mode>0 && FormSpecialPasteCHR->RadioAsSilhouette->Checked==true) ? true:false;
	bool bAsInvSilhouette = (mode>0 && FormSpecialPasteCHR->RadioAsInvSilhouette->Checked==true) ? true:false;
	//apply bitplane masks
	if (!(FormCHREditor->ButtonBitmaskLo->Down||FormCHREditor->ButtonBitmaskHi->Down))
		{if(Applytopaste1->Checked) return;}
	if (!FormCHREditor->ButtonBitmaskLo->Down&&Applytopaste1->Checked) {fLen-=8; fOff=8;}
	if (!FormCHREditor->ButtonBitmaskHi->Down&&Applytopaste1->Checked) {fLen-=8;}





	//get clipboard
	OpenClipboard(Handle);

	HGLOBAL hClipBuf = GetClipboardData(CF_CHR);

	if (hClipBuf)
	{
		LPSTR gSrc = (LPSTR)GlobalLock(hClipBuf);

		if (gSrc != NULL)
		{
			chrCopyWidth =*((int*)&gSrc[0*sizeof(int)]);
			chrCopyHeight=*((int*)&gSrc[1*sizeof(int)]);
			chrCopyRect  =*((int*)&gSrc[2*sizeof(int)]);
			memcpy(chrCopy,&gSrc[12],sizeof(chrCopy));
            memcpy(propCopy,&gSrc[12+sizeof(chrCopy)],sizeof(propCopy));

			GlobalUnlock(gSrc);
		}
	}

	CloseClipboard();


	//if clipboard was 2-dimensional
	if(chrCopyRect)
	{
		if(chrCopyWidth<1||chrCopyHeight<1) return;
		SetUndo();
		GetSelection(chrSelection,x,y,w,h);


		//determines if flex-paste should be used and modifies for iterations accordingly.
		bIsFlex = (w>selectThres || h>selectThres);
		maxlen = chrCopyHeight * chrCopyWidth;
	
		if(bIsFlex)
		{
		   xlen = w;
		   ylen = h;
		}
		else
		{
			xlen = chrCopyWidth;
			ylen = chrCopyHeight;
		}

		//start of paste from buffer.
		pp=0;

		for(i=0;i<ylen;i++)
		{
			for(j=0;j<xlen;j++)
			{
				if (countlen >= maxlen) continue;

				if(x+j<16&&y+i<16)  //safety for making sure selection isnt larger than table.
				{
					for(int m=0;m<16;m++) mask[m]=0xFF; //set mask.
                    for(int m=0;m<16;m++) subMask[m]=0xFF; //set mask.
					for(int l=0;l<8;l++)
					{

						pd=(bankViewTable[set+(x+j) + (y+i)*16]%chrmax) + tileViewTable[(x+j) + (y+i)*16]*16 + l;
						//set mask
						if(Applytopaste2->Checked){

							if (b0) mask[l]	   =~mask[l]	|(chr[pd]	|chr[pd+8]);
							if (b1) mask[l]    = mask[l]   &~(chr[pd]	&(chr[pd]^chr[pd+8]));
							if (b2)	mask[l]    = mask[l]   &~(chr[pd+8]	&(chr[pd]^chr[pd+8]));
							if (b3) mask[l]	   = mask[l]	-(chr[pd]	&chr[pd+8]);

							mask[l+8]	=mask[l]; //makes the 1bit mask applicable to 2bit gfx.

							//used by subtractive paste with offset.
							if(bSubMask){
								subMask[l]	   =~subMask[l]	|(chr[pd]	|chr[pd+8]);
								subMask[l+8] 	=subMask[l];
							}
						}
					}

					//prep special mode pastemask
					for(int m=0;m<16;m++) pasteMask[m]=0xFF; //set mask.
					for(int l=0;l<8;l++)
					{

						if (pm0) pasteMask[l]	 =~pasteMask[l]	   |(chrCopy[pp+l]	|chrCopy[pp+8+l]);
						if (pm1) pasteMask[l]    = pasteMask[l]   &~(chrCopy[pp+l]	&(chrCopy[pp+l]^chrCopy[pp+8+l]));
						if (pm2) pasteMask[l]    = pasteMask[l]   &~(chrCopy[pp+8+l]	&(chrCopy[pp+l]^chrCopy[pp+8+l]));
						if (pm3) pasteMask[l]	 = pasteMask[l]	   -(chrCopy[pp+l]	&chrCopy[pp+8+l]);

						pasteMask[l+8]	=pasteMask[l]; //makes the 1bit mask applicable to 2bit gfx.



					}
					//for(int m=0;m<16;m++) pasteMask[m]= ~pasteMask[m];



					for(k=0;k<fLen;k++)  //pasting tile
					{

						pd=(bankViewTable[set+(x+j) + (y+i)*16]%chrmax) + tileViewTable[(x+j) + (y+i)*16]*16 + k;



						//if-else begins
						if (bPasteMaskSolids || bPasteMaskCol0 || bPasteMaskCustom || bOnTop )   //includes checkboxes for borrowing the pastemask to algorithms b and c.
						{
							if (bKeepMask) {chr[pd+fOff]= (chr[pd+fOff] & ~pasteMask[k+fOff]);}   //cuts out everything except masked area
							else chr[pd+fOff]= (chr[pd+fOff] &pasteMask[k+fOff]); //cuts out areas to be pasted over.
						}


						else if(bAddPaste || bBitOR || bBitAND || bBitNAND ||bSubPaste || bKeepSim || bKeepDiff  )
						{
							//do nothing
						}
						else 				{chr[pd+fOff]= (chr[pd+fOff]&~mask[k+fOff]);}     //replace mode    (normal)


						//step 2: merge methods
						if(bKeepMask || bRemoveMask){
							//do nothing, mask was kept/cut in previous step.
						}

						else if(bAddPaste ||bSubPaste || bKeepSim || bKeepDiff){
							//colour # merge
							int pd2=(bankViewTable[set+(x+j) + (y+i)*16]%chrmax) + tileViewTable[(x+j) + (y+i)*16]*16 + (k%8);
							for(int m=0; m<8; m++) {

								pdColID[m]=0;
								if(bKeepSim || bKeepDiff) {
									pdColID[m]+= (((chr[pd2]&~pasteMask[k%8]) & (1 << m)) != 0)? 1:0;
									pdColID[m]+= (((chr[pd2+8]&~pasteMask[(k%8)+8]) & (1 << m)) != 0)? 2:0;
								}
								else{
									pdColID[m]+= (((chr[pd2]) & (1 << m)) != 0)? 1:0;
									pdColID[m]+= (((chr[pd2+8]) & (1 << m)) != 0)? 2:0;
								}
								psColID[m]=0;
								psColID[m]+= (((chrCopy[pp+(k%8)]&~pasteMask[k%8]) & (1 << m)) != 0)? 1:0;
								psColID[m]+= (((chrCopy[pp+(k%8)+8]&~pasteMask[(k%8)+8]) & (1 << m)) != 0)? 2:0;
							}


							//method
							if(bAddPaste){

									for(int m=0; m<8; m++) { pdColID[m] = max(min(pdColID[m]+psColID[m]-add_offset, 3),0);}
									tmpTile[k]=0;
									tmpTile[k+8]=0;
									for(int m=0; m<8; m++){

											tmpTile[k%8]   |= (pdColID[m]&1)<<(m);
											tmpTile[(k%8)+8] |= ((pdColID[m]&2)>>1)<<(m);
									}


							}
							if(bSubPaste){
									for(int m=0; m<8; m++) { pdColID[m] = min(max(pdColID[m]-psColID[m]+sub_offset,0),3);}
									tmpTile[k]=0;
									tmpTile[k+8]=0;
									for(int m=0; m<8; m++){

											tmpTile[k%8]   |= (((pdColID[m]&1)<<(m))&subMask[k%8]);
											tmpTile[(k%8)+8] |= ((((pdColID[m]&2)>>1)<<(m))&subMask[(k%8)+8]);
									}

							}

							if(bKeepSim){
									for(int m=0; m<8; m++) {
										if 	(pdColID[m] == psColID[m]) compMask |= (1 << m);
										else 						   compMask &= ~(1 << m);
									}
									compMask |= ~mask[k+fOff]; //preserve protected colours
									tmpTile[k+fOff] = chr[pd+fOff] & compMask;
									//chr[pd+fOff] &= compMask;
							}

							if(bKeepDiff){
										for(int m=0; m<8; m++) {
										if 	(pdColID[m] == psColID[m]) compMask &= ~(1 << m);
										else                           compMask |= (1 << m);
									}
									compMask |= ~mask[k+fOff]; //preserve protected colours
									tmpTile[k+fOff] = chr[pd+fOff] & compMask;

							}
							

						}
						else{
							//bitwise merge actions

							if(bAsSilhouette){
								if(bBitAND)      	chr[pd+fOff]=chr[pd+fOff]& (tmpChrCol[k+fOff]&mask[k+fOff]&~pasteMask[k+fOff]);
								else if(bBitNAND)   chr[pd+fOff]=chr[pd+fOff]&~(tmpChrCol[k+fOff]&mask[k+fOff]&~pasteMask[k+fOff]);
								else 			   	chr[pd+fOff]=chr[pd+fOff]| (tmpChrCol[k+fOff]&mask[k+fOff]&~pasteMask[k+fOff]); //normal replace, and OR.
							}
							else if(bAsInvSilhouette){
								if(bBitAND)      	chr[pd+fOff]=chr[pd+fOff]& (tmpChrCol[k+fOff]&mask[k+fOff]&pasteMask[k+fOff]);
								else if(bBitNAND)   chr[pd+fOff]=chr[pd+fOff]&~(tmpChrCol[k+fOff]&mask[k+fOff]&pasteMask[k+fOff]);
								else 			   	chr[pd+fOff]=chr[pd+fOff]| (tmpChrCol[k+fOff]&mask[k+fOff]&pasteMask[k+fOff]); //normal replace, and OR.
							}
							else{   //normal
								if(bBitAND)      	chr[pd+fOff]=chr[pd+fOff]& (chrCopy[pp+k+fOff]&mask[k+fOff]&~pasteMask[k+fOff]);
								else if(bBitNAND)   chr[pd+fOff]=chr[pd+fOff]&~(chrCopy[pp+k+fOff]&mask[k+fOff]&~pasteMask[k+fOff]);
								else 			   	chr[pd+fOff]=chr[pd+fOff]| (chrCopy[pp+k+fOff]&mask[k+fOff]&~pasteMask[k+fOff]); //normal replace, and OR.
							}
						}

					}
					propOff =  tileViewTable[(x+j) + (y+i)*16];
					tileProperties[((bankViewTable[set+(x+j) + (y+i)*16]/16)%propmax) + propOff]=propCopy[(pp/16)];

				}
				//paste things that need to be pasted from buffer
				if(bKeepSim || bKeepDiff || bAddPaste || bSubPaste){
					for(int l=fOff;l<fLen;l++)
					{
						int tmppd = (bankViewTable[set+(x+j) + (y+i)*16]%chrmax) + tileViewTable[(x+j) + (y+i)*16]*16 + l;
						if(bKeepDiff) chr[tmppd]= (tmpTile[l+fOff] &mask[l+fOff]&~pasteMask[l+fOff]);
						else chr[tmppd]= (chr[tmppd]&~mask[l+fOff]) | (tmpTile[l+fOff] &mask[l+fOff]&~pasteMask[l+fOff]);
					}
				}

				pp+=16;
				countlen++;
			}
		}
		//ShowMessage(pdColID[0]);
	}
	//if clipboard was 1-dimensional (from multi-select, likely).
	else
	{
		if(chrCopyWidth<1) return;
		SetUndo();
		GetSelection(chrSelection,x,y,w,h);
		bIsFlex = (w>selectThres || h>selectThres);

		if(bIsFlex)
		{
		   xlen = w;
		   ylen = h;
		}
		else
		{
			xlen = chrCopyWidth;
			ylen = 1;
		}
		pp=0+fOff;
		pd=tileActive*16+fOff;
		int prop_p = 0;
		int prop_d = tileActive;
		//added to signal the origin to the user, in case of no selection or present multi selection
		if (!bIsFlex) SelectTile(tileActive);

		for(j=0;j<ylen;j++)
		{
			for(i=0;i<xlen;i++)
			{
				if (countlen >= chrCopyWidth) continue;
				if (bIsFlex) {pd=(bankViewTable[set+(x+i) +(y+j)*16]%chrmax)+(((x+i)*16 +(y+j)*256 + fOff)&0x0fff);
							  prop_d=((bankViewTable[set+(x+i) +(y+j)*16]/16)%propmax)+(((x+i) +(y+j)*16)&0xff);
				}
				else 		 {pd=(bankViewTable[set+(x+i) +(y+j)*16]%chrmax)+(pd&0x0fff);
							  prop_d=((bankViewTable[set+(x+i) +(y+j)*16]/16)%propmax)+(prop_d&0xff);
				}
				for(int m=0;m<16;m++) mask[m]=0xFF; //set mask.

				for(int l=0;l<8;l++)
				{
					//pd=bankActive+(x+j)*16+(y+i)*256+l;
					//set mask
					if(Applytopaste2->Checked){

						if (b0) mask[l]	   =~mask[l]	|(chr[pd+l]	|chr[pd+8+l]);
						if (b1) mask[l]    = mask[l]   &~(chr[pd+l]	&(chr[pd+l]^chr[pd+8+l]));
						if (b2)	mask[l]    = mask[l]   &~(chr[pd+8+l]	&(chr[pd+l]^chr[pd+8+l]));
						if (b3) mask[l]	   = mask[l]	-(chr[pd+l]	&chr[pd+8+l]);

						mask[l+8]	=mask[l]; //makes the 1bit mask applicable to 2bit gfx.
					}
				}

				for(k=0;k<fLen;k++)
				{
					//new; using colour protection masks
					chr[pd]=(chr[pd]&~mask[k]);          //+fOff
					chr[pd]=chr[pd]|(chrCopy[pp]&mask[k]);   //+fOff
					pd++;
					pp++;

					//original
					//chr[pd++]=chrCopy[pp++];
				}
				if(fLen==8){pp+=8;pd+=8;}
				countlen++;

				tileProperties[prop_d]=propCopy[prop_p];
				prop_p++;
				prop_d++;
			}
		}
	}
	UpdateAll();
	/*
	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);
	if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
	if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
	if (FormNavigator->Visible) FormNavigator->Draw(false,false);
	cueUpdateMetasprite=true;
	*/
}



void __fastcall TFormMain::NameTableScrollLeft(bool all)
{
	unsigned char temp[1024];
	int i,j,k;

	for(k=0;k<(all?2:1);k++)
	{
		for(i=0;i<nameTableHeight;i++) temp[i]=nameTable[i*nameTableWidth];

		for(i=0;i<nameTableWidth-1;i++)
		{
			for(j=0;j<nameTableHeight;j++)
			{
				nameTable[j*nameTableWidth+i]=nameTable[j*nameTableWidth+i+1];
			}
		}

		for(i=0;i<nameTableHeight;i++) nameTable[i*nameTableWidth+nameTableWidth-1]=temp[i];
	}

	if(all)
	{
		for(i=0;i<nameTableHeight/2;i++) temp[i]=AttrGet(0,i*2,false,false);

		for(i=0;i<nameTableWidth/2-1;i++)
		{
			for(j=0;j<nameTableHeight/2;j++)
			{
				AttrSet(i*2,j*2,AttrGet(i*2+2,j*2,false,false),false);
			}
		}

		for(i=0;i<nameTableHeight/2;i++) AttrSet(nameTableWidth-2,i*2,temp[i],false);
	}

	UpdateNameTable(-1,-1,true);


	bNavDoSnap=false;
	bNavDoAlign=false;
	bNavDoZoomchange=false;
	FormNavigator->DelayedDraw->Enabled=false;  //reset timer on every event
	FormNavigator->DelayedDraw->Interval=300;
	FormNavigator->DelayedDraw->Enabled=true;

	//FormNavigator->CueDrawTimer->Enabled=true;
	//FormNavigator->Draw(false,false,false);
	//FormNavigator->UpdateLines(true);

}



void __fastcall TFormMain::NameTableScrollRight(bool all)
{
	unsigned char temp[1024];
	int i,j,k;

	for(k=0;k<(all?2:1);k++)
	{
		for(i=0;i<nameTableHeight;i++) temp[i]=nameTable[i*nameTableWidth+nameTableWidth-1];

		for(i=nameTableWidth-1;i>0;i--)
		{
			for(j=0;j<nameTableHeight;j++)
			{
				nameTable[j*nameTableWidth+i]=nameTable[j*nameTableWidth+i-1];
			}
		}

		for(i=0;i<nameTableHeight;i++) nameTable[i*nameTableWidth]=temp[i];
	}

	if(all)
	{
		for(i=0;i<nameTableHeight/2;i++) temp[i]=AttrGet(nameTableWidth-2,i*2,false,false);

		for(i=nameTableWidth/2-1;i>0;i--)
		{
			for(j=0;j<nameTableHeight/2;j++)
			{
				AttrSet(i*2,j*2,AttrGet(i*2-2,j*2,false,false),false);
			}
		}

		for(i=0;i<nameTableHeight/2;i++) AttrSet(0,i*2,temp[i],false);
	}

	UpdateNameTable(-1,-1,true);

	bNavDoSnap=false;
	bNavDoAlign=false;
	bNavDoZoomchange=false;
	FormNavigator->DelayedDraw->Enabled=false;  //reset timer on every event
	FormNavigator->DelayedDraw->Interval=300;
	FormNavigator->DelayedDraw->Enabled=true;

	//FormNavigator->Draw(false,false,false);
	//FormNavigator->UpdateLines(true);
	//FormNavigator->Draw(false,false);
}



void __fastcall TFormMain::NameTableScrollUp(bool all)
{
	unsigned char temp[1024];
	int i,j,k;

	for(k=0;k<(all?2:1);k++)
	{
		for(i=0;i<nameTableWidth;i++) temp[i]=nameTable[i];

		for(i=0;i<nameTableHeight-1;i++)
		{
			for(j=0;j<nameTableWidth;j++)
			{
				nameTable[i*nameTableWidth+j]=nameTable[(i+1)*nameTableWidth+j];
			}
		}

		for(i=0;i<nameTableWidth;i++) nameTable[nameTableWidth*nameTableHeight-nameTableWidth+i]=temp[i];
	}

	if(all)
	{
		for(i=0;i<nameTableWidth/2;i++) temp[i]=AttrGet(i*2,0,false,false);

		for(i=0;i<nameTableHeight/2-1;i++)
		{
			for(j=0;j<nameTableWidth/2;j++)
			{
				AttrSet(j*2,i*2,AttrGet(j*2,i*2+2,false,false),false);
			}
		}

		for(i=0;i<nameTableWidth/2;i++) AttrSet(i*2,nameTableHeight-2,temp[i],false);
	}

	UpdateNameTable(-1,-1,true);

	bNavDoSnap=false;
	bNavDoAlign=false;
	bNavDoZoomchange=false;
	FormNavigator->DelayedDraw->Enabled=false;  //reset timer on every event
	FormNavigator->DelayedDraw->Interval=300;
	FormNavigator->DelayedDraw->Enabled=true;

	//FormNavigator->Draw(false,false,false);
	//FormNavigator->UpdateLines(true);
}



void __fastcall TFormMain::NameTableScrollDown(bool all)
{
	unsigned char temp[1024];
	int i,j,k;

	for(k=0;k<(all?2:1);k++)
	{
		for(i=0;i<nameTableWidth;i++) temp[i]=nameTable[nameTableWidth*nameTableHeight-nameTableWidth+i];

		for(i=nameTableHeight-1;i>0;i--)
		{
			for(j=0;j<nameTableWidth;j++)
			{
				nameTable[i*nameTableWidth+j]=nameTable[(i-1)*nameTableWidth+j];
			}
		}

		for(i=0;i<nameTableWidth;i++) nameTable[i]=temp[i];
	}

	if(all)
	{
		for(i=0;i<nameTableWidth/2;i++) temp[i]=AttrGet(i*2,nameTableHeight-2,false,false);

		for(i=nameTableHeight/2-1;i>0;i--)
		{
			for(j=0;j<nameTableWidth/2;j++)
			{
				AttrSet(j*2,i*2,AttrGet(j*2,i*2-2,false,false),false);
			}
		}

		for(i=0;i<nameTableWidth/2;i++) AttrSet(i*2,0,temp[i],false);
	}

	UpdateNameTable(-1,-1,true);

	bNavDoSnap=false;
	bNavDoAlign=false;
	bNavDoZoomchange=false;
	FormNavigator->DelayedDraw->Enabled=false;  //reset timer on every event
	FormNavigator->DelayedDraw->Interval=300;
	FormNavigator->DelayedDraw->Enabled=true;
	//FormNavigator->Draw(false,false,false);
	//FormNavigator->UpdateLines(true);
}



bool __fastcall TFormMain::OpenCHR(AnsiString name, bool openAll)
{
	unsigned char buf[4096*4];
	FILE *file;
	int i,pp,off,size,type;
	int threshold=4096;
	int set=bankActive/16;
	if(bImportCHRCarry) threshold=8192;

	file=fopen(name.c_str(),"rb");

	type=-1;

	if(file)
	{
		size=get_file_size(file);

		off=0;

		switch(size)
		{
		case 16384:
			fread(buf,16384,1,file);
			for(int tile=0;tile<1024;tile++){
				pp=(tile%256)*16 + bankViewTable[tile];
				memcpy(chr+pp,buf+tile*16,16);
			}
			type=5;
			break;



		case 8192:
			fread(buf,8192,1,file);
			for(int tile=0;tile<512;tile++){
				if(tile+set<1024){
					pp=(tile%256)*16 + bankViewTable[((tile+set)%1024)];
					memcpy(chr+pp,buf+tile*16,16);
				}
			}

			type=4;
			break;

		case 4096:
			fread(buf,4096,1,file);
			for(int tile=0;tile<256;tile++){
				if(tile+set<1024){
					pp=(tile%256)*16 + bankViewTable[((tile+set)%1024)];
					memcpy(chr+pp,buf+tile*16,16);
				}
			}
			type=3;
			break;

		default:
			if((size<16384)&&!(size&15))
			{
				fread(buf,size,1,file);

				for(int tile=0;tile<size/16;tile++){

					if(bImportCHRWrap){
						pp=((tileViewTable[tileActive]+tile)%256)*16+bankViewTable[(tileActive+tile+set)%(256*4)];
					}
					else pp=((tileViewTable[tileActive]+tile)%256)*16+bankViewTable[(tileActive+tile+set)%(256*4)];


					if(tile+tileActive>=threshold) {
						if(bImportCHRSkip) break;
					}

					memcpy(chr+pp,buf+tile*16,16);
				}

				if(size==2048) type=2; else if(size==1024) type=1; else type=0;
			}
			else if(size>16384&&!(size&15))
			{

				unsigned int oldChrBanks=chrBanks;
				chrBanks=(size/4096);
				if(chrBanks>oldChrBanks) {
					chr = (char*) realloc(chr, chrBanks*4096);
					tileProperties = (char*) realloc(tileProperties, chrBanks*256);
				}
				fread(chr,size,1,file);
				OpenByAssociationListMaker->Enabled=true;

			}
			else
			{
				Application->MessageBox("Wrong file size","Error",MB_OK);
				fclose(file);
				return false;
			}
		}
	}
	else{
		if(!openAll){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+name;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);   }
	}
	fclose(file);

	if(type<0) return false;

	SetLatestCHR(off,size, true);
	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);

	if(openByFileDone) {
		if (FormNavigator->Visible) FormNavigator->Draw(true,true,false);
		//if (FormCHRbit->Visible)	FormCHRbit->UpdateBitButtons();
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;

	return true;
}

bool __fastcall TFormMain::OpenTprop(AnsiString name, bool openAll)
{
	unsigned char buf[1024];
	FILE *file;
	int i,pp,size,type;
	int set = bankActive/16;
	file=fopen(name.c_str(),"rb");

	type=-1;

	if(file)
	{
		size=get_file_size(file);

		//off=0;

		switch(size)
		{
		case 1024:
			 fread(buf,1024,1,file);
			for(i=0;i<size;i++)	tileProperties[bankViewTable[i]/16+i]=buf[i];
			type=3;
			break;
		case 512:
			fread(buf,512,1,file);
			for(i=0;i<size;i++)	tileProperties[bankViewTable[i]/16 + i]=buf[i];
			type=2;
			break;

		case 256:
			fread(buf,256,1,file);
			for(i=0;i<size;i++)	tileProperties[bankViewTable[set+i]/16+i]=buf[i];
			type=1;
			break;

		default:
			if(size<256)
			{
				fread(buf,size,1,file);

				pp=tileActive;
				//off=pp;

				for(i=0;i<size;i++)
				{
					tileProperties[bankViewTable[set+pp]/16+pp++]=buf[i];
					if(pp>=256) pp=0;
				}
				type=0;
			}
			else
			{
				Application->MessageBox("Wrong file size","Error",MB_OK);
				fclose(file);
				return false;
			}
		}
	}
    else{
		if(!openAll){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+name;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);
		}
	}
	fclose(file);
	if(openAll) return false;
	if(type<0) {
		Application->MessageBox("Unknown tprop type","Warning", MB_OK);
		return false;
	}
	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);

	if(openByFileDone) {
		if (FormNavigator->Visible) FormNavigator->Draw(true,true,false);
		if (FormCHRbit->Visible)	FormCHRbit->UpdateBitButtons(false);
		if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
	}
	return true;
}


void __fastcall TFormMain::SaveCHR(int offset,int size,bool fromSet)
{
	int set=bankActive/16;
	unsigned char buf[4096*4];
	FILE *file;
	int i,pp,off;
	AnsiString name;

	if(!SaveDialogChr->Execute()) return;

	name=RemoveExt(SaveDialogChr->FileName)+".chr";

	if(!OverwritePrompt(name)) return;

	file=fopen(name.c_str(),"rb");

	if(file)
	{
		fseek(file,0,SEEK_END);
		i=ftell(file);
		fclose(file);

		if(size!=i)
		{
			if(Application->MessageBox(("Previous file has different size ("+IntToStr(i)+" bytes)!\nDo you really want to overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES) return;
		}
	}

	file=fopen(name.c_str(),"wb");

	if(!file) return;

	if(fromSet){


		if(size==4096||size==8192||size==8192*2)
		{
			if(size==4096){
				//memcpy(buf,chr+bankViewTable[bankActive/16],4096);
				for(int tile=0;tile<256;tile++){

					pp=tile*16+bankViewTable[(tile+set)];
					memcpy(buf+tile*16,chr+pp,16);
				}
			}
			else if(size==8192){


				for(int tile=0;tile<512;tile++){

						pp=(tile%256)*16+bankViewTable[(tile+set)%1024];
						memcpy(buf+tile*16,chr+pp,16);

				}
			}
			else if(size==8192*2){

				for(int tile=0;tile<256*4;tile++){

						pp=(tile%256)*16+bankViewTable[tile];
						memcpy(buf+tile*16,chr+pp,16);
					
				}
			}

		}
		else
		{
			int off=offset/16;
			int s=size/16;
			int desttile=0;
			for(int tile=off;tile<s+off;tile++)
			{
			   pp=(tile%256)*16+bankViewTable[(((tile%256)+set)%1024)];
			   memcpy(buf+desttile*16,chr+pp,16);
			   desttile++;
			}
		}
	}
	else{
		if(size==4096){
			memcpy(buf,chr+curViewCHRbankCanvas2*4096,4096);
		}
		else{
			pp=offset;

			for(i=0;i<size;i++)
			{
				if(pp>=4096) pp=0;

				buf[i]=chr[curViewCHRbankCanvas2*4096+pp];

				++pp;
			}
		}

	}

	fwrite(buf,size,1,file);
	fclose(file);

	SetLatestCHR(offset,size,fromSet);
}

void __fastcall TFormMain::SaveTprop(int offset, int size)
{
	unsigned char buf[512];
	FILE *file;
	int i,pp,off;
	AnsiString name;
	int set = bankActive/16;

	if(!SaveDialogTprop->Execute()) return;

	name=RemoveExt(SaveDialogTprop->FileName)+".tprop";

	if(!OverwritePrompt(name)) return;

	file=fopen(name.c_str(),"rb");

	if(file)
	{
		fseek(file,0,SEEK_END);
		i=ftell(file);
		fclose(file);

		if(size!=i)
		{
			if(Application->MessageBox(("Previous file has different size ("+IntToStr(i)+" bytes)!\nDo you really want to overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES) return;
		}
	}

	file=fopen(name.c_str(),"wb");

	if(!file) return;

	if(size==256||size==1024)
	{
		if(size==256)
		{
			for(int i=0;i<size;i++)buf[i]=(tileProperties[bankViewTable[set+i]/16+i])&0xFF;

		}
		else
		{
			 for(int i=0;i<size;i++)buf[i]=tileProperties[bankViewTable[set+i]/16+i]&0xFF;

		}
	}
	else
	{
		pp=offset;

		for(i=0;i<size;i++)
		{
			if(pp>=256) pp=0;

			buf[i]=tileProperties[bankViewTable[set+pp]/16+pp]&0xFF;

			++pp;
		}
	}

	fwrite(buf,size,1,file);
	fclose(file);
}

void __fastcall TFormMain::InterleaveCHR(bool dir)
{
	unsigned char buf[4096];
	uint32_t bufProp[256];

	int table[256];
	int i,j,pp,pd,num;
	//int chrmax = chrBanks*4096;
	//int propmax = chrBanks*256;

	SetUndo();

	for(i=0;i<16;i++)
	{
		num=(i/2)*32+(i&1);

		for(j=0;j<16;j++)
		{
			if(dir) table[i*16+j]=num; else table[num]=i*16+j;
			num+=2;
		}
	}
	int set=bankActive/16;

	for(i=0;i<256;i++)
	{
		pp=bankViewTable[set+i]+i*16;
		pd=table[i]*16;

		for(j=0;j<16;j++)
		{
			buf[pd++]=chr[pp++];
		}
		bufProp[table[i]]=tileProperties[(bankViewTable[set+i]/16)+i];
	}

	//transfer buf to chr
	for(i=0;i<256;i++)
	{
		memcpy(chr+bankViewTable[set+i]+i*16,buf+i*16,16);
		tileProperties[bankViewTable[set+i]/16+i]=bufProp[i];
	}
	//memcpy(chr+bankViewTable[set],buf,4096);
	//memcpy(tileProperties+((bankViewTable[set]/16)%propmax),bufProp,256*sizeof(uint32_t));

	UpdateAll();
	/*
	UpdateTiles(true);
	//WorkCHRToBankCHR();
	UpdateNameTable(-1,-1,true);
	if (FormNavigator->Visible) FormNavigator->Draw(false,false);
	if (FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
	if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;  */
}



int parse_skip_empty(unsigned char* data,int ptr,int size)
{
	char c;
	
	if(ptr<0) return ptr;
	
	while(ptr<size)
	{
		c=data[ptr];

		if(c>' ') break;

		++ptr;
	}

	return ptr;
}

int parse_skip_tag(unsigned char* data,int ptr,int size,const char* tag)
{
	if(ptr<0) return ptr;
	
	if(!memcmp(&data[ptr],tag,strlen(tag))) return ptr+strlen(tag); else return -1;
}

int parse_skip_value(unsigned char* data,int ptr,int size)
{
	char c;
	
	if(ptr<0) return ptr;
	
	while(ptr<size)
	{
		c=data[ptr];

		if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='$'||c=='_')) break;

		++ptr;
	}

	return ptr;
}

int parse_get_value(unsigned char* data,int ptr,int size)
{
	int n;
	char c;
	bool hex;

	if(ptr<0) return ptr;
	
	n=0;
	hex=false;
	
	while(ptr<size)
	{
		c=data[ptr++];

		if(c>='a'&&c<='z') c-=32;

		if(c=='$'||c=='X')
		{
			hex=true;
			continue;
		}

		if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9'))) break;

		if(!hex)
			{
				if(c>='0'&&c<='9')
				{
					n=n*10+(c-'0');
				}
				else return -1;
			}
			else
			{
				if(c>='0'&&c<='9')
				{
					n=n*16+(c-'0');
				}
				else
				if(c>='A'&&c<='F')
				{
					n=n*16+(c-'A'+10);
				}
				else return -1;
			}
	}

	return n;
}



bool __fastcall TFormMain::OpenNameTable(AnsiString name, bool openAll)
{
	FILE *file;
	unsigned char *src,*out;
	unsigned char *dst_mrle;
	unsigned char dst[1024];
	int i,tag,pp,pd,ptr,wdt,hgt,src_size;
	AnsiString ext;

	ext=GetExt(name);

	file=fopen(name.c_str(),"rb");
	if((!file) && (openAll)) return false;
	if(!file){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+name;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);
		return false;
	}


	src_size=get_file_size(file);

	src=(unsigned char*)malloc(src_size);

	if(!src)
	{
		fclose(file);
		return false;
	}

	fread(src,src_size,1,file);
	fclose(file);

	if(ext=="map"||ext=="mrle")
	{
		wdt=src[src_size-4]+src[src_size-3]*256;
		hgt=src[src_size-2]+src[src_size-1]*256;

		if(wdt>NAME_MAX_WIDTH||hgt>NAME_MAX_WIDTH)
		{
			Application->MessageBox("Map size is too large, or a wrong file format","Error",MB_OK);
			free(src);
			return false;
		}

		nameTableWidth=wdt;
		nameTableHeight=hgt;
	}
	if(ext=="map"){
		memcpy(nameTable,src,name_size());
		memcpy(attrTable,&src[name_size()],attr_size());
	}
	else if (ext=="mrle")
	{

			int nt_mrle=wdt*hgt;
			int attr_mrle=(wdt/4)*(hgt/4);
			dst_mrle=(unsigned char*)malloc(nt_mrle+attr_mrle);
			//
			tag=src[0];
			pp=1;
			pd=0;

			while(pp<src_size-4)
			{
				  if(src[pp]==tag)
				  {
					  pp++;
					  if(src[pp]==0) break;
					  for(i=0;i<src[pp];i++) dst_mrle[pd++]=dst_mrle[pd-1];
					  pp++;
				  }
				  else
				  {
					dst_mrle[pd++]=src[pp++];
				  }
			}
			memcpy(nameTable,dst_mrle,nt_mrle);
			memcpy(attrTable,dst_mrle+nt_mrle,attr_mrle);
            free(dst_mrle);

	}
	else
	{
		if(ext=="asm"||ext=="s")
		{
			ptr=0;
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_value(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,":");

			if(ptr<0)
			{
				Application->MessageBox("Can't parse assembly code","Error",MB_OK);
				free(src);
				return false;
			}

			out=(unsigned char*)malloc(2048);
			pd=0;

			while(pd<2048)
			{
				if(ptr>=src_size) break;
				
				if(ptr<0)
				{
					Application->MessageBox("Error while parsing assembly file","Error",MB_OK);
					free(out);
					free(src);
					return false;
				}

				ptr=parse_skip_empty(src,ptr,src_size);

				if(src[ptr]=='.')
				{
					ptr=parse_skip_tag(src,ptr,src_size,".byte");
					ptr=parse_skip_tag(src,ptr,src_size,".db");    //fg
					ptr=parse_skip_empty(src,ptr,src_size);
				}

				out[pd++]=parse_get_value(src,ptr,src_size);
				ptr=parse_skip_value(src,ptr,src_size);
				ptr=parse_skip_empty(src,ptr,src_size);
				
				if(src[ptr]==',') ++ptr;
			}

			free(src);
			src=out;
			src_size=pd;
			if(src_size!=960&&src_size!=1024) ext="rle";
		}

		if(ext=="h")
		{
			ptr=0;
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,"const");
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,"unsigned");
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,"char");
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_value(src,ptr,src_size);
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,"[");
			ptr=parse_skip_empty(src,ptr,src_size);
			//size=parse_get_value(src,ptr,src_size);
			ptr=parse_skip_value(src,ptr,src_size);
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,"]");
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,"=");
			ptr=parse_skip_empty(src,ptr,src_size);
			ptr=parse_skip_tag(src,ptr,src_size,"{");

			if(ptr<0)
			{
				Application->MessageBox("Can't parse C header","Error",MB_OK);
				free(src);
				return false;
			}

			out=(unsigned char*)malloc(2048);
			pd=0;

			while(pd<2048)
			{
				if(ptr>=src_size) break;

				if(ptr<0)
				{
					Application->MessageBox("Error while parsing C header","Error",MB_OK);
					free(out);
					free(src);
					return false;
				}

				ptr=parse_skip_empty(src,ptr,src_size);
				out[pd++]=parse_get_value(src,ptr,src_size);
				ptr=parse_skip_value(src,ptr,src_size);
				ptr=parse_skip_empty(src,ptr,src_size);
				if(src[ptr]=='}') break;
				ptr=parse_skip_tag(src,ptr,src_size,",");
			}

			free(src);
			src=out;
			src_size=pd;
			if(src_size!=960&&src_size!=1024) ext="rle";
		}

		if(ext!="nrle" && ext!="rle")  //any other, predominantly .nam
		{
			if(src_size==960||src_size==1024)
			{
				memcpy(nameTable,src,960);

				if(src_size==1024) memcpy(attrTable,&src[960],64);
			}
			else if(src_size==768) // TMS9918 mode 2 (MSX etc)
			{
				//32x24
				memcpy(nameTable,src,768);  //PN without CT (colour table; currently not supported).
				nameTableWidth=32;
				nameTableHeight=24;
			}
			else if (src_size==896) //PCE/TurboGFX
			{
				//32x28
				memcpy(nameTable,src,896);  //no colour information either.
                nameTableWidth=32;
				nameTableHeight=28;
			}

			else
			{
				Application->MessageBox("NES Nametable should be 960 or 1024 bytes long","Error",MB_OK);
				free(src);
				return false;
			}
		}
		else
		{
			if(src_size<2048)
			{
				tag=src[0];
				pp=1;
				pd=0;

				while(pp<src_size)
				{
					if(src[pp]==tag)
					{
						pp++;
						if(src[pp]==0) break;
						for(i=0;i<src[pp];i++) dst[pd++]=dst[pd-1];
						pp++;
					}
					else
					{
						dst[pd++]=src[pp++];
					}
				}
			}

			if(pd==959||pd==1023) pd++;//for really old files saved when RLE packer had a bug
			if(pd==960||pd==1024) memcpy(nameTable,dst,960);
			if(pd==1024) memcpy(attrTable,dst+960,64);
			if(pd!=960&&pd!=1024) Application->MessageBox("Can't decode RLE","Error",MB_OK);
		}

		nameTableWidth=32;
		nameTableHeight=30;
	}

	free(src);

	UpdateAll();

	return true;
}



bool __fastcall TFormMain::OpenPalette(AnsiString name, bool openAll)
{
	
	FILE *file;
	unsigned char pal[64]; //4 sets, 16 colours.
	int i;
	AnsiString ext;
	ext=GetExt(name);
	file=fopen(name.c_str(),"rb");
	
	int wrap=0;
	if(file)
	{

		int size =get_file_size(file);
		if((ext=="dat" && size>=16) || (size==16)) //nominal behaviour.
		{
			fread(pal,16,1,file);
			fclose(file);

			for(i=0;i<4;i++)
			{
				bgPal[palBank*16+0*4+i]=pal[i+0];
				bgPal[palBank*16+1*4+i]=pal[i+4];
				bgPal[palBank*16+2*4+i]=pal[i+8];
				bgPal[palBank*16+3*4+i]=pal[i+12];
			}

			UpdateAll();

			if(ext=="pal")return true; //non .pal extensions (such as .dat) count as imports and do not overload session file names.
		}
		else if (size == 4 || size == 8 || size == 12) //single subpalette, multiple subpalettes or just some list of colours smaller than a full set.
		{
			fread(pal,size,1,file);
			fclose(file);

			for(i=0;i<size;i++)
			{
				if (palActive*4+i >= palActive*4+16 && wrap==0)
				{
					wrap -= palActive*4;
                    
				}
				bgPal[palBank*16+palActive*4+i+wrap]=pal[i+0];
			}
			if(wrap<0){
				LabelStats->Caption="Note: Subpalette wraparound occurred.";
				StatusUpdateWaiter->Enabled=true;
				holdStats=true;
			}
            UpdateAll();
		}
		else if (size == 32 || size == 48 || size == 64) //multiple sets of subpalettes.
		{
			fread(pal,size,1,file);
			fclose(file);

			for(i=0;i<size;i++)
			{
				if (palBank*16 +i >= palBank*16 + 64 && wrap == 0)
				{
					wrap -= palBank*16;
				}
				bgPal[palBank*16+i+wrap]=pal[i+0];
			}
            if(wrap<0){
				LabelStats->Caption="Note: Subpal Set wraparound occurred.";
				StatusUpdateWaiter->Enabled=true;
				holdStats=true;
			}
			UpdateAll();
		}

		else //error handling
		{
			if (ext=="dat") Application->MessageBox("YY-CHR .dat should be at least 16 bytes long","Error",MB_OK);
			else Application->MessageBox("File should be 4, 8, 12, 16, 32, 48 or 64 bytes long","Error",MB_OK);
		}
	}
	else{
		if(!openAll){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+name;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);
		}  }
	return false;
}



void __fastcall TFormMain::GetSelection(TRect r,int &x,int &y,int &w,int &h)
{
	if(r.left<r.right)
	{
		x=r.left;
		w=r.right-x;
	}
	else
	{
		x=r.right;
		w=r.left-x;
	}
	if(r.top<r.bottom)
	{
		y=r.top;
		h=r.bottom-y;
	}
	else
	{
		y=r.bottom;
		h=r.top-y;
	}
}



void __fastcall TFormMain::CopyMap(bool cut)
{
	int i,j,x,y,w,h,pp;

	GetSelection(nameSelection,x,y,w,h);

	if(w&&h&&cut) SetUndo();

	pp=0;

	for(i=0;i<h;++i)
	{
		for(j=0;j<w;++j)
		{
			nameCopy[pp]=nameTable[y*nameTableWidth+x+j];
			attrCopy[pp]=AttrGet(x+j,y,false,false);

			if(cut&&SpeedButtonTiles->Down) nameTable[y*nameTableWidth+x+j]=nullTile;

			++pp;
		}

		++y;
	}

	nameCopyWidth=w;
	nameCopyHeight=h;

	DWORD Len=2*sizeof(int)+sizeof(nameCopy)+sizeof(attrCopy);

	HGLOBAL hDst=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,Len);

	if (hDst)
	{
		LPSTR gDst=(LPSTR)GlobalLock(hDst);

		if (gDst)
		{
			if (OpenClipboard(Handle))
			{
				EmptyClipboard();

				*((int*)&gDst[0*sizeof(int)])=nameCopyWidth;
				*((int*)&gDst[1*sizeof(int)])=nameCopyHeight;
				memcpy(&gDst[8],nameCopy,sizeof(nameCopy));
				memcpy(&gDst[8+sizeof(nameCopy)],attrCopy,sizeof(attrCopy));

				GlobalUnlock(hDst);

				SetClipboardData(CF_NAM,hDst);
				CloseClipboard();
			}
		}
	}

	if(cut) {
		UpdateNameTable(-1,-1,true);
		if(FormNavigator->Visible)
		{
			GetSelection(nameSelection,x,y,w,h);
			FormNavigator->DrawRange(x,y,w,h,false);
			FormNavigator->UpdateLines(false);
		}
	}  //FormNavigator->Draw(false,false);}
}

void __fastcall TFormMain::CopyMetatiles(bool cut, bool fullPage)
{

	int i,j,x,y,w,h,d,cnt;

	bool doTile  = FormMetatileTool->btnTiles->Down;
	bool doPal   = FormMetatileTool->btnAttr->Down;
	bool doProps = FormMetatileTool->btnProps->Down;

	bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
	bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
	bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;

	char *currentTable_id = NULL;
	char *currentTable_pal = NULL;
	char *currentTable_props = NULL;
	extern TRect setSelection;
	if(bTab2x2){
		d=2;
		currentTable_id = metatileSet_2x2_id;
		currentTable_pal = metatileSet_2x2_pal;
		currentTable_props = metatileSet_2x2_props;
	 }
	if(bTab4x4){
		d=4;
		currentTable_id = metatileSet_4x4_id;
		currentTable_pal = metatileSet_4x4_pal;
		currentTable_props = metatileSet_4x4_props;
	 }
	if(bTab8x8){
		d=8;
		currentTable_id = metatileSet_8x8_id;
		currentTable_pal = metatileSet_8x8_pal;
		currentTable_props = metatileSet_8x8_props;
	 }


	int meta;
	int metasOnRow=sqrt(iSetView);

	if(fullPage){
		meta=iListViewOff; //+ which set, set size..
		w=metasOnRow;
		h=metasOnRow;


	}
	else{
		GetSelection(setSelection,x,y,w,h);
		meta=(y/d)*metasOnRow + (x/d) + iSetViewOff + iListViewOff;
		w=w/d;
		h=h/d;
	}
	if(w&&h&&cut) SetUndo();


	unsigned char *metaCopy_id = (char*)calloc((w*h*d*d),sizeof(unsigned char));
	unsigned char *metaCopy_pal = (char*)calloc((w*h*d*d),sizeof(unsigned char));
	unsigned char *metaCopy_props = (char*)calloc((w*h*d*d),sizeof(unsigned char));

	cnt=0;
	for(i=0;i<h;i++)
	{
		for(j=0;j<w;j++)
		{

			int dst= cnt*d*d;
			int src= (meta + i*metasOnRow + j)*d*d;

				memcpy(&metaCopy_id[dst] 		, &currentTable_id[src]		,d*d);
				memcpy(&metaCopy_pal[dst] 		, &currentTable_pal[src]	,d*d);
				memcpy(&metaCopy_props[dst] 	, &currentTable_props[src]	,d*d);

				if(cut&&doTile) memset(&currentTable_id[src],nullTile,d*d);
				if(cut&&doPal) memset(&currentTable_pal[src],nullTile,d*d);
				if(cut&&doProps) memset(&currentTable_props[src],nullTile,d*d);


				cnt++;
		}
	}




	//DWORD Len=3*sizeof(int)+sizeof(metaCopy_id)+sizeof(metaCopy_pal)+sizeof(metaCopy_props);
	DWORD Len=3*sizeof(int)+(w*h*d*d*3);

	HGLOBAL hDst=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,Len);

	if (hDst)
	{
		LPSTR gDst=(LPSTR)GlobalLock(hDst);

		if (gDst)
		{
			if (OpenClipboard(Handle))
			{
				EmptyClipboard();

				if (cut) LabelStats->Caption="Metatile(s) cut,\nw: "+IntToStr(w)+", h: "+IntToStr(h)+", format: "+IntToStr(d)+"x"+IntToStr(d);
				else     LabelStats->Caption="Metatile(s) copied,\nw: "+IntToStr(w)+", h: "+IntToStr(h)+", format: "+IntToStr(d)+"x"+IntToStr(d);

				holdStats=true;
				*((int*)&gDst[0*sizeof(int)])=w;
				*((int*)&gDst[1*sizeof(int)])=h;
				*((int*)&gDst[2*sizeof(int)])=d;
				memcpy(&gDst[12]					,metaCopy_id	,(w*h*d*d));
				memcpy(&gDst[12+(w*h*d*d)]			,metaCopy_pal	,(w*h*d*d));
				memcpy(&gDst[12+(w*h*d*d*2)]		,metaCopy_props ,(w*h*d*d));


				GlobalUnlock(hDst);

				SetClipboardData(CF_METATILE,hDst);
				CloseClipboard();
			}
		}
	}

	if(cut) {
		FormMetatileTool->Draw();
		if(FormMetatileTool->btnMap->Down){
			UpdateNameTable(-1,-1,true);
			if(FormNavigator->Visible)
			{
				//GetSelection(nameSelection,x,y,w,h);
				//FormNavigator->DrawRange(x,y,w,h,false);
				FormNavigator->UpdateLines(false);
			}
		}
	}

   free(metaCopy_id);
   free(metaCopy_pal);
   free(metaCopy_props);

}

void __fastcall TFormMain::CopyMapCodeASM(void)
{
	char str[65536],buf[1024];
	int i,j,x,y,w,h;
	x=0;y=0;

	if(nameSelection.left>=0&&nameSelection.top>=0)
	{
		GetSelection(nameSelection,x,y,w,h);
		LabelStats->Caption="Selected tilenames (w"+IntToStr(w)+"x"+IntToStr(h)+")\nput on clipboard as ASM text.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}
	else
	{
		w=nameTableWidth;
		h=nameTableHeight;
		LabelStats->Caption="Map put on clipboard as ASM text.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}


	strcpy(str,"");

	for(i=0;i<h;i++)
	{
		if(byte1->Checked) strcat(str,"\t.byte ");
		if(db1->Checked) strcat(str,"\t.db ");


		for(j=0;j<w;j++)
		{
			sprintf(buf,"$%2.2x%c",nameTable[(y+i)*nameTableWidth+x+j],j<w-1?',':'\n');
			strcat(str,buf);
		}
	}

	Clipboard()->SetTextBuf(str);
}



void __fastcall TFormMain::CopyMapCodeC(bool rle)
{
	char str[65536],buf[1024];
	unsigned char src[65536],dst[65536];
	int i,j,x,y,w,h,ptr,size;
	x=0;y=0;
	bool tmp=false;
	if(nameSelection.left>=0&&nameSelection.top>=0)
	{
		GetSelection(nameSelection,x,y,w,h);
		tmp=true;
	}
	else
	{
		w=nameTableWidth;
		h=nameTableHeight;
	}
	if(!rle)
	{
		sprintf(str,"const unsigned char nametable[%i*%i]={\n",w,h);

		for(i=0;i<h;i++)
		{
			strcat(str,"\t");

			for(j=0;j<w;j++)
			{
				sprintf(buf,"0x%2.2x",nameTable[(y+i)*nameTableWidth+x+j]);

				strcat(str,buf);

				if(i*w+j<w*h-1) strcat(str,",");
			}

			strcat(str,"\n");
		}
	}
	else
	{
		ptr=0;

		for(i=0;i<h;i++) for(j=0;j<w;j++) src[ptr++]=nameTable[(y+i)*nameTableWidth+x+j];

		size=encode_rle(src,ptr,dst);

		if(size<0)
		{
			Application->MessageBox("No unused tiles found, can't be encoded with RLE","Error",MB_OK);
			return;
		}

		sprintf(str,"const unsigned char nametable[%i]={\n",size);

		for(i=0;i<size;++i)
		{
			sprintf(buf,"0x%2.2x",dst[i]);
			strcat(str,buf);

			if(i<size-1) strcat(str,",");
			if(((i&15)==15)||(i==size-1)) strcat(str,"\n");
		}
	}

	strcat(str,"};\n\n");
	Clipboard()->SetTextBuf(str);

	if(tmp)
	{
		LabelStats->Caption="Selected names (w"+IntToStr(w)+"x"+IntToStr(h)+")\nput on clipboard as C text.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;

	}
	else
	{
		LabelStats->Caption="Map put on clipboard as C text.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
    }
}




void __fastcall TFormMain::CopyMetaSpriteCodeC(bool hflip)
{
	char str[65536],buf[1024];
	int i,j,x,y,w,h;

	if(nameSelection.left>=0&&nameSelection.top>=0)
	{
		GetSelection(nameSelection,x,y,w,h);

		sprintf(str,"const unsigned char metasprite[]={\n");

		if(!hflip)
		{
			for(i=0;i<h;i++)
			{
				for(j=0;j<w;j++)
				{
					sprintf(buf,"\t%s,%s,0x%2.2x,%i,\n",num_to_3char(j*8),num_to_3char(i*8),nameTable[(y+i)*nameTableWidth+x+j],AttrGet(x+j,y+i,false,false));
					strcat(str,buf);
				}
			}
		}
		else
		{
			for(i=0;i<h;i++)
			{
				for(j=0;j<w;j++)
				{
					sprintf(buf,"\t%s,%s,0x%2.2x,%i|OAM_FLIP_H,\n",num_to_3char(j*8),num_to_3char(i*8),nameTable[(y+i)*nameTableWidth+x+(w-1-j)],AttrGet(x+(w-1-j),y+i,false,false));
					strcat(str,buf);
				}
			}
		}

		strcat(str,"\t128\n};\n\n");

		Clipboard()->SetTextBuf(str);
	}
	else
	{
		LabelStats->Caption="WARNING: Needs a map selection. No action was taken.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
    }
}



void __fastcall TFormMain::PasteMap(void)
{
	int i,j,x,y,w,h,pp;

	OpenClipboard(Handle);

	HGLOBAL hClipBuf = GetClipboardData(CF_NAM);

	if (hClipBuf)
	{
		LPSTR gSrc = (LPSTR)GlobalLock(hClipBuf);

		if (gSrc != NULL)
		{
			nameCopyWidth =*((int*)&gSrc[0*sizeof(int)]);
			nameCopyHeight=*((int*)&gSrc[1*sizeof(int)]);
			memcpy(nameCopy,&gSrc[8],sizeof(nameCopy));
			memcpy(attrCopy,&gSrc[8+sizeof(nameCopy)],sizeof(attrCopy));

			GlobalUnlock(gSrc);
		}
	}

	CloseClipboard();

	if(nameCopyHeight<1||nameCopyWidth<1) return;

	GetSelection(nameSelection,x,y,w,h);
	int bufY = y;

	if(w&&h) SetUndo();

	pp=0;

	for(i=0;i<nameCopyHeight;i++)
	{
		for(j=0;j<nameCopyWidth;j++)
		{
			if(x+j<nameTableWidth&&y<nameTableHeight)
			{
				if(nameCopy[pp]!=nullTile||!ingorepastingnulltiles1->Checked)
				{
					if(SpeedButtonTiles->Down) nameTable[y*nameTableWidth+x+j]=nameCopy[pp];
					if(SpeedButtonPal->Down) AttrSet(x+j,y,attrCopy[pp],false);
				}
			}
			pp++;
		}
		y++;
	}

	//FormNavigator->Draw(false,false);
	if(FormNavigator->Visible) {
		FormNavigator->DrawRange(x,bufY,nameCopyWidth,nameCopyHeight,false);
		FormNavigator->UpdateLines(false);
	}
	UpdateNameTable(-1,-1,true);
}


void __fastcall TFormMain::PasteMetatiles(bool align)
{
	int i,j,x,y,w,h,d,cnt;
	//--
	bool doTile  = FormMetatileTool->btnTiles->Down;
	bool doPal   = FormMetatileTool->btnAttr->Down;
	bool doProps = FormMetatileTool->btnProps->Down;

	bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
	bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
	bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;

	char *currentTable_id = NULL;
	char *currentTable_pal = NULL;
	char *currentTable_props = NULL;
	extern TRect setSelection;
	size_t currentMaxSize;
	if(bTab2x2){
		d=2;
		currentTable_id = metatileSet_2x2_id;
		currentTable_pal = metatileSet_2x2_pal;
		currentTable_props = metatileSet_2x2_props;
		currentMaxSize = metatileSets_2x2*64*d*d;
	 }
	if(bTab4x4){
		d=4;
		currentTable_id = metatileSet_4x4_id;
		currentTable_pal = metatileSet_4x4_pal;
		currentTable_props = metatileSet_4x4_props;
		currentMaxSize = metatileSets_4x4*64*d*d;
	 }
	if(bTab8x8){
		d=8;
		currentTable_id = metatileSet_8x8_id;
		currentTable_pal = metatileSet_8x8_pal;
		currentTable_props = metatileSet_8x8_props;
		currentMaxSize = metatileSets_8x8*64*d*d;
	 }

	int meta;
	int metasOnRow=sqrt(iSetView);
	int buf_w;
	int buf_h;
	int buf_d;

	//--
	OpenClipboard(Handle);

	HGLOBAL hClipBuf = GetClipboardData(CF_METATILE);

	if (hClipBuf)
	{
		LPSTR gSrc = (LPSTR)GlobalLock(hClipBuf);

		if (gSrc != NULL)
		{

			buf_w =*((int*)&gSrc[0*sizeof(int)]);
			buf_h =*((int*)&gSrc[1*sizeof(int)]);
			buf_d =*((int*)&gSrc[2*sizeof(int)]);

			//allocate buffer here
			size_t buf_whdd= buf_w * buf_h * buf_d * buf_d;

			unsigned char *metaCopy_id = (char*)calloc((buf_whdd),sizeof(unsigned char));
			unsigned char *metaCopy_pal = (char*)calloc((buf_whdd),sizeof(unsigned char));
			unsigned char *metaCopy_props = (char*)calloc((buf_whdd),sizeof(unsigned char));

			memcpy(metaCopy_id,&gSrc[12],buf_whdd);
			memcpy(metaCopy_pal,&gSrc[12+buf_whdd],buf_whdd);
			memcpy(metaCopy_props,&gSrc[12+buf_whdd*2],buf_whdd);

			GlobalUnlock(gSrc);
			CloseClipboard();

			if(d==buf_d)
			{
				//if(nameCopyHeight<1||nameCopyWidth<1) return;
				if(align){
					meta=iListViewOff; //+ which set, set size..
					w=metasOnRow;
					h=metasOnRow;
				}
				else{
					GetSelection(setSelection,x,y,w,h);
					meta=(y/d)*metasOnRow + (x/d) + iSetViewOff + iListViewOff;
					w=w/d;
					h=h/d;
				}

				if(buf_w&&buf_h) SetUndo();

				cnt=0;

				for(i=0;i<buf_h;i++)
				{
					for(j=0;j<buf_w;j++)
					{
						size_t src=cnt*d*d;
						size_t dst=(meta + j + (i*metasOnRow))*d*d;
						if(dst+(d*d)<currentMaxSize) //protect from overflow.
						{
							//if(nameCopy[pp]!=nullTile||!ingorepastingnulltiles1->Checked)

								if(doTile)  memcpy(currentTable_id+dst,metaCopy_id+src,d*d);
								if(doPal)   memcpy(currentTable_pal+dst,metaCopy_pal+src,d*d);
								if(doProps) memcpy(currentTable_props+dst,metaCopy_props+src,d*d);

						}
						cnt++;
					}
				}
				//LabelStats->Caption=IntToStr(metaCopy_id[0])+","+IntToStr(currentTable_id[0]);

				if(buf_w&&buf_h){
				LabelStats->Caption="Pasted metatiles,\nw: "+IntToStr(buf_w)+", h: "+IntToStr(buf_h)+", format: "+IntToStr(buf_d)+"x"+IntToStr(buf_d);
				holdStats=true;
				}

			}
			else   //mismatch of dimensions. Ought to throw a dialog with conversion options in the future.
			{
				LabelStats->Caption="Warning: metatile type mismatch.\nCurrent table: "+IntToStr(d)+"x"+IntToStr(d)+", Metatiles in clipboard: "+IntToStr(buf_d)+"x"+IntToStr(buf_d)+"\nFuture versions may feature a converter dialog.";
			}

			FormMetatileTool->Draw();

			if(FormMetatileTool->btnMap->Down){
				if(FormNavigator->Visible) {
				   //	FormNavigator->DrawRange(x,bufY,nameCopyWidth,nameCopyHeight,false);
					FormNavigator->UpdateLines(false);
				}
				UpdateNameTable(-1,-1,true);
			}
		free(metaCopy_id); 
		free(metaCopy_pal);
		free(metaCopy_props);

		}
	}
}


void __fastcall TFormMain::FillMap(bool bUseNull)
{
	int i,j,x,y,w,h;
	int tx,ty,tw,th;
	int tile;
	GetSelection(nameSelection,x,y,w,h);
	GetSelection(chrSelection,tx,ty,tw,th);

	int bufY=y;

	if(w&&h) SetUndo();

	for(i=0;i<h;i++)
	{
		for(j=0;j<w;j++)
		{

			tile = tileActive+(j%tw)+((i%th)*16);

			tile = bUseNull ? nullTile:tile;
			if(SpeedButtonTiles->Down) nameTable[y*nameTableWidth+x+j]=tile;
			if(SpeedButtonPal->Down) AttrSet(x+j,y,palActive,false);
		}
		y++;
	}

	UpdateNameTable(-1,-1,true);
    if(FormNavigator->Visible)
		{
			FormNavigator->DrawRange(x,bufY,w,h,false);
			FormNavigator->UpdateLines(false);
		}

	//FormNavigator->Draw(false,false);
}



void __fastcall TFormMain::OpenAll(AnsiString name)
{
	if(OpenCHR(name+".chr",true))
	{
		SaveDialogChr->FileName=name;
	}
	else
	{
		if(OpenCHR(name+".bin",true)) SaveDialogChr->FileName=name;
	}

	if(OpenNameTable(name+".nam",true))
	{
		SaveDialogName->FileName=name;
	}
	else
	{
		if(OpenNameTable(name+".rle",true))
		{
			SaveDialogName->FileName=name;
		}
		else
		{
			if(OpenNameTable(name+".map",true))
			{
				SaveDialogMap->FileName=name;
			}
		}
	}
	
	if(OpenPalette(name+".pal",true))
	{
		SaveDialogPal->FileName=name;
	}
	
	if(OpenMetaSprites(name+".msb",true))
	{
		SaveDialogMetaSpriteBank->FileName=name;
	}
	
	if(OpenTprop(name+".tprop",true))
    {
		SaveDialogTprop->FileName=name;
	}
}



void __fastcall TFormMain::UpdateStats(void)
{
	if (holdStats==true) {cueStats=true; return;}

	bool bClone = ( bBufCtrl &&  !bBufShift && bBufAlt);
	//bool bSwap	= (!bBufCtrl && !bBufShift && !bBufAlt);     //commented out to suppress warning
	bool bMove	= ( bBufCtrl && !bBufShift && !bBufAlt);

	//AnsiString str;
	//AnsiString tmp;
	int i,j,vx,vy,cnt,tile,off,col,sel,frame;

	cueStats=false;
	throttleOK=false;
	FormCaption = Application->Title;
	//first do the window titlebar
	strStat=FormCaption;
	AppendStr(strStat,"  ");

	if(nameTableWidth==32&&nameTableHeight==30)
	{
		//str+="[std nametable]";
		TabSheetName->Caption="[F1] Screen";
		AppendStr(strStat,"[standard screen]");
	}
	else
	{
		TabSheetName->Caption="[F1] Map";
		vx=nameTableViewX;
		vy=nameTableViewY;

		if(vx<0) vx=0;
		if(vy<0) vy=0;

		AppendStr(strStat,"[map "+IntToStr(nameTableWidth)
			+"x"+IntToStr(nameTableHeight)+"]"
			+"["+IntToStr(vx)+","+IntToStr(vy)+"]");
	}

	frame=GetNameTableFrame();

	if(frame>=0) AppendStr(strStat," [frame "+IntToStr(frame)+"]");



	//Filename in titlebar
	AnsiString parentFolder = ExtractFileDir(SaveDialogSession->FileName);
	AnsiString fileNameOnly = ExtractFileName(SaveDialogSession->FileName);
	AnsiString result = parentFolder + "\\" + fileNameOnly;
	int maxlen=70;
	switch(iShowFilepath){
		case 1:
			AppendStr(strStat," "+fileNameOnly);
			break;
		case 2:
            tmp:
			if (!parentFolder.IsEmpty()) {
				AppendStr(strStat," "+ShortenFileName(result,maxlen,1));
			}
			else { // If parent folder empty, just append filename.
				AppendStr(strStat, " " + fileNameOnly);
			}
			break;
		case 3:
			AppendStr(strStat," "+ShortenFileName(SaveDialogSession->FileName,maxlen,1));
			break;
		default:
		goto tmp;
	}

	if(unsavedChanges==true) AppendStr(strStat,"*");

	Caption=strStat;

	//-----
	//now do the status bar
	//-----
	if (GetKeyState(VK_CAPITAL)&&(!SpeedButtonTypeIn->Down))
		{cueStats=true;return;}

	bAllowLockMessage=true;

	strStat="---";
	if(spriteHover<0)
	if(PageControlEditor->ActivePage==TabSheetSprite)
	if(ListBoxSpriteList->Items->Count)
	{
		//if (str=="---") str="";
		//else str+="\t\t\t";

		strStat=IntToStr(ListBoxSpriteList->Items->Count)+" object(s) in metasprite "
			+IntToStr(metaSpriteActive)+".";
		tmpStat=strStat+"\t";
	}
	else {tmpStat="Metasprite slot is vacant.\t"; strStat=tmpStat;}

	if(tileXC>=0&&tileYC>=0)
	{
		//if (SpeedButtonMarqTile->Down)
		//{
			tile=tileViewTable[tileYC*16+tileXC];
			int bank_addr=bankViewTable[tileYC*16+tileXC + bankActive/16] + tile*16;
			int bank=bank_addr/(16*256);
			int bank_off = (bank_addr & 0xfff)/16;
			cnt=0;
			int selcnt=0;
			for(i=0;i<nameTableWidth*nameTableHeight;i++) if(nameTable[i]==tile) ++cnt;

			sel=0;

			for(i=0;i<256;i++) if(chrSelected[tileViewTable[i]])
			{
				++sel;
				for(j=0;j<nameTableWidth*nameTableHeight;j++) if(nameTable[j]==tileViewTable[i]) ++selcnt;
			}

			strStat="";
			strStat+=tmpStat
				+"Tile: $"+IntToHex(tile,2)
				+"\tEntries: "+IntToStr(cnt)
				+"\tSelected: "+IntToStr(sel)
				+"\tEntries in selection: "+IntToStr(selcnt)
				+"\nBank: "+IntToStr(bank)
				+"\tBank addr: $"+IntToHex(bank_addr,4)
				+"\t\tBank offset: $"+IntToHex(bank_off,2);
		//}

		if (SpeedButtonMarqTile->Down) strStat+="\nSelect: click\tBox-select: shift-drag\t\tMulti-select: ctrl-click\t\tSwitch mode: shift+D";
		if (SpeedButtonDrawTile->Down) strStat+="\nPen: click\tEyedropper: right-click\tBucket: ctrl-click\nBox select: shift-drag\t\tSwitch mode: shift+D";



	}
	if(bImageNameAccepted||bImageTileAccepted)
	{
		strStat="Drag: ";
		if(bClone)		strStat+="CLONE.\tCopies selection to destination.\n\t\t\tModifiers: [Ctrl] Move content.  [Ctrl+Alt] Clone.";
		else if(bMove)  strStat+="MOVE.\t\tMoves selection to destination; leaving 'null tiles' behind.\n\t\t\tModifiers: [Ctrl] Move content.  [Ctrl+Alt] Clone";
		else   			strStat+="SWAP.\t\tSwaps selection with destination.\n\t\t\tModifiers: [Ctrl] Move content.  [Ctrl+Alt] Clone.\n\n\t\t\tIf Diagonal swaps overlap, the content swapped back reorders to fit vacant space.";

	}
	
	if(nameXC>=0&&nameXC<nameTableWidth&&nameYC>=0&&nameYC<nameTableHeight)
	{
			off=nameYC*nameTableWidth+nameXC;
			int isScreen = 0;
			if  (nameTableWidth*nameTableHeight == 32*30) isScreen = 32*30;
			strStat="XY: "+IntToStr(nameXC)+","+IntToStr(nameYC)
				+" \tOff: $"+IntToHex(off,4)
				+"\tName: $"+IntToHex(nameTable[off],2)
				+"\tAtXY: "+IntToStr(nameXC/2)+","+IntToStr(nameYC/2)
				+"\tAtOff: $"+IntToHex( nameYC/4*(nameTableWidth/4) + nameXC/4+isScreen,4)    //nameTableWidth*nameTableHeight
				+"."+IntToStr((nameXC&2)+(nameYC&2)*2)
				+"="+IntToStr(AttrGet(nameXC,nameYC,false,false))
				+"\t($"+IntToHex(attrTable[nameYC/4*(nameTableWidth/4)+nameXC/4],2)+")"
				+"\tMetaSpr:"+IntToStr(metaSpriteActive);
			if(nameSelection.left>=0)
				strStat+="\tWH:"+IntToStr(nameSelection.right-nameSelection.left)
				+","+IntToStr(nameSelection.bottom-nameSelection.top);
			if (SpeedButtonMarqTile->Down&&nameSelection.left>=0) //&&!bOutsideSel
				strStat+="\nMove selection: [Shift+Rightdrag]\t\tSwap: [Right-drag]\t\t\tFill selection: [F]\nMove content:  [Ctrl+Rightdrag]\t\tClone: [Ctrl+Alt+Rightdrag]";

			else if (SpeedButtonMarqTile->Down) strStat+="\nPlace: click\tFind tile: right-click \tSelect: shift-drag\tQuickpaste: V+click\tSwitch mode: shift+D";
			if (SpeedButtonDrawTile->Down) strStat+="\nPen: click\tEyedropper: right-click\tBucket: ctrl-click\tSelect: shift-drag\tSwitch mode: shift+D";

			//if(nameSelection.left>=0) str+="\nTip:\t[F] to fill selection.";

	}

	if(palHover>=0)
	{
		strStat="Pal:"+IntToStr(palHover)+"\tEntry:"
			+IntToStr(palColHover)
			+"\tAdr:"+IntToHex(0x3f00+palHover*4+palColHover,4)
			+"\tColour:$"+IntToHex(bgPal[palBank*16+palHover*4+palColHover],2)
			+"\nTip:\tC-click / V-click a subpalette to copy/paste. Shift-C/Shift-V to copy/paste the whole set."
			+"\n\tV-click repeatedly to cycle pastes from a clipboarded palette set.";
			//+ " v key: " + BoolToString(clickV);
	}

	if(colHover>=0)
	{
		col=colHover;

		//if(col==0x0d||col==0x1d) col=0x0f;

		strStat="Colour: $"+IntToHex(col,2);
		if(safeColsOnly)
		{
			switch(col)    //fg todo: make this optional
			{
				case 0x0d:
				case 0x1d:
				case 0x0e:
				case 0x1e:
				case 0x2e:
				case 0x3e:
				case 0x1f:
				case 0x2f:
				case 0x3f:
				strStat+=" -> $0F\t\t\t\t\t";
				break;
				default: strStat+="\t\t\t\t\t\t";
			}
		}
		strStat+="Click: Pick.\tAlt-click: Pick backdrop.\nCtrl-click: Ripple-pick subpalette.\t\t\tCtrl-rightclick: Ripple-skip through subpalette.\nCtrl+Shift-click: Ripple-pick whole set.\t\tCtrl+Shift-rightclick: Ripple-skip through whole set.\nShift-click: Pick and step to next subpal.\t\tShift-rightclick: Skip to next subpal.";
		}
	if(spriteHover>=0)
	{
		off=metaSpriteActive*64*4+spriteHover*4;

		strStat="Sprite:"+IntToStr(spriteHover)
			+"\tX:"+IntToStr(metaSprites[off+3]-spriteGridX)
			+"\tY:"+IntToStr(metaSprites[off]-spriteGridY)
			+"\tTile:$"+IntToHex(metaSprites[off+1],2)
			+"\tPal:"+IntToStr(metaSprites[off+2]&3);

		if(metaSprites[off+2]&OAM_FLIP_H) strStat+=" HFlip";
		if(metaSprites[off+2]&OAM_FLIP_V) strStat+=" VFlip";

	}

	if(CHREditorHover>=0)
	{
		strStat="Pen: click\tBucket: ctrl-click\tEyedropper: Right-click\nIncrement mode ink: shift-click\tDecrement mode ink: shift-alt-click\nSwap bucket: ctrl+alt+right-click\tNoncontigous bucket: ctrl+alt-click";
	}
	if(bigTilesHover>=0&&PageControlEditor->ActivePage==TabSheetTile)
	{
		if (SpeedButtonMarqTile->Down) strStat="[Layout mode]\tSwitch mode: Shift+D\nSelect: click\t\tBox-select: shift-drag";
		if (SpeedButtonDrawTile->Down) strStat="[Draw mode]\t\tSwitch mode: Shift+D\nPen: click\t\tBucket: ctrl-click\nSample: right-click\tNC-Bucket: ctrl+alt+click\nBox select: shift-drag";

		if(tileXC>=0&&tileYC>=0&&SpeedButtonMarqTile->Down)
		{
			tile=tileYC*16+tileXC;
			cnt=0;
			int selcnt=0;
			for(i=0;i<nameTableWidth*nameTableHeight;i++) if(nameTable[i]==tile) ++cnt;

			sel=0;

			for(i=0;i<256;i++) if(chrSelected[i])
			{
				++sel;
				for(j=0;j<nameTableWidth*nameTableHeight;j++) if(nameTable[j]==i) ++selcnt;
			}
			strStat+="\nTile: $"+IntToHex(tile,2)
				+"\t\tEntries: "+IntToStr(cnt)
				+"\nSelected: "+IntToStr(sel)
				+"\t\tEntries in selection: "+IntToStr(selcnt);
		}
	}
	LabelStats->Caption=strStat;

	strStat="";
	tmpStat="";
}



void __fastcall TFormMain::SetUndo(void)
{
	//BROKE STUDIO
	int i;
	for(i=0;i<256;i++) metaSpriteNamesUndo[i]=metaSpriteNames[i];
	//
	for(i=0;i<256;i++) undo_collisionIDlabel[i]=collisionIDlabel[i];

	//FG: these were added because of the new toggle behaviour of sort/clear actions

		MCHRRemoveDoubles->Checked=false;
		MCHRRemoveUnused->Checked=false;

		MCHRRemoveDoubles->Caption="&Remove duplicates";
		MCHRRemoveUnused->Caption="Re&move unused";

		MCHRFreqSort->Checked=false;
		MCHRDensitySort->Checked=false;

		MCHRFreqSort->Caption="Sort by &frequency";
		MCHRDensitySort->Caption="Sort by d&ensity";

		MCHRFindUnused->Checked=false;
		MCHRFindDoubles->Checked=false;

		MCHRFindUnused->Caption="Find &unused";
		MCHRFindDoubles->Caption="Find &duplicates";

	unsavedChanges=true;
	Savesession1->Enabled=true;
	UpdateStats();
	//


	undoNameTableWidth	=	nameTableWidth;
	undoNameTableHeight =	nameTableHeight;

	undoSpriteGridX		=	spriteGridX;
	undoSpriteGridY     =   spriteGridY;

	memcpy(bgPalUndo,bgPal,sizeof(bgPal));

	//memcpy(undoChr,chr,8192);

	memcpy(undoNameTable,nameTable,name_size());
	memcpy(undoAttrTable,attrTable,attr_size());
	memcpy(undoMetaSprites,metaSprites,sizeof(undoMetaSprites));

	memcpy(propConditionalUndo,propConditional,sizeof(propConditionalUndo));
	memcpy(propConditionUndo,propCondition,sizeof(propConditionUndo));

	//chr bank pointers
	memcpy(undo_bankViewTable,bankViewTable,sizeof(undo_bankViewTable));

	//chr bank stuff

	size_t tmpSize = chrBanks*4096;
	size_t tmpSize_undo = undo_chrBanks*4096;
	if (tmpSize > tmpSize_undo) {
		undoChr = (char*) realloc(undoChr, tmpSize);
		tilePropertiesUndo = (char*) realloc(tilePropertiesUndo, tmpSize/16);
	}

	memcpy(undoChr, chr, tmpSize);
    memcpy(tilePropertiesUndo,tileProperties,tmpSize/16);


	tmpSize = chrBanks_LinkBuf*4096;
	tmpSize_undo = undo_chrBanks_LinkBuf*4096;
	if (tmpSize > tmpSize_undo) {
		undoChr_LinkBuf = (char*) realloc(undoChr_LinkBuf, tmpSize);
		tilePropertiesUndo_LinkBuf  = (char*)realloc(tilePropertiesUndo_LinkBuf, tmpSize/16);
	}
	memcpy(undoChr_LinkBuf,chr_LinkBuf,tmpSize);
	memcpy(tilePropertiesUndo_LinkBuf,tileProperties_LinkBuf,tmpSize/16);

	undo_chrBanks = chrBanks;
	undo_chrBankLabels = chrBankLabels;

	//metatiles

	 tmpSize		 = metatileSets_2x2*META_2x2;
	 tmpSize_undo = undo_metatileSets_2x2*META_2x2;

	if(tmpSize > tmpSize_undo) {
		undo_metatileSet_2x2_id = (char*) realloc(undo_metatileSet_2x2_id, tmpSize);
		undo_metatileSet_2x2_pal = (char*) realloc(undo_metatileSet_2x2_pal, tmpSize);
		undo_metatileSet_2x2_props = (char*) realloc(undo_metatileSet_2x2_props, tmpSize);


	}
	memcpy(undo_metatileSet_2x2_id, metatileSet_2x2_id, tmpSize);
	memcpy(undo_metatileSet_2x2_pal, metatileSet_2x2_pal, tmpSize);
	memcpy(undo_metatileSet_2x2_props, metatileSet_2x2_props, tmpSize);


	undo_metatileSets_2x2 = metatileSets_2x2;

	tmpSize		 = metatileSets_4x4*META_4x4;
	tmpSize_undo = undo_metatileSets_4x4*META_4x4;

	if(tmpSize > tmpSize_undo) {
		undo_metatileSet_4x4_id = (char*) realloc(undo_metatileSet_4x4_id, tmpSize);
		undo_metatileSet_4x4_pal = (char*) realloc(undo_metatileSet_4x4_pal, tmpSize);
		undo_metatileSet_4x4_props = (char*) realloc(undo_metatileSet_4x4_props, tmpSize);
	}

	memcpy(undo_metatileSet_4x4_id, metatileSet_4x4_id, tmpSize);
	memcpy(undo_metatileSet_4x4_pal, metatileSet_4x4_pal, tmpSize);
	memcpy(undo_metatileSet_4x4_props, metatileSet_4x4_props, tmpSize);

	undo_metatileSets_4x4 = metatileSets_4x4;



	tmpSize		 = metatileSets_8x8*META_8x8;
	tmpSize_undo = undo_metatileSets_8x8*META_8x8;

	if(tmpSize > tmpSize_undo) {
		undo_metatileSet_8x8_id = (char*) realloc(undo_metatileSet_8x8_id, tmpSize);
		undo_metatileSet_8x8_pal = (char*) realloc(undo_metatileSet_8x8_pal, tmpSize);
		undo_metatileSet_8x8_props = (char*) realloc(undo_metatileSet_8x8_props, tmpSize);
	}

	memcpy(undo_metatileSet_8x8_id, metatileSet_8x8_id, tmpSize);
	memcpy(undo_metatileSet_8x8_pal, metatileSet_8x8_pal, tmpSize);
	memcpy(undo_metatileSet_8x8_props, metatileSet_8x8_props, tmpSize);


	undo_metatileSets_8x8 = metatileSets_8x8;




	//brush masks

	for(int i=0;i<14;i++){
		memcpy(ptr_undo_tableBrush[i],ptr_tableBrush[i],256*sizeof(int));
	}


}



void __fastcall TFormMain::Undo(void)
{
	//BROKE STUDIO
	int i,tmp;
	AnsiString tmpAnsi;

	for(i=0;i<256;i++)
	{
		tmpAnsi=metaSpriteNames[i];
		metaSpriteNames[i]=metaSpriteNamesUndo[i];
		metaSpriteNamesUndo[i]=tmpAnsi;
	}

	for(i=0;i<256;i++)
	{
		tmpAnsi=collisionIDlabel[i];
		collisionIDlabel[i]=undo_collisionIDlabel[i];
		undo_collisionIDlabel[i]=tmpAnsi;
	}

	int tmpBrushTable[16][16];
	for(int i=0;i<14;i++){
		memcpy(tmpBrushTable,ptr_tableBrush[i],256*sizeof(int));
		memcpy(ptr_tableBrush[i],ptr_undo_tableBrush[i],256*sizeof(int));
		memcpy(ptr_undo_tableBrush[i],tmpBrushTable,256*sizeof(int));
	}
	
	//figure out which is larger to copy the right size.
	int tmp_AttrSize = (nameTableWidth+3)/4*((nameTableHeight+3)/4);
	int tmp_UndoAttrSize = (undoNameTableWidth+3)/4*((undoNameTableHeight+3)/4);
	int tmp_MaxAttr = tmp_AttrSize > tmp_UndoAttrSize
					? tmp_AttrSize : tmp_UndoAttrSize;

	int tmp_NameSize = nameTableWidth*nameTableHeight;
	int tmp_UndoNameSize = undoNameTableWidth*undoNameTableHeight;
	int tmp_MaxName = tmp_NameSize > tmp_UndoNameSize
					? tmp_NameSize : tmp_UndoNameSize;

	mem_exchange(bgPalUndo,bgPal,sizeof(bgPal));



	mem_exchange(undoNameTable,nameTable,tmp_MaxName);     //name_size()
	mem_exchange(undoAttrTable,attrTable,tmp_MaxAttr);     //attr_size()

	mem_exchange(undoMetaSprites,metaSprites,sizeof(undoMetaSprites));

	mem_32exchange(undo_bankViewTable,bankViewTable,sizeof(undo_bankViewTable));

	mem_exchange(propConditionalUndo,propConditional,sizeof(propConditionalUndo));
	swap_bool_2d(propConditionUndo, propCondition, 4, 8);



	tmp=nameTableWidth;		nameTableWidth=undoNameTableWidth;	undoNameTableWidth=tmp;
	tmp=nameTableHeight;	nameTableHeight=undoNameTableHeight;	undoNameTableHeight=tmp;

	tmp=spriteGridX;        spriteGridX=undoSpriteGridX;	undoSpriteGridX=tmp;
	tmp=spriteGridY;        spriteGridY=undoSpriteGridY;    undoSpriteGridY=tmp;

	//chr bank stuff

	tmp_chrBankLabels	= chrBankLabels;
	chrBankLabels		= undo_chrBankLabels;
	undo_chrBankLabels	= tmp_chrBankLabels;

	size_t tmpSize = max(chrBanks*4096,undo_chrBanks*4096);

	if (tmpSize > chrBanks*4096){
		chr = (char*)realloc(chr,tmpSize);
		tileProperties = (char*)realloc(tileProperties,tmpSize/16);
	}
	if (tmpSize > undo_chrBanks*4096){
		undoChr = (char*)realloc(undoChr,tmpSize);
		tilePropertiesUndo = (char*)realloc(tilePropertiesUndo,tmpSize/16);
	}
	mem_exchange(undoChr,	chr	,tmpSize);
	mem_exchange(tilePropertiesUndo,tileProperties,tmpSize/16);


	tmpSize = max(chrBanks_LinkBuf*4096,undo_chrBanks_LinkBuf*4096);

	if (tmpSize > chrBanks_LinkBuf*4096)
		chr_LinkBuf = (char*)realloc(chr_LinkBuf,tmpSize);
	if (tmpSize > undo_chrBanks_LinkBuf*4096)
		undoChr_LinkBuf = (char*)realloc(undoChr_LinkBuf,tmpSize);


	mem_exchange(undoChr_LinkBuf,chr_LinkBuf,tmpSize);
	mem_exchange(tilePropertiesUndo_LinkBuf,tileProperties_LinkBuf,tmpSize/16);


	tmp=chrBanks;			chrBanks=undo_chrBanks;			undo_chrBanks=tmp;
	tmp=chrBanks_LinkBuf;	chrBanks=undo_chrBanks_LinkBuf;	undo_chrBanks_LinkBuf=tmp;

	//metatiles
	tmpSize = max(metatileSets_2x2*META_2x2,undo_metatileSets_2x2*META_2x2);
	if (tmpSize > (unsigned)metatileSets_2x2*META_2x2){
		metatileSet_2x2_id = (char*) realloc(metatileSet_2x2_id, tmpSize);
		metatileSet_2x2_pal = (char*) realloc(metatileSet_2x2_pal, tmpSize);
		metatileSet_2x2_props = (char*) realloc(metatileSet_2x2_props, tmpSize);
	}
	if (tmpSize > (unsigned)undo_metatileSets_2x2*META_2x2){
		undo_metatileSet_2x2_id = (char*) realloc(undo_metatileSet_2x2_id, tmpSize);
		undo_metatileSet_2x2_pal = (char*) realloc(undo_metatileSet_2x2_pal, tmpSize);
		undo_metatileSet_2x2_props = (char*) realloc(undo_metatileSet_2x2_props, tmpSize);
	}
	mem_exchange(undo_metatileSet_2x2_id,	metatileSet_2x2_id	 ,tmpSize);
	mem_exchange(undo_metatileSet_2x2_pal,	metatileSet_2x2_pal	 ,tmpSize);
	mem_exchange(undo_metatileSet_2x2_props,metatileSet_2x2_props,tmpSize);

	tmpSize = max(metatileSets_4x4*META_4x4,undo_metatileSets_4x4*META_4x4);
	if (tmpSize > (unsigned)metatileSets_4x4*META_4x4){
		metatileSet_4x4_id = (char*) realloc(metatileSet_4x4_id, tmpSize);
		metatileSet_4x4_pal = (char*) realloc(metatileSet_4x4_pal, tmpSize);
		metatileSet_4x4_props = (char*) realloc(metatileSet_4x4_props, tmpSize);
	}
	if (tmpSize > (unsigned)undo_metatileSets_4x4*META_4x4){
		undo_metatileSet_4x4_id = (char*) realloc(undo_metatileSet_4x4_id, tmpSize);
		undo_metatileSet_4x4_pal = (char*) realloc(undo_metatileSet_4x4_pal, tmpSize);
		undo_metatileSet_4x4_props = (char*) realloc(undo_metatileSet_4x4_props, tmpSize);
	}
	mem_exchange(undo_metatileSet_4x4_id,	metatileSet_4x4_id	 ,tmpSize);
	mem_exchange(undo_metatileSet_4x4_pal,	metatileSet_4x4_pal	 ,tmpSize);
	mem_exchange(undo_metatileSet_4x4_props,metatileSet_4x4_props,tmpSize);

	tmpSize = max(metatileSets_8x8*META_8x8,undo_metatileSets_8x8*META_8x8);
	if (tmpSize > (unsigned)metatileSets_8x8*META_8x8){
		metatileSet_8x8_id = (char*) realloc(metatileSet_8x8_id, tmpSize);
		metatileSet_8x8_pal = (char*) realloc(metatileSet_8x8_pal, tmpSize);
		metatileSet_8x8_props = (char*) realloc(metatileSet_8x8_props, tmpSize);
	}
	if (tmpSize > (unsigned)undo_metatileSets_8x8*META_8x8){
		undo_metatileSet_8x8_id = (char*) realloc(undo_metatileSet_8x8_id, tmpSize);
		undo_metatileSet_8x8_pal = (char*) realloc(undo_metatileSet_8x8_pal, tmpSize);
		undo_metatileSet_8x8_props = (char*) realloc(undo_metatileSet_8x8_props, tmpSize);
	}
	mem_exchange(undo_metatileSet_8x8_id,	metatileSet_8x8_id	 ,tmpSize);
	mem_exchange(undo_metatileSet_8x8_pal,	metatileSet_8x8_pal	 ,tmpSize);
	mem_exchange(undo_metatileSet_8x8_props,metatileSet_8x8_props,tmpSize);

	//sanitize line drawing buffer
	memset (arr_linePlotCHR,0,sizeof(arr_linePlotCHR2));
	memset (arr_linePlotCHR2,0,sizeof(arr_linePlotCHR2));
	//memcpy (chrBuf, chr, 8192);      //fix
	for(int set=0;set<4;set++){
		for(int tile=0;tile<256;tile++){
			memcpy(chrBuf+set*256+tile*16,chr+tileViewTable[tile]*16 + bankViewTable[set*256+tile],16);
		}
	}
	lineUpX= -1;
	lineUpY= -1;

	UpdateAll();
}


void __fastcall TFormMain::SetTile(int tile)
{
	int i;
	bool b=FormCHREditor->btn2x2mode->Down;
	int m=b?2:1;
	int n=m;
	if(b)  // done at right or wrong level? This turns some of the latter steps redundancies but keeping them will allow for an easy reversal if i change my mind.
	{
		if(tile%16==15) tile--;
		if(tile/16==15) tile-=16;
	}
	tileActive=tile;
	chrSelection.left=tile&15;
	chrSelection.top=tile/16;

	//these only have meaningful effect if b is true since they serve to
	//revert its effect in corner cases. If this is not desireable in the future
	//they can be wrapped in the b condition, similar to what is done below for
	//tileSelRect.

	if(!((tileActive+1)&0x0F))    m=1;
	if(tileActive>=0xF0)		  n=1;

	chrSelection.right=chrSelection.left+m;
	chrSelection.bottom=chrSelection.top+n;

	for(i=0;i<256;++i) chrSelected[i]=0;


	if(b){
		//these are used by "selected only"
		chrSelected[tile]=1;
		if(m>1)chrSelected[tile+1]=1;
		if(n>1)chrSelected[tile+16]=1;
		if(m>1&&n>1)chrSelected[tile+17]=1;
		//chr editor draw tools operate on these.
		tileSelRectWdt=(tileActive&15)<0x0F?2:1;
		tileSelRectHgt=tileActive<0xF0?2:1;
	}
	else
	{
		chrSelected[tile]=1;
		tileSelRectWdt=1;
		tileSelRectHgt=1;

	}
	chrSelectRect=true;
	cueUpdateTiles=true;
	cueUpdateNametable=true;
	//UpdateTiles(true);
}



void __fastcall TFormMain::SetBMPPalette(Graphics::TBitmap* bmp)
{
	PALETTEENTRY pal[16];
	int i,col;

	for(i=0;i<16;i++)
	{
		col=outPalette[bgPal[palBank*16+(i>>2)*4+(i&3)]];

		pal[i].peBlue =(col>>16)&0xff;
		pal[i].peGreen=(col>>8)&0xff;
		pal[i].peRed  =(col&0xff);
	}

	SetPaletteEntries(bmp->Palette,0,16,pal);
}





int fget_i32(FILE *file)
{
	unsigned char buf[4];

	fread(buf,4,1,file);

	return buf[0]|(buf[1]<<8)|(buf[2]<<16)|(buf[3]<<24);
}



bool fget_bool(FILE *file)
{
	return fgetc(file)?true:false;
}



AnsiString fget_ansistring(FILE *file)
{
	AnsiString str;
	char *temp;
	int len;

	len=fget_i32(file);

	temp=(unsigned char*)malloc(len+1);
	fread(temp,len,1,file);
	temp[len]=0;
	str=temp;
	free(temp);

	return str;
}



void nss_put_bytes(FILE *file,const char *name,unsigned char *data,int size)
{
	int i,byte,cnt;

	fprintf(file,"%s",name);

	byte=-1;
	cnt=0;

	for(i=0;i<size;++i)
	{
		if(byte!=data[i])
		{
			if(cnt>1) fprintf(file,"[%x]",cnt);

			byte=data[i];

			fprintf(file,"%2.2x",byte);

			cnt=1;
		}
		else
		{
			++cnt;
		}
	}

	if(cnt>1) fprintf(file,"[%x]",cnt);

	fprintf(file,"\n");
}
char* str_put_bytes(unsigned char *data,int size, char* buffer)
{
	int i,byte,cnt;
	 //large enough
    buffer[0]='\0'; 
	//sprintf(str,"%s",name);

	byte=-1;
	cnt=0;

	for(i=0;i<size;++i)
	{
		if(byte!=data[i])
		{
			if(cnt>1) sprintf(buffer + strlen(buffer),"[%x]",cnt);

			byte=data[i];

			sprintf(buffer + strlen(buffer),"%2.2x",byte);

			cnt=1;
		}
		else
		{
			++cnt;
		}
	}

	if(cnt>1) sprintf(buffer + strlen(buffer),"[%x]",cnt);

	sprintf(buffer + strlen(buffer),"\n");
	return buffer;
}

void nss_put_bytes_from_int(FILE *file,const char *name,int *data,int size)
{
	int i,byte,cnt;

	fprintf(file,"%s",name);

	byte=-1;
	cnt=0;

	for(i=0;i<size;++i)
	{
		if(byte!=(unsigned char)data[i])
		{
			if(cnt>1) fprintf(file,"[%x]",cnt);

			byte=(unsigned char)data[i];

			fprintf(file,"%2.2x",byte);

			cnt=1;
		}
		else
		{
			++cnt;
		}
	}

	if(cnt>1) fprintf(file,"[%x]",cnt);

	fprintf(file,"\n");
}



void nss_put_bools(FILE *file, const char *name, bool *data, int size) {
	int i, cnt;
	bool bit;

	fprintf(file, "%s", name);

	bit = false;
	cnt = 0;

	for (i = 0; i < size; ++i) {
		if (bit != data[i]) {
			if (cnt > 1) fprintf(file, "[%x]", cnt);

			bit = data[i];

			fprintf(file, "%1.1x", bit ? 1 : 0);

			cnt = 1;
		} else {
			++cnt;
		}
	}

	if (cnt > 1) fprintf(file, "[%x]", cnt);

	fprintf(file, "\n");
}
void nss_put_2D_bools(FILE *file, const char *name, bool data[][8], int rows, int cols) {
	int i, j, cnt;
	bool bit;

	fprintf(file, "%s", name);

	bit = false;
	cnt = 0;

	for (i = 0; i < cols; ++i) {
		for (j = 0; j < rows; ++j) {
			if (bit != data[j][i]) {
				if (cnt > 1) fprintf(file, "[%x]", cnt);

                bit = data[j][i];

				fprintf(file, "%1.1x", bit ? 1 : 0);

                cnt = 1;
            } else {
                ++cnt;
            }
        }
    }

    if (cnt > 1) fprintf(file, "[%x]", cnt);

    fprintf(file, "\n");
}


void nss_put_uint32s(FILE *file, const char *name, uint32_t *data, int size) {
	int i, cnt;
	uint32_t element;
	fprintf(file, "%s", name);
	size=size/4;
	element = 0xFFFFFFFF;
	cnt = 0;

	for(i=0;i<size;++i)
	{
		if(element!=data[i])
		{
			if(cnt>1) fprintf(file,"[%x]",cnt);

			element=data[i];

			fprintf(file,"%8.8x",element);

			cnt=1;
		}
		else
		{
			++cnt;
		}
	}

	if (cnt > 1) fprintf(file, "[%x]", cnt);

	fprintf(file, "\n");
}

bool nss_get_bytes(char* data,int size,const char* tag,unsigned char *dst,int dst_size)
{
	char c1,c2;
	int ptr,len,prev;

	len=strlen(tag);
	ptr=0;

	while (size - len > 0) //while(size)
	{
		if(!memcmp(data,tag,len))
		{
			data+=len;
			prev=0;

			while(ptr<dst_size)
			{
				c1=*data++;

				if(c1=='[')	//repeat tag
				{
					len=0;

					while(1)
					{
						c1=*data++;

						if(c1==']') break;

						if(c1>='0'&&c1<='9') c1=c1-'0'; else if(c1>='a'&&c1<='f') c1=c1-'a'+10; else return false;

						len=(len<<4)|c1;
					}

					while(len>1)
					{
						dst[ptr++]=prev;

						--len;
					}

					continue;
				}

				c2=*data++;

				if(c1>='0'&&c1<='9') c1=c1-'0'; else if(c1>='a'&&c1<='f') c1=c1-'a'+10; else return false;
				if(c2>='0'&&c2<='9') c2=c2-'0'; else if(c2>='a'&&c2<='f') c2=c2-'a'+10; else return false;

				prev=(c1<<4)+c2;

				dst[ptr++]=prev;
			}

			return true;
		}

		++data;
		--size;
	}

	return false;
}

bool nss_get_int_from_bytes(char* data,int size,const char* tag,int *dst,int dst_size)
{
	char c1,c2;
	int ptr,len,prev;

	len=strlen(tag);
	ptr=0;

	while (size - len > 0) //while(size)
	{
		if(!memcmp(data,tag,len))
		{
			data+=len;
			prev=0;

			while(ptr<dst_size)
			{
				c1=*data++;

				if(c1=='[')	//repeat tag
				{
					len=0;

					while(1)
					{
						c1=*data++;

						if(c1==']') break;

						if(c1>='0'&&c1<='9') c1=c1-'0'; else if(c1>='a'&&c1<='f') c1=c1-'a'+10; else return false;

						len=(len<<4)|c1;
					}

					while(len>1)
					{
						dst[ptr++]=prev;

						--len;
					}

					continue;
				}

				c2=*data++;

				if(c1>='0'&&c1<='9') c1=c1-'0'; else if(c1>='a'&&c1<='f') c1=c1-'a'+10; else return false;
				if(c2>='0'&&c2<='9') c2=c2-'0'; else if(c2>='a'&&c2<='f') c2=c2-'a'+10; else return false;

				prev=(c1<<4)+c2;

				dst[ptr++]=prev;
			}

			return true;
		}

		++data;
		--size;
	}

	return false;
}



void nss_get_2D_bools(const char* data, int size, const char* tag, bool dst[][8], int rows, int cols)
{
	int ptr, len;
	bool bit;

	len = strlen(tag);
	ptr = 0;

	while (size - len > 0) {
		if (!memcmp(data, tag, len)) {
			data += len;
			bit = false;
			ptr = 0;

			while (ptr < rows * cols) {
				char c = *data++;

                if (c == '[') {
                    len = 0;

                    while (1) {
                        c = *data++;

                        if (c == ']') break;

                        if (c >= '0' && c <= '9') {
                            len = len * 16 + (c - '0');
                        } else if (c >= 'a' && c <= 'f') {
                            len = len * 16 + (c - 'a' + 10);
                        } else {
                            return;
                        }
                    }

                    for (int i = 0; i < len - 1; i++) {
                        int col = ptr / rows;
                        int row = ptr % rows;
                        dst[row][col] = bit;
                        ptr++;
                    }
                } else {
                    bit = (c == '1');
                    int col = ptr / rows;
                    int row = ptr % rows;
                    dst[row][col] = bit;
                    ptr++;
                }
            }

            return;
        }

        data++;
        size--;
    }
}
bool nss_get_bools(char* data, int size, const char* tag, bool* dst, int dst_size) {
	int tag_len = strlen(tag);
	int ptr= 0;

	while (size >= tag_len) {
		if (!memcmp(data, tag, tag_len)) {
			data += tag_len;

			bool bit = false;
			int cnt;// = 0;

			while (ptr < dst_size) {
				char c = *data++;

                if (c == '[') {
					int repeat_cnt = 0;

                    while (1) {
						c = *data++;

                        if (c == ']') {
                            break;
						}

                        if (c >= '0' && c <= '9') {
							repeat_cnt = (repeat_cnt << 4) | (c - '0');
						} else if (c >= 'a' && c <= 'f') {
                            repeat_cnt = (repeat_cnt << 4) | (c - 'a' + 10);
                        } else {
                            return false;
                        }
                    }

                    while (repeat_cnt-- > 1) {
                        dst[ptr++] = bit;
                    }

                    continue;
                }

                if (c == '0') {
                    bit = false;
                } else if (c == '1') {
                    bit = true;
                } else {
                    return false;
                }

                dst[ptr++] = bit;
            }

            return true;
		}

        ++data;
        --size;
    }

    return false;
}

std::vector<std::string> deserializeVector(const char* text, const char* prefix) {
    std::vector<std::string> result;
    const size_t prefixLen = strlen(prefix);
    const char* start = strstr(text, prefix);
	if (start != NULL) {
        start += prefixLen;
        const char* end = start;
		while (*end != '\0' && *end != '\n') {
            if (*end == ',') {
                result.push_back(std::string(start, end - start));
                start = end + 1;
            } else if (*end == '=') {
                break;
			}
            ++end;
        }
        // Add the last element (if any) to the vector.
        if (start != end) {
            result.push_back(std::string(start, end - start));
        } else {
            result.push_back("(missing)");
        }
    } else {
        result.push_back("(missing)");
    }
    return result;
}


int hexstr_to_int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return 10 + c - 'a';
    } else if (c >= 'A' && c <= 'F') {
        return 10 + c - 'A';
    } else {
		return -1; // invalid hexadecimal digit
	}
}

bool nss_get_uint32s(const char* data, int size, const char* tag, uint32_t* dst, int dst_size) {
	int len = strlen(tag);
	int i = 0, ptr = 0;
	uint32_t prev, byte = 0xFFFFFFFF;
	bClearUint32Array=false;
	bool notag=false;
	while (i < size) {
		if (!memcmp(data + i, tag, len))
		{
			i += len;
			ptr = 0;
			prev = 0xFFFFFFFF;

			while (ptr < dst_size) {
				char c1 = data[i++];

				if (c1 == '[') {  // repeat tag
					int repeat_count = 0;

					while (true) {
						char c2 = data[i++];
						if (c2 == ']') break;

						if (c2 >= '0' && c2 <= '9') {
							repeat_count = (repeat_count << 4) | (c2 - '0');
						} else if (c2 >= 'a' && c2 <= 'f') {
							repeat_count = (repeat_count << 4) | (c2 - 'a' + 10);
						} else {


							return false;

						}
					}

					if (byte == 0xFFFFFFFF) {
						// missing first byte

						return false;
					}

					for (int j = 0; j < repeat_count-1; j++) {
						dst[ptr++] = byte;
					}

					byte = 0xFFFFFFFF;

					continue;
				}

				char c2 = data[i++];
				char c3 = data[i++];	char c4 = data[i++];
				char c5 = data[i++];	char c6 = data[i++];
				char c7 = data[i++];	char c8 = data[i++];

				if (c1 >= '0' && c1 <= '9') {c1 -= '0';}
				else if (c1 >= 'a' && c1 <= 'f') {c1 = c1 - 'a' + 10;
				} else {return false;}

				if (c2 >= '0' && c2 <= '9') {c2 -= '0';}
				else if (c2 >= 'a' && c2 <= 'f') {c2 = c2 - 'a' + 10;}
				else {return false;}

				if (c3 >= '0' && c3 <= '9') {c3 -= '0';}
				else if (c3 >= 'a' && c3 <= 'f') {c3 = c3 - 'a' + 10;
				} else {return false;}

				if (c4 >= '0' && c4 <= '9') {c4 -= '0';}
				else if (c4 >= 'a' && c4 <= 'f') {c4 = c4 - 'a' + 10;}
				else {return false;}

				if (c5 >= '0' && c5 <= '9') {c5 -= '0';}
				else if (c5 >= 'a' && c5 <= 'f') {c5 = c5 - 'a' + 10;
				} else {return false;}

				if (c6 >= '0' && c6 <= '9') {c6 -= '0';}
				else if (c6 >= 'a' && c6 <= 'f') {c6 = c6 - 'a' + 10;}
				else {return false;}

				if (c7 >= '0' && c7 <= '9') {c7 -= '0';}
				else if (c7 >= 'a' && c7 <= 'f') {c7 = c7 - 'a' + 10;
				} else {return false;}

				if (c8 >= '0' && c8 <= '9') {c8 -= '0';}
				else if (c8 >= 'a' && c8 <= 'f') {c8 = c8 - 'a' + 10;}
				else {return false;}

				int tmp_el = (c1 << 4) | c2;
				tmp_el = (tmp_el << 4) | c4;
				tmp_el = (tmp_el << 4) | c5;
				tmp_el = (tmp_el << 4) | c6;
				tmp_el = (tmp_el << 4) | c7;
				byte = (tmp_el << 4) | c8;

				//byte = (c1 << 4) | c2;


				if ((byte != prev) || prev == 0xFFFFFFFF) {
					if (ptr == dst_size) {
						// not enough space in dst

						return false;
					}
					dst[ptr++] = byte;

					prev = byte;

				}
			}

			return true;
		}
		else
		{
			notag=true;
		}
		i++;
	}
	if(notag){
			  bClearUint32Array=true;
	}//Application->MessageBox("Opening old (pre nexxt 1.0) session.","Warning",MB_OK);
	return false;
}

char* nss_get_str(char* data,int size,const char* tag)
{
	static char str[16384];
	char c;
	int ptr,len;

	len=strlen(tag);
	ptr=0;

	while(size)
	{
		if(!memcmp(data,tag,len))
		{
			data+=len;

			while(size)
			{
				c=*data++;
				if(c<32) break;

				str[ptr++]=c;
			}

			str[ptr]=0;

			return str;
		}

		++data;
		--size;
	}

	return NULL;
}



int nss_get_int(char* data,int size,const char* tag)
{
	char c;
	int len,n,sign;

	sign=1;

	len=strlen(tag);

	while(size)
	{
		if(!memcmp(data,tag,len))
		{
			data+=len;
			n=0;

			c=*data;

			if(c=='-')
			{
				sign=-1;
				*data++;
			}

			while(size)
			{
				c=*data++;
				if(c<'0'||c>'9') break;

				n=n*10+(c-'0');

				--size;
			}

			return n*sign;
		}

		++data;
		--size;
	}

	return 0;
}



bool nss_get_bool(char* data,int size,const char* tag)
{
	return nss_get_int(data,size,tag)?true:false;
}



bool __fastcall TFormMain::LoadSession1x(AnsiString filename)
{
	FILE *file;
	unsigned char temp[16];
	int i,j;

	file=fopen(filename.c_str(),"rb");

	if(!file) return false;

	//signature

	fread(temp,sizeof(sessionIDStr),1,file);

	if(memcmp(temp,sessionIDStr,sizeof(sessionIDStr)))
	{
		fclose(file);
		Application->MessageBox("Session data does not match current version of the program or has been corrupted","Error",MB_OK);

		return false;
	}

	//arrays

	fread(chr      ,sizeof(chr)      ,1,file);
	fread(chrCopy  ,sizeof(chrCopy)  ,1,file);
	fread(nameTable,sizeof(nameTable),1,file);
	fread(attrTable,sizeof(attrTable),1,file);
	fread(nameCopy ,sizeof(nameCopy) ,1,file);
	fread(attrCopy ,sizeof(attrCopy) ,1,file);
	fread(undoChr  ,sizeof(undoChr)  ,1,file);
	fread(undoNameTable,sizeof(undoNameTable),1,file);
	fread(undoAttrTable,sizeof(undoAttrTable),1,file);

	//palette

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			bgPal[palBank*16+i*4+j]=fget_i32(file);
		}
	}

	//screen buttons state

	SpeedButtonTiles    ->Down=fget_bool(file);
	SpeedButtonChecker  ->Down=fget_bool(file);
	SpeedButtonChrBank1 ->Down=fget_bool(file);
	SpeedButtonChrBank2 ->Down=fget_bool(file);
	SpeedButtonGridAll  ->Down=fget_bool(file);
	SpeedButtonGridTile ->Down=fget_bool(file);
	SpeedButtonGridAtr  ->Down=fget_bool(file);
	SpeedButtonGridBlock->Down=fget_bool(file);
	SpeedButtonPal      ->Down=fget_bool(file);

	//variables

	bgPalCur  =fget_i32(file);
	palActive =fget_i32(file);
	tileActive=fget_i32(file);
	bankActive=fget_i32(file);
	ppuMask   =fget_i32(file);

	nameSelection.Left  =fget_i32(file);
	nameSelection.Right =fget_i32(file);
	nameSelection.Top   =fget_i32(file);
	nameSelection.Bottom=fget_i32(file);
	nameCopyWidth       =fget_i32(file);
	nameCopyHeight      =fget_i32(file);

	chrSelection.Left  =fget_i32(file);
	chrSelection.Right =fget_i32(file);
	chrSelection.Top   =fget_i32(file);
	chrSelection.Bottom=fget_i32(file);
	chrCopyWidth       =fget_i32(file);
	chrCopyHeight      =fget_i32(file);
	chrCopyRect        =fget_bool(file);
	chrSelectRect      =fget_bool(file);

	for(i=0;i<256;i++) chrSelected[i]=fget_bool(file);


	//save dialogs settings

	SaveDialogChr ->FilterIndex=fget_i32(file);
	SaveDialogName->FilterIndex=fget_i32(file);

	SaveDialogChr ->FileName=fget_ansistring(file);
	SaveDialogName->FileName=fget_ansistring(file);
	SaveDialogPal ->FileName=fget_ansistring(file);

	fclose(file);

	return true;
}



bool __fastcall TFormMain::LoadSession2x(AnsiString filename)
{
	FILE *file;
	unsigned char temp[16];
	int i,j;

	file=fopen(filename.c_str(),"rb");

	if(!file) return false;

	//signature

	fread(temp,sizeof(sessionIDStr),1,file);

	//arrays

	fread(chr      ,sizeof(chr)      ,1,file);
	fread(chrCopy  ,sizeof(chrCopy)  ,1,file);
	fread(nameTable,sizeof(nameTable),1,file);
	fread(attrTable,sizeof(attrTable),1,file);
	fread(nameCopy ,sizeof(nameCopy) ,1,file);
	fread(attrCopy ,sizeof(attrCopy) ,1,file);
	fread(undoChr  ,sizeof(undoChr)  ,1,file);
	fread(undoNameTable,sizeof(undoNameTable),1,file);
	fread(undoAttrTable,sizeof(undoAttrTable),1,file);
	fread(metaSprites  ,sizeof(metaSprites)  ,1,file);

	//palette

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			bgPal[palBank*16+i*4+j]=fget_i32(file);
		}
	}

	//screen buttons state

	SpeedButtonTiles    ->Down=fget_bool(file);
	SpeedButtonChecker  ->Down=fget_bool(file);
	SpeedButtonSelTiles ->Down=fget_bool(file);
	SpeedButtonChrBank1 ->Down=fget_bool(file);
	SpeedButtonChrBank2 ->Down=fget_bool(file);
	SpeedButtonGridAll  ->Down=fget_bool(file);
	SpeedButtonGridTile ->Down=fget_bool(file);
	SpeedButtonGridAtr  ->Down=fget_bool(file);
	SpeedButtonGridBlock->Down=fget_bool(file);
	SpeedButtonPal      ->Down=fget_bool(file);
	SpeedButtonTypeIn   ->Down=fget_bool(file);
	SpeedButtonFrameAll ->Down=fget_bool(file);
	SpeedButtonFrameSelected->Down=fget_bool(file);
	SpeedButtonFrameNone    ->Down=fget_bool(file);
	SpeedButtonSpriteSnap   ->Down=fget_bool(file);
	SpeedButtonSprite8x16   ->Down=fget_bool(file);

	SpeedButtonSpriteBank->Enabled=SpeedButtonSprite8x16->Down;

	//variables

	bgPalCur  =fget_i32(file);
	palActive =fget_i32(file);
	tileActive=fget_i32(file);
	bankActive=fget_i32(file);
	ppuMask   =fget_i32(file);

	metaSpriteActive=fget_i32(file);
	spriteActive=fget_i32(file);
	spriteGridX =fget_i32(file);
	spriteGridY =fget_i32(file);

	nameSelection.Left  =fget_i32(file);
	nameSelection.Right =fget_i32(file);
	nameSelection.Top   =fget_i32(file);
	nameSelection.Bottom=fget_i32(file);
	nameCopyWidth       =fget_i32(file);
	nameCopyHeight      =fget_i32(file);

	chrSelection.Left  =fget_i32(file);
	chrSelection.Right =fget_i32(file);
	chrSelection.Top   =fget_i32(file);
	chrSelection.Bottom=fget_i32(file);
	chrCopyWidth       =fget_i32(file);
	chrCopyHeight      =fget_i32(file);
	chrCopyRect        =fget_bool(file);
	chrSelectRect      =fget_bool(file);

	for(i=0;i<256;i++) chrSelected[i]=fget_bool(file);

	//save dialogs settings

	SaveDialogChr ->FilterIndex=fget_i32(file);
	SaveDialogName->FilterIndex=fget_i32(file);

	SaveDialogChr ->FileName=fget_ansistring(file);
	SaveDialogName->FileName=fget_ansistring(file);
	SaveDialogPal ->FileName=fget_ansistring(file);
	SaveDialogMetaSpriteBank->FileName=fget_ansistring(file);

	fclose(file);

	return true;
}


bool __fastcall TFormMain::LoadSessionText(AnsiString filename)
{
	FILE *file;
	char *text;
	int i,j,size;

	file=fopen(filename.c_str(),"rb");

	if(!file) return false;

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	text=(char*)malloc(size+1);
	text[size]=0;

	fread(text,size,1,file);
	fclose(file);

	LabelStats->Caption="Loading...";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;

	//palette

	nss_get_bytes(text,size,"Palette=",bgPal,sizeof(bgPal));

	//screen buttons state

	SpeedButtonTiles    ->Down=nss_get_bool(text,size,"BtnTiles=");
	SpeedButtonChecker  ->Down=nss_get_bool(text,size,"BtnChecker=");
	SpeedButtonSelTiles ->Down=nss_get_bool(text,size,"BtnSelTiles=");
	SpeedButtonChrBank1 ->Down=nss_get_bool(text,size,"BtnChrBank1=");
	SpeedButtonChrBank2 ->Down=nss_get_bool(text,size,"BtnChrBank2=");
	SpeedButtonGridAll  ->Down=nss_get_bool(text,size,"BtnGridAll=");
	SpeedButtonGridTile ->Down=nss_get_bool(text,size,"BtnGridTile=");
	SpeedButtonGridAtr  ->Down=nss_get_bool(text,size,"BtnGridAtr=");
	SpeedButtonGridBlock->Down=nss_get_bool(text,size,"BtnGridBlock=");
	SpeedButtonGridScreen->Down=nss_get_bool(text,size,"BtnGridScr=");

	btnSelectedOnlyInverted->Down=nss_get_bool(text,size,"BtnSelectedOnlyInverted=");
	btnMask->Down=nss_get_bool(text,size,"BtnSolidityMask=");

	SpeedButtonPal      ->Down=nss_get_bool(text,size,"BtnPal=");
	SpeedButtonTypeIn   ->Down=nss_get_bool(text,size,"BtnTypeIn=");
	SpeedButtonFrameAll ->Down=nss_get_bool(text,size,"BtnFrameAll=");
	SpeedButtonFrameSelected->Down=nss_get_bool(text,size,"BtnFrameSelected=");
	SpeedButtonFrameNone    ->Down=nss_get_bool(text,size,"BtnFrameNone=");
	SpeedButtonSpriteSnap   ->Down=nss_get_bool(text,size,"BtnSpriteSnap=");
	SpeedButtonSprite8x16   ->Down=nss_get_bool(text,size,"BtnSprite8x16=");
	ButtonForceView   ->Down=nss_get_bool(text,size,"BtnForce8x16=");


	MImportBestOffsets               ->Checked=nss_get_bool(text,size,"MenuBestOffsets=");
	MImportLossy                     ->Checked=nss_get_bool(text,size,"MenuLossy=");
	MImportThreshold                 ->Checked=nss_get_bool(text,size,"MenuThreshold=");
	MImportNoColorData               ->Checked=nss_get_bool(text,size,"MenuNoColorData=");
	MPutSelectionToMetaspriteAutoInc ->Checked=nss_get_bool(text,size,"MenuMetaSprAutoInc=");
	MPutSelectionToMetaspriteSkipZero->Checked=nss_get_bool(text,size,"MenuMetaSprSkipZero=");
	MPutSelectionToMetaspriteMerge   ->Checked=nss_get_bool(text,size,"MenuMetaSprMerge=");

	MSaveIncName                     ->Checked=nss_get_bool(text,size,"MenuSaveIncName=");
	MSaveIncAttr                     ->Checked=nss_get_bool(text,size,"MenuSaveIncAttr=");
	MSaveRLE                         ->Checked=nss_get_bool(text,size,"MenuSaveRLE=");

	//these menu items have duplicates in the file menu.
	includeattributes1->Checked=MSaveIncAttr->Checked;
	includeattributes1->Checked=MSaveIncAttr->Checked;
	forceNESlibRLEpacking1->Checked=MSaveRLE->Checked;

	SpeedButtonSpriteBank->Enabled=SpeedButtonSprite8x16->Down;

	if(CheckTag(text,size,"CheckLinkedCHRmode="))
		bLinkedCHRmode=nss_get_bool(text,size,"CheckLinkedCHRmode=");
	else  bLinkedCHRmode=false;


	for(i=0;i<8;i++)
	{
		char label[24];
		sprintf(label, "VarBtnStateMtProps%i=", i);
		btnStateMtProps[i]			=nss_get_bool(text,size,label);
	}

	for(i=0;i<8;i++)
	{
		char label[24];
		sprintf(label, "VarBtnStateCHRProps%i=", i);
		btnStateProps[i]			=nss_get_bool(text,size,label);
	}

	if(CheckTag(text,size,"CheckMetatileEditor_ReserveFirst="))
		bMT_Reserve1st=nss_get_bool(text,size,"CheckMetatileEditor_ReserveFirst=");
	else  bMT_Reserve1st=true;
	if(CheckTag(text,size,"CheckMetatileEditor_AlignToScreen="))
		bMT_AlignScr=nss_get_bool(text,size,"CheckMetatileEditor_AlignToScreen=");
	else  bMT_AlignScr=true;


	if(CheckTag(text,size,"BtnMetatileEditor_UseAttributes="))
		bMT_UseAttr=nss_get_bool(text,size,"BtnMetatileEditor_UseAttributes=");
	else  bMT_UseAttr=true;
	if(CheckTag(text,size,"BtnMetatileEditor_Clonestamp="))
		bMT_Clonestamp=nss_get_bool(text,size,"BtnMetatileEditor_Clonestamp=");
	else  bMT_Clonestamp=false;
	if(CheckTag(text,size,"BtnMetatileEditor_CloneSnap="))
		bMT_CloneSnap=nss_get_bool(text,size,"BtnMetatileEditor_CloneSnap=");
	else  bMT_CloneSnap=true;

	if(CheckTag(text,size,"BtnMetatileEditor_Tiles="))
		bMT_Tiles=nss_get_bool(text,size,"BtnMetatileEditor_Tiles=");
	else  bMT_Tiles=true;
	if(CheckTag(text,size,"BtnMetatileEditor_Attributes="))
		bMT_Attr=nss_get_bool(text,size,"BtnMetatileEditor_Attributes=");
	else  bMT_Attr=true;
	if(CheckTag(text,size,"BtnMetatileEditor_Properties="))
		bMT_Props=nss_get_bool(text,size,"BtnMetatileEditor_Properties=");
	else  bMT_Props=true;
	if(CheckTag(text,size,"BtnMetatileEditor_Map="))
		bMT_Map=nss_get_bool(text,size,"BtnMetatileEditor_Map=");
	else  bMT_Map=false;

	if(CheckTag(text,size,"ChkSwapBanks_Properties="))
		bBS_InclLabel=nss_get_bool(text,size,"ChkSwapBanks_Properties=");
	else  bBS_InclLabel=true;
		if(CheckTag(text,size,"ChkSwapBanks_Labels="))
		bBS_InclProps=nss_get_bool(text,size,"ChkSwapBanks_Labels=");
	else  bBS_InclProps=true;


	//variables

	bgPalCur  =nss_get_int(text,size,"VarBgPalCur=");
	palActive =nss_get_int(text,size,"VarPalActive=");
	tileActive=nss_get_int(text,size,"VarTileActive=");
	bankActive=nss_get_int(text,size,"VarBankActive=");
	ppuMask   =nss_get_int(text,size,"VarPPUMask=");

	//conditional initialization to useful presets
	if(bgPalCur==3) bgPalCurOld=0; else bgPalCurOld=3;
	if(palActive==1) palActiveOld=0;  else palActiveOld=1;

    iWarnMsprCyan =nss_get_int(text,size,"VarCustomScanlineWarningCyan=");


	tileViewTagBuf  =nss_get_int(text,size,"VarTileViewTagBuf=");

	ppuMaskSet[0]   =nss_get_int(text,size,"VarPPUMaskSet0=");
	ppuMaskSet[1]   =nss_get_int(text,size,"VarPPUMaskSet1=");
	ppuMaskSet[2]   =nss_get_int(text,size,"VarPPUMaskSet2=");
	ppuMaskSet[3]   =nss_get_int(text,size,"VarPPUMaskSet3=");

	palBank   =nss_get_int(text,size,"VarPalBank=");

	metaSpriteActive=nss_get_int(text,size,"VarMetaSpriteActive=");
	spriteActive=nss_get_int(text,size,"VarSpriteActive=");
	spriteGridX =nss_get_int(text,size,"VarSpriteGridX=");
	spriteGridY =nss_get_int(text,size,"VarSpriteGridY=");

	nameTableWidth      =nss_get_int(text,size,"VarNameW=");
	nameTableHeight     =nss_get_int(text,size,"VarNameH=");
	nameTableViewX      =nss_get_int(text,size,"VarNameViewX=");
	nameTableViewY      =nss_get_int(text,size,"VarNameViewY=");
	nameSelection.Left  =nss_get_int(text,size,"VarNameSelectionL=");
	nameSelection.Right =nss_get_int(text,size,"VarNameSelectionR=");
	nameSelection.Top   =nss_get_int(text,size,"VarNameSelectionT=");
	nameSelection.Bottom=nss_get_int(text,size,"VarNameSelectionB=");
	nameCopyWidth       =nss_get_int(text,size,"VarNameCopyW=");
	nameCopyHeight      =nss_get_int(text,size,"VarNameCopyH=");

	chrSelection.Left  =nss_get_int(text,size,"VarCHRSelectionL=");
	chrSelection.Right =nss_get_int(text,size,"VarCHRSelectionR=");
	chrSelection.Top   =nss_get_int(text,size,"VarCHRSelectionT=");
	chrSelection.Bottom=nss_get_int(text,size,"VarCHRSelectionB=");
	chrCopyWidth       =nss_get_int(text,size,"VarCHRCopyW=");
	chrCopyHeight      =nss_get_int(text,size,"VarCHRCopyH=");
	chrCopyRect        =nss_get_bool(text,size,"VarCHRCopyRect=");
	chrSelectRect      =nss_get_bool(text,size,"VarCHRSelectRect=");

	mtClickID		   =nss_get_int(text,size,"VarMtClickID=");
	mtClickID_store_2x2=nss_get_int(text,size,"VarMtClickID_store_2x2=");
	mtClickID_store_4x4=nss_get_int(text,size,"VarMtClickID_store_4x4=");
	mtClickID_store_8x8=nss_get_int(text,size,"VarMtClickID_store_8x8=");

	iBrushPresetIndex=nss_get_int(text,size,"VarBrushPresetIndex=");

	metatileSets_2x2     			=nss_get_int(text,size,"VarMetatileSets_2x2=");
	undo_metatileSets_2x2			=nss_get_int(text,size,"VarUndo_metatileSets_2x2=");
	checkpoint_metatileSets_2x2		=nss_get_int(text,size,"VarCheckpoint_metatileSets_2x2=");
	undo_checkpoint_metatileSets_2x2=nss_get_int(text,size,"VarUndo_checkpoint_metatileSets_2x2=");

	metatileSets_4x4				=nss_get_int(text,size,"VarMetatileSets_4x4=");
	undo_metatileSets_4x4			=nss_get_int(text,size,"VarUndo_metatileSets_4x4=");
	checkpoint_metatileSets_4x4		=nss_get_int(text,size,"VarCheckpoint_metatileSets_4x4=");
	undo_checkpoint_metatileSets_4x4=nss_get_int(text,size,"VarUndo_checkpoint_metatileSets_4x4=");

	metatileSets_8x8				=nss_get_int(text,size,"VarMetatileSets_8x8=");
	undo_metatileSets_8x8			=nss_get_int(text,size,"VarUndo_metatileSets_8x8=");
	checkpoint_metatileSets_8x8		=nss_get_int(text,size,"VarCheckpoint_metatileSets_8x8=");
	undo_checkpoint_metatileSets_8x8=nss_get_int(text,size,"VarUndo_checkpoint_metatileSets_8x8=");



	//arrays
	//nss_get_bytes(text,size,"ppuMaskSet=",ppuMaskSet,sizeof(ppuMaskSet));
	nss_get_bytes(text,size,"VarCHRSelected=",chrSelected,sizeof(chrSelected));

	//working pal is loaded before everything else.
	nss_get_bytes(text,size,"PalUndo="    ,bgPalUndo    ,sizeof(bgPalUndo));


	nss_get_bytes(text,size,"CHRCopy="    ,chrCopy      ,sizeof(chrCopy));


	nss_get_bytes(text,size,"NameTable="  ,nameTable    ,name_size());
	nss_get_bytes(text,size,"NameCopy="   ,nameCopy     ,name_size());
	nss_get_bytes(text,size,"NameUndo="   ,undoNameTable,name_size());

	nss_get_bytes(text,size,"AttrTable="  ,attrTable    ,attr_size());
	nss_get_bytes(text,size,"AttrCopy="   ,attrCopy     ,attr_size());
	nss_get_bytes(text,size,"AttrUndo="   ,undoAttrTable,attr_size());


	nss_get_bytes(text,size,"MetaSprites=",metaSprites,sizeof(metaSprites));

	nss_get_bytes(text,size,"TilePropConditionType=",propConditional,sizeof(propConditional));
	nss_get_bytes(text,size,"TilePropConditionTypeUndo=",propConditionalUndo,sizeof(propConditional));

	LabelStats->Caption="Loading viewtables...";
	if(CheckTag(text,size, "32_BankViewTable="))
		nss_get_uint32s(text,size,"32_BankViewTable=",bankViewTable,sizeof(bankViewTable));
	else {
		for(int b=0;b<4;b++)
		for (int i=0; i < 256; i++) {
			bankViewTable[b*256 + i] = b*4096;
		}
	}
	if(CheckTag(text,size, "32_BankViewTableUndo="))
		nss_get_uint32s(text,size,"32_BankViewTableUndo=",undo_bankViewTable,sizeof(undo_bankViewTable));
	else {
		for(int b=0;b<4;b++)
		for (int i=0; i < 256; i++) {
			undo_bankViewTable[b*256 + i] = b*4096;
		}
	}


   //


	nss_get_2D_bools(text,size,"PropertyConditions=",propCondition,4,8);
	nss_get_2D_bools(text,size,"PropertyConditionsUndo=",propConditionUndo,4,8);


	//reallocatable tables, metatile & chr bank stuff
	chrBanks					=nss_get_int(text,size,"VarCHRBanks=");
	undo_chrBanks               =nss_get_int(text,size,"VarCHRBanksUndo=");
	checkpoint_chrBanks         =nss_get_int(text,size,"VarCheckpoint_CHRBanks=");
	undo_checkpoint_chrBanks    =nss_get_int(text,size,"VarCheckpoint_CHRBanksUndo=");

	chrBanks_LinkBuf					=nss_get_int(text,size,"VarCHRBanks_LinkBuf=");
	undo_chrBanks_LinkBuf               =nss_get_int(text,size,"VarCHRBanksUndo_LinkBuf=");
	checkpoint_chrBanks_LinkBuf         =nss_get_int(text,size,"VarCheckpoint_CHRBanks_LinkBuf=");
	undo_checkpoint_chrBanks_LinkBuf    =nss_get_int(text,size,"VarCheckpoint_CHRBanksUndo_LinkBuf=");

	bool bGenerateCHRBanks = ((chrBanks==0)
		||(undo_chrBanks==0)
		||(checkpoint_chrBanks==0)
		||(undo_checkpoint_chrBanks==0)
		)? true:false;


	if(chrBanks==0)
		chrBanks=4;
	if(undo_chrBanks==0)
		undo_chrBanks=4;
	if(checkpoint_chrBanks==0)
		checkpoint_chrBanks=4;
	if(undo_checkpoint_chrBanks==0)
		undo_checkpoint_chrBanks=4;

   if(chrBanks_LinkBuf==0)
		chrBanks_LinkBuf=4;
	if(undo_chrBanks_LinkBuf==0)
		undo_chrBanks_LinkBuf=4;
	if(checkpoint_chrBanks_LinkBuf==0)
		checkpoint_chrBanks_LinkBuf=4;
	if(undo_checkpoint_chrBanks_LinkBuf==0)
		undo_checkpoint_chrBanks_LinkBuf=4;

	size_t tmpSize;
	tmpSize = chrBanks*4096;
	chr = (char*) realloc(chr, tmpSize);
	tileProperties = (char*) realloc(tileProperties, tmpSize/16);

	tmpSize = undo_chrBanks*4096;
	undoChr = (char*) realloc(undoChr, tmpSize);
	tilePropertiesUndo = (char*) realloc(tilePropertiesUndo, tmpSize/16);


	tmpSize = checkpoint_chrBanks*4096;
	checkpointChr = (char*) realloc(checkpointChr, tmpSize);
	checkpointTileProperties = (char*) realloc(checkpointTileProperties, tmpSize/16);

	tmpSize = undo_checkpoint_chrBanks*4096;
	undoCheckpointChr = (char*) realloc(undoCheckpointChr, tmpSize);
	checkpointTilePropertiesUndo = (char*) realloc(checkpointTilePropertiesUndo, tmpSize/16);



	if(CheckTag(text,size, "32_TileCollisionProperties=")){
			nss_get_uint32s(text,size,"32_TileCollisionProperties=",tileProperties32,sizeof(tileProperties32));
			memset(tileProperties,0,chrBanks*256);
			for (size_t i = 0; i < 512; i++) {
				tileProperties[i] = static_cast<char>(tileProperties32[i] & 0xFF);
			}
	}
	else {
		memset(tileProperties32,0,sizeof(tileProperties32));
		if(CheckTag(text,size, "8_TileCollisionProperties=")){
			nss_get_bytes(text,size,"8_TileCollisionProperties=",tileProperties,chrBanks*256);
		}
		else memset(tileProperties,0,chrBanks*256);
	}


	if(CheckTag(text,size, "32_TileCollisionPropertiesUndo=")){
			nss_get_uint32s(text,size,"32_TileCollisionPropertiesUndo=",tilePropertiesUndo32,sizeof(tilePropertiesUndo32));
			memset(tilePropertiesUndo,0,undo_chrBanks*256);
			for (size_t i = 0; i < 512; i++) {
				tilePropertiesUndo[i] = static_cast<char>(tilePropertiesUndo32[i] & 0xFF);
			}
	}
	else {
		memset(tilePropertiesUndo32,0,sizeof(tilePropertiesUndo32));
		if(CheckTag(text,size, "8_TileCollisionPropertiesUndo=")){
			nss_get_bytes(text,size,"8_TileCollisionPropertiesUndo=",tilePropertiesUndo,undo_chrBanks*256);
		}
		else memset(tilePropertiesUndo,0,undo_chrBanks*256);
	}




	//Checkpoint stuff
	nss_get_bytes(text,size,"Checkpoint_Palette="  ,bgPalCheckpoint 		,sizeof(bgPal));
	nss_get_bytes(text,size,"Checkpoint_PalUndo="  ,bgPalUndoCheckPoint  ,sizeof(bgPalUndo));

	nss_get_bytes(text,size,"Checkpoint_CHRMain="  ,checkpointChr          ,checkpoint_chrBanks*4096);
	nss_get_bytes(text,size,"Checkpoint_CHRUndo="  ,undoCheckpointChr      ,undo_checkpoint_chrBanks*4096);

	nss_get_bytes(text,size,"Checkpoint_NameTable=",checkpointNameTable    ,name_size());
	nss_get_bytes(text,size,"Checkpoint_NameUndo=" ,undoCheckpointNameTable,name_size());

	nss_get_bytes(text,size,"Checkpoint_AttrTable=",checkpointAttrTable    ,attr_size());
	nss_get_bytes(text,size,"Checkpoint_AttrUndo=" ,undoCheckpointAttrTable,attr_size());

	nss_get_bytes(text,size,"Checkpoint_MetaSprites=",checkpointMetaSprites,sizeof(metaSprites));

	if(CheckTag(text,size, "Checkpoint_32_BankViewTable="))
		nss_get_uint32s(text,size,"Checkpoint_32_BankViewTable=",checkpoint_bankViewTable,sizeof(checkpoint_bankViewTable));
	else {
		for(int b=0;b<4;b++)
		for (int i=0; i < 256; i++) {
			checkpoint_bankViewTable[b*256 + i] = b*4096;
		}
	}
	if(CheckTag(text,size, "Checkpoint_32_BankViewTableUndo="))
		nss_get_uint32s(text,size,"Checkpoint_32_BankViewTableUndo=",undo_checkpoint_bankViewTable,sizeof(undo_checkpoint_bankViewTable));
	else {
		for(int b=0;b<4;b++)
		for (int i=0; i < 256; i++) {
			undo_checkpoint_bankViewTable[b*256 + i] = b*4096;
		}
	}

	if(CheckTag(text,size, "Checkpoint_32_TileCollisionProperties=")){
			nss_get_uint32s(text,size,"Checkpoint_32_TileCollisionProperties=",checkpointTileProperties32,sizeof(checkpointTileProperties32));
			memset(checkpointTileProperties,0,checkpoint_chrBanks*256);
			for (size_t i = 0; i < 512; i++) {
				checkpointTileProperties[i] = static_cast<char>(checkpointTileProperties32[i] & 0xFF);
			}
	}
	else  {
		memset(checkpointTileProperties32,0,sizeof(checkpointTileProperties32));
		if(CheckTag(text,size, "Checkpoint_8_TileCollisionProperties=")){
			nss_get_bytes(text,size,"Checkpoint_8_TileCollisionProperties=",checkpointTileProperties,checkpoint_chrBanks*256);
		}
		else memset(checkpointTileProperties,0,checkpoint_chrBanks*256);
	}


	if(CheckTag(text,size, "Checkpoint_32_TileCollisionPropertiesUndo=")){
			nss_get_uint32s(text,size,"Checkpoint_32_TileCollisionPropertiesUndo=",checkpointTilePropertiesUndo32,sizeof(checkpointTilePropertiesUndo32));
			memset(checkpointTilePropertiesUndo,0,undo_checkpoint_chrBanks*256);
			for (size_t i = 0; i < 512; i++) {
				checkpointTilePropertiesUndo[i] = static_cast<char>(checkpointTilePropertiesUndo32[i] & 0xFF);
			}
	}
	else  {
		memset(checkpointTilePropertiesUndo32,0,sizeof(checkpointTilePropertiesUndo32));
		if(CheckTag(text,size, "Checkpoint_8_TileCollisionPropertiesUndo=")){
			nss_get_bytes(text,size,"Checkpoint_8_TileCollisionPropertiesUndo=",checkpointTilePropertiesUndo,undo_checkpoint_chrBanks*256);
		}
		else memset(checkpointTilePropertiesUndo,0,undo_checkpoint_chrBanks*256);
	}




	nss_get_2D_bools(text,size,"Checkpoint_PropertyConditions=",checkpointPropCondition,4,8);
	nss_get_2D_bools(text,size,"Checkpoint_PropertyConditionsUndo=",checkpointPropConditionUndo,4,8);

	nss_get_bytes(text,size,"Checkpoint_TilePropConditionType=",checkpointPropConditional,sizeof(propConditional));
	nss_get_bytes(text,size,"Checkpoint_TilePropConditionTypeUndo=",checkpointPropConditionalUndo,sizeof(propConditional));






	nss_get_bytes(text,size,"CHRMain="    ,chr          ,chrBanks*4096);
	nss_get_bytes(text,size,"CHRUndo="    ,undoChr      ,undo_chrBanks*4096);


	if(bGenerateCHRBanks){    //make a generic setup
		//memcpy(chrBank,chr,4096*2);
		//memcpy(undo_chrBank,undoChr,4096*2);
		//memcpy(checkpoint_chrBank,checkpointChr,4096*2);
		//memcpy(undo_checkpoint_chrBank,undoCheckpointChr,4096*2);

		chrBankLabels.clear();
		undo_chrBankLabels.clear();
		checkpoint_chrBankLabels.clear();
		undo_checkpoint_chrBankLabels.clear();


		for(unsigned int k=0;k<chrBanks;k++){chrBankLabels.push_back("Unlabeled");}
		for(unsigned int k=0;k<undo_chrBanks;k++){undo_chrBankLabels.push_back("Unlabeled");}
		for(unsigned int k=0;k<checkpoint_chrBanks;k++){checkpoint_chrBankLabels.push_back("Unlabeled");}
		for(unsigned int k=0;k<undo_checkpoint_chrBanks;k++){undo_checkpoint_chrBankLabels.push_back("Unlabeled");}


	}



	else{
        //FIX
		//nss_get_bytes(text,size,"CHRBank=",chrBank,sizeof(chrBank));
		//nss_get_bytes(text,size,"CHRBankUndo=",undo_chrBank,sizeof(undo_chrBank));
		//nss_get_bytes(text,size,"Checkpoint_CHRBank=",checkpoint_chrBank,sizeof(checkpoint_chrBank));
		//nss_get_bytes(text,size,"Checkpoint_CHRBankUndo=",undo_checkpoint_chrBank,sizeof(undo_checkpoint_chrBank));

	
		//WorkCHRToBankCHR();


		//make extra sure these tags exist, getting the vectors right is crucial to avoid errors.

		if(CheckTag(text,size, "CHRBankLabels=")) chrBankLabels = deserializeVector(text, "CHRBankLabels=");
		else {
			chrBankLabels.clear();
			for(unsigned int k=0;k<chrBanks;k++){chrBankLabels.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "CHRBankLabelsUndo=")) undo_chrBankLabels = deserializeVector(text, "CHRBankLabelsUndo=");
		else{
			undo_chrBankLabels.clear();
			for(unsigned int k=0;k<undo_chrBanks;k++){undo_chrBankLabels.push_back("Unlabeled");}
		}

		if(CheckTag(text,size, "Checkpoint_CHRBankLabels=")) checkpoint_chrBankLabels = deserializeVector(text, "Checkpoint_CHRBankLabels=");
		else{
			checkpoint_chrBankLabels.clear();
			for(unsigned int k=0;k<checkpoint_chrBanks;k++){checkpoint_chrBankLabels.push_back("Unlabeled");}
		}

		if(CheckTag(text,size, "Checkpoint_CHRBankLabelsUndo=")) undo_checkpoint_chrBankLabels = deserializeVector(text, "Checkpoint_CHRBankLabelsUndo=");
		else{
			undo_checkpoint_chrBankLabels.clear();
			for(unsigned int k=0;k<undo_checkpoint_chrBanks;k++){undo_checkpoint_chrBankLabels.push_back("Unlabeled");}

		}
	}
	tmp_chrBankLabels.clear();



	bool bGenerateMetatileBanks =
		 ((!CheckTag(text,size, "VarMetatileSets_2x2="))
		||(!CheckTag(text,size, "VarUndo_metatileSets_2x2="))
		||(!CheckTag(text,size, "VarCheckpoint_metatileSets_2x2="))
		||(!CheckTag(text,size, "VarUndo_checkpoint_metatileSets_2x2="))

		||(!CheckTag(text,size, "VarMetatileSets_4x4="))
		||(!CheckTag(text,size, "VarUndo_metatileSets_4x4="))
		||(!CheckTag(text,size, "VarCheckpoint_metatileSets_4x4="))
		||(!CheckTag(text,size, "VarUndo_checkpoint_metatileSets_4x4="))

		||(!CheckTag(text,size, "VarMetatileSets_8x8="))
		||(!CheckTag(text,size, "VarUndo_metatileSets_8x8="))
		||(!CheckTag(text,size, "VarCheckpoint_metatileSets_8x8="))
		||(!CheckTag(text,size, "VarUndo_checkpoint_metatileSets_8x8="))

		)? true:false;

	if(bGenerateMetatileBanks)
	{
	   metatileSets_2x2=4;
	   metatileSets_4x4=4;
	   metatileSets_8x8=2;

	   undo_metatileSets_2x2=4;
	   undo_metatileSets_4x4=4;
	   undo_metatileSets_8x8=2;

	   checkpoint_metatileSets_2x2=4;
	   checkpoint_metatileSets_4x4=4;
	   checkpoint_metatileSets_8x8=2;

	   undo_checkpoint_metatileSets_2x2=4;
	   undo_checkpoint_metatileSets_4x4=4;
	   undo_checkpoint_metatileSets_8x8=2;

	}

	//do this regardless which.
	metatileSet_2x2_id 					  = (char*) realloc(metatileSet_2x2_id, metatileSets_2x2*META_2x2);
	undo_metatileSet_2x2_id				  = (char*) realloc(undo_metatileSet_2x2_id, undo_metatileSets_2x2*META_2x2);
	metatileSet_2x2_pal				      = (char*) realloc(metatileSet_2x2_pal, metatileSets_2x2*META_2x2);
	undo_metatileSet_2x2_pal			  = (char*) realloc(undo_metatileSet_2x2_pal, undo_metatileSets_2x2*META_2x2);
	metatileSet_2x2_props				  = (char*) realloc(metatileSet_2x2_props, metatileSets_2x2*META_2x2);
	undo_metatileSet_2x2_props			  = (char*) realloc(undo_metatileSet_2x2_props, undo_metatileSets_2x2*META_2x2);

	checkpoint_metatileSet_2x2_id 		  = (char*) realloc(checkpoint_metatileSet_2x2_id, checkpoint_metatileSets_2x2*META_2x2);
	undo_checkpoint_metatileSet_2x2_id	  = (char*) realloc(undo_checkpoint_metatileSet_2x2_id, undo_checkpoint_metatileSets_2x2*META_2x2);
	checkpoint_metatileSet_2x2_pal		  = (char*) realloc(checkpoint_metatileSet_2x2_pal, checkpoint_metatileSets_2x2*META_2x2);
	undo_checkpoint_metatileSet_2x2_pal	  = (char*) realloc(undo_checkpoint_metatileSet_2x2_pal, undo_checkpoint_metatileSets_2x2*META_2x2);
	checkpoint_metatileSet_2x2_props	  = (char*) realloc(checkpoint_metatileSet_2x2_props, checkpoint_metatileSets_2x2*META_2x2);
	undo_checkpoint_metatileSet_2x2_props = (char*) realloc(undo_checkpoint_metatileSet_2x2_props, undo_checkpoint_metatileSets_2x2*META_2x2);

	metatileSet_4x4_id 					  = (char*) realloc(metatileSet_4x4_id, metatileSets_4x4*META_4x4);
	undo_metatileSet_4x4_id				  = (char*) realloc(undo_metatileSet_4x4_id, undo_metatileSets_4x4*META_4x4);
	metatileSet_4x4_pal				      = (char*) realloc(metatileSet_4x4_pal, metatileSets_4x4*META_4x4);
	undo_metatileSet_4x4_pal			  = (char*) realloc(undo_metatileSet_4x4_pal, undo_metatileSets_4x4*META_4x4);
	metatileSet_4x4_props				  = (char*) realloc(metatileSet_4x4_props, metatileSets_4x4*META_4x4);
	undo_metatileSet_4x4_props			  = (char*) realloc(undo_metatileSet_4x4_props, undo_metatileSets_4x4*META_4x4);

	checkpoint_metatileSet_4x4_id 		  = (char*) realloc(checkpoint_metatileSet_4x4_id, checkpoint_metatileSets_4x4*META_4x4);
	undo_checkpoint_metatileSet_4x4_id	  = (char*) realloc(undo_checkpoint_metatileSet_4x4_id, undo_checkpoint_metatileSets_4x4*META_4x4);
	checkpoint_metatileSet_4x4_pal		  = (char*) realloc(checkpoint_metatileSet_4x4_pal, checkpoint_metatileSets_4x4*META_4x4);
	undo_checkpoint_metatileSet_4x4_pal	  = (char*) realloc(undo_checkpoint_metatileSet_4x4_pal, undo_checkpoint_metatileSets_4x4*META_4x4);
	checkpoint_metatileSet_4x4_props	  = (char*) realloc(checkpoint_metatileSet_4x4_props, checkpoint_metatileSets_4x4*META_4x4);
	undo_checkpoint_metatileSet_4x4_props = (char*) realloc(undo_checkpoint_metatileSet_4x4_props, undo_checkpoint_metatileSets_4x4*META_4x4);

	metatileSet_8x8_id 					  = (char*) realloc(metatileSet_8x8_id, metatileSets_8x8*META_8x8);
	undo_metatileSet_8x8_id				  = (char*) realloc(undo_metatileSet_8x8_id, undo_metatileSets_8x8*META_8x8);
	metatileSet_8x8_pal				      = (char*) realloc(metatileSet_8x8_pal, metatileSets_8x8*META_8x8);
	undo_metatileSet_8x8_pal			  = (char*) realloc(undo_metatileSet_8x8_pal, undo_metatileSets_8x8*META_8x8);
	metatileSet_8x8_props				  = (char*) realloc(metatileSet_8x8_props, metatileSets_8x8*META_8x8);
	undo_metatileSet_8x8_props			  = (char*) realloc(undo_metatileSet_8x8_props, undo_metatileSets_8x8*META_8x8);

	checkpoint_metatileSet_8x8_id 		  = (char*) realloc(checkpoint_metatileSet_8x8_id, checkpoint_metatileSets_8x8*META_8x8);
	undo_checkpoint_metatileSet_8x8_id	  = (char*) realloc(undo_checkpoint_metatileSet_8x8_id, undo_checkpoint_metatileSets_8x8*META_8x8);
	checkpoint_metatileSet_8x8_pal		  = (char*) realloc(checkpoint_metatileSet_8x8_pal, checkpoint_metatileSets_8x8*META_8x8);
	undo_checkpoint_metatileSet_8x8_pal	  = (char*) realloc(undo_checkpoint_metatileSet_8x8_pal, undo_checkpoint_metatileSets_8x8*META_8x8);
	checkpoint_metatileSet_8x8_props	  = (char*) realloc(checkpoint_metatileSet_8x8_props, checkpoint_metatileSets_8x8*META_8x8);
	undo_checkpoint_metatileSet_8x8_props = (char*) realloc(undo_checkpoint_metatileSet_8x8_props, undo_checkpoint_metatileSets_8x8*META_8x8);

	mtContent_2x2                         = (char*) realloc(mtContent_2x2, 64*metatileSets_2x2);
	//mtUsage_2x2                           = (uint32_t*) realloc(mtUsage_2x2, 64*metatileSets_2x2*sizeof(uint32_t));

	mtContent_4x4                         = (char*) realloc(mtContent_4x4, 64*metatileSets_4x4);
	//mtUsage_4x4                           = (uint32_t*) realloc(mtUsage_4x4, 64*metatileSets_4x4*sizeof(uint32_t));

	mtContent_8x8                         = (char*) realloc(mtContent_8x8, 64*metatileSets_8x8);
	//mtUsage_8x8                           = (uint32_t*) realloc(mtUsage_8x8, 64*metatileSets_8x8*sizeof(uint32_t));

	//ask again
	if(bGenerateMetatileBanks){

		memset(metatileSet_2x2_id,0,64*4*metatileSets_2x2);
		memset(metatileSet_2x2_pal,0,64*4*metatileSets_2x2);
		memset(metatileSet_2x2_props,0,64*4*metatileSets_2x2);

		memset(undo_metatileSet_2x2_id,0,64*4*metatileSets_2x2);
		memset(undo_metatileSet_2x2_pal,0,64*4*metatileSets_2x2);
		memset(undo_metatileSet_2x2_props,0,64*4*metatileSets_2x2);

		memset(checkpoint_metatileSet_2x2_id,0,64*4*metatileSets_2x2);
		memset(checkpoint_metatileSet_2x2_pal,0,64*4*metatileSets_2x2);
		memset(checkpoint_metatileSet_2x2_props,0,64*4*metatileSets_2x2);

		memset(undo_checkpoint_metatileSet_2x2_id,0,64*4*metatileSets_2x2);
		memset(undo_checkpoint_metatileSet_2x2_pal,0,64*4*metatileSets_2x2);
		memset(undo_checkpoint_metatileSet_2x2_props,0,64*4*metatileSets_2x2);

		memset(mtContent_2x2,0,64*metatileSets_2x2);
		//memset(mtUsage_2x2,0,64*metatileSets_2x2*sizeof(uint32_t));



		memset(metatileSet_4x4_id,0,64*16*metatileSets_4x4);
		memset(metatileSet_4x4_pal,0,64*16*metatileSets_4x4);
		memset(metatileSet_4x4_props,0,64*16*metatileSets_4x4);

		memset(undo_metatileSet_4x4_id,0,64*16*metatileSets_4x4);
		memset(undo_metatileSet_4x4_pal,0,64*16*metatileSets_4x4);
		memset(undo_metatileSet_4x4_props,0,64*16*metatileSets_4x4);

		memset(checkpoint_metatileSet_4x4_id,0,64*16*metatileSets_4x4);
		memset(checkpoint_metatileSet_4x4_pal,0,64*16*metatileSets_4x4);
		memset(checkpoint_metatileSet_4x4_props,0,64*16*metatileSets_4x4);

		memset(undo_checkpoint_metatileSet_4x4_id,0,64*16*metatileSets_4x4);
		memset(undo_checkpoint_metatileSet_4x4_pal,0,64*16*metatileSets_4x4);
		memset(undo_checkpoint_metatileSet_4x4_props,0,64*16*metatileSets_4x4);

		memset(mtContent_4x4,0,64*metatileSets_4x4);
		//memset(mtUsage_4x4,0,64*metatileSets_4x4*sizeof(uint32_t));



		memset(metatileSet_8x8_id,0,64*64*metatileSets_8x8);
		memset(metatileSet_8x8_pal,0,64*64*metatileSets_8x8);
		memset(metatileSet_8x8_props,0,64*64*metatileSets_8x8);

		memset(undo_metatileSet_8x8_id,0,64*64*metatileSets_8x8);
		memset(undo_metatileSet_8x8_pal,0,64*64*metatileSets_8x8);
		memset(undo_metatileSet_8x8_props,0,64*64*metatileSets_8x8);

		memset(checkpoint_metatileSet_8x8_id,0,64*64*metatileSets_8x8);
		memset(checkpoint_metatileSet_8x8_pal,0,64*64*metatileSets_8x8);
		memset(checkpoint_metatileSet_8x8_props,0,64*64*metatileSets_8x8);

		memset(undo_checkpoint_metatileSet_8x8_id,0,64*64*metatileSets_8x8);
		memset(undo_checkpoint_metatileSet_8x8_pal,0,64*64*metatileSets_8x8);
		memset(undo_checkpoint_metatileSet_8x8_props,0,64*64*metatileSets_8x8);

		memset(mtContent_8x8,0,64*metatileSets_8x8);
		//memset(mtUsage_8x8,0,64*metatileSets_8x8*sizeof(uint32_t));

		metatileSetLabels_2x2.clear();
		undo_metatileSetLabels_2x2.clear();
		checkpoint_metatileSetLabels_2x2.clear();
		undo_checkpoint_metatileSetLabels_2x2.clear();

		for( int k=0;k<metatileSets_2x2;k++){metatileSetLabels_2x2.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_2x2;k++){undo_metatileSetLabels_2x2.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_2x2;k++){checkpoint_metatileSetLabels_2x2.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_2x2;k++){undo_checkpoint_metatileSetLabels_2x2.push_back("Unlabeled");}

		metatileSetLabels_4x4.clear();
		undo_metatileSetLabels_4x4.clear();
		checkpoint_metatileSetLabels_4x4.clear();
		undo_checkpoint_metatileSetLabels_4x4.clear();

		for( int k=0;k<metatileSets_4x4;k++){metatileSetLabels_4x4.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_4x4;k++){undo_metatileSetLabels_4x4.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_4x4;k++){checkpoint_metatileSetLabels_4x4.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_4x4;k++){undo_checkpoint_metatileSetLabels_4x4.push_back("Unlabeled");}

		metatileSetLabels_8x8.clear();
		undo_metatileSetLabels_8x8.clear();
		checkpoint_metatileSetLabels_8x8.clear();
		undo_checkpoint_metatileSetLabels_8x8.clear();

		for( int k=0;k<metatileSets_8x8;k++){metatileSetLabels_8x8.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_8x8;k++){undo_metatileSetLabels_8x8.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_8x8;k++){checkpoint_metatileSetLabels_8x8.push_back("Unlabeled");}
		for( int k=0;k<metatileSets_8x8;k++){undo_checkpoint_metatileSetLabels_8x8.push_back("Unlabeled");}


	}

	else{


		//metatile stuff pt1 - normal
		nss_get_bytes(text,size,"MetatileSet_2x2_id=",metatileSet_2x2_id, metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"MetatileSet_2x2_idUndo=",undo_metatileSet_2x2_id, undo_metatileSets_2x2*META_2x2);

		nss_get_bytes(text,size,"MetatileSet_2x2_pal=", metatileSet_2x2_pal, metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"MetatileSet_2x2_palUndo=", undo_metatileSet_2x2_pal, undo_metatileSets_2x2*META_2x2);

		nss_get_bytes(text,size,"MetatileSet_2x2_props=", metatileSet_2x2_props, metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"MetatileSet_2x2_propsUndo=", undo_metatileSet_2x2_props, undo_metatileSets_2x2*META_2x2);


		nss_get_bytes(text,size,"MetatileSet_4x4_id=",metatileSet_4x4_id, metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"MetatileSet_4x4_idUndo=",undo_metatileSet_4x4_id, undo_metatileSets_4x4*META_4x4);

		nss_get_bytes(text,size,"MetatileSet_4x4_pal=", metatileSet_4x4_pal, metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"MetatileSet_4x4_palUndo=", undo_metatileSet_4x4_pal, undo_metatileSets_4x4*META_4x4);

		nss_get_bytes(text,size,"MetatileSet_4x4_props=", metatileSet_4x4_props, metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"MetatileSet_4x4_propsUndo=", undo_metatileSet_4x4_props, undo_metatileSets_4x4*META_4x4);


		nss_get_bytes(text,size,"MetatileSet_8x8_id=",metatileSet_8x8_id, metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"MetatileSet_8x8_idUndo=",undo_metatileSet_8x8_id, undo_metatileSets_8x8*META_8x8);

		nss_get_bytes(text,size,"MetatileSet_8x8_pal=", metatileSet_8x8_pal, metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"MetatileSet_8x8_palUndo=", undo_metatileSet_8x8_pal, undo_metatileSets_8x8*META_8x8);

		nss_get_bytes(text,size,"MetatileSet_8x8_props=", metatileSet_8x8_props, metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"MetatileSet_8x8_propsUndo=", undo_metatileSet_8x8_props, undo_metatileSets_8x8*META_8x8);


		//metatile stuff pt2 - checkpoints
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_2x2_id=",checkpoint_metatileSet_2x2_id, checkpoint_metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_2x2_idUndo=",undo_checkpoint_metatileSet_2x2_id, undo_checkpoint_metatileSets_2x2*META_2x2);

		nss_get_bytes(text,size,"Checkpoint_MetatileSet_2x2_pal=", checkpoint_metatileSet_2x2_pal, checkpoint_metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_2x2_palUndo=", undo_checkpoint_metatileSet_2x2_pal, undo_checkpoint_metatileSets_2x2*META_2x2);

		nss_get_bytes(text,size,"Checkpoint_MetatileSet_2x2_props=", checkpoint_metatileSet_2x2_props, checkpoint_metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_2x2_propsUndo=", undo_checkpoint_metatileSet_2x2_props, undo_checkpoint_metatileSets_2x2*META_2x2);


		nss_get_bytes(text,size,"Checkpoint_MetatileSet_4x4_id=",checkpoint_metatileSet_4x4_id, checkpoint_metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_4x4_idUndo=",undo_checkpoint_metatileSet_4x4_id, undo_checkpoint_metatileSets_4x4*META_4x4);

		nss_get_bytes(text,size,"Checkpoint_MetatileSet_4x4_pal=", checkpoint_metatileSet_4x4_pal, checkpoint_metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_4x4_palUndo=", undo_checkpoint_metatileSet_4x4_pal, undo_checkpoint_metatileSets_4x4*META_4x4);

		nss_get_bytes(text,size,"Checkpoint_MetatileSet_4x4_props=", checkpoint_metatileSet_4x4_props, checkpoint_metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_4x4_propsUndo=", undo_checkpoint_metatileSet_4x4_props, undo_checkpoint_metatileSets_4x4*META_4x4);


		nss_get_bytes(text,size,"Checkpoint_MetatileSet_8x8_id=",checkpoint_metatileSet_8x8_id, checkpoint_metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_8x8_idUndo=",undo_checkpoint_metatileSet_8x8_id, undo_checkpoint_metatileSets_8x8*META_8x8);

		nss_get_bytes(text,size,"Checkpoint_MetatileSet_8x8_pal=", checkpoint_metatileSet_8x8_pal, checkpoint_metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_8x8_palUndo=", undo_checkpoint_metatileSet_8x8_pal, undo_checkpoint_metatileSets_8x8*META_8x8);

		nss_get_bytes(text,size,"Checkpoint_MetatileSet_8x8_props=", checkpoint_metatileSet_8x8_props, checkpoint_metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"Checkpoint_MetatileSet_8x8_propsUndo=", undo_checkpoint_metatileSet_8x8_props, undo_checkpoint_metatileSets_8x8*META_8x8);


		//make extra sure these tags exist, getting the vectors right is crucial to avoid errors.

		if(CheckTag(text,size, "MetatileSetLabels_2x2=")) metatileSetLabels_2x2 = deserializeVector(text, "MetatileSetLabels_2x2=");
		else {
			metatileSetLabels_2x2.clear();
			for( int k=0;k<metatileSets_2x2;k++){metatileSetLabels_2x2.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "MetatileSetLabels_2x2Undo=")) undo_metatileSetLabels_2x2 = deserializeVector(text, "MetatileSetLabels_2x2Undo=");
		else{
			undo_metatileSetLabels_2x2.clear();
			for( int k=0;k<undo_metatileSets_2x2;k++){undo_metatileSetLabels_2x2.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "Checkpoint_MetatileSetLabels_2x2=")) checkpoint_metatileSetLabels_2x2 = deserializeVector(text, "Checkpoint_MetatileSetLabels_2x2=");
		else{
			checkpoint_metatileSetLabels_2x2.clear();
			for( int k=0;k<checkpoint_metatileSets_2x2;k++){checkpoint_metatileSetLabels_2x2.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "Checkpoint_MetatileSetLabels_2x2Undo=")) undo_checkpoint_metatileSetLabels_2x2 = deserializeVector(text, "Checkpoint_MetatileSetLabels_2x2Undo=");
		else{
			undo_checkpoint_metatileSetLabels_2x2.clear();
			for( int k=0;k<undo_checkpoint_metatileSets_2x2;k++){undo_checkpoint_metatileSetLabels_2x2.push_back("Unlabeled");}
		}


		if(CheckTag(text,size, "MetatileSetLabels_4x4=")) metatileSetLabels_4x4 = deserializeVector(text, "MetatileSetLabels_4x4=");
		else {
			metatileSetLabels_4x4.clear();
			for( int k=0;k<metatileSets_4x4;k++){metatileSetLabels_4x4.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "MetatileSetLabels_4x4Undo=")) undo_metatileSetLabels_4x4 = deserializeVector(text, "MetatileSetLabels_4x4Undo=");
		else{
			undo_metatileSetLabels_4x4.clear();
			for( int k=0;k<undo_metatileSets_4x4;k++){undo_metatileSetLabels_4x4.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "Checkpoint_MetatileSetLabels_4x4=")) checkpoint_metatileSetLabels_4x4 = deserializeVector(text, "Checkpoint_MetatileSetLabels_4x4=");
		else{
			checkpoint_metatileSetLabels_4x4.clear();
			for( int k=0;k<checkpoint_metatileSets_4x4;k++){checkpoint_metatileSetLabels_4x4.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "Checkpoint_MetatileSetLabels_4x4Undo=")) undo_checkpoint_metatileSetLabels_4x4 = deserializeVector(text, "Checkpoint_MetatileSetLabels_4x4Undo=");
		else{
			undo_checkpoint_metatileSetLabels_4x4.clear();
			for( int k=0;k<undo_checkpoint_metatileSets_4x4;k++){undo_checkpoint_metatileSetLabels_4x4.push_back("Unlabeled");}
		}



		if(CheckTag(text,size, "MetatileSetLabels_8x8=")) metatileSetLabels_8x8 = deserializeVector(text, "MetatileSetLabels_8x8=");
		else {
			metatileSetLabels_8x8.clear();
			for( int k=0;k<metatileSets_8x8;k++){metatileSetLabels_8x8.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "MetatileSetLabels_8x8Undo=")) undo_metatileSetLabels_8x8 = deserializeVector(text, "MetatileSetLabels_8x8Undo=");
		else{
			undo_metatileSetLabels_8x8.clear();
			for( int k=0;k<undo_metatileSets_8x8;k++){undo_metatileSetLabels_8x8.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "Checkpoint_MetatileSetLabels_8x8=")) checkpoint_metatileSetLabels_8x8 = deserializeVector(text, "Checkpoint_MetatileSetLabels_8x8=");
		else{
			checkpoint_metatileSetLabels_8x8.clear();
			for( int k=0;k<checkpoint_metatileSets_8x8;k++){checkpoint_metatileSetLabels_8x8.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "Checkpoint_MetatileSetLabels_8x8Undo=")) undo_checkpoint_metatileSetLabels_8x8 = deserializeVector(text, "Checkpoint_MetatileSetLabels_8x8Undo=");
		else{
			undo_checkpoint_metatileSetLabels_8x8.clear();
			for( int k=0;k<undo_checkpoint_metatileSets_8x8;k++){undo_checkpoint_metatileSetLabels_8x8.push_back("Unlabeled");}
		}

		

	}


	tmp_metatileSetLabels_2x2.clear();
	tmp_metatileSetLabels_4x4.clear();
	tmp_metatileSetLabels_8x8.clear();


	//get collision labels   - used by bitbutton readouts
	for (i = 0; i <8; i++) {
		char label[20];
		sprintf(label, "propertyCHRlabel%i=", i);
		propCHRlabel[i] = nss_get_str(text,size,label);
		if(propCHRlabel[i]=="")
			{
				propCHRlabel[i]="Label"+IntToStr(i);
			}
	}
	//get collisionID labels -used by 0-255 ID readouts.
	for (i = 0; i <256; i++) {
		char label[28];
		sprintf(label, "collisionIDlabel%i=", i);
		collisionIDlabel[i] = nss_get_str(text,size,label);

		sprintf(label, "collisionID_R%i=", i);
		if(CheckTag(text,size,label)) collisionID_R[i] = (char)nss_get_int(text,size,label);
		else collisionID_R[i]=0xff;

		sprintf(label, "collisionID_G%i=", i);
		if(CheckTag(text,size,label)) collisionID_G[i] = (char)nss_get_int(text,size,label);
		else collisionID_G[i]=0xff;

		sprintf(label, "collisionID_B%i=", i);
		if(CheckTag(text,size,label)) collisionID_B[i] = (char)nss_get_int(text,size,label);
		else collisionID_B[i]=0xff;

		}



	//BROKE STUDIO
	for(i=0;i<256;i++)
	{
		if(metaSprites[i*64*4]<255)
		{
			char label[14];
			sprintf(label, "MetaSprite%i=", i);
			metaSpriteNames[i] = nss_get_str(text,size,label);
			if(metaSpriteNames[i]=="")
			{
				metaSpriteNames[i]="Metasprite"+IntToStr(i);

			}
		}
	}
	metaSpriteBankName=nss_get_str(text,size,"MetaSpriteBankName=");
	if(metaSpriteBankName=="")
	{
		metaSpriteBankName="spr";
	}


	//brush masks
	if(CheckTag(text,size,"iBrushSize=")){
		nss_get_int_from_bytes(text,size,"iBrushSize=",iBrushSize,14);
	}
	else {
		for(i=0;i<14;i++){
			int tmpi=(i%7)+2;
			iBrushSize[i]=tmpi;
		}
		iBrushSize[8]=16;
	}
	if(CheckTag(text,size,"iBrushSnapSize_x=")){
		nss_get_int_from_bytes(text,size,"iBrushSnapSize_x=",iBrushSnapSize_x,14);
	}
	else {
		for(i=0;i<14;i++){
			int tmpi=(i%7)+2;
			iBrushSnapSize_x[i]=tmpi;
		}
		iBrushSnapSize_x[8]=8;
	}
	if(CheckTag(text,size,"iBrushSnapSize_y=")){
		nss_get_int_from_bytes(text,size,"iBrushSnapSize_y=",iBrushSnapSize_y,14);
	}
	else {
		for(i=0;i<14;i++){
			int tmpi=(i%7)+2;
			iBrushSnapSize_y[i]=tmpi;
		}
		iBrushSnapSize_y[8]=8;
	}
	if(CheckTag(text,size,"iBrushCursorAlignment=")){
		nss_get_int_from_bytes(text,size,"iBrushCursorAlignment=",iBrushCursorAlignment,14);
	}
	else {
		for(i=0;i<14;i++){
			iBrushCursorAlignment[i]=4;
		}
	}

	AnsiString tmpBrushString;
	AnsiString tmpBrushSet;
	AnsiString tmpBrushID;
	for(i=0;i<14;i++){
		tmpBrushSet= i<7? "a":"b";
		tmpBrushID = IntToStr((i%7)+2);
		tmpBrushString="BrushMask_"+tmpBrushID+tmpBrushSet+"=";
		if(CheckTag(text,size,tmpBrushString.c_str()))
			nss_get_int_from_bytes(text,size,tmpBrushString.c_str(),&(*ptr_tableBrush[i])[0][0],256);
	}


	//save dialogs settings

	SaveDialogChr ->FilterIndex=nss_get_int(text,size,"FilterCHR=");
	SaveDialogName->FilterIndex=nss_get_int(text,size,"FilterName=");

	SaveDialogChr ->FileName=nss_get_str(text,size,"FileNameCHR=");
	SaveDialogName->FileName=nss_get_str(text,size,"FileNameName=");
	SaveDialogPal ->FileName=nss_get_str(text,size,"FileNamePal=");
	SaveDialogMetaSpriteBank->FileName=nss_get_str(text,size,"FileNameMetaSpriteBank=");
	SaveDialogTprop->FileName=nss_get_str(text,size,"FileNameTileProps=");
    SaveDialogMetatiles->FileName=nss_get_str(text,size,"FileNameMetatileText=");

	if(CheckTag(text,size,"FileNameAssociatedMetatiles="))
		{ansiFileNameAssociatedMetatiles=nss_get_str(text,size,"FileNameAssociatedMetatiles=");}
	else  {ansiFileNameAssociatedMetatiles="";}
	if(CheckTag(text,size,"FileNameAssociatedTileset="))
		{ansiFileNameAssociatedTileset=nss_get_str(text,size,"FileNameAssociatedTileset=");}
	else  {ansiFileNameAssociatedTileset="";}
	if(CheckTag(text,size,"CheckboxDoAssociateMetatiles="))
		{bAssociateMetatiles=nss_get_bool(text,size,"CheckboxDoAssociateMetatiles=");}
	else {bAssociateMetatiles=false;}
	if(CheckTag(text,size,"CheckboxDoAssociateTileset="))
		{bAssociateTileset=nss_get_bool(text,size,"CheckboxDoAssociateTileset=");}
	else {bAssociateTileset=false;}


	if(CheckTag(text,size,"RadioAutoexport_UseWorkDir="))
		{bAutoExportRadio1=nss_get_bool(text,size,"RadioAutoexport_UseWorkDir=");}
	else {bAutoExportRadio1=false;}

	if(CheckTag(text,size,"RadioAutoexport_UseSubDir_Project="))
		{bAutoExportRadio2=nss_get_bool(text,size,"RadioAutoexport_UseSubDir_Project=");}
	else {bAutoExportRadio2=false;}

	if(CheckTag(text,size,"RadioAutoexport_UseSubDir_NEXXT="))
		{bAutoExportRadio3=nss_get_bool(text,size,"RadioAutoexport_UseSubDir_NEXXT=");}
	else {bAutoExportRadio3=false;}

	if(CheckTag(text,size,"CheckAutoexport_MetatileAsBMP="))
		{bAutoExport_metatiles=nss_get_bool(text,size,"CheckAutoexport_MetatileAsBMP=");}
	else {bAutoExport_metatiles=false;}



	//handle linked transfer.
	if(bLinkedCHRmode)
	{

		if(EnterLinkedCHRmode(ansiFileNameAssociatedTileset))
		{

		}
		else
		{
			Application->MessageBox("Linked chr mode of this file has no valid path and will be turned off.\nCheck path to/existence of linked file","Warning: Linked CHR mode",MB_OK);
			bLinkedCHRmode=false;
		}
		//rest is handled by pipeline helper form update.
	}


	//final preparations
	SpeedButtonPalBankB->Down=false; MPalB->Checked=false;
	SpeedButtonPalBankC->Down=false; MPalC->Checked=false;
	SpeedButtonPalBankD->Down=false; MPalD->Checked=false;
	SpeedButtonPalBankA->Down=false; MPalA->Checked=false;

	if(palBank==1) 		{SpeedButtonPalBankB->Down=true; MPalB->Checked=true;}
	else if(palBank==2) {SpeedButtonPalBankC->Down=true; MPalC->Checked=true;}
	else if(palBank==3) {SpeedButtonPalBankD->Down=true; MPalD->Checked=true;}
	else 		   		{SpeedButtonPalBankA->Down=true; MPalA->Checked=true;}
	if (ppuMask!=ppuMaskSet[palBank])
	//if they differ, this means we've loaded an old nss. NEXXT proceeds to clone the entry to reconstruct the behaviour from old versions.
	{
		for (int i = 0; i <= 3; i++) ppuMaskSet[i]=ppuMask;
	}
	ppuMask=ppuMaskSet[palBank];
    AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
    TabSheetTile->Caption=tmp_LinkCHRstr+"[F3] Tiles";
	if(PageControlEditor->ActivePage==TabSheetTile){

		GroupBoxTiles->Caption="Screen (view only)";
	}
	else
	{

		GroupBoxTiles->Caption=tmp_LinkCHRstr+"Tileset";
		if(PageControlEditor->ActivePage==TabSheetSprite)
		{
			if(GroupBoxSpriteList->Visible) ActiveControl=ListBoxSpriteList;
			Fill1->Enabled=false;
		}
		if(PageControlEditor->ActivePage==TabSheetName)
		{
			Fill1->Enabled=true;
		}
	}
	if(uiScale>1 && PageControlEditor->ActivePage==TabSheetTile)
	{	GroupBoxTileControl->Visible=true;}
	else
	{	GroupBoxTileControl->Visible=false;	}




	LabelStats->Caption="Done.";
    StatusUpdateWaiter->Enabled=true;
	holdStats=true;

	return true;
}



bool __fastcall TFormMain::LoadSession(AnsiString filename)
{
	FILE *file;
	unsigned char temp[16];
	int i,j,ver;
	bool r;

	file=fopen(filename.c_str(),"rb");

	if(!file) return false;

	fread(temp,sizeof(sessionIDStr),1,file);
	fclose(file);

	ver=0;

	if(!memcmp(temp,"NESSTses",8)) ver=1;//load legacy binary format
	if(!memcmp(temp,"NSTses00",8)) ver=2;//load old binary format
	if(!memcmp(temp,sessionIDStr,8)) ver=3;//current text format

	switch(ver)
	{
	case 1: r=LoadSession1x(filename); break;
	case 2: r=LoadSession2x(filename); break;
	case 3: r=LoadSessionText(filename); break;
	}

	if(r)
	{
		SaveDialogSession->FileName=filename;

		nameXC=-1;
		nameYC=-1;
		tileXC=-1;
		tileYC=-1;
		palHover=-1;
		palColHover=-1;
		colHover=-1;

		UpdateRGBM();
		palette_calc();
		DrawPalettes();
		UpdateTiles(true);
        //WorkCHRToBankCHR();
		UpdateNameTable(-1,-1,true);
		UpdateExternalForms->Enabled=true;
		//if (FormNavigator->Visible) FormNavigator->Draw(true,true);

		return true;
	}

	Application->MessageBox("Unknown or corrupted session data format","Error",MB_OK);

	return false;
}



void __fastcall TFormMain::SaveSession(AnsiString filename)
{
	FILE *file;
	int i,j;
	unsigned char pal[16];

	file=fopen(filename.c_str(),"wt");

	if(!file)
	{
		Application->MessageBox("Can't save session","Error",MB_OK);
		return;
	}

	//signature

	fprintf(file,"%s\n\n",sessionIDStr);

	//screen buttons state

	fprintf(file,"BtnTiles=%i\n"        ,SpeedButtonTiles        ->Down?1:0);
	fprintf(file,"BtnChecker=%i\n"      ,SpeedButtonChecker      ->Down?1:0);
	fprintf(file,"BtnSelTiles=%i\n"     ,SpeedButtonSelTiles     ->Down?1:0);
	fprintf(file,"BtnChrBank1=%i\n"     ,SpeedButtonChrBank1     ->Down?1:0);
	fprintf(file,"BtnChrBank2=%i\n"     ,SpeedButtonChrBank2     ->Down?1:0);
	fprintf(file,"BtnGridAll=%i\n"      ,SpeedButtonGridAll      ->Down?1:0);
	fprintf(file,"BtnGridTile=%i\n"     ,SpeedButtonGridTile     ->Down?1:0);
	fprintf(file,"BtnGridAtr=%i\n"      ,SpeedButtonGridAtr      ->Down?1:0);
	fprintf(file,"BtnGridBlock=%i\n"    ,SpeedButtonGridBlock    ->Down?1:0);

	fprintf(file,"BtnGridScr=%i\n"    ,SpeedButtonGridScreen    ->Down?1:0);

	fprintf(file,"BtnSelectedOnlyInverted=%i\n"    ,btnSelectedOnlyInverted->Down?1:0);
	fprintf(file,"BtnSolidityMask=%i\n"    ,btnMask				->Down?1:0);

	fprintf(file,"BtnPal=%i\n"          ,SpeedButtonPal          ->Down?1:0);
	fprintf(file,"BtnTypeIn=%i\n"       ,SpeedButtonTypeIn       ->Down?1:0);
	fprintf(file,"BtnFrameAll=%i\n"     ,SpeedButtonFrameAll     ->Down?1:0);
	fprintf(file,"BtnFrameSelected=%i\n",SpeedButtonFrameSelected->Down?1:0);
	fprintf(file,"BtnFrameNone=%i\n"    ,SpeedButtonFrameNone    ->Down?1:0);
	fprintf(file,"BtnSpriteSnap=%i\n"   ,SpeedButtonSpriteSnap   ->Down?1:0);
	fprintf(file,"BtnSprite8x16=%i\n"   ,SpeedButtonSprite8x16   ->Down?1:0);
	fprintf(file,"BtnForce8x16=%i\n"	,ButtonForceView         ->Down?1:0);

	fprintf(file,"MenuBestOffsets=%i\n"    ,MImportBestOffsets->Checked?1:0);
	fprintf(file,"MenuLossy=%i\n"          ,MImportLossy->Checked?1:0);
	fprintf(file,"MenuThreshold=%i\n"      ,MImportThreshold->Checked?1:0);
	fprintf(file,"MenuNoColorData=%i\n"    ,MImportNoColorData->Checked?1:0);
	fprintf(file,"MenuMetaSprAutoInc=%i\n" ,MPutSelectionToMetaspriteAutoInc->Checked?1:0);
	fprintf(file,"MenuMetaSprSkipZero=%i\n",MPutSelectionToMetaspriteSkipZero->Checked?1:0);
	fprintf(file,"MenuMetaSprMerge=%i\n"   ,MPutSelectionToMetaspriteMerge->Checked?1:0);
	fprintf(file,"MenuSaveIncName=%i\n"    ,MSaveIncName->Checked?1:0);
	fprintf(file,"MenuSaveIncAttr=%i\n"    ,MSaveIncAttr->Checked?1:0);
	fprintf(file,"MenuSaveRLE=%i\n"        ,MSaveRLE->Checked?1:0);


	fprintf(file,"CheckLinkedCHRmode=%i\n"	,bLinkedCHRmode?1:0);

	fprintf(file,"CheckMetatileEditor_ReserveFirst=%i\n"	,FormMetatileTool->chkReserve1st->Checked?1:0);
	fprintf(file,"CheckMetatileEditor_AlignToScreen=%i\n"	,FormMetatileTool->chkAlignScr->Checked?1:0);

	fprintf(file,"BtnMetatileEditor_UseAttributes=%i\n"	,FormMetatileTool->btnUseAttr->Down?1:0);
	fprintf(file,"BtnMetatileEditor_Clonestamp=%i\n"	,FormMetatileTool->btnClonestamp->Down?1:0);
	fprintf(file,"BtnMetatileEditor_CloneSnap=%i\n"		,FormMetatileTool->btnCloneSnap->Down?1:0);
	fprintf(file,"BtnMetatileEditor_Tiles=%i\n"			,FormMetatileTool->btnTiles->Down?1:0);
	fprintf(file,"BtnMetatileEditor_Attributes=%i\n"	,FormMetatileTool->btnAttr->Down?1:0);
	fprintf(file,"BtnMetatileEditor_Properties=%i\n"	,FormMetatileTool->btnProps->Down?1:0);
	fprintf(file,"BtnMetatileEditor_Map=%i\n"			,FormMetatileTool->btnMap->Down?1:0);

	fprintf(file,"ChkSwapBanks_Properties=%i\n"	,FormSwapBanks->chkInclProps->Checked?1:0);
	fprintf(file,"ChkSwapBanks_Labels=%i\n"			,FormSwapBanks->chkInclLabel->Checked?1:0);




	//variables
	fprintf(file,"VarTileViewTagBuf=%i\n"   ,tileViewTagBuf);

	fprintf(file,"VarBgPalCur=%i\n"  ,bgPalCur);
	fprintf(file,"VarPalActive=%i\n" ,palActive);
	fprintf(file,"VarTileActive=%i\n",tileActive);
	fprintf(file,"VarBankActive=%i\n",bankActive);
	fprintf(file,"VarPPUMask=%i\n"   ,ppuMask);

	fprintf(file,"VarPPUMaskSet0=%i\n"   ,ppuMaskSet[0]);
	fprintf(file,"VarPPUMaskSet1=%i\n"   ,ppuMaskSet[1]);
	fprintf(file,"VarPPUMaskSet2=%i\n"   ,ppuMaskSet[2]);
	fprintf(file,"VarPPUMaskSet3=%i\n"   ,ppuMaskSet[3]);

	fprintf(file,"VarPalBank=%i\n"   ,palBank);

	fprintf(file,"VarMetaSpriteActive=%i\n",metaSpriteActive);
	fprintf(file,"VarSpriteActive=%i\n"    ,spriteActive);
	fprintf(file,"VarSpriteGridX=%i\n"     ,spriteGridX);
	fprintf(file,"VarSpriteGridY=%i\n"     ,spriteGridY);

	fprintf(file,"VarNameW=%i\n"         ,nameTableWidth);
	fprintf(file,"VarNameH=%i\n"         ,nameTableHeight);
	fprintf(file,"VarNameViewX=%i\n"     ,nameTableViewX);
	fprintf(file,"VarNameViewY=%i\n"     ,nameTableViewY);
	fprintf(file,"VarNameSelectionL=%i\n",nameSelection.Left);
	fprintf(file,"VarNameSelectionR=%i\n",nameSelection.Right);
	fprintf(file,"VarNameSelectionT=%i\n",nameSelection.Top);
	fprintf(file,"VarNameSelectionB=%i\n",nameSelection.Bottom);
	fprintf(file,"VarNameCopyW=%i\n"     ,nameCopyWidth);
	fprintf(file,"VarNameCopyH=%i\n"     ,nameCopyHeight);

	fprintf(file,"VarCHRSelectionL=%i\n",chrSelection.Left);
	fprintf(file,"VarCHRSelectionR=%i\n",chrSelection.Right);
	fprintf(file,"VarCHRSelectionT=%i\n",chrSelection.Top);
	fprintf(file,"VarCHRSelectionB=%i\n",chrSelection.Bottom);
	fprintf(file,"VarCHRCopyW=%i\n"     ,chrCopyWidth);
	fprintf(file,"VarCHRCopyH=%i\n"     ,chrCopyHeight);
	fprintf(file,"VarCHRCopyRect=%i\n"  ,chrCopyRect?1:0);
	fprintf(file,"VarCHRSelectRect=%i\n",chrSelectRect?1:0);

	fprintf(file,"VarCHRBanks=%i\n"     			,chrBanks);
	fprintf(file,"VarCHRBanksUndo=%i\n"     		,undo_chrBanks);
	fprintf(file,"VarCheckpoint_CHRBanks=%i\n"     	,checkpoint_chrBanks);
	fprintf(file,"VarCheckpoint_CHRBanksUndo=%i\n"  ,undo_checkpoint_chrBanks);

	fprintf(file,"VarMtClickID=%i\n", mtClickID);
	fprintf(file,"VarMtClickID_store_2x2=%i\n", mtClickID_store_2x2);
	fprintf(file,"VarMtClickID_store_4x4=%i\n", mtClickID_store_4x4);
	fprintf(file,"VarMtClickID_store_8x8=%i\n", mtClickID_store_8x8);

	fprintf(file,"VarMetatileSets_2x2=%i\n", metatileSets_2x2);
	fprintf(file,"VarUndo_metatileSets_2x2=%i\n", undo_metatileSets_2x2);
	fprintf(file,"VarCheckpoint_metatileSets_2x2=%i\n", checkpoint_metatileSets_2x2);
	fprintf(file,"VarUndo_checkpoint_metatileSets_2x2=%i\n", undo_checkpoint_metatileSets_2x2);

	fprintf(file,"VarMetatileSets_4x4=%i\n", metatileSets_4x4);
	fprintf(file,"VarUndo_metatileSets_4x4=%i\n", undo_metatileSets_4x4);
	fprintf(file,"VarCheckpoint_metatileSets_4x4=%i\n", checkpoint_metatileSets_4x4);
	fprintf(file,"VarUndo_checkpoint_metatileSets_4x4=%i\n", undo_checkpoint_metatileSets_4x4);

	fprintf(file,"VarMetatileSets_8x8=%i\n", metatileSets_8x8);
	fprintf(file,"VarUndo_metatileSets_8x8=%i\n", undo_metatileSets_8x8);
	fprintf(file,"VarCheckpoint_metatileSets_8x8=%i\n", checkpoint_metatileSets_8x8);
	fprintf(file,"VarUndo_checkpoint_metatileSets_8x8=%i\n", undo_checkpoint_metatileSets_8x8);

	fprintf(file,"VarCustomScanlineWarningCyan=%i\n",iWarnMsprCyan);
	fprintf(file,"VarBrushPresetIndex=%i\n", iBrushPresetIndex);


	for(i=0;i<8;i++)
	{
		fprintf(file,"BtnStateMtProps%i=%i\n",i,btnStateMtProps[i]);
	}

	for(i=0;i<8;i++)
	{
		fprintf(file,"BtnStateCHRProps%i=%i\n",i,btnStateProps[i]);
	}

	for(i=0;i<8;i++)
	{
		fprintf(file,"propertyCHRlabel%i=%s\n",i,propCHRlabel[i]);
	}
    fprintf(file,"\n");
	//Collision ID labels
	for(i=0;i<256;i++)
	{
		if(collisionIDlabel[i]!="")
		//if(!collisionIDlabel[i].empty())
		{
			fprintf(file,"collisionIDlabel%i=%s\n",i,collisionIDlabel[i].c_str());
		}


		if(collisionID_R[i]!=0xff)
			fprintf(file,"collisionID_R%i=%i\t",i,collisionID_R[i]);
		if(collisionID_G[i]!=0xff)
			fprintf(file,"collisionID_G%i=%i\t",i,collisionID_G[i]);
		if(collisionID_B[i]!=0xff)
			fprintf(file,"collisionID_B%i=%i\n",i,collisionID_B[i]);

	}
	fprintf(file,"\n");

	//BROKE STUDIO
	for(i=0;i<256;i++)
	{
		if(metaSprites[i*64*4]<255)
		{
			fprintf(file,"MetaSprite%i=%s\n",i,metaSpriteNames[i]);
		}
	}
    fprintf(file,"\n");
	fprintf(file,"MetaSpriteBankName=%s\n",metaSpriteBankName);
    
	//save dialogs settings

	fprintf(file,"\nFilterCHR=%i\n" ,SaveDialogChr ->FilterIndex);
	fprintf(file,"FilterName=%i\n",SaveDialogName->FilterIndex);

	fprintf(file,"FileNameCHR=%s\n"           ,SaveDialogChr           ->FileName.c_str());
	fprintf(file,"FileNameName=%s\n"          ,SaveDialogName          ->FileName.c_str());
	fprintf(file,"FileNamePal=%s\n"           ,SaveDialogPal           ->FileName.c_str());
	fprintf(file,"FileNameMetaSpriteBank=%s\n",SaveDialogMetaSpriteBank->FileName.c_str());
	fprintf(file,"FileNameTileProps=%s\n"	  ,SaveDialogTprop->FileName.c_str());
	fprintf(file,"FileNameMetatileText=%s\n"  ,SaveDialogMetatiles->FileName.c_str());


	fprintf(file,"FileNameAssociatedMetatiles=%s\n",ansiFileNameAssociatedMetatiles.c_str());
	fprintf(file,"FileNameAssociatedTileset=%s\n"	  ,ansiFileNameAssociatedTileset.c_str());
	fprintf(file,"CheckboxDoAssociateMetatiles=%i\n"        ,bAssociateMetatiles?1:0);
	fprintf(file,"CheckboxDoAssociateTileset=%i\n"      ,bAssociateTileset?1:0);

    fprintf(file,"RadioAutoexport_UseWorkDir=%i\n"			,FormPipelineHelper->RadioButton1->Checked?1:0);
	fprintf(file,"RadioAutoexport_UseSubDir_Project=%i\n"	,FormPipelineHelper->RadioButton2->Checked?1:0);
	fprintf(file,"RadioAutoexport_UseSubDir_NEXXT=%i\n"		,FormPipelineHelper->RadioButton3->Checked?1:0);


	fprintf(file,"CheckAutoexport_MetatileAsBMP=%i\n"			,FormPipelineHelper->chkAutoExpMetatilesBMP->Checked?1:0);




	//arrays
	nss_put_bytes(file,"\n\nVarCHRSelected=",chrSelected,sizeof(chrSelected));

	nss_put_bytes(file,"\n\nPalette="  ,bgPal        ,sizeof(bgPal));
	nss_put_bytes(file,"\n\nPalUndo="  ,bgPalUndo    ,sizeof(bgPalUndo));

	if(bLinkedCHRmode){  //if true, the real chr is in LinkBuf, so fetch it from there.
		nss_put_bytes(file,"\n\nCHRMain="  ,chr_LinkBuf          ,chrBanks_LinkBuf*4096);
		nss_put_bytes(file,"\n\nCHRUndo="  ,undoChr_LinkBuf      ,undo_chrBanks_LinkBuf*4096);

	}
	else{        //normal
		nss_put_bytes(file,"\n\nCHRMain="  ,chr          ,chrBanks*4096);
		nss_put_bytes(file,"\n\nCHRUndo="  ,undoChr      ,undo_chrBanks*4096);
	}

	nss_put_bytes(file,"\n\nCHRCopy="  ,chrCopy      ,sizeof(chrCopy));

	nss_put_bytes(file,"\n\nNameTable=",nameTable    ,name_size());
	nss_put_bytes(file,"\n\nNameCopy=" ,nameCopy     ,name_size());
	nss_put_bytes(file,"\n\nNameUndo=" ,undoNameTable,name_size());

	nss_put_bytes(file,"\n\nAttrTable=",attrTable    ,attr_size());
	nss_put_bytes(file,"\n\nAttrCopy=" ,attrCopy     ,attr_size());
	nss_put_bytes(file,"\n\nAttrUndo=" ,undoAttrTable,attr_size());

	nss_put_bytes(file,"\n\nMetaSprites=",metaSprites,sizeof(metaSprites));

	nss_put_bytes_from_int(file,"\n\niBrushSize=" ,iBrushSize,sizeof(14));
	nss_put_bytes_from_int(file,"\n\niBrushSnapSize_x=" ,iBrushSnapSize_x,sizeof(14));
	nss_put_bytes_from_int(file,"\n\niBrushSnapSize_y=" ,iBrushSnapSize_y,sizeof(14));
	nss_put_bytes_from_int(file,"\n\niBrushCursorAlignment=" ,iBrushCursorAlignment,sizeof(14));


	AnsiString tmpBrushString;
	AnsiString tmpBrushSet;
	AnsiString tmpBrushID;
	for(i=0;i<14;i++){
		tmpBrushSet= i<7? "a":"b";
		tmpBrushID = IntToStr((i%7)+2);
		tmpBrushString="\n\nBrushMask_"+tmpBrushID+tmpBrushSet+"=";
		nss_put_bytes_from_int(file,tmpBrushString.c_str(),&(*ptr_tableBrush[i])[0][0],256);
	}

	nss_put_uint32s(file,"\n\n32_BankViewTable=",bankViewTable,sizeof(bankViewTable));
	nss_put_uint32s(file,"\n\n32_BankViewTableUndo=",undo_bankViewTable,sizeof(undo_bankViewTable));

	/*
	nss_put_uint32s(file,"\n\n32_TileCollisionProperties=",tileProperties32,sizeof(tileProperties));
	nss_put_uint32s(file,"\n\n32_TileCollisionPropertiesUndo=",tilePropertiesUndo32,sizeof(tilePropertiesUndo));
	*/

	nss_put_bytes(file,"\n\n8_TileCollisionProperties=",tileProperties,chrBanks*256);
	nss_put_bytes(file,"\n\n8_TileCollisionPropertiesUndo=",tilePropertiesUndo,undo_chrBanks*256);

	nss_put_2D_bools(file,"\n\nPropertyConditions=",propCondition,4,8);
	nss_put_2D_bools(file,"\n\nPropertyConditionsUndo=",propConditionUndo,4,8);

	nss_put_bytes(file,"\n\nTilePropConditionType=",propConditional,sizeof(propConditional));
	nss_put_bytes(file,"\n\nTilePropConditionTypeUndo=",propConditionalUndo,sizeof(propConditional));

	//metatile stuff pt1
	nss_put_bytes(file,"\n\nMetatileSet_2x2_id=",metatileSet_2x2_id, metatileSets_2x2*META_2x2);
	nss_put_bytes(file,"\n\nMetatileSet_2x2_idUndo=",undo_metatileSet_2x2_id, undo_metatileSets_2x2*META_2x2);

	nss_put_bytes(file,"\n\nMetatileSet_2x2_pal=", metatileSet_2x2_pal, metatileSets_2x2*META_2x2);
	nss_put_bytes(file,"\n\nMetatileSet_2x2_palUndo=", undo_metatileSet_2x2_pal, undo_metatileSets_2x2*META_2x2);

	nss_put_bytes(file,"\n\nMetatileSet_2x2_props=", metatileSet_2x2_props, metatileSets_2x2*META_2x2);
	nss_put_bytes(file,"\n\nMetatileSet_2x2_propsUndo=", undo_metatileSet_2x2_props, undo_metatileSets_2x2*META_2x2);


	nss_put_bytes(file,"\n\nMetatileSet_4x4_id=",metatileSet_4x4_id, metatileSets_4x4*META_4x4);
	nss_put_bytes(file,"\n\nMetatileSet_4x4_idUndo=",undo_metatileSet_4x4_id, undo_metatileSets_4x4*META_4x4);

	nss_put_bytes(file,"\n\nMetatileSet_4x4_pal=", metatileSet_4x4_pal, metatileSets_4x4*META_4x4);
	nss_put_bytes(file,"\n\nMetatileSet_4x4_palUndo=", undo_metatileSet_4x4_pal, undo_metatileSets_4x4*META_4x4);

	nss_put_bytes(file,"\n\nMetatileSet_4x4_props=", metatileSet_4x4_props, metatileSets_4x4*META_4x4);
	nss_put_bytes(file,"\n\nMetatileSet_4x4_propsUndo=", undo_metatileSet_4x4_props, undo_metatileSets_4x4*META_4x4);


	nss_put_bytes(file,"\n\nMetatileSet_8x8_id=",metatileSet_8x8_id, metatileSets_8x8*META_8x8);
	nss_put_bytes(file,"\n\nMetatileSet_8x8_idUndo=",undo_metatileSet_8x8_id, undo_metatileSets_8x8*META_8x8);

	nss_put_bytes(file,"\n\nMetatileSet_8x8_pal=", metatileSet_8x8_pal, metatileSets_8x8*META_8x8);
	nss_put_bytes(file,"\n\nMetatileSet_8x8_palUndo=", undo_metatileSet_8x8_pal, undo_metatileSets_8x8*META_8x8);

	nss_put_bytes(file,"\n\nMetatileSet_8x8_props=", metatileSet_8x8_props, metatileSets_8x8*META_8x8);
	nss_put_bytes(file,"\n\nMetatileSet_8x8_propsUndo=", undo_metatileSet_8x8_props, undo_metatileSets_8x8*META_8x8);

	std::string serializedVector = "";
	for (std::vector<std::string>::const_iterator iter = metatileSetLabels_2x2.begin(); iter != metatileSetLabels_2x2.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != metatileSetLabels_2x2.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\nMetatileSetLabels_2x2=%s",serializedVector.c_str());

	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = undo_metatileSetLabels_2x2.begin(); iter != undo_metatileSetLabels_2x2.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_metatileSetLabels_2x2.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\MetatileSetLabels_2x2Undo=%s",serializedVector.c_str());


	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = metatileSetLabels_4x4.begin(); iter != metatileSetLabels_4x4.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != metatileSetLabels_4x4.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\nMetatileSetLabels_4x4=%s",serializedVector.c_str());

	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = undo_metatileSetLabels_4x4.begin(); iter != undo_metatileSetLabels_4x4.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_metatileSetLabels_4x4.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\MetatileSetLabels_4x4Undo=%s",serializedVector.c_str());


	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = metatileSetLabels_8x8.begin(); iter != metatileSetLabels_8x8.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != metatileSetLabels_8x8.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\nMetatileSetLabels_8x8=%s",serializedVector.c_str());

	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = undo_metatileSetLabels_8x8.begin(); iter != undo_metatileSetLabels_8x8.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_metatileSetLabels_8x8.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\MetatileSetLabels_8x8Undo=%s",serializedVector.c_str());


	serializedVector.clear();
	for (std::vector<std::string>::const_iterator iter = chrBankLabels.begin(); iter != chrBankLabels.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != chrBankLabels.end()) {
			serializedVector += ",";
		}
	}
	fprintf(file,"\n\nCHRBankLabels=%s",serializedVector.c_str());

	serializedVector.clear();
	for (std::vector<std::string>::const_iterator iter = undo_chrBankLabels.begin(); iter != undo_chrBankLabels.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_chrBankLabels.end()) {
			serializedVector += ",";
		}
	}
	fprintf(file,"\n\nCHRBankLabelsUndo=%s",serializedVector.c_str());



	//checkpoint data

	nss_put_bytes(file,"\n\nCheckpoint_Palette="  ,bgPalCheckpoint 		,sizeof(bgPal));
	nss_put_bytes(file,"\n\nCheckpoint_PalUndo="  ,bgPalUndoCheckPoint  ,sizeof(bgPalUndo));

	if(bLinkedCHRmode){
		nss_put_bytes(file,"\n\nCheckpoint_CHRMain="  ,checkpointChr_LinkBuf          ,checkpoint_chrBanks_LinkBuf*4096);
		nss_put_bytes(file,"\n\nCheckpoint_CHRUndo="  ,undoCheckpointChr_LinkBuf      ,undo_checkpoint_chrBanks_LinkBuf*4096);
	}
	else{
		nss_put_bytes(file,"\n\nCheckpoint_CHRMain="  ,checkpointChr          ,checkpoint_chrBanks*4096);
		nss_put_bytes(file,"\n\nCheckpoint_CHRUndo="  ,undoCheckpointChr      ,undo_checkpoint_chrBanks*4096);
	}



	nss_put_bytes(file,"\n\nCheckpoint_NameTable=",checkpointNameTable    ,name_size());
	nss_put_bytes(file,"\n\nCheckpoint_NameUndo=" ,undoCheckpointNameTable,name_size());

	nss_put_bytes(file,"\n\nCheckpoint_AttrTable=",checkpointAttrTable    ,attr_size());
	nss_put_bytes(file,"\n\nCheckpoint_AttrUndo=" ,undoCheckpointAttrTable,attr_size());

	nss_put_bytes(file,"\n\nCheckpoint_MetaSprites=",checkpointMetaSprites,sizeof(metaSprites));


	nss_put_uint32s(file,"\n\nCheckpoint_32_BankViewTable=",bankViewTable,sizeof(bankViewTable));
	nss_put_uint32s(file,"\n\nCheckpoint_32_BankViewTableUndo=",undo_bankViewTable,sizeof(undo_bankViewTable));


	/*
	nss_put_uint32s(file,"\n\nCheckpoint_32_TileCollisionProperties=",checkpointTileProperties32,sizeof(tileProperties));
	nss_put_uint32s(file,"\n\nCheckpoint_32_TileCollisionPropertiesUndo=",checkpointTilePropertiesUndo32,sizeof(tilePropertiesUndo));
	*/

	nss_put_bytes(file,"\n\nCheckpoint_8_TileCollisionProperties=",checkpointTileProperties,checkpoint_chrBanks*256);
	nss_put_bytes(file,"\n\nCheckpoint_8_TileCollisionPropertiesUndo=",checkpointTilePropertiesUndo,undo_checkpoint_chrBanks*256);


	nss_put_2D_bools(file,"\n\nCheckpoint_PropertyConditions=",checkpointPropCondition,4,8);
	nss_put_2D_bools(file,"\n\nCheckpoint_PropertyConditionsUndo=",checkpointPropConditionUndo,4,8);

	nss_put_bytes(file,"\n\nCheckpoint_TilePropConditionType=",checkpointPropConditional,sizeof(propConditional));
	nss_put_bytes(file,"\n\nCheckpoint_TilePropConditionTypeUndo=",checkpointPropConditionalUndo,sizeof(propConditional));

	//metatile stuff pt2
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_2x2_id=",checkpoint_metatileSet_2x2_id, checkpoint_metatileSets_2x2*META_2x2);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_2x2_idUndo=",undo_checkpoint_metatileSet_2x2_id, undo_checkpoint_metatileSets_2x2*META_2x2);

	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_2x2_pal=", checkpoint_metatileSet_2x2_pal, checkpoint_metatileSets_2x2*META_2x2);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_2x2_palUndo=", undo_checkpoint_metatileSet_2x2_pal, undo_checkpoint_metatileSets_2x2*META_2x2);

	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_2x2_props=", checkpoint_metatileSet_2x2_props, checkpoint_metatileSets_2x2*META_2x2);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_2x2_propsUndo=", undo_checkpoint_metatileSet_2x2_props, undo_checkpoint_metatileSets_2x2*META_2x2);


	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_4x4_id=",checkpoint_metatileSet_4x4_id, checkpoint_metatileSets_4x4*META_4x4);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_4x4_idUndo=",undo_checkpoint_metatileSet_4x4_id, undo_checkpoint_metatileSets_4x4*META_4x4);

	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_4x4_pal=", checkpoint_metatileSet_4x4_pal, checkpoint_metatileSets_4x4*META_4x4);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_4x4_palUndo=", undo_checkpoint_metatileSet_4x4_pal, undo_checkpoint_metatileSets_4x4*META_4x4);

	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_4x4_props=", checkpoint_metatileSet_4x4_props, checkpoint_metatileSets_4x4*META_4x4);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_4x4_propsUndo=", undo_checkpoint_metatileSet_4x4_props, undo_checkpoint_metatileSets_4x4*META_4x4);


	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_8x8_id=",checkpoint_metatileSet_8x8_id, checkpoint_metatileSets_8x8*META_8x8);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_8x8_idUndo=",undo_checkpoint_metatileSet_8x8_id, undo_checkpoint_metatileSets_8x8*META_8x8);

	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_8x8_pal=", checkpoint_metatileSet_8x8_pal, checkpoint_metatileSets_8x8*META_8x8);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_8x8_palUndo=", undo_checkpoint_metatileSet_8x8_pal, undo_checkpoint_metatileSets_8x8*META_8x8);

	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_8x8_props=", checkpoint_metatileSet_8x8_props, checkpoint_metatileSets_8x8*META_8x8);
	nss_put_bytes(file,"\n\nCheckpoint_MetatileSet_8x8_propsUndo=", undo_checkpoint_metatileSet_8x8_props, undo_checkpoint_metatileSets_8x8*META_8x8);



	serializedVector.clear();
	for (std::vector<std::string>::const_iterator iter = checkpoint_metatileSetLabels_2x2.begin(); iter != checkpoint_metatileSetLabels_2x2.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != checkpoint_metatileSetLabels_2x2.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\nCheckpoint_MetatileSetLabels_2x2=%s",serializedVector.c_str());

	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = undo_checkpoint_metatileSetLabels_2x2.begin(); iter != undo_checkpoint_metatileSetLabels_2x2.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_checkpoint_metatileSetLabels_2x2.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\Checkpoint_MetatileSetLabels_2x2Undo=%s",serializedVector.c_str());


	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = checkpoint_metatileSetLabels_4x4.begin(); iter != checkpoint_metatileSetLabels_4x4.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != checkpoint_metatileSetLabels_4x4.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\nCheckpoint_MetatileSetLabels_4x4=%s",serializedVector.c_str());

	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = undo_checkpoint_metatileSetLabels_4x4.begin(); iter != undo_checkpoint_metatileSetLabels_4x4.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_checkpoint_metatileSetLabels_4x4.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\Checkpoint_MetatileSetLabels_4x4Undo=%s",serializedVector.c_str());


	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = checkpoint_metatileSetLabels_8x8.begin(); iter != checkpoint_metatileSetLabels_8x8.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != checkpoint_metatileSetLabels_8x8.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\nCheckpoint_MetatileSetLabels_8x8=%s",serializedVector.c_str());

	serializedVector.clear();

	for (std::vector<std::string>::const_iterator iter = undo_checkpoint_metatileSetLabels_8x8.begin(); iter != undo_checkpoint_metatileSetLabels_8x8.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_checkpoint_metatileSetLabels_8x8.end()) {serializedVector += ",";}
	}
	fprintf(file,"\n\Checkpoint_MetatileSetLabels_8x8Undo=%s",serializedVector.c_str());


	serializedVector.clear();
	for (std::vector<std::string>::const_iterator iter = checkpoint_chrBankLabels.begin(); iter != checkpoint_chrBankLabels.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != checkpoint_chrBankLabels.end()) {
			serializedVector += ",";
		}
	}
	fprintf(file,"\n\nCheckpoint_CHRBankLabels=%s",serializedVector.c_str());

	serializedVector.clear();
	for (std::vector<std::string>::const_iterator iter = undo_checkpoint_chrBankLabels.begin(); iter != undo_checkpoint_chrBankLabels.end(); ++iter) {
		serializedVector += *iter;
		if (iter + 1 != undo_checkpoint_chrBankLabels.end()) {
			serializedVector += ",";
		}
	}

	fprintf(file,"\n\nCheckpoint_CHRBankLabelsUndo=%s\n",serializedVector.c_str());





	fclose(file);

	unsavedChanges=false;
	Savesession1->Enabled=false;
    //auto update modified CHR link
	if((bLinkedCHRmode) && ansiFileNameAssociatedTileset!=""){
	   file=fopen(ansiFileNameAssociatedTileset.c_str(),"r+");

		if(!file)
		{
			Application->MessageBox("Couldn't modify linked tileset session","Error",MB_OK);
		}
		else{
			AnsiString ext=GetExt(ansiFileNameAssociatedTileset);
			//.chr file handling
			if((ext=="chr") || (ext=="bin"))
			{
				fseek(file,0,SEEK_SET);
				fwrite(chr,chrBanks*4096,1,file);
			}
			//.nss session handling
			else if(ext=="nss"){
				char *str_data;
				char *text;
				int newSize=0; //used to write the new length of the file, or abort.
				fseek(file,0,SEEK_END);
				int size=ftell(file);
				fseek(file,0,SEEK_SET);

				text=(char*)malloc(size+1);
				text[size]=0;
				fread(text,size,1,file);
				char* bufstr=(char*)malloc(chrBanks*4096*4);
				str_data = str_put_bytes(chr,chrBanks*4096,bufstr);
				newSize=ReplaceTagInFile(text, size,"CHRMain=" ,str_data, strlen(str_data));

				if(newSize>0){
					fseek(file, 0, SEEK_SET);
					fwrite(text, 1, newSize, file);
				}
				else    //chr tag is missing. just append it to the bottom.
				{
					fseek(file, 0, SEEK_END);
					nss_put_bytes(file,"\n\nCHRMain="  ,chr          ,chrBanks*4096);
				}
				//edit chr banks tag.
				//---------
				fseek(file,0,SEEK_END);
				size=ftell(file);
				fseek(file,0,SEEK_SET);
				text=(char*)realloc(text,size+1);
				text[size]=0;
				fread(text,size,1,file);
				str_data = IntToStr(chrBanks).c_str();
				newSize=ReplaceTagInFile(text, size,"VarCHRBanks=" ,str_data, strlen(str_data));
				if(newSize>0){
					fseek(file, 0, SEEK_SET);
					fwrite(text, 1, newSize, file);
				}
				else    //chrbanks tag is missing. Append it to the bottom.
				{
					fseek(file, 0, SEEK_END);
					fprintf(file,"VarCHRBanks=%i\n"     			,chrBanks);
				}
				//include tileprops
                fseek(file,0,SEEK_END);
				size=ftell(file);
				fseek(file,0,SEEK_SET);
				text=(char*)realloc(text,size+1);
				text[size]=0;
				fread(text,size,1,file);
				str_data = str_put_bytes(tileProperties,chrBanks*256,bufstr);
				newSize=ReplaceTagInFile(text, size,"8_TileCollisionProperties=" ,str_data, strlen(str_data));
                if(newSize>0){
					fseek(file, 0, SEEK_SET);
					fwrite(text, 1, newSize, file);
				}
				else    //chr tag is missing. just append it to the bottom.
				{
					fseek(file, 0, SEEK_END);
					nss_put_bytes(file,"\n\8_TileCollisionProperties="  ,tileProperties          ,chrBanks*256);
				}


				//include serialized vectors for bank labels.
				//--
				fseek(file,0,SEEK_END);
				size=ftell(file);
				fseek(file,0,SEEK_SET);
				text=(char*)realloc(text,size+1);
				text[size]=0;
				fread(text,size,1,file);
                serializedVector.clear();
				for (std::vector<std::string>::const_iterator iter = chrBankLabels.begin(); iter != chrBankLabels.end(); ++iter) {
					serializedVector += *iter;
					if (iter + 1 != chrBankLabels.end()) {
						serializedVector += ",";
					}
				}
				//str_data = serializedVector.c_str();
				newSize=ReplaceTagInFile(text, size,"CHRBankLabels=" ,serializedVector.c_str(), strlen(serializedVector.c_str()));
				if(newSize>0){
					fseek(file, 0, SEEK_SET);
					fwrite(text, 1, newSize, file);
				}
				else    //chrbankLabels tag is missing. Append it to the bottom.
				{
					fseek(file, 0, SEEK_END);
					fprintf(file,"\n\nCHRBankLabels=%s",serializedVector.c_str());
				}

				//--
				free(text);
				free(bufstr);
			}   //nss closure
		}      //file exists closure
		fclose(file);
	}   //autosave linked chr closure

	UpdateStats();
}
/*
void __fastcall TFormMain::FindFirstDuplicate(int tileID, int &firstDuplicateTile, bool &bDidFind)
{
	int i,j;
	bool dupe;
	bDidFind=false;
	for(i=0;i<256;++i)
	{
	used=false;
	for(j=0;j<nameTableWidth*nameTableHeight;++j)
		{
			if(nameTable[j]==i) {dupe=true; break;}
		}
		if(!dupe) firstDuplicateTile=i; {bDidFind=true; return;}
	}
}
*/

void __fastcall TFormMain::FindFirstUnused(int &firstUnusedTile, bool &bDidFind)
{
	int i,j;
	bool used;
	bDidFind=false;
	for(i=0;i<256;++i)
	{
	used=false;
	for(j=0;j<nameTableWidth*nameTableHeight;++j)
		{
			if(nameTable[j]==i) {used=true; break;}
		}
		if(!used) firstUnusedTile=i; {bDidFind=true; return;}
	}

}
void __fastcall TFormMain::SaveLineToolConfig(void)
{
	//BlockDrawing(true);

	FILE *file;

	file=fopen((globalDir+"linetool_presets.cfg").c_str(),"wt");

	if(!file)
	{
		//Application->MessageBox("Couldn't save preferences to file","Warning",MB_OK);
		return;
	}

	fprintf(file,"=========================================================\n");
	fprintf(file,"Line toolbox presets for %s\n",Application->Title.c_str());
	fprintf(file,"=========================================================\n\n\n");

	//
	fprintf(file,"\n\Preset A:\n");
	fprintf(file,"=========================================================\n");

	fprintf(file,"\nLine Style:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Dots A=%i\n"				,bLinePreset_styleDots[0]);
	fprintf(file,"Dashes A=%i\n"			,bLinePreset_styleDashes[0]);
	fprintf(file,"Dash Length A=%i\n"		,iLinePreset_styleRepeat[0]);

	fprintf(file,"\nLine + Brush FX:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Enable Brush FX A=%i\n"	,bLinePreset_taperEnabled[0]);
	fprintf(file,"Taper In A=%i\n"			,bLinePreset_taperIn[0]);
	fprintf(file,"Taper Out A=%i\n"			,bLinePreset_taperOut[0]);
	fprintf(file,"Taper From Mid A=%i\n"	,bLinePreset_taperFromMid[0]);


	fprintf(file,"\nApplication mode:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Quick A=%i\n"			,bLinePreset_modeQuick[0]);
	fprintf(file,"Coat A=%i\n"			,bLinePreset_modeCoat[0]);

	fprintf(file,"\nx0y0 Adjust settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Autoreset x0y0 A=%i\n"			,bLinePreset_adjustAutoreset[0]);

	fprintf(file,"\nScrollwheel Roster:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Enable Line A=%i\n"		,bLinePreset_rosterEnableLine[0]);
	fprintf(file,"Roster Enable Curve A=%i\n"		,bLinePreset_rosterEnableCurve[0]);
	fprintf(file,"Roster Enable Knee A=%i\n"		,bLinePreset_rosterEnableKnee[0]);
	fprintf(file,"Roster Enable Right A=%i\n"		,bLinePreset_rosterEnableRight[0]);

	fprintf(file,"Roster Enable Rect A=%i\n"		,bLinePreset_rosterEnableRect[0]);
	fprintf(file,"Roster Enable Ellipse A=%i\n"		,bLinePreset_rosterEnableEllipse[0]);

	fprintf(file,"Roster Enable Hyperbola A=%i\n"	,bLinePreset_rosterEnableHyperbola[0]);
	fprintf(file,"Roster Enable Hyperline A=%i\n"	,bLinePreset_rosterEnableHyperline[0]);
	fprintf(file,"Roster Enable Hypercave A=%i\n"	,bLinePreset_rosterEnableHypercave[0]);

	fprintf(file,"\nScrollwheel Roster settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Autoreset A=%i\n"	,bLinePreset_rosterReset[0]);
	fprintf(file,"Roster AutosizeY A=%i\n"	,bLinePreset_rosterAutosize[0]);
	fprintf(file,"Roster Cursor A=%i\n"		,iLinePreset_rosterIndex[0]);

    fprintf(file,"\n\Preset B:\n");
	fprintf(file,"=========================================================\n");

	fprintf(file,"\nLine Style:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Dots B=%i\n"				,bLinePreset_styleDots[1]);
	fprintf(file,"Dashes B=%i\n"			,bLinePreset_styleDashes[1]);
	fprintf(file,"Dash Length B=%i\n"		,iLinePreset_styleRepeat[1]);

	fprintf(file,"\nLine + Brush FX:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Enable Brush FX B=%i\n"	,bLinePreset_taperEnabled[1]);
	fprintf(file,"Taper In B=%i\n"			,bLinePreset_taperIn[1]);
	fprintf(file,"Taper Out B=%i\n"			,bLinePreset_taperOut[1]);
	fprintf(file,"Taper From Mid B=%i\n"	,bLinePreset_taperFromMid[1]);


	fprintf(file,"\nApplication mode:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Quick B=%i\n"			,bLinePreset_modeQuick[1]);
	fprintf(file,"Coat B=%i\n"			,bLinePreset_modeCoat[1]);

	fprintf(file,"\nx0y0 Adjust settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Autoreset x0y0 B=%i\n"			,bLinePreset_adjustAutoreset[1]);

	fprintf(file,"\nScrollwheel Roster:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Enable Line B=%i\n"		,bLinePreset_rosterEnableLine[1]);
	fprintf(file,"Roster Enable Curve B=%i\n"		,bLinePreset_rosterEnableCurve[1]);
	fprintf(file,"Roster Enable Knee B=%i\n"		,bLinePreset_rosterEnableKnee[1]);
	fprintf(file,"Roster Enable Right B=%i\n"		,bLinePreset_rosterEnableRight[1]);

	fprintf(file,"Roster Enable Rect B=%i\n"		,bLinePreset_rosterEnableRect[1]);
	fprintf(file,"Roster Enable Ellipse B=%i\n"		,bLinePreset_rosterEnableEllipse[1]);

	fprintf(file,"Roster Enable Hyperbola B=%i\n"	,bLinePreset_rosterEnableHyperbola[1]);
	fprintf(file,"Roster Enable Hyperline B=%i\n"	,bLinePreset_rosterEnableHyperline[1]);
	fprintf(file,"Roster Enable Hypercave B=%i\n"	,bLinePreset_rosterEnableHypercave[1]);

	fprintf(file,"\nScrollwheel Roster settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Autoreset B=%i\n"	,bLinePreset_rosterReset[1]);
	fprintf(file,"Roster AutosizeY B=%i\n"	,bLinePreset_rosterAutosize[1]);
	fprintf(file,"Roster Cursor B=%i\n"		,iLinePreset_rosterIndex[1]);


	fprintf(file,"\n\Preset C:\n");
	fprintf(file,"=========================================================\n");

	fprintf(file,"\nLine Style:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Dots C=%i\n"				,bLinePreset_styleDots[2]);
	fprintf(file,"Dashes C=%i\n"			,bLinePreset_styleDashes[2]);
	fprintf(file,"Dash Length C=%i\n"		,iLinePreset_styleRepeat[2]);

	fprintf(file,"\nLine + Brush FX:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Enable Brush FX C=%i\n"	,bLinePreset_taperEnabled[2]);
	fprintf(file,"Taper In C=%i\n"			,bLinePreset_taperIn[2]);
	fprintf(file,"Taper Out C=%i\n"			,bLinePreset_taperOut[2]);
	fprintf(file,"Taper From Mid C=%i\n"	,bLinePreset_taperFromMid[2]);


	fprintf(file,"\nApplication mode:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Quick C=%i\n"			,bLinePreset_modeQuick[2]);
	fprintf(file,"Coat C=%i\n"			,bLinePreset_modeCoat[2]);

	fprintf(file,"\nx0y0 Adjust settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Autoreset x0y0 C=%i\n"			,bLinePreset_adjustAutoreset[2]);

	fprintf(file,"\nScrollwheel Roster:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Enable Line C=%i\n"		,bLinePreset_rosterEnableLine[2]);
	fprintf(file,"Roster Enable Curve C=%i\n"		,bLinePreset_rosterEnableCurve[2]);
	fprintf(file,"Roster Enable Knee C=%i\n"		,bLinePreset_rosterEnableKnee[2]);
	fprintf(file,"Roster Enable Right C=%i\n"		,bLinePreset_rosterEnableRight[2]);

	fprintf(file,"Roster Enable Rect C=%i\n"		,bLinePreset_rosterEnableRect[2]);
	fprintf(file,"Roster Enable Ellipse C=%i\n"		,bLinePreset_rosterEnableEllipse[2]);

	fprintf(file,"Roster Enable Hyperbola C=%i\n"	,bLinePreset_rosterEnableHyperbola[2]);
	fprintf(file,"Roster Enable Hyperline C=%i\n"	,bLinePreset_rosterEnableHyperline[2]);
	fprintf(file,"Roster Enable Hypercave C=%i\n"	,bLinePreset_rosterEnableHypercave[2]);

	fprintf(file,"\nScrollwheel Roster settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Autoreset C=%i\n"	,bLinePreset_rosterReset[2]);
	fprintf(file,"Roster AutosizeY C=%i\n"	,bLinePreset_rosterAutosize[2]);
	fprintf(file,"Roster Cursor C=%i\n"		,iLinePreset_rosterIndex[2]);


	fprintf(file,"\n\Preset D:\n");
	fprintf(file,"=========================================================\n");

	fprintf(file,"\nLine Style:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Dots D=%i\n"				,bLinePreset_styleDots[3]);
	fprintf(file,"Dashes D=%i\n"			,bLinePreset_styleDashes[3]);
	fprintf(file,"Dash Length D=%i\n"		,iLinePreset_styleRepeat[3]);

	fprintf(file,"\nLine + Brush FX:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Enable Brush FX D=%i\n"	,bLinePreset_taperEnabled[3]);
	fprintf(file,"Taper In D=%i\n"			,bLinePreset_taperIn[3]);
	fprintf(file,"Taper Out D=%i\n"			,bLinePreset_taperOut[3]);
	fprintf(file,"Taper From Mid D=%i\n"	,bLinePreset_taperFromMid[3]);


	fprintf(file,"\nApplication mode:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Quick D=%i\n"			,bLinePreset_modeQuick[3]);
	fprintf(file,"Coat D=%i\n"			,bLinePreset_modeCoat[3]);

	fprintf(file,"\nx0y0 Adjust settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Autoreset x0y0 D=%i\n"			,bLinePreset_adjustAutoreset[3]);

	fprintf(file,"\nScrollwheel Roster:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Enable Line D=%i\n"		,bLinePreset_rosterEnableLine[3]);
	fprintf(file,"Roster Enable Durve D=%i\n"		,bLinePreset_rosterEnableCurve[3]);
	fprintf(file,"Roster Enable Knee D=%i\n"		,bLinePreset_rosterEnableKnee[3]);
	fprintf(file,"Roster Enable Right D=%i\n"		,bLinePreset_rosterEnableRight[3]);

	fprintf(file,"Roster Enable Rect D=%i\n"		,bLinePreset_rosterEnableRect[3]);
	fprintf(file,"Roster Enable Ellipse D=%i\n"		,bLinePreset_rosterEnableEllipse[3]);

	fprintf(file,"Roster Enable Hyperbola D=%i\n"	,bLinePreset_rosterEnableHyperbola[3]);
	fprintf(file,"Roster Enable Hyperline D=%i\n"	,bLinePreset_rosterEnableHyperline[3]);
	fprintf(file,"Roster Enable Hypercave D=%i\n"	,bLinePreset_rosterEnableHypercave[3]);

	fprintf(file,"\nScrollwheel Roster settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Roster Autoreset D=%i\n"	,bLinePreset_rosterReset[3]);
	fprintf(file,"Roster AutosizeY D=%i\n"	,bLinePreset_rosterAutosize[3]);
	fprintf(file,"Roster Cursor D=%i\n"		,iLinePreset_rosterIndex[3]);

	fclose(file);
	//BlockDrawing(false);

}

bool __fastcall TFormMain::LoadLineToolConfig(void)
{
	FILE *file;
	char *text;
	int i,j,size;

	file=fopen((globalDir+"linetool_presets.cfg").c_str(),"rb");

	if(!file) return false;

	BlockDrawing(true);

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	text=(char*)malloc(size+1);
	text[size]=0;

	fread(text,size,1,file);
	fclose(file);

	//Preset A
	//Line Style:
	bLinePreset_styleDots[0]=nss_get_bool(text,size,"Dots A=\n");
	bLinePreset_styleDashes[0]=nss_get_bool(text,size,"Dashes A=\n");
	iLinePreset_styleRepeat[0]=nss_get_int(text,size,"Dash Length A=\n");

	//Line + Brush FX:
	bLinePreset_taperEnabled[0]=nss_get_bool(text,size,"Enable Brush FX A=\n");
	bLinePreset_taperIn[0]=nss_get_bool(text,size,"Taper In A=\n");
	bLinePreset_taperOut[0]=nss_get_bool(text,size,"Taper Out A=\n");
	bLinePreset_taperFromMid[0]=nss_get_bool(text,size,"Taper From Mid A=\n");

	//Application mode:
	bLinePreset_modeQuick[0]=nss_get_bool(text,size,"Quick A=\n");
	bLinePreset_modeCoat[0]=nss_get_bool(text,size,"Coat A=\n");

	//0y0 Adjust settings:
	bLinePreset_adjustAutoreset[0]=nss_get_bool(text,size,"Autoreset x0y0 A\n");

	//Scrollwheel Roster:
	bLinePreset_rosterEnableLine[0]=nss_get_bool(text,size,"Roster Enable Line A=\n");
	bLinePreset_rosterEnableCurve[0]=nss_get_bool(text,size,"Roster Enable Curve A\n");
	bLinePreset_rosterEnableKnee[0]=nss_get_bool(text,size,"Roster Enable Knee A\n");
	bLinePreset_rosterEnableRight[0]=nss_get_bool(text,size,"Roster Enable Right A=\n");

	bLinePreset_rosterEnableRect[0]=nss_get_bool(text,size,"Roster Enable Rect A=\n");
	bLinePreset_rosterEnableEllipse[0]=nss_get_bool(text,size,"Roster Enable Ellipse A=\n");

	bLinePreset_rosterEnableHyperbola[0]=nss_get_bool(text,size,"Roster Enable Hyperbola A=\n");
	bLinePreset_rosterEnableHyperline[0]=nss_get_bool(text,size,"Roster Enable Hyperline A=\n");
	bLinePreset_rosterEnableHypercave[0]=nss_get_bool(text,size,"Roster Enable Hypercave A=\n");

	//Scrollwheel Roster settings:
	bLinePreset_rosterReset[0]=nss_get_bool(text,size,"Roster Autoreset A=\n");
	bLinePreset_rosterAutosize[0]=nss_get_bool(text,size,"Roster AutosizeY A=\n");
	iLinePreset_rosterIndex[0]=nss_get_int(text,size,"Roster Cursor A=\n");

	//Preset B:
	//Line Style:
	bLinePreset_styleDots[1]=nss_get_bool(text,size,"Dots B=\n");
	bLinePreset_styleDashes[1]=nss_get_bool(text,size,"Dashes B=\n");
	iLinePreset_styleRepeat[1]=nss_get_int(text,size,"Dash Length B=\n");

	//Line + Brush FX:
	bLinePreset_taperEnabled[1]=nss_get_bool(text,size,"Enable Brush FX B=\n");
	bLinePreset_taperIn[1]=nss_get_bool(text,size,"Taper In B=\n");
	bLinePreset_taperOut[1]=nss_get_bool(text,size,"Taper Out B=\n");
	bLinePreset_taperFromMid[1]=nss_get_bool(text,size,"Taper From Mid B=\n");

	//Application mode:
	bLinePreset_modeQuick[1]=nss_get_bool(text,size,"Quick B=\n");
	bLinePreset_modeCoat[1]=nss_get_bool(text,size,"Coat B=\n");

	//x0y0 Adjust settings:
	bLinePreset_adjustAutoreset[1]=nss_get_bool(text,size,"Autoreset x0y0 B=\n");

	//Scrollwheel Roster:
	bLinePreset_rosterEnableLine[1]=nss_get_bool(text,size,"Roster Enable Line B=\n");
	bLinePreset_rosterEnableCurve[1]=nss_get_bool(text,size,"Roster Enable Curve B=\n");
	bLinePreset_rosterEnableKnee[1]=nss_get_bool(text,size,"Roster Enable Knee B=\n");
	bLinePreset_rosterEnableRight[1]=nss_get_bool(text,size,"Roster Enable Right B=\n");

	bLinePreset_rosterEnableRect[1]=nss_get_bool(text,size,"Roster Enable Rect B=\n");
	bLinePreset_rosterEnableEllipse[1]=nss_get_bool(text,size,"Roster Enable Ellipse B=\n");

	bLinePreset_rosterEnableHyperbola[1]=nss_get_bool(text,size,"Roster Enable Hyperbola B=\n");
	bLinePreset_rosterEnableHyperline[1]=nss_get_bool(text,size,"Roster Enable Hyperline B=\n");
	bLinePreset_rosterEnableHypercave[1]=nss_get_bool(text,size,"Roster Enable Hypercave B=\n");

	//Scrollwheel Roster settings:
	bLinePreset_rosterReset[1]=nss_get_bool(text,size,"Roster Autoreset B=\n");
	bLinePreset_rosterAutosize[1]=nss_get_bool(text,size,"Roster AutosizeY B=\n");
	iLinePreset_rosterIndex[1]=nss_get_int(text,size,"Roster Cursor B=\n");

	//Preset C:
	//Line Style:
	bLinePreset_styleDots[2]=nss_get_bool(text,size,"Dots C=\n");
	bLinePreset_styleDashes[2]=nss_get_bool(text,size,"Dashes C=\n");
	iLinePreset_styleRepeat[2]=nss_get_int(text,size,"Dash Length C=\n");

	//Line + Brush FX:
	bLinePreset_taperEnabled[2]=nss_get_bool(text,size,"Enable Brush FX C=\n");
	bLinePreset_taperIn[2]=nss_get_bool(text,size,"Taper In C=\n");
	bLinePreset_taperOut[2]=nss_get_bool(text,size,"Taper Out C=\n");
	bLinePreset_taperFromMid[2]=nss_get_bool(text,size,"Taper From Mid C=\n");

	//Application mode:
	bLinePreset_modeQuick[2]=nss_get_bool(text,size,"Quick C=\n");
	bLinePreset_modeCoat[2]=nss_get_bool(text,size,"Coat C=\n");

	//x0y0 Adjust settings:
	bLinePreset_adjustAutoreset[2]=nss_get_bool(text,size,"Autoreset x0y0 C=\n");

	//Scrollwheel Roster:
	bLinePreset_rosterEnableLine[2]=nss_get_bool(text,size,"Roster Enable Line C=\n");
	bLinePreset_rosterEnableCurve[2]=nss_get_bool(text,size,"Roster Enable Curve C=\n");
	bLinePreset_rosterEnableKnee[2]=nss_get_bool(text,size,"Roster Enable Knee C=\n");
	bLinePreset_rosterEnableRight[2]=nss_get_bool(text,size,"Roster Enable Right C=\n");

	bLinePreset_rosterEnableRect[2]=nss_get_bool(text,size,"Roster Enable Rect C=\n");
	bLinePreset_rosterEnableEllipse[2]=nss_get_bool(text,size,"Roster Enable Ellipse C=\n");

	bLinePreset_rosterEnableHyperbola[2]=nss_get_bool(text,size,"Roster Enable Hyperbola C=\n");
	bLinePreset_rosterEnableHyperline[2]=nss_get_bool(text,size,"Roster Enable Hyperline C=\n");
	bLinePreset_rosterEnableHypercave[2]=nss_get_bool(text,size,"Roster Enable Hypercave C=\n");

	//Scrollwheel Roster settings:
	bLinePreset_rosterReset[2]=nss_get_bool(text,size,"Roster Autoreset C=\n");
	bLinePreset_rosterAutosize[2]=nss_get_bool(text,size,"Roster AutosizeY C=\n");
	iLinePreset_rosterIndex[2]=nss_get_int(text,size,"Roster Cursor C=\n");

	//Preset D:
	//Line Style:
	bLinePreset_styleDots[3]=nss_get_bool(text,size,"Dots D=\n");
	bLinePreset_styleDashes[3]=nss_get_bool(text,size,"Dashes D=\n");
	iLinePreset_styleRepeat[3]=nss_get_int(text,size,"Dash Length D=\n");

	//Line + Brush FX:
	bLinePreset_taperEnabled[3]=nss_get_bool(text,size,"Enable Brush FX D=\n");
	bLinePreset_taperIn[3]=nss_get_bool(text,size,"Taper In D=\n");
	bLinePreset_taperOut[3]=nss_get_bool(text,size,"Taper Out D=\n");
	bLinePreset_taperFromMid[3]=nss_get_bool(text,size,"Taper From Mid D=\n");

	//Application mode:
	bLinePreset_modeQuick[3]=nss_get_bool(text,size,"Quick D=\n");
	bLinePreset_modeCoat[3]=nss_get_bool(text,size,"Coat D=\n");

	//x0y0 Adjust settings:
	bLinePreset_adjustAutoreset[3]=nss_get_bool(text,size,"Autoreset x0y0 D=\n");

	//Scrollwheel Roster:
	bLinePreset_rosterEnableLine[3]=nss_get_bool(text,size,"Roster Enable Line D=\n");
	bLinePreset_rosterEnableCurve[3]=nss_get_bool(text,size,"Roster Enable Durve D=\n");
	bLinePreset_rosterEnableKnee[3]=nss_get_bool(text,size,"Roster Enable Knee D=\n");
	bLinePreset_rosterEnableRight[3]=nss_get_bool(text,size,"Roster Enable Right D=\n");

	bLinePreset_rosterEnableRect[3]=nss_get_bool(text,size,"Roster Enable Rect D=\n");
	bLinePreset_rosterEnableEllipse[3]=nss_get_bool(text,size,"Roster Enable Ellipse D=\n");

	bLinePreset_rosterEnableHyperbola[3]=nss_get_bool(text,size,"Roster Enable Hyperbola D=\n");
	bLinePreset_rosterEnableHyperline[3]=nss_get_bool(text,size,"Roster Enable Hyperline D=\n");
	bLinePreset_rosterEnableHypercave[3]=nss_get_int(text,size,"Roster Enable Hypercave D=\n");

    //Scrollwheel Roster settings:
	bLinePreset_rosterReset[3]=nss_get_bool(text,size,"Roster Autoreset D=\n");
	bLinePreset_rosterAutosize[3]=nss_get_bool(text,size,"Roster AutosizeY D=\n");
	iLinePreset_rosterIndex[3]=nss_get_int(text,size,"Roster Cursor D=\n");

    BlockDrawing(false);
	return true;
}


void __fastcall TFormMain::SaveConfig(void)
{
	BlockDrawing(true);

	FILE *file;

	file=fopen((globalDir+"preferences.cfg").c_str(),"wt");

	if(!file)
	{
		Application->MessageBox("Couldn't save preferences to file","Warning",MB_OK);
		return;
	}

	fprintf(file,"=========================================================\n");
	fprintf(file,"Preferences for %s\n",Application->Title.c_str());
	fprintf(file,"=========================================================\n\n\n");

	fprintf(file,"\nRadio options (for each group, only 1 will be checked):\n");
	fprintf(file,"=========================================================\n");

	fprintf(file,"\nCHR Import, custom filesize, overflowing data:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"CHR Import Wrap=%i\n"				,bImportCHRWrap?1:0);
	fprintf(file,"CHR Import Carry=%i\n"			,bImportCHRCarry?1:0);
	fprintf(file,"CHR Import Ignore=%i\n"			,bImportCHRSkip?1:0);


	fprintf(file,"\nText export Asm Syntax:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,".byte=%i\n"				,byte1				->Checked?1:0);
	fprintf(file,".db=%i\n"					,db1				->Checked?1:0);

	fprintf(file,"\nText export Asm Sign symbol:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Ca65 Sign=%i\n"			,SignCa65				->Checked?1:0);
	fprintf(file,"Normal Sign=%i\n"			,SignOther				->Checked?1:0);


	fprintf(file,"\nSprite Metadata header/terminator:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"None=%i\n"			,Noterminator1		->Checked?1:0);
	fprintf(file,"Sprite count=%i\n"	,Spritecountheader1	->Checked?1:0);
	fprintf(file,"N flag=%i\n"			,Nflagterminator1	->Checked?1:0);
	fprintf(file,"$FF=%i\n"				,FFterminator1		->Checked?1:0);
	fprintf(file,"Single $00=%i\n"		,Single00terminator1->Checked?1:0);
	fprintf(file,"Double $00=%i\n"		,Double00terminator1->Checked?1:0);

	fprintf(file,"\nASCII base offset (default is -0x20 in classic NESST):\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"-0x20=%i\n"					,MASCIIneg20h		->Checked?1:0);
	fprintf(file,"-0x30=%i\n"					,MASCIIneg30h		->Checked?1:0);
	fprintf(file,"-0x40=%i\n"					,MASCIIneg40h		->Checked?1:0);

	fprintf(file,"\nWorkspace arrangement - Spritelist:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Spritelist Left=%i\n"					,Sprlistl1			->Checked?1:0);
	fprintf(file,"Spritelist Center=%i\n"				,Sprlistc1			->Checked?1:0);

	fprintf(file,"\nWorkspace arrangement - CHR tools:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"CHR tools Top=%i\n"					,CHReditortoolbartop->Checked?1:0);
	fprintf(file,"CHR tools Bottom=%i\n"				,CHReditortoolbarbottom->Checked?1:0);

	fprintf(file,"\nMenu - Open/Save Binary Actions:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"OSBA in File menu=%i\n"				,iRadioOpenSave<=1?1:0);
	fprintf(file,"OSBA in type menus=%i\n"				,iRadioOpenSave==2?1:0);
	fprintf(file,"OSBA in both=%i\n"					,iRadioOpenSave>=3?1:0);

	fprintf(file,"\nTitlebar - Show filename:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Show filename=%i\n"				,iShowFilepath<=1?1:0);
	fprintf(file,"Show file and folder=%i\n"				,iShowFilepath==2?1:0);
	fprintf(file,"Show full path=%i\n"					,iShowFilepath>=3?1:0);

	fprintf(file,"\nScreen grid rules - Navigator:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Navigator Always=%i\n"				,AlwaysNavigator1		->Checked?1:0);
	fprintf(file,"Navigator MouseOver=%i\n"				,MouseNavigator1		->Checked?1:0);
	fprintf(file,"Navigator Mouse or Button=%i\n"		,MouseButtonNavigator1	->Checked?1:0);
	fprintf(file,"Navigator Button=%i\n"				,ButtonNavigator1		->Checked?1:0);
	fprintf(file,"Navigator Never=%i\n"					,NeverNavigator1		->Checked?1:0);

	fprintf(file,"\nScreen grid rules - Main canvas:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Main canvas Always=%i\n"				,AlwaysCanvas1		->Checked?1:0);
	fprintf(file,"Main canvas MouseOver=%i\n"			,MouseCanvas1		->Checked?1:0);
	fprintf(file,"Main canvas Mouse or Button=%i\n"		,MouseButtonCanvas1	->Checked?1:0);
	fprintf(file,"Main canvas Button=%i\n"				,ButtonCanvas1		->Checked?1:0);
	fprintf(file,"Main canvas Never=%i\n"				,NeverCanvas1		->Checked?1:0);


	fprintf(file,"\nIncrease/Decrease ink behaviour:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Cap min/max=%i\n"						,IncDecCap1			->Checked?1:0);
	fprintf(file,"Wrap around=%i\n"						,IncDecWraparound1	->Checked?1:0);

	fprintf(file,"\nIncrease/Decrease ink application:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Per click=%i\n"						,IncDecPerclick1	->Checked?1:0);
	fprintf(file,"Over distance=%i\n"					,OverDistance1		->Checked?1:0);

	fprintf(file,"\nIncrease/Decrease ink flow:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Quickest=%i\n"						,IncDecFlow1		->Checked?1:0);
	fprintf(file,"Quick=%i\n"							,IncDecFlow2		->Checked?1:0);
	fprintf(file,"Medium=%i\n"							,IncDecFlow3		->Checked?1:0);
	fprintf(file,"Slow=%i\n"							,IncDecFlow4		->Checked?1:0);
	fprintf(file,"Slowest=%i\n"							,IncDecFlow5		->Checked?1:0);

	fprintf(file,"\nStartup settings part 1:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"GUI Scale x2=%i\n"			,prefStartScale2x		?1:0);
	fprintf(file,"GUI Scale x3=%i\n"			,prefStartScale3x	   	?1:0);
	fprintf(file,"GUI Scale x4=%i\n"			,prefStartScale4x	  	?1:0);

	fprintf(file,"Preselected colour 0=%i\n"	,prefStartCol0		?1:0);
	fprintf(file,"Preselected colour 1=%i\n"	,prefStartCol1	   	?1:0);
	fprintf(file,"Preselected colour 2=%i\n"	,prefStartCol2	  	?1:0);
	fprintf(file,"Preselected colour 3=%i\n"	,prefStartCol3	  	?1:0);

	fprintf(file,"Preselected subpalette 0=%i\n"	,prefStartSubpal0	?1:0);
	fprintf(file,"Preselected subpalette 1=%i\n"	,prefStartSubpal1	?1:0);
	fprintf(file,"Preselected subpalette 2=%i\n"	,prefStartSubpal2	?1:0);
	fprintf(file,"Preselected subpalette 3=%i\n"	,prefStartSubpal3	?1:0);

	//checkbox options
	fprintf(file,"\n\nCheckbox options:\n");
	fprintf(file,"=========================================================\n");

	fprintf(file,"\nNotification settings:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Notify before entering linked chr mode=%i\n", bNotify_linkedmodewarning);

	fprintf(file,"\nEditor bitmask options:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Apply to pen=%i\n"					,Applytopen1		->Checked?1:0);
	fprintf(file,"Apply to rotate=%i\n"					,Applytorotate1		->Checked?1:0);
	fprintf(file,"Apply to mirror=%i\n"					,Applytomirror1		->Checked?1:0);
	fprintf(file,"Apply to nudge=%i\n"					,Applytonudge1		->Checked?1:0);
	fprintf(file,"Apply to paste=%i\n"					,Applytopaste1		->Checked?1:0);

	fprintf(file,"\nMetasprite text export:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Ask for metasprite name=%i\n"			,AskMetaName1		->Checked?1:0);
	fprintf(file,"Ask for metasprite bank name=%i\n"	,AskBankName1		->Checked?1:0);

	fprintf(file,"\nFind/Remove/Sort options:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Sort tiles on removal=%i\n"			,sortonremoval1		->Checked?1:0);
	fprintf(file,"Force: look on active tab only=%i\n"			,ForceActiveTab1	->Checked?1:0);
	fprintf(file,"Include NT/map=%i\n"					,IncludeNametables1	->Checked?1:0);
	fprintf(file,"Include metasprites=%i\n"			,IncludeMetasprites1->Checked?1:0);

	fprintf(file,"\nMisc. Grids and guides options:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Yellow metasprite scanline warning=%i\n"	,bWarnMsprYellow	?1:0);
	fprintf(file,"Orange metasprite scanline warning=%i\n"	,bWarnMsprOrange	?1:0);
	fprintf(file,"Red metasprite scanline warning=%i\n"		,bWarnMsprRed		?1:0);
	fprintf(file,"Autoshow grid while dragging=%i\n"		,AutoViewDragMode1->Checked?1:0);

	fprintf(file,"\nMisc. Editor options:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Autostore last used colour=%i\n"	,AutostoreLastUsed->Checked	?1:0);


	fprintf(file,"\nWorkspace options part 2:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Snap forms to monitor edge=%i\n"		,bSnapToScreen?1:0);
	fprintf(file,"Lightbox mode alpha=%i\n"				,iGlobalAlpha);

	fprintf(file,"\nNametable binary export:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Include nametable=%i\n"				,MSaveIncName		->Checked?1:0);
	fprintf(file,"Include attributes=%i\n"				,MSaveIncAttr		->Checked?1:0);
	fprintf(file,"Perform RLE compression=%i\n"			,MSaveRLE			->Checked?1:0);

	fprintf(file,"\nBMP import:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Best offsets=%i\n"				,MImportBestOffsets	->Checked?1:0);
	fprintf(file,"Lossy import=%i\n"				,MImportLossy		->Checked?1:0);
	fprintf(file,"Tile threshold=%i\n"				,MImportThreshold	->Checked?1:0);
	fprintf(file,"Without colour data=%i\n"			,MImportNoColorData	->Checked?1:0);

	fprintf(file,"\nPalette text export:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Include filename in palette label=%i\n"			,bExportPalFilename?1:0);
	fprintf(file,"Include set in palette label=%i\n"				,bExportPalSet?1:0);


	fprintf(file,"\nAdvanced preferences:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Force 0x0f=%i\n"						,SafeColours		->Checked?1:0);
	fprintf(file,"Force shared BG colour=%i\n"		   	,SharedBGcol		->Checked?1:0);

	fprintf(file,"\nStartup settings part 2:\n");
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"Show CHR editor=%i\n"				,prefStartShowCHR		?1:0);
	fprintf(file,"Show Metasprite manager=%i\n"		,prefStartShowMM		?1:0);

	fprintf(file,"Show Brushes Toolbox=%i\n"		,prefStartShowBrushes	?1:0);
	fprintf(file,"Show Bucket Toolbox=%i\n"		,prefStartShowBucket		?1:0);
	fprintf(file,"Show Lines Toolbox=%i\n"		,prefStartShowLines			?1:0);


	fprintf(file,"Show grid=%i\n"					,prefStartGridShow		?1:0);
	fprintf(file,"Grid x1=%i\n"						,prefStartGrid1			?1:0);
	fprintf(file,"Grid x2=%i\n"						,prefStartGrid2			?1:0);
	fprintf(file,"Grid x4=%i\n"						,prefStartGrid4			?1:0);
	fprintf(file,"Grid 32x30=%i\n"					,prefStartGrid32x30		?1:0);
	fprintf(file,"Grid CHR Pixels=%i\n"				,prefStartGridPixelCHR	?1:0);
	fprintf(file,"Grid CHR Tiles=%i\n"				,prefStartGridTilesCHR	?1:0);
	fprintf(file,"Grid CHR Midpoints=%i\n"			,prefStartGridMidpointsCHR	?1:0);



	fclose(file);
	BlockDrawing(false);

}

bool __fastcall TFormMain::LoadConfig(void)
{
	FILE *file;
	char *text;
	int i,j,size;

	file=fopen((globalDir+"preferences.cfg").c_str(),"rb");

	if(!file) return false;

	BlockDrawing(true);

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	text=(char*)malloc(size+1);
	text[size]=0;

	fread(text,size,1,file);
	fclose(file);


	if(CheckTag(text,size,"Notify before entering linked chr mode=")) {
		bNotify_linkedmodewarning = nss_get_bool(text,size,"Notify before entering linked chr mode=");
	} else { bNotify_linkedmodewarning=true;}
	
	bImportCHRWrap=nss_get_bool(text,size,"CHR Import Wrap=");
	bImportCHRCarry=nss_get_bool(text,size,"CHR Import Carry=");
	bImportCHRSkip=nss_get_bool(text,size,"CHR Import Ignore=");

	byte1					->Checked=nss_get_bool(text,size,".byte=");
	db1						->Checked=nss_get_bool(text,size,".db=");

	SignCa65				->Checked=nss_get_bool(text,size,"Ca65 Sign=");
	SignOther				->Checked=nss_get_bool(text,size,"Normal Sign=");

	Noterminator1			->Checked=nss_get_bool(text,size,"None=");
	Spritecountheader1		->Checked=nss_get_bool(text,size,"Sprite count=");
	Nflagterminator1		->Checked=nss_get_bool(text,size,"N flag=");
	FFterminator1			->Checked=nss_get_bool(text,size,"$FF=");
	Single00terminator1		->Checked=nss_get_bool(text,size,"Single $00=");
	Double00terminator1		->Checked=nss_get_bool(text,size,"Double $00=");
	
	MASCIIneg20h			->Checked=nss_get_bool(text,size,"-0x20=");
	MASCIIneg30h			->Checked=nss_get_bool(text,size,"-0x30=");
	MASCIIneg40h			->Checked=nss_get_bool(text,size,"-0x40=");

	Sprlistl1				->Checked=nss_get_bool(text,size,"Spritelist Left=");
	Sprlistc1				->Checked=nss_get_bool(text,size,"Spritelist Center=");
	CHReditortoolbartop		->Checked=nss_get_bool(text,size,"CHR tools Top=");
	CHReditortoolbarbottom	->Checked=nss_get_bool(text,size,"CHR tools Bottom=");

	if(Sprlistl1->Checked) GroupBoxMetaSprite->Align=alRight;
	if(Sprlistc1->Checked) GroupBoxMetaSprite->Align=alLeft;

	//CHR EDITOR fixed in its own createForm event.

	bool tmpRadioOpenSave1=nss_get_bool(text,size,"OSBA in File menu=");
	bool tmpRadioOpenSave2=nss_get_bool(text,size,"OSBA in type menus=");
	bool tmpRadioOpenSave3=nss_get_bool(text,size,"OSBA in both=");
	if	 (tmpRadioOpenSave1)   iRadioOpenSave=1;
	else if(tmpRadioOpenSave2) iRadioOpenSave=2;
	else if(tmpRadioOpenSave3) iRadioOpenSave=3;
	else iRadioOpenSave=1;

	bool tmpShowFilepath1=nss_get_bool(text,size,"Show filename=");
	bool tmpShowFilepath2=nss_get_bool(text,size,"Show file and folder=");
	bool tmpShowFilepath3=nss_get_bool(text,size,"Show full path=");
	if	 (tmpShowFilepath1)   iShowFilepath=1;
	else if(tmpShowFilepath2) iShowFilepath=2;
	else if(tmpShowFilepath3) iShowFilepath=3;
	else iShowFilepath=2;

	AlwaysNavigator1		->Checked=nss_get_bool(text,size,"Navigator Always=");
	MouseNavigator1         ->Checked=nss_get_bool(text,size,"Navigator MouseOver=");
	MouseButtonNavigator1   ->Checked=nss_get_bool(text,size,"Navigator Mouse or Button=");
	ButtonNavigator1        ->Checked=nss_get_bool(text,size,"Navigator Button=");
	NeverNavigator1         ->Checked=nss_get_bool(text,size,"Navigator Never=");

	AlwaysCanvas1			->Checked=nss_get_bool(text,size,"Main canvas Always=");
	MouseCanvas1         	->Checked=nss_get_bool(text,size,"Main canvas MouseOver=");
	MouseButtonCanvas1   	->Checked=nss_get_bool(text,size,"Main canvas Mouse or Button=");
	ButtonCanvas1        	->Checked=nss_get_bool(text,size,"Main canvas Button=");
	NeverCanvas1         	->Checked=nss_get_bool(text,size,"Main canvas Never=");

	IncDecCap1				->Checked=nss_get_bool(text,size,"Cap min/max=");
	IncDecWraparound1		->Checked=nss_get_bool(text,size,"Wrap around=");

	IncDecPerclick1			->Checked=nss_get_bool(text,size,"Per click=");
	OverDistance1			->Checked=nss_get_bool(text,size,"Over distance=");

	IncDecFlow1				->Checked=nss_get_bool(text,size,"Quickest=");
	IncDecFlow2				->Checked=nss_get_bool(text,size,"Quick=");
	IncDecFlow3				->Checked=nss_get_bool(text,size,"Medium=");
	IncDecFlow4				->Checked=nss_get_bool(text,size,"Slow=");
	IncDecFlow5				->Checked=nss_get_bool(text,size,"Slowest=");

	Applytopen1				->Checked=nss_get_bool(text,size,"Apply to pen=");
	Applytorotate1			->Checked=nss_get_bool(text,size,"Apply to rotate=");
	Applytomirror1			->Checked=nss_get_bool(text,size,"Apply to mirror=");
	Applytonudge1			->Checked=nss_get_bool(text,size,"Apply to nudge=");
	Applytopaste1			->Checked=nss_get_bool(text,size,"Apply to paste=");

	AskMetaName1			->Checked=nss_get_bool(text,size,"Ask for metasprite name=");
	AskBankName1			->Checked=nss_get_bool(text,size,"Ask for metasprite bank name=");

	sortonremoval1			->Checked=nss_get_bool(text,size,"Sort tiles on removal=");
	ForceActiveTab1			->Checked=nss_get_bool(text,size,"Force: look on active tab only=");
	IncludeNametables1		->Checked=nss_get_bool(text,size,"Include NT/map=");
	IncludeMetasprites1		->Checked=nss_get_bool(text,size,"Include metasprites=");

	MSaveIncName			->Checked=nss_get_bool(text,size,"Include nametable=");
	MSaveIncAttr			->Checked=nss_get_bool(text,size,"Include attributes=");
	MSaveRLE				->Checked=nss_get_bool(text,size,"Perform RLE compression=");

	SafeColours				->Checked=nss_get_bool(text,size,"Force 0x0f=");
	SharedBGcol				->Checked=nss_get_bool(text,size,"Force shared BG colour=");
	sharedCol0=SharedBGcol->Checked;

	bWarnMsprYellow                  =nss_get_bool(text,size,"Yellow metasprite scanline warning=");
	bWarnMsprOrange                  =nss_get_bool(text,size,"Orange metasprite scanline warning=");
	bWarnMsprRed                     =nss_get_bool(text,size,"Red metasprite scanline warning=");
	AutoViewDragMode1		->Checked=nss_get_bool(text,size,"Autoshow grid while dragging=");

	AutostoreLastUsed		->Checked=nss_get_bool(text,size,"Autostore last used colour=");

	bSnapToScreen					 =nss_get_bool(text,size,"Snap forms to monitor edge=");
	iGlobalAlpha					 =nss_get_int(text,size,"Lightbox mode alpha=");
	if(iGlobalAlpha<140) iGlobalAlpha=140;
	if(iGlobalAlpha>220) iGlobalAlpha=220;

	prefStartShowCHR		=nss_get_bool(text,size,"Show CHR editor=");
	prefStartShowMM			=nss_get_bool(text,size,"Show Metasprite manager=");

	prefStartShowBrushes	=nss_get_bool(text,size,"Show Brushes Toolbox=");
	prefStartShowBucket		=nss_get_bool(text,size,"Show Bucket Toolbox=");
	prefStartShowLines		=nss_get_bool(text,size,"Show Lines Toolbox=");


	bExportPalFilename		=nss_get_bool(text,size,"Include filename in palette label=");
	bExportPalSet           =nss_get_bool(text,size,"Include set in palette label=");

	SpeedButtonGridTile		->Down=nss_get_bool(text,size,"Grid x1=");
	SpeedButtonGridAtr		->Down=nss_get_bool(text,size,"Grid x2=");
	SpeedButtonGridBlock	->Down=nss_get_bool(text,size,"Grid x4=");
	SpeedButtonGridAll		->Down=nss_get_bool(text,size,"Show grid=");
	SpeedButtonGridScreen   ->Down=nss_get_bool(text,size,"Grid 32x30=");

	CHRpixelgrid1			->Checked=nss_get_bool(text,size,"Grid CHR Pixels=");
	ShowCHREditortilegrid1	->Checked=nss_get_bool(text,size,"Grid CHR Tiles=");
    ShowmidgridinCHReditor1 ->Checked=nss_get_bool(text,size,"Grid CHR Midpoints=");
	//SpeedButtonGridAll->Caption=SpeedButtonGridAll->Down?"on":"off";

	prefStartGrid1 		= SpeedButtonGridTile->Down;
	prefStartGrid2 		= SpeedButtonGridAtr->Down;
	prefStartGrid4 		= SpeedButtonGridBlock->Down;
	prefStartGridShow 	= SpeedButtonGridAll->Down;
	prefStartGrid32x30   =SpeedButtonGridScreen->Down;
	prefStartGridPixelCHR=CHRpixelgrid1->Checked;

	prefStartScale2x	=nss_get_bool(text,size,"GUI Scale x2=");
	prefStartScale3x    =nss_get_bool(text,size,"GUI Scale x3=");
	prefStartScale4x    =nss_get_bool(text,size,"GUI Scale x4=");

	prefStartCol0       =nss_get_bool(text,size,"Preselected colour 0=");
	prefStartCol1       =nss_get_bool(text,size,"Preselected colour 1=");
	prefStartCol2       =nss_get_bool(text,size,"Preselected colour 2=");
	prefStartCol3       =nss_get_bool(text,size,"Preselected colour 3=");

	prefStartSubpal0    =nss_get_bool(text,size,"Preselected subpalette 0=");
	prefStartSubpal1    =nss_get_bool(text,size,"Preselected subpalette 1=");
	prefStartSubpal2    =nss_get_bool(text,size,"Preselected subpalette 2=");
	prefStartSubpal3    =nss_get_bool(text,size,"Preselected subpalette 3=");



	BlockDrawing(false);
	return true;
}

void __fastcall TFormMain::UpdateRGBM(void)
{
	SpeedButtonMaskB->Caption="B"+IntToStr((ppuMask&0x80?1:0));
	SpeedButtonMaskG->Caption="G"+IntToStr((ppuMask&0x40?1:0));
	SpeedButtonMaskR->Caption="R"+IntToStr((ppuMask&0x20?1:0));
	SpeedButtonMaskM->Caption="M"+IntToStr((ppuMask&0x01?1:0));

}



bool __fastcall TFormMain::MouseTypeIn(int X,int Y)
{
	if(SpeedButtonTypeIn->Down)
	{
		nameSelection.left  =nameTableViewX+X/(8*uiScale);
		nameSelection.top   =nameTableViewY+Y/(8*uiScale);
		nameSelection.right =nameSelection.left+1;
		nameSelection.bottom=nameSelection.top +1;

		UpdateNameTable(-1,-1,true);
        if(FormNavigator->Visible)
		{
			//FormNavigator->DrawRange(nameSelection.left,nameSelection.top,nameSelection.right,nameSelection.bottom);
			FormNavigator->UpdateLines(true);
		}

		//FormNavigator->Draw(false,false);
		UpdateStats();

		return true;
	}

	return false;
}



void __fastcall TFormMain::NameTableTypeIn(int tile)
{
	int dx,dy;

	if(nameSelection.left>=0&&nameSelection.right>=0)
	{
		dx=nameSelection.left;
		dy=nameSelection.top;

		SetUndo();

		if(SpeedButtonTiles->Down) nameTable[dy*nameTableWidth+dx]=(tile+typeInFontOffset)&255;

		if(SpeedButtonPal->Down) AttrSet(dx,dy,palActive,false);

		++nameSelection.left;

		if(nameSelection.left>=nameTableWidth) nameSelection.left=nameTableWidth-1;
		if(nameSelection.left<0) nameSelection.left=0;

		nameSelection.right =nameSelection.left+1;
		nameSelection.bottom=nameSelection.top+1;

		UpdateNameTable(dx,dy,false);
		//FormNavigator->Draw(false,false); //bookmark: monitor for performance
        if(FormNavigator->Visible)
		{
			FormNavigator->DrawRange(dx,dy,dx+1,dy+1,false);
			FormNavigator->UpdateLines(true);
		}
		UpdateStats();


	}
}



void __fastcall TFormMain::DrawMetaSprite(TImage *img,int spr_id,int scale,bool grid,bool frame_all,bool frame_none)
{
	int i,j,x,y,cy,cnt,tile,attr,pp,hcol,vcol,bank;
	char str[128];
	TColor frame;
	unsigned char *d1;
	bool sel[64];
	int check[128];
	//bool bSilhouette=btnSilhouetteMetasprite->Down;
	//clear

	img->Picture->Bitmap->Canvas->Brush->Style=bsSolid;
	img->Picture->Bitmap->Canvas->Brush->Color=(TColor)outPalette[bgPal[palBank*16+0]];
	img->Picture->Bitmap->Canvas->FillRect(TRect(0,0,128*scale,128*scale));

	//draw grid

	if(grid)
	{
		for(i=0;i<img->Width;i+=8*scale)
		{
			d1=(unsigned char*)img->Picture->Bitmap->ScanLine[i];

			for(j=0;j<img->Width;j+=2)
			{
				hcol=(spriteGridY*scale==i)?128:64;

				*d1+++=hcol;
				*d1+++=hcol;
				*d1+++=hcol;

				d1+=3;
			}

			for(j=1;j<img->Height;j+=2)
			{
				d1=(unsigned char*)img->Picture->Bitmap->ScanLine[j]+i*3;

				vcol=(spriteGridX*scale==i)?128:64;

				*d1+++=vcol;
				*d1+++=vcol;
				*d1+++=vcol;
			}

			*d1=*d1;//to prevent warning
		}
	}

	//draw sprites

	pp=spr_id*64*4+63*4;

	memset(check,0,sizeof(check));

	for(i=63;i>=0;--i)//reverse order to make proper sprites drawing priority
	{
		y   =metaSprites[pp+0];
		tile=metaSprites[pp+1];
		attr=metaSprites[pp+2];
		x   =metaSprites[pp+3];

		if(y<255)
		{
			frame=frame_all?clGray:clBlack;


			if(i<ListBoxSpriteList->Items->Count)
				if(!frame_none&&(ListBoxSpriteList->Selected[i]))
				frame=clWhite; //clMenu if we need the distinction
			if(!frame_none&&(spriteActive==i)) frame=clWhite;

			DrawSpriteTile(img->Picture,x,y,tile,attr,frame,scale);

			cy=y;

			for(j=0;j<(SpeedButtonSprite8x16->Down?16:8);++j)
			{
				if(cy>=-63&&cy<128) ++check[cy];

				++cy;
			}
		}

		pp-=4;
	}

	for(i=0;i<128;++i)
	{
		if(check[i]>4 && bWarnMsprYellow)
		{
			DrawSpriteDot(img->Picture,1,img->Height/scale/2-64+i,(TColor)0x00daff,scale);
			DrawSpriteDot(img->Picture,126,img->Height/scale/2-64+i,(TColor)0x00daff,scale);
		}
		if(check[i]>7 && bWarnMsprOrange)
		{
			DrawSpriteDot(img->Picture,1,img->Height/scale/2-64+i,(TColor)0x0090f0,scale);
			DrawSpriteDot(img->Picture,2,img->Height/scale/2-64+i,(TColor)0x0090f0,scale);
			DrawSpriteDot(img->Picture,126,img->Height/scale/2-64+i,(TColor)0x0090f0,scale);
			DrawSpriteDot(img->Picture,125,img->Height/scale/2-64+i,(TColor)0x0090f0,scale);
		}
		if(check[i]>8 && bWarnMsprRed)
		{
			DrawSpriteDot(img->Picture,1,img->Height/scale/2-64+i,(TColor)0x2222e0,scale);
			DrawSpriteDot(img->Picture,2,img->Height/scale/2-64+i,(TColor)0x2222e0,scale);
			DrawSpriteDot(img->Picture,3,img->Height/scale/2-64+i,(TColor)0x2222e0,scale);
			DrawSpriteDot(img->Picture,126,img->Height/scale/2-64+i,(TColor)0x2222e0,scale);
			DrawSpriteDot(img->Picture,125,img->Height/scale/2-64+i,(TColor)0x2222e0,scale);
			DrawSpriteDot(img->Picture,124,img->Height/scale/2-64+i,(TColor)0x2222e0,scale);
		}
		if(check[i]>iWarnMsprCyan && bWarnMsprCyan)
		{
			DrawSpriteDot(img->Picture,0,img->Height/scale/2-64+i,(TColor)0xaaaa00,scale);
			DrawSpriteDot(img->Picture,127,img->Height/scale/2-64+i,(TColor)0xaaaa00,scale);


		}
	}

	img->Repaint();
}


void __fastcall TFormMain::DrawMetaSpriteExport(TPicture *img,int spr_id,int scale,bool grid,bool frame_all,bool frame_none)
{
	int i,j,x,y,cy,cnt,tile,attr,pp,hcol,vcol,bank;
	char str[128];
	TColor frame;
	unsigned char *d1;
	bool sel[64];
	int check[128];

	//clear

	img->Bitmap->Canvas->Brush->Style=bsSolid;
	img->Bitmap->Canvas->Brush->Color=(TColor)outPalette[bgPal[palBank*16+0]];
	img->Bitmap->Canvas->FillRect(TRect(0,0,128*scale,128*scale));

	//draw grid
	/*
	if(grid)
	{
		for(i=0;i<img->Width;i+=8*scale)
		{
			d1=(unsigned char*)img->Bitmap->ScanLine[i];

			for(j=0;j<img->Width;j+=2)
			{
				hcol=(spriteGridY*scale==i)?128:64;

				*d1+++=hcol;
				*d1+++=hcol;
				*d1+++=hcol;

				d1+=3;
			}

			for(j=1;j<img->Height;j+=2)
			{
				d1=(unsigned char*)img->Bitmap->ScanLine[j]+i*3;

				vcol=(spriteGridX*scale==i)?128:64;

				*d1+++=vcol;
				*d1+++=vcol;
				*d1+++=vcol;
			}

			*d1=*d1;//to prevent warning
		}
	} */

	//draw sprites

	pp=spr_id*64*4+63*4;

	memset(check,0,sizeof(check));

	for(i=63;i>=0;--i)//reverse order to make proper sprites drawing priority
	{
		y   =metaSprites[pp+0];
		tile=metaSprites[pp+1];
		attr=metaSprites[pp+2];
		x   =metaSprites[pp+3];

		if(y<255)
		{
			frame=frame_all?clGray:clBlack;


			if(i<ListBoxSpriteList->Items->Count)
				if(!frame_none&&(ListBoxSpriteList->Selected[i]))
				frame=clWhite; //clMenu if we need the distinction
			if(!frame_none&&(spriteActive==i)) frame=clWhite;
			DrawSpriteTile(img,x,y,tile,attr,frame,scale);

			cy=y;

			for(j=0;j<(SpeedButtonSprite8x16->Down?16:8);++j)
			{
				if(cy>=-63&&cy<64) ++check[cy];

				++cy;
			}
		}

		pp-=4;
	}

	/*for(i=0;i<128;++i)
	{
		if(check[i]>8)
		{
			DrawSpriteDot(img,1,img->Height/scale/2-64+i,(TColor)0x0000ff,scale);
			DrawSpriteDot(img,126,img->Height/scale/2-64+i,(TColor)0x0000ff,scale);
		}
	}

	//img->Repaint(); */
}

//BROKE STUDIO
void __fastcall TFormMain::UpdateMetaSpriteLabel(void)
{
	LabelMetaSprite->Caption=IntToStr(metaSpriteActive)+": "+metaSpriteNames[metaSpriteActive];
}
//

void __fastcall TFormMain::UpdateMetaSprite(bool rebuildList)
{
	int i,j,x,y,cnt,tile,attr,pp,hcol,vcol,bank;
	char str[128];
	TColor frame;
	unsigned char *d1;
	bool sel[64];
	cueUpdateMetasprite=false;
	DrawMetaSprite(ImageMetaSprite,metaSpriteActive,uiScale,SpeedButtonSpriteGrid->Down,SpeedButtonFrameAll->Down,SpeedButtonFrameNone->Down);

	//update list
	if(rebuildList){
		//int tmp_item=ListBoxSpriteList->ItemIndex;

		pp=metaSpriteActive*64*4;
		cnt=0;

		for(i=0;i<64;++i)
		{
			if(metaSprites[pp]<255) ++cnt;

			pp+=4;
		}

		if(spriteActive>cnt-1) spriteActive=cnt-1;

		if(spriteActive<0) spriteActive=-1;

		for(i=0;i<64;++i)
		{
			if(i<ListBoxSpriteList->Items->Count) sel[i]=ListBoxSpriteList->Selected[i]; else sel[i]=false;
		}

		if(spriteActive>=0) sel[spriteActive]=true;

		ListBoxSpriteList->Clear();

		pp=metaSpriteActive*64*4;

		for(i=0;i<64;++i)
		{
			if(metaSprites[pp]<255)
			{
				x   =metaSprites[pp+3]-spriteGridX;
				y   =metaSprites[pp+0]-spriteGridY;
				tile=metaSprites[pp+1];
				bank=metaSprites[pp+1]&1;

				if(SpeedButtonSprite8x16->Down) tile&=0xfe;

				sprintf(str,"%2.2x: X%c%3.3i Y%c%3.3i Tile $%2.2x Pal %i ",i,x<0?'-':' ',abs(x),y<0?'-':' ',abs(y),tile,metaSprites[pp+2]&3);

				if(SpeedButtonSprite8x16->Down) strcat(str,!bank?"A ":"B ");

				if(metaSprites[pp+2]&OAM_FLIP_V) strcat(str,"V");
				if(metaSprites[pp+2]&OAM_FLIP_H) strcat(str,"H");
				if(metaSprites[pp+2]&OAM_PRIO)	 strcat(str,"P");
				if(metaSprites[pp+2]&OAM_B4) 	 strcat(str,"4");
				if(metaSprites[pp+2]&OAM_B3) 	 strcat(str,"3");
				if(metaSprites[pp+2]&OAM_B2) 	 strcat(str,"2");
				ListBoxSpriteList->Items->Add(str);

				ListBoxSpriteList->Selected[ListBoxSpriteList->Items->Count-1]=sel[i];
			}

			pp+=4;
		}
	//ListBoxSpriteList->ItemIndex=tmp_item;

	if(spriteActive>=0) ListBoxSpriteList->ItemIndex=spriteActive;
	}
	//LabelMetaSprite->Caption="Metasprite "+IntToStr(metaSpriteActive);
	//^disabled to accomodate the below merge
	//BROKE STUDIO
	UpdateMetaSpriteLabel();   //might want to move to MM update routine?
	//


	cueUpdateMM=true;

}



void squeeze_sprites(void)
{
	int i,j,k,pp;

	pp=0;

	for(i=0;i<256;++i)
	{
		for(j=0;j<63;++j)
		{
			if(metaSprites[pp+j*4]==255)
			{
				memcpy(&metaSprites[pp+j*4],&metaSprites[pp+j*4+4],64*4-j*4-4);

				for(k=0;k<4;++k) metaSprites[pp+63*4+k]=255;
			}
		}

		pp+=64*4;
	}
	if(spriteActive>=0)pp=metaSpriteActive*64*4+spriteActive*4;
	else pp=metaSpriteActive*64*4;

	while(pp>=4)
	{
		if(metaSprites[pp]==255) --spriteActive; else break;

		pp-=4;
	}
}



void __fastcall TFormMain::MoveSprite(int dx,int dy)
{
	int i,off;

	if(SpeedButtonSpriteSnap->Down)
	{
		dx*=8;
		dy*=8;
	}

	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			off=metaSpriteActive*64*4+i*4;

			if(metaSprites[off]<255)
			{
				if(SpeedButtonSpriteSnap->Down)
				{
					if(dy!=0)
					{
						if(metaSprites[off+0]&7) metaSprites[off+0]&=~7; 
						else metaSprites[off+0]+=dy;
						if (metaSprites[off+0]==255) metaSprites[off+0]==248; //Y must not be 255 since this is handled as nonexistence
					}

					if(dx!=0)
					{
						if(metaSprites[off+3]&7) metaSprites[off+3]&=~7; else metaSprites[off+3]+=dx;
					}
				}
				else
				{
					if(!(metaSprites[off+0]+dy==255)) metaSprites[off+0]+=dy;
					else
					{
                        LabelStats->Caption="WARNING: Cannot move sprite(s) into Ypos 255 (this value signifies nonexistence in NEXXT).";
						StatusUpdateWaiter->Enabled=true;
						holdStats=true;
                    }
					metaSprites[off+3]+=dx;
				}
			}
		}
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}




int __fastcall TFormMain::GetSpriteID(int x,int y)
{
	int i,pp,hgt;

	if(SpeedButtonSprite8x16->Down) hgt=16; else hgt=8;

	pp=metaSpriteActive*64*4;

	x/=uiScale;
	y/=uiScale;

	for(i=0;i<64;++i)
	{
		if(y>=metaSprites[pp]&&y<metaSprites[pp]+hgt&&x>=metaSprites[pp+3]&&x<metaSprites[pp+3]+8) return i;

		pp+=4;
	}

	return -1;
}



void __fastcall TFormMain::SpriteSnap(int id)
{
	int x,y,off;

	off=metaSpriteActive*64*4+id*4;

	if(metaSprites[off]<255)
	{
		x=metaSprites[off+3];
		y=metaSprites[off+0];

		if((x&7)<4) x&=~7; else x=(x&~7)+8;
		if((y&7)<4) y&=~7; else y=(y&~7)+8;

		metaSprites[off+3]=x;
		metaSprites[off+0]=y;
	}
}



void __fastcall TFormMain::SelectSprite(int id,bool multi)
{
	int i,off;
	bool b8x16=(SpeedButtonSprite8x16->Down);

	spriteActive=id;

	if(!multi)
	{
		for(i=0;i<ListBoxSpriteList->Items->Count;++i)
		{
			ListBoxSpriteList->Selected[i]=(i==id)?true:false;
		}
	}

	if(spriteActive>=0)
	{
		off=metaSpriteActive*64*4+spriteActive*4;

		if(metaSprites[off]<255)
		{

			if(b8x16)	SelectTile8x16   (metaSprites[off+1]);
			else		SelectTile       (metaSprites[off+1]);

			SelectPalette(metaSprites[off+2]&3);

			UpdateTiles(true);
		}
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}

void __fastcall TFormMain::SelectTile8x16(int id)
{
	int i,w,h;
    //which tile viewmode?
	bool b8x16=(ButtonForceView->Down || N8x161->Checked);
	if(b8x16) {
		tileActive=Get_8x16pos(id);
		w=1;
		h=2;
	}
	else{
		tileActive = ((id/2)*2);
		w=2;
		h=1;
	}

	chrSelection.left  =tileActive&15;
	chrSelection.top   =tileActive/16;
	chrSelection.right =chrSelection.left + w;
	chrSelection.bottom=chrSelection.top  + h;

	for(i=0;i<256;++i) chrSelected[i]=0;

	chrSelected[tileActive]=1;
	chrSelectRect=true;
    tileSelRectWdt      = chrSelection.right-chrSelection.left;
	tileSelRectHgt		= chrSelection.bottom-chrSelection.top;

	if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
	if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
}


void __fastcall TFormMain::SelectTile(int id)
{
	int i;
	bool b8x16=(SpeedButtonSprite8x16->Down && (ButtonForceView->Down || N8x161->Checked));

	tileActive=id;

	int hgt= b8x16?2:1;

	chrSelection.left  =tileActive&15;
	chrSelection.top   =tileActive/16;
	chrSelection.right =chrSelection.left + 1;
	chrSelection.bottom=chrSelection.top  + hgt;

	for(i=0;i<256;++i) chrSelected[i]=0;

	chrSelected[tileActive]=1;
	chrSelectRect=true;
    tileSelRectWdt      = chrSelection.right-chrSelection.left;
	tileSelRectHgt		= chrSelection.bottom-chrSelection.top;

	if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
	if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
}



void __fastcall TFormMain::SelectPalette(int id)
{
	palActive=id;
	DrawPalettes();
}



bool __fastcall TFormMain::OpenMetaSprites(AnsiString name, bool openAll)
{
	FILE *file;
	int size;
	unsigned char data[2];

	file=fopen(name.c_str(),"rb");
	if((!file) && (openAll)) return false;
	if(!file){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+name;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);
		return false;
	}
	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	if(size!=256*64*4+2) return false;

	fread(data,2,1,file);
	fread(metaSprites,256*64*4,1,file);
	fclose(file);

	spriteGridX=data[0];
	spriteGridY=data[1];

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;

	return true;
}



void __fastcall TFormMain::FindDoublesUnused(bool unused)
{
	int i,j,cnt,used;
	int set=bankActive/16;
	bool forceActive = ForceActiveTab1->Checked;
	bool sweepNT = IncludeNametables1->Checked;
	bool sweepMS = IncludeMetasprites1->Checked;

	cnt=0;

	for(i=0;i<256;++i)
	{
		chrSelected[i]=0;

		if(unused)
		{
			used=0;

			if((forceActive)||!(sweepNT||sweepMS)) //if no sweep checkbox is set, also default to active.
			{
				if(PageControlEditor->ActivePage==TabSheetSprite)
					{for(j=0;j<256*64*4;j+=4) if(metaSprites[j+1]==i && metaSprites[j]!=0xFF) ++used;}
				else
					{for(j=0;j<nameTableWidth*nameTableHeight;++j)
					if(nameTable[j]==i) ++used;}
			}
			else
			{
				if(sweepNT) {
					for(j=0;j<nameTableWidth*nameTableHeight;++j) if(nameTable[j]==i) ++used;}
				if(sweepMS) {
					for(j=0;j<256*64*4;j+=4) if(metaSprites[j+1]==i && metaSprites[j]!=0xFF) ++used;}
			}

			if(!used)
			{
				chrSelected[ViewTablePos(i)]=1;
				++cnt;
			}
		}
		else
		{
			for(j=0;j<256;++j)
			{
				//skip comparisons against self & against empty tiles
				if(i!=j&&GetTileHasContents(chr+bankViewTable[set+j]+(j<<4)))
				{
					if(!memcmp(chr+bankViewTable[set+j]+(j<<4),chr+bankViewTable[set+i]+(i<<4),16))
					{
						chrSelected[ViewTablePos(i)]=1;
						++cnt;
					}
				}
			}
			/*
			//original NESST routine. Finds doubles of the current active tile.
			if(!memcmp(chr+bankActive+(tileActive<<4),chr+bankActive+(i<<4),16))
			{
				chrSelected[i]=1;
				++cnt;
			}
			*/
		}
	}

	if(cnt)
	{
		chrSelectRect=false;
		nameSelection.left=-1;
		nameSelection.top=-1;

		UpdateTiles(true);
		UpdateNameTable(-1,-1,true);
		//FormNavigator->Draw(false,false);
		if(openByFileDone) {
			if (FormNavigator->Visible) FormNavigator->Draw(true,true,false);
			if (FormCHRbit->Visible)	FormCHRbit->UpdateBitButtons(false);
			if(FormMTprops->Visible)    FormMTprops->UpdateBitButtons_tileClick(false);
		}
		UpdateStats();
	}
}



void __fastcall TFormMain::RemoveDoublesUnused(bool unused)
{
	int i,j,k,pp,x,y,w,h,used;
    int set=bankActive/16;

	bool clear[256],skip[256];

	bool forceActive = ForceActiveTab1->Checked;
	bool sweepNT = IncludeNametables1->Checked;
	bool sweepMS = IncludeMetasprites1->Checked;

	SetUndo();

	GetSelection(chrSelection,x,y,w,h);

	for(i=0;i<256;++i)
	{
		clear[i]=false;
		skip[i]=false;
	}

	//these allow nonselected tiles to be skipped.
	if(chrSelectRect)
	{
		if(w>1||h>1)
		{
			for(i=0;i<16;++i)
			{
				for(j=0;j<16;++j)
				{
					skip[i*16+j]=(i>=y&&i<y+h&&j>=x&&j<x+w)?false:true;
				}
			}
		}
	}
	else
	{
		for(i=0;i<256;++i) skip[i]= chrSelected[i]?false:true;
	}

	for(i=0;i<256;++i)
	{
		if(!clear[i]&&!skip[i])
		{
			if(unused)
			{
				used=0;

				if((forceActive)||!(sweepNT||sweepMS)) //if no sweep checkbox is set, also default to active.
				{
					if(PageControlEditor->ActivePage==TabSheetSprite)
						{for(j=0;j<256*64*4;j+=4) if(metaSprites[j+1]==i && metaSprites[j]!=0xFF) ++used;}
					else
						{for(j=0;j<nameTableWidth*nameTableHeight;++j)
							if(nameTable[j]==i) ++used;}
				}
				else
				{
					if(sweepNT) {
						for(j=0;j<nameTableWidth*nameTableHeight;++j) if(nameTable[j]==i) ++used;}
					if(sweepMS) {
						for(j=0;j<256*64*4;j+=4) //added an exception for sprites with an Y of FF since thatÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s used to imply nonexistence. 
							if(metaSprites[j+1]==i && metaSprites[j]!=0xFF) ++used;}
				}

				if(!used) clear[i]=true;
			}
			else  //action is for doubles
			{
				for(j=i+1;j<256;j++)
				{
					if(!memcmp(&chr[bankViewTable[set+i]+i*16],&chr[bankViewTable[set+j]+j*16],16)&&!skip[j])
					{
						clear[j]=true;

						for(k=0;k<nameTableWidth*nameTableHeight;k++) if(nameTable[k]==j) nameTable[k]=i;

						for(k=0;k<256*64*4;k+=4) if(metaSprites[k+1]==j) metaSprites[k+1]=i;
					}
				}
			}
		}
	}

	//now remove
	pp=0;
	int prep_p=0;
	if (sortonremoval1->Checked)
	{
		for(i=0;i<256;++i)
		{
			if(!clear[i]&&!skip[i])
			{
				while(skip[pp/16]) pp+=16;

				for(j=0;j<nameTableWidth*nameTableHeight;++j) if(nameTable[j]==i) nameTable[j]=pp/16;

				for(j=0;j<256*64*4;j+=4) if(metaSprites[j+1]==i) metaSprites[j+1]=pp/16;

				for(j=0;j<16;++j)
				{
					chr[bankViewTable[set+pp/16]+pp]=chr[bankViewTable[set+i]+i*16+j];

					++pp;
				}
				tileProperties[(bankViewTable[set+prep_p]/16)+prep_p]=tileProperties[(bankViewTable[set+i]/16)+i];
				++prep_p;
			}
		}
		for(;pp<4096;pp++) if(!skip[pp/16])			chr[bankViewTable[set+pp/16]+pp]=0;
		for(;prep_p<256;prep_p++) if(!skip[prep_p]) tileProperties[(bankViewTable[set+prep_p]/16)+prep_p]=0;
	}
	else {
		for(;pp<4096;pp++) if(clear[pp/16]&&!skip[pp/16]){
			chr[bankViewTable[set+pp/16]+pp]=0;
		}
		for(;prep_p<256;prep_p++) if(clear[prep_p]&&!skip[prep_p]){
			tileProperties[(bankViewTable[set+prep_p]/16)+prep_p]=0;
		}
	}

	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);
	if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
	if (FormNavigator->Visible) FormNavigator->Draw(false,false,false);
	if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);

	cueUpdateMetasprite=true;
}



bool __fastcall TFormMain::OverwritePrompt(AnsiString filename)
{
	if(FileExists(filename)) if(Application->MessageBox(("File "+filename+" already exists. Overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES) return false;

	return true;
}



void __fastcall TFormMain::SetLatestCHR(int offset,int size, bool fromSet)
{
	AnsiString str;

	CHRLatestOffset=offset;
	CHRLatestSize=size;
	CHRLatestFromSet=fromSet;
	str="Latest ";

	switch(size)
	{
	case 1024: str+="(1K"; break;
	case 2048: str+="(2K"; break;
	case 4096: str+="(4K"; break;
	case 8192: str+="(8K"; break;
	default: str+="(Selection"; break;
	}

	str+=" from tile $"+IntToHex(CHRLatestOffset/16,2)+")";

	MCHRSaveLatest->Caption=str;
}



void __fastcall TFormMain::UpdateAll(void)
{


     


	bool normal;

	if(nameTableWidth>=32&&nameTableHeight>=30) normal=true; else normal=false;
    Saveasscreen32x301->Enabled=normal;
	MSaveNameTableBIN->Enabled=normal;

	if(nameTableWidth==32&&nameTableHeight==30) normal=true; else normal=false;
	MSaveNameTableASM->Enabled=normal;
	MSaveNameTableC  ->Enabled=normal;

	if(FormSetSize)
	{
		FormSetSize->NewWidth =nameTableWidth;
		FormSetSize->NewHeight=nameTableHeight;

		FormSetSize->RadioButtonNormal->Checked=normal;
		FormSetSize->RadioButtonUser  ->Checked=!normal;
	}
	FormMain->AlphaBlendValue=iGlobalAlpha;
	FormMain->ScreenSnap=bSnapToScreen;

	pal_validate();

	CorrectView();
	DrawPalettes();
	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);
	if (FormNavigator!= NULL) {
		if(FormNavigator->Visible){
			FormNavigator->CueLinesTimer->Enabled=true;
			bNavDoSnap=false;
			bNavDoAlign=false;
			bNavDoZoomchange=false;
			FormNavigator->DelayedDraw->Enabled=false;  //reset timer on every event
			FormNavigator->DelayedDraw->Interval=50;
			FormNavigator->DelayedDraw->Enabled=true;
		}
	}

	//FormNavigator->Draw(false,false,false);
	if (FormBankCHR != NULL)FormBankCHR->Update();

	if (FormCHRbit != NULL) FormCHRbit->UpdateBitButtons(false);
	if(FormMTprops != NULL) FormMTprops->UpdateBitButtons_tileClick(false);

	if (FormMetatileTool != NULL)FormMetatileTool->UpdateUI(true);     //true=cue, false=direct
	if (FormPropConditions != NULL)FormPropConditions->SetConditions();
	if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;
	if (FormBrush != NULL) if(FormBrush->Visible) FormBrush->Draw(); 
	if (FormSwapBanks != NULL) if(FormSwapBanks->Visible) FormSwapBanks->FullUpdate(true);

	if (FormPropID != NULL) if(FormPropID->Visible) FormPropID->UpdateUI();


	cueUpdateMetasprite=true;
	cueStats=true;

}



void __fastcall TFormMain::CorrectView(void)
{
	if(nameTableWidth<32)
	{
		nameTableViewX=-(32-nameTableWidth)/2;
	}
	else
	{
		if(nameTableViewX>=nameTableWidth-32) nameTableViewX=nameTableWidth-32;
		if(nameTableViewX<0) nameTableViewX=0;
	}

	if(nameTableHeight<32)
	{
		nameTableViewY=-(32-nameTableHeight)/2;
	}
	else
	{

		if(nameTableViewY>=nameTableHeight-30) nameTableViewY=nameTableHeight-30;  //was 32, but led to a bug where you couldnÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´ scroll to the last portion of the map in some cases.
        if(nameTableViewY<0) nameTableViewY=0;
	}
}



void __fastcall TFormMain::FlipMetaSprites(bool flip_h,bool flip_v,bool flip_pos)
{
	int i,off,sx,sy,xs,xe,ys,ye,cx,cy,hwdt,hhgt;

	SetUndo();
	
	xs= 16834;
	xe=-16384;
	ys= 16384;
	ye=-16384;

	hwdt=4;
	hhgt=!SpeedButtonSprite8x16->Down?4:8;

	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			off=metaSpriteActive*64*4+i*4;

			if(metaSprites[off]<255)
			{
				sx=metaSprites[off+3]+hwdt;
				sy=metaSprites[off+0]+hhgt;

				if(sx<xs) xs=sx;
				if(sx>xe) xe=sx;
				if(sy<ys) ys=sy;
				if(sy>ye) ye=sy;
			}
		}
	}

	cx=xs+(xe-xs)/2;
	cy=ys+(ye-ys)/2;

	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			off=metaSpriteActive*64*4+i*4;

			if(metaSprites[off]<255)
			{
				if(flip_h)
				{
					metaSprites[off+2]^=OAM_FLIP_H;

					if(flip_pos) if(xs!=xe)
					{
						sx=metaSprites[off+3]-cx+hwdt;

						sx=-sx;

						metaSprites[off+3]=sx-hwdt+cx;
					}
				}

				if(flip_v)
				{
					metaSprites[off+2]^=OAM_FLIP_V;

					if(flip_pos) if(ys!=ye)
					{
						sy=metaSprites[off+0]-cy+hhgt;

						sy=-sy;

						metaSprites[off+0]=sy-hhgt+cy;
					}
				}
			}
		}
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}

void __fastcall TFormMain::FlipMetaSprites_justPosition(bool flip_h,bool flip_v)
{
	int i,off,sx,sy,xs,xe,ys,ye,cx,cy,hwdt,hhgt;

	SetUndo();
	
	xs= 16834;
	xe=-16384;
	ys= 16384;
	ye=-16384;

	hwdt=4;
	hhgt=!SpeedButtonSprite8x16->Down?4:8;

	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			off=metaSpriteActive*64*4+i*4;

			if(metaSprites[off]<255)
			{
				sx=metaSprites[off+3]+hwdt;
				sy=metaSprites[off+0]+hhgt;

				if(sx<xs) xs=sx;
				if(sx>xe) xe=sx;
				if(sy<ys) ys=sy;
				if(sy>ye) ye=sy;
			}
		}
	}

	cx=xs+(xe-xs)/2;
	cy=ys+(ye-ys)/2;

	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			off=metaSpriteActive*64*4+i*4;

			if(metaSprites[off]<255)
			{
				if(flip_h)
				{
					 if(xs!=xe)
					{
						sx=metaSprites[off+3]-cx+hwdt;

						sx=-sx;

						metaSprites[off+3]=sx-hwdt+cx;
					}
				}

				if(flip_v)
				{
					if(ys!=ye)
					{
						sy=metaSprites[off+0]-cy+hhgt;

						sy=-sy;

						metaSprites[off+0]=sy-hhgt+cy;
					}
				}
			}
		}
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}


void __fastcall TFormMain::MovePaletteCursor(int off)
{
	int ptr,col,h,b;

	SetUndo();

	ptr=palBank*16+palActive*4+bgPalCur;

	col=bgPal[ptr];

	h=col&0x0f;
	b=(col>>4)&3;

	if(b==0&&h==15) h=13;

	if(off<-1||off>1)
	{
		if(off<0)
		{
			--b;

			if(h==13&&b==1) b=0;
		}
		else
		{
			++b;

			if(h==13&&b==1) b=2;
		}
	}
	else
	{
		if(off<0) --h; else ++h;
	}

	if(h<0) h=13;
	if(h>13) h=0;
	if(b<0) b=3;
	if(b>3) b=0;

	col=h|(b<<4);
	bgPal[ptr]=col;

	UpdateAll();
}



void __fastcall TFormMain::MetaSpriteCopy(void)
{
	memcpy(metaSpriteCopy,&metaSprites[metaSpriteActive*64*4],sizeof(metaSpriteCopy));
 	//BROKE STUDIO
	strcpy(metaSpriteNameCopy, metaSpriteNames[metaSpriteActive].c_str());

	DWORD Len=sizeof(metaSpriteCopy)+sizeof(metaSpriteNameCopy);
	//
	//DWORD Len=sizeof(metaSpriteCopy);

	HGLOBAL hDst=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,Len);

	if (hDst)
	{
		LPSTR gDst=(LPSTR)GlobalLock(hDst);

		if (gDst)
		{
			if (OpenClipboard(Handle))
			{
				EmptyClipboard();

				memcpy(gDst,metaSpriteCopy,sizeof(metaSpriteCopy));
                //BROKE STUDIO
				memcpy(gDst+sizeof(metaSpriteCopy),metaSpriteNameCopy,sizeof(metaSpriteNameCopy));
				//
				GlobalUnlock(hDst);

				SetClipboardData(CF_META,hDst);
				CloseClipboard();
			}
		}
	}
}



void __fastcall TFormMain::MetaSpritePaste(void)
{
	SetUndo();
	
	OpenClipboard(Handle);

	HGLOBAL hClipBuf = GetClipboardData(CF_META);

	if (hClipBuf)
	{
		LPSTR gSrc = (LPSTR)GlobalLock(hClipBuf);

		if (gSrc != NULL)
		{
			memcpy(metaSpriteCopy,gSrc,sizeof(metaSpriteCopy));
			//BROKE STUDIO
			//if (sizeof(gSrc) > sizeof(metaSpriteCopy)) {  //maybe add this in case the clipboard content comes from an older version)
				memcpy(metaSpriteNameCopy,gSrc+sizeof(metaSpriteCopy),sizeof(metaSpriteNameCopy));
			//}

			//
			GlobalUnlock(gSrc);
		}
	}

	CloseClipboard();

	if(metaSpriteCopy[0]<255)
	{
		memcpy(&metaSprites[metaSpriteActive*64*4],metaSpriteCopy,sizeof(metaSpriteCopy));
		//BROKE STUDIO
		int count=0;
		for (int i = 0; i < 256; i++) {
			AnsiString trimmedName = metaSpriteNames[i].Trim();
			if (trimmedName == metaSpriteNameCopy) {
			count++;
			} else {
				int underscoreIndex = trimmedName.LastDelimiter("__");
				if (underscoreIndex > 0) {
					AnsiString baseName = trimmedName.SubString(1, underscoreIndex - 1);
					if (baseName == metaSpriteNameCopy) {
						AnsiString suffix = trimmedName.SubString(underscoreIndex + 1, trimmedName.Length() - underscoreIndex);
						try {
							int suffixValue = suffix.ToInt();
							if (suffixValue >= 1 && suffixValue <= 1024) {
						 count++;
							}
						} catch (...) {
						// Ignore any errors parsing the suffix as an integer.
						}
					}
				}
			}
		}


		metaSpriteNames[metaSpriteActive] = AnsiString(metaSpriteNameCopy)+"__"+IntToStr(count);
		//
		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	}
}



void __fastcall TFormMain::PaletteCopy(void)
{
	cf_palRotation=0;  //reset subpalette paste rotation

	DWORD Len=16;

	HGLOBAL hDst=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,Len);

	if (hDst)
	{
		LPSTR gDst=(LPSTR)GlobalLock(hDst);

		if (gDst)
		{
			if (OpenClipboard(Handle))
			{
				EmptyClipboard();

				memcpy(gDst,&bgPal[palBank*16],16);

				GlobalUnlock(hDst);

				SetClipboardData(CF_PAL,hDst);
				CloseClipboard();
			}
		}
	}
}



void __fastcall TFormMain::PalettePaste(void)
{
	OpenClipboard(Handle);

	HGLOBAL hClipBuf=GetClipboardData(CF_PAL);

	if (hClipBuf)
	{
		LPSTR gSrc=(LPSTR)GlobalLock(hClipBuf);

		if (gSrc!=NULL)
		{
			memcpy(&bgPal[palBank*16],gSrc,16);

			GlobalUnlock(gSrc);
		}
	}

	CloseClipboard();
	UpdateAll();
}



bool __fastcall TFormMain::IsBlockDrawing(void)
{
	return BlockDrawingFlag;
}



void __fastcall TFormMain::BlockDrawing(bool block)
{
	if(block)
	{
		BlockDrawingFlag=block;
	}
	else
	{
		TimerBlock->Enabled=false;
		TimerBlock->Interval=DrawTimerDuration;
		TimerBlock->Enabled=true;
	}
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormPaint(TObject *Sender)
{
	UpdateAll();

}
//---------------------------------------------------------------------------

int FormOriginalWidth;
int FormOriginalHeight;
int PanelEditAreaOriginalWidth;
int PanelEditAreaOriginalHeight;
int PanelToolbarOriginalWidth;
int PanelToolbarOriginalHeight;
int PanelTilesetOriginalHeight;
int GroupBoxTilesOriginalHeight;
int PageControlEditorOriginalHeight;
int GroupBoxMetaSpriteOriginalWidth;
int PanelSpriteViewOriginalHeight;

int GroupBoxLayoutOriginalLeft;
int GroupBoxViewOriginalLeft;
int GroupBoxLayoutOriginalTop;
int GroupBoxViewOriginalTop;


int LayBtnWdt;
int LayBtnHgt;
int LayBtnTop;
int LayBtnLeft;

int LayTxt1Top;	
int LayTxt1Left;	
int LayTxt2Top;	
int LayTxt3Top;
int GridTxtLeft;	
int TypeInOffsTop;
int TypeInOffsLeft;

int Grid1Left;
int Grid2Left;
int Grid4Left;

int BtnMarqLeft;
int BtnMarqTop;
int	BtnMarqWdt;
int	BtnMarqHgt;
int BtnDrawLeft;
int BtnDrawTop;
int	BtnDrawHgt;
int BtnDrawWdt;

int GroupSprListWdt;
int GroupSprListHgt;
int GroupMetaSprWdt;
int GroupMetaSprHgt;
int GroupMetaSprLeft;
int BtnSprGridLeft;
int BtnClrMetSprLeft;
int BtnSprSnapLeft;
int BtnSpr8x16Left;

int BtnForceViewLeft;
int BtnTileViewLeft;

int BtnSprHFlipLeft;
int BtnSprVFlipLeft;
int BtnSprRotLeft;

int BtnClrMetSprWdt;
int BtnSprSnapWdt;
int BtnSpr8x16Wdt;
int PnlSprToolbarWdt;


void __fastcall TFormMain::UpdateMenu(void)
{
	bool bShowNew 	 =  (iRadioOpenSave<=1 || iRadioOpenSave>=3);
	bool bShowClassic =  (iRadioOpenSave==2 || iRadioOpenSave>=3);

	CHR1->Visible			=	bShowNew ? true:false;
	Canvas1->Visible		=	bShowNew ? true:false;
	Palettes1->Visible		=	bShowNew ? true:false;
	Metaspritebank1->Visible=	bShowNew ? true:false;

	MOpenNameTable->Visible		=	bShowClassic ? true:false;
	MSaveMap->Visible			=	bShowClassic ? true:false;
	MSaveNameTableBIN->Visible	=	bShowClassic ? true:false;
	MSaveNameTableASM->Visible	=	bShowClassic ? true:false;
	MSaveNameTableC->Visible    =	bShowClassic ? true:false;
	Options2->Visible           =	bShowClassic ? true:false;
	N17->Visible              	=	bShowClassic ? true:false;

	MCHROpen->Visible           =	bShowClassic ? true:false;
	MCHRSave->Visible           =	bShowClassic ? true:false;
	N1->Visible                 =	bShowClassic ? true:false;

	MOpenMetaSpriteBank->Visible=	bShowClassic ? true:false;
	MSaveMetaSpriteBank->Visible=	bShowClassic ? true:false;
	Savesinglemetasprite1->Visible= bShowClassic ? true:false;
	N23->Visible                =	bShowClassic ? true:false;

	MPaletteOpen->Visible 	=	bShowClassic ? true:false;
	MPaletteSave->Visible 	=	bShowClassic ? true:false;
	bool bAddAs=false;
	if (FormSpecialPasteCHR!= NULL) {
		

		//mode A
		if(FormSpecialPasteCHR->RadioSolids->Checked) {SpecialA1->Caption="Special A: Paste solids";  bAddAs=true;}
		if(FormSpecialPasteCHR->RadioCol0->Checked) {SpecialA1->Caption="Special A: Paste colour 0 (backdrop)";  bAddAs=true;}
		if(FormSpecialPasteCHR->RadioCustom->Checked) {SpecialA1->Caption="Special A: Paste custom colours";   bAddAs=true;}

		//mode B
		if(FormSpecialPasteCHR->RadioOnTop->Checked) 	SpecialB1->Caption="Special B: Paste on top";


		if(FormSpecialPasteCHR->RadioAdd->Checked) 	SpecialB1->Caption="Special B: Addditive paste";
		if(FormSpecialPasteCHR->RadioAdd1->Checked) SpecialB1->Caption="Special B: Addditive (-1) paste";
		if(FormSpecialPasteCHR->RadioAdd2->Checked) SpecialB1->Caption="Special B: Addditive (-2) paste";
		if(FormSpecialPasteCHR->RadioAdd3->Checked) SpecialB1->Caption="Special B: Addditive (-3) paste";

		if(FormSpecialPasteCHR->RadioBitAND->Checked) 	{SpecialB1->Caption="Special B: Bitwise AND paste";  bAddAs=true;}
		if(FormSpecialPasteCHR->RadioBitOR->Checked)    SpecialB1->Caption="Special B: Bitwise OR paste";

		if(FormSpecialPasteCHR->RadioKeepSim->Checked) 		SpecialB1->Caption="Special B: Keep colour matches";
		if(FormSpecialPasteCHR->RadioKeepMask->Checked) 	SpecialB1->Caption="Special B: Keep pixels in colour mask";


		//mode C
		if(FormSpecialPasteCHR->RadioBehind->Checked) 	SpecialC1->Caption="Special C: Paste behind";


		if(FormSpecialPasteCHR->RadioSub->Checked) 	SpecialC1->Caption="Special C: Subtractive paste";
		if(FormSpecialPasteCHR->RadioSub1->Checked) SpecialC1->Caption="Special C: Subtractive (+1) paste";
		if(FormSpecialPasteCHR->RadioSub2->Checked) SpecialC1->Caption="Special C: Subtractive (+2) paste";
		if(FormSpecialPasteCHR->RadioSub3->Checked) SpecialC1->Caption="Special C: Subtractive (+3) paste";

		if(FormSpecialPasteCHR->RadioBitNAND->Checked) 	SpecialC1->Caption="Special C: Bitwise NAND";  bAddAs=true;}

		if(FormSpecialPasteCHR->RadioKeepDiff->Checked) 	SpecialC1->Caption="Special C: Keep colour difference";
		if(FormSpecialPasteCHR->RadioRemoveMask->Checked) 	SpecialC1->Caption="Special C: Remove pixels in colour mask";


		if (bAddAs) {
			if (FormSpecialPasteCHR->RadioAsSilhouette->Checked){
					SpecialA1->Caption+=" (as silhouette)";


					if(FormSpecialPasteCHR->RadioOnTop->Checked)	SpecialB1->Caption+=" (as silhouette)";
					if(FormSpecialPasteCHR->RadioBehind->Checked)	SpecialB1->Caption+=" (as silhouette)";
					if(FormSpecialPasteCHR->RadioBitOR->Checked)	SpecialB1->Caption+=" (as silhouette)";
					if(FormSpecialPasteCHR->RadioBitAND->Checked)	SpecialB1->Caption+=" (as silhouette)";
					if(FormSpecialPasteCHR->RadioBitNAND->Checked)	SpecialC1->Caption+=" (as silhouette)";
			}
			 if (FormSpecialPasteCHR->RadioAsInvSilhouette->Checked){
					SpecialA1->Caption+=" (as inv. silhouette)";

					if(FormSpecialPasteCHR->RadioOnTop->Checked)	SpecialB1->Caption+=" (as inv. silhouette)";
					if(FormSpecialPasteCHR->RadioBehind->Checked)	SpecialB1->Caption+=" (as inv. silhouette)";
					if(FormSpecialPasteCHR->RadioBitOR->Checked)	SpecialB1->Caption+=" (as inv. silhouette)";
					if(FormSpecialPasteCHR->RadioBitAND->Checked)	SpecialB1->Caption+=" (as inv. silhouette)";
					if(FormSpecialPasteCHR->RadioBitNAND->Checked)	SpecialC1->Caption+=" (as inv. silhouette)";
			}
		}


}

void __fastcall TFormMain::UpdateUIScale(void)
{
	MView2x->Checked=(uiScale==2)?true:false;
	MView3x->Checked=(uiScale==3)?true:false;
	MView4x->Checked=(uiScale==4)?true:false;

	if(uiScale>2 && PageControlEditor->ActivePage==TabSheetTile)
	{	GroupBoxTileControl->Visible=true;}
	else
	{	GroupBoxTileControl->Visible=false;	}

	Width =FormOriginalWidth -512-256+256*uiScale+128*uiScale;
	Height=FormOriginalHeight-512+256*uiScale;

	PanelEditArea->Width =PanelEditAreaOriginalWidth -512+256*uiScale;
	PanelToolbar ->Width =PanelToolbarOriginalWidth  -256+128*uiScale;
	PanelTileset ->Height=PanelTilesetOriginalHeight -256+128*uiScale;

	GroupBoxTiles->Height=GroupBoxTilesOriginalHeight-256+128*uiScale;
	PageControlEditor ->Height=PageControlEditorOriginalHeight-512+256*uiScale;
	GroupBoxMetaSprite->Width=GroupBoxMetaSpriteOriginalWidth -256+128*uiScale;
	PanelSpriteView   ->Height=PanelSpriteViewOriginalHeight  -256+128*uiScale;


	GroupBoxLayout->Left=GroupBoxLayoutOriginalLeft -256+128*uiScale;
	GroupBoxView->Left=GroupBoxViewOriginalLeft -256+128*uiScale;
	GroupBoxLayout->Top=GroupBoxLayoutOriginalTop -256+128*uiScale;
	GroupBoxView->Top=GroupBoxViewOriginalTop -256+128*uiScale;

    GroupBoxPal->Height=200 -100+50*uiScale;


	ButtonTileView->Left=BtnTileViewLeft -256+128*uiScale;
	//ButtonForceView->Left=BtnForceViewLeft -256+128*uiScale;


		GroupBoxTiles->Height+= -16 + 8*uiScale;
		if(uiScale==1) GroupBoxTiles->Height+= -12;
		GroupBoxLayout->Top+=   -16 + 8*uiScale;
		GroupBoxView->Top+=     -16 + 8*uiScale;


	SpeedButtonChrBank1->Height=22 -16 +8*uiScale;
	SpeedButtonChrBank2->Height=22 -16 +8*uiScale;
	SpeedButtonChrBank3->Height=22 -16 +8*uiScale;
	SpeedButtonChrBank4->Height=22 -16 +8*uiScale;
	SpeedButtonChrBank1->Width=28 -20 +10*uiScale;
	SpeedButtonChrBank2->Width=28 -20 +10*uiScale;
	SpeedButtonChrBank3->Width=28 -20 +10*uiScale;
	SpeedButtonChrBank4->Width=28 -20 +10*uiScale;
	SpeedButtonChrBank1->Left=5;
	SpeedButtonChrBank2->Left=33 -20 +10*uiScale;
	SpeedButtonChrBank3->Left=62 -40 +20*uiScale;
	SpeedButtonChrBank4->Left=91 -60 +30*uiScale;
	SpeedButtonAutocreate->Left=189 -80 +40*uiScale;

	SpeedButtonMarqTile->Left=189 -100 +50*uiScale;
	SpeedButtonAutocreate->Left=189 -100 +50*uiScale;
	SpeedButtonDrawTile->Left=226 -120 +60*uiScale;
	SpeedButtonDrawTile->Width=35 -20 +10*uiScale;
	SpeedButtonMarqTile->Width=35 -20 +10*uiScale;
	SpeedButtonAutocreate->Width=73 -40 +20*uiScale;

	if(uiScale==1) btnCHRedit->Height=22 -16 +8*uiScale;
	if(uiScale>1)btnCHRedit->Height=16 -6 +3*uiScale;

	SpeedButtonDrawTile->Height=16 -6 +3*uiScale;
	SpeedButtonMarqTile->Height=16 -6 +3*uiScale;

	if(uiScale==1){
		SpeedButtonChrBank1->Font->Height=-10;
		SpeedButtonChrBank2->Font->Height=-10;
		SpeedButtonChrBank3->Font->Height=-10;
		SpeedButtonChrBank4->Font->Height=-10;
	}
	if(uiScale==2){
		SpeedButtonChrBank1->Font->Height=-11;
		SpeedButtonChrBank2->Font->Height=-11;
		SpeedButtonChrBank3->Font->Height=-11;
		SpeedButtonChrBank4->Font->Height=-11;
	}
	if(uiScale>2){
		SpeedButtonChrBank1->Font->Height=-14;
		SpeedButtonChrBank2->Font->Height=-14;
		SpeedButtonChrBank3->Font->Height=-14;
		SpeedButtonChrBank4->Font->Height=-14;
	}


	if(uiScale==2){TextNullTile->Top=18 -6 +3*uiScale;
		SpeedButtonAutocreate->Top=18 -6 +3*uiScale;
	}
	else {
		TextNullTile->Top=20 -6 +3*uiScale;
		SpeedButtonAutocreate->Top=20 -6 +3*uiScale;
	}
	btnCHRedit->Left=123 -80 +40*uiScale;
	btnCHRedit->Width=64 -20 +10*uiScale;


	TextNullTile->Height=15 -6 +3*uiScale;
	TextNullTile->Left=123 -80 +40*uiScale;
	TextNullTile->Width=64 -20 +10*uiScale;
    TextNullTile->Visible= uiScale>1? true:false;
	labelToggleSet->Top= 22 - 16 + 8*uiScale;
	labelToggleSet->Visible= uiScale>1? true:false;

	if(uiScale>1){
		PaintBoxPal->Width=256 -256 + 128*uiScale;
		PaintBoxPal->Height=133 -133 + 66*uiScale;
	}
	LabelPal1->Left=120 - 94 + 47*uiScale;
	LabelPal3->Left=120 - 94 + 47*uiScale;
	LabelPal2->Top=57 - 22 + 11*uiScale;
	LabelPal3->Top=57 - 22 + 11*uiScale;

	btnHueMinus->Left=222 - 224 + 112*uiScale;
	btnHueTiltMinus->Left=222 - 224 + 112*uiScale;
	btnBrightMinus->Left=222 - 224 + 112*uiScale;
	btnHuePlus->Left=242 - 240 + 120*uiScale;
	btnHueTiltPlus->Left=242 - 240 + 120*uiScale;
	btnBrightPlus->Left=242 - 240 + 120*uiScale;

	btnHueMinus->Width=20 - 16 + 8*uiScale;
	btnHueTiltMinus->Width=20 - 16 + 8*uiScale;
	btnBrightMinus->Width=20 - 16 + 8*uiScale;
	btnHuePlus->Width=20 - 16 + 8*uiScale;
	btnHueTiltPlus->Width=20 - 16 + 8*uiScale;
	btnBrightPlus->Width=20 - 16 + 8*uiScale;


	btnHueTiltMinus->Top=41 - 10 + 5*uiScale;
	btnBrightMinus->Top=58 - 20 + 10*uiScale;


	btnHueTiltPlus->Top=41 - 10 + 5*uiScale;
	btnBrightPlus->Top=58 - 20 + 10*uiScale;



	btnHueMinus->Height=17 - 10 + 5*uiScale;
	btnHueTiltMinus->Height=17 - 10 + 5*uiScale;
	btnBrightMinus->Height=17 - 10 + 5*uiScale;
	btnHuePlus->Height=17 - 10 + 5*uiScale;
	btnHueTiltPlus->Height=17 - 10 + 5*uiScale;
	btnBrightPlus->Height=17 - 10 + 5*uiScale;

	SpeedButtonPalBankA->Top=162 -106 +53*uiScale;
	SpeedButtonPalBankB->Top=162 -106 +53*uiScale;
	SpeedButtonPalBankC->Top=162 -106 +53*uiScale;
	SpeedButtonPalBankD->Top=162 -106 +53*uiScale;

	SpeedButtonMaskB->Top=162 -106 +53*uiScale;
	SpeedButtonMaskG->Top=162 -106 +53*uiScale;
	SpeedButtonMaskR->Top=162 -106 +53*uiScale;
	SpeedButtonMaskM->Top=162 -106 +53*uiScale;

	if(uiScale>2){
		SpeedButtonSubpalCopy->Top=162 -106 +53*uiScale;
		SpeedButtonSubpalPaste->Top=162 -106 +53*uiScale;
		SpeedButtonSubpalCopy->Height=22;
		SpeedButtonSubpalPaste->Height=22;

		SpeedButtonPalBankA->Left=7;
		SpeedButtonPalBankB->Left=31 -16 +8*uiScale;
		SpeedButtonPalBankC->Left=55 -16*2 +8*uiScale*2;
		SpeedButtonPalBankD->Left=79 -16*3 +8*uiScale*3;

		SpeedButtonSubpalCopy->Left=114 -16*5 +8*uiScale*5;
		SpeedButtonSubpalPaste->Left=154 -16*6 +8*uiScale*6;

		int tmpoff = uiScale>3? 16:0;

		SpeedButtonMaskB->Left=168+40 -16*7 +8*uiScale*7 +tmpoff;
		SpeedButtonMaskG->Left=192+40 -16*8 +8*uiScale*8 +tmpoff;
		SpeedButtonMaskR->Left=216+40 -16*9 +8*uiScale*9 +tmpoff;
		SpeedButtonMaskM->Left=240+40 -16*10 +8*uiScale*10 +tmpoff;

		SpeedButtonPalBankA->Width=23 -16 +8*uiScale;
		SpeedButtonPalBankB->Width=23 -16 +8*uiScale;
		SpeedButtonPalBankC->Width=23 -16 +8*uiScale;
		SpeedButtonPalBankD->Width=23 -16 +8*uiScale;

		SpeedButtonSubpalCopy->Width=40 -16 +8*uiScale;
		SpeedButtonSubpalPaste->Width=40 -16 +8*uiScale;

		SpeedButtonMaskB->Width=23 -16 +8*uiScale;
		SpeedButtonMaskG->Width=23 -16 +8*uiScale;
		SpeedButtonMaskR->Width=23 -16 +8*uiScale;
		SpeedButtonMaskM->Width=23 -16 +8*uiScale;


	}
	else{
		SpeedButtonSubpalCopy->Top=160 -106 +53*uiScale;
		SpeedButtonSubpalPaste->Top=176 -106 +53*uiScale;
		SpeedButtonSubpalCopy->Height=16;
		SpeedButtonSubpalPaste->Height=16;

		SpeedButtonPalBankA->Left=7;
		SpeedButtonPalBankB->Left=31;
		SpeedButtonPalBankC->Left=55;
		SpeedButtonPalBankD->Left=79;

		SpeedButtonSubpalCopy->Left=114;
		SpeedButtonSubpalPaste->Left=114;

		SpeedButtonMaskB->Left=168;
		SpeedButtonMaskG->Left=192;
		SpeedButtonMaskR->Left=216;
		SpeedButtonMaskM->Left=240;

		SpeedButtonPalBankA->Width=23;
		SpeedButtonPalBankB->Width=23;
		SpeedButtonPalBankC->Width=23;
		SpeedButtonPalBankD->Width=23;

		SpeedButtonSubpalCopy->Width=40;
		SpeedButtonSubpalPaste->Width=40;

		SpeedButtonMaskB->Width=23;
		SpeedButtonMaskG->Width=23;
		SpeedButtonMaskR->Width=23;
		SpeedButtonMaskM->Width=23;



	}
	ImageName      ->Width =256*uiScale;
	ImageName      ->Height=256*uiScale;
	ImageTiles     ->Width =128*uiScale;
	ImageTiles     ->Height=128*uiScale;
	ImageBigTiles  ->Width =256*uiScale;
	ImageBigTiles  ->Height=256*uiScale;
	ImageMetaSprite->Width =128*uiScale;
	ImageMetaSprite->Height=128*uiScale;

	ImageName      ->Picture->Bitmap->SetSize(ImageName      ->Width,ImageName      ->Height);
	BufBmpName	   ->SetSize			   	 (ImageName      ->Width,ImageName      ->Height);

	BufBmpTiles    ->SetSize			   	 (ImageTiles     ->Width,ImageTiles     ->Height);

	ImageBigTiles  ->Picture->Bitmap->SetSize(ImageTiles     ->Width*2,ImageTiles     ->Height*2);
	ImageMetaSprite->Picture->Bitmap->SetSize(ImageMetaSprite->Width,ImageMetaSprite->Height);

	if(PageControlEditor->ActivePage!=TabSheetTile)
	{
		ImageTiles     ->Picture->Bitmap->SetSize(ImageTiles     ->Width,ImageTiles     ->Height);
		UpdateNameTable(-1,-1,true);
		UpdateTiles(true);
	}
	else
	{
		bForceNameScale=true;
		UpdateNameTable(-1,-1,false);
		UpdateTiles(false);
		bForceNameScale=false;

		//PageControlEditor->ActivePage=TabSheetTile;
        ImageTiles     ->Picture->Bitmap->SetSize(ImageName     ->Width*2,ImageName     ->Height*2);
		if (uiScale!=2) ImageTiles->Stretch = true;
	}


	if(uiScale==1)
	{
		GroupBoxPal->Visible=false;
        GroupBoxSpriteList->Visible		=false;
		GroupBoxLayout->Align=alTop;
		GroupBoxView->Align=alBottom;
		GroupBoxView->Height=83;
		GroupBoxView->Width=139;
		GroupBoxLayout->Height=83;
		GroupBoxLayout->Width=139;

		int compactHgt=4;
		//SpeedButtonTiles->Width	=LayBtnWdt;
		SpeedButtonTiles->Height		=LayBtnHgt-compactHgt;
		SpeedButtonPal->Height	    	=LayBtnHgt-compactHgt;
		SpeedButtonTypeIn->Height		=LayBtnHgt-compactHgt;
		SpeedButtonSelTiles->Height		=LayBtnHgt-compactHgt;
		btnSelectedOnlyInverted->Height	=LayBtnHgt-compactHgt;

		SpeedButtonChecker->Height	=LayBtnHgt-compactHgt;
		btnMask->Height				=LayBtnHgt-compactHgt;
		SpeedButtonGridAll->Height	=LayBtnHgt-compactHgt;
		SpeedButtonGridTile->Height =LayBtnHgt-compactHgt;
		SpeedButtonGridAtr->Height  =LayBtnHgt-compactHgt;
		SpeedButtonGridBlock->Height=LayBtnHgt-compactHgt;
		SpeedButtonGridScreen->Height=LayBtnHgt-compactHgt;

		SpeedButtonTiles->Top		=LayBtnTop;
		SpeedButtonPal->Top	    	=LayBtnTop+LayBtnHgt+2-compactHgt;
		SpeedButtonTypeIn->Top		=LayBtnTop+(LayBtnHgt+2)*2-compactHgt*2;
		SpeedButtonSelTiles->Top	=LayBtnTop;
		btnSelectedOnlyInverted->Top=LayBtnTop;
		SpeedButtonChecker->Top		=LayBtnTop+LayBtnHgt+2-compactHgt;
		btnMask->Top				=LayBtnTop+LayBtnHgt+2-compactHgt;

		SpeedButtonGridAll->Top		=LayBtnTop+(LayBtnHgt+2)*2-compactHgt*2;
		SpeedButtonGridTile->Top 	=LayBtnTop+(LayBtnHgt+2)*2-compactHgt*2;
		SpeedButtonGridAtr->Top  	=LayBtnTop+(LayBtnHgt+2)*2-compactHgt*2;
		SpeedButtonGridBlock->Top	=LayBtnTop+(LayBtnHgt+2)*2-compactHgt*2;
		SpeedButtonGridScreen->Top  =LayBtnTop+(LayBtnHgt+2)*2-compactHgt*2;
		//SpeedButtonTiles->Left		=LayBtnLeft;

		LabelApplyPatterns->Top		=LayTxt1Top-2;
		LabelApplyAttr->Top			=LayTxt2Top-compactHgt*1-2;
		LabelTypeIn->Top			=LayTxt3Top-compactHgt*2-2;
		//LabelSelOnly->Top			=LayTxt1Top-2;
		//LabelAttrChecker->Top		=LayTxt2Top-compactHgt*1-2;
		//LabelGrid->Top				=LayTxt3Top-compactHgt*2-2;

		//LabelApplyPatterns->Left	=LayTxt1Left;
		//Label3->Left				=GridTxtLeft;
		StaticTextFontOffset->Top	=TypeInOffsTop-compactHgt*2-2;
		StaticTextFontOffset->Left	=TypeInOffsLeft;

		SpeedButtonGridTile->Left	=Grid1Left;
		SpeedButtonGridAtr->Left	=Grid2Left;
		SpeedButtonGridBlock->Left	=Grid4Left;

		SpeedButtonMarqTile->Left	=TextNullTile->Left;
		SpeedButtonMarqTile->Top	=TextNullTile->Top+16;
		SpeedButtonDrawTile->Left	=TextNullTile->Left+32;
		SpeedButtonDrawTile->Top	=TextNullTile->Top+16;

		SpeedButtonMarqTile->Width	=BtnMarqWdt-4;
		SpeedButtonMarqTile->Height	=BtnMarqHgt-2;

		SpeedButtonDrawTile->Width	=BtnDrawWdt-6;
		SpeedButtonDrawTile->Height	=BtnDrawHgt-2;


		//GroupBoxSpriteList->Width		=GroupSprListWdt;
		//GroupBoxSpriteList->Height		=GroupSprListHgt;
		GroupBoxMetaSprite->Width		=256;
		GroupBoxMetaSprite->Height		=256;
		GroupBoxMetaSprite->Left		=0;
		SpeedButtonSpriteGrid->Left		=BtnSprGridLeft-15;
		SpeedButtonClearMetaSprite->Left=BtnClrMetSprLeft-15;
		SpeedButtonSpriteSnap->Left		=BtnSprSnapLeft-15;
		SpeedButtonSprite8x16->Left 	=BtnSpr8x16Left-15;

		SpeedButtonClearMetaSprite->Width=BtnClrMetSprWdt-5;
		SpeedButtonSpriteSnap->Width	 =BtnSprSnapWdt-5;
		SpeedButtonSprite8x16->Width 	 =BtnSpr8x16Wdt-5;

		SpeedButtonMetaSpriteHFlip->Left=BtnSprHFlipLeft-3;
		SpeedButtonMetaSpriteVFlip->Left=BtnSprVFlipLeft-3;
		SpeedButtonMetaSpriteRotate->Left=BtnSprRotLeft-3;

		PanelSpriteToolbar->Width		=252;
	}
	else
	{


		GroupBoxPal->Visible=true;
		GroupBoxLayout->Align=alLeft;

		GroupBoxView->Align=alRight;

		GroupBoxView->Height=94;
		GroupBoxView->Width=139;
		GroupBoxLayout->Height=94;
		GroupBoxLayout->Width=139;

		///SpeedButtonTiles->Width			=LayBtnWdt;
		SpeedButtonTiles->Height		=LayBtnHgt;
		SpeedButtonPal->Height	    	=LayBtnHgt;
		SpeedButtonTypeIn->Height		=LayBtnHgt;
		SpeedButtonSelTiles->Height		=LayBtnHgt;
		btnSelectedOnlyInverted->Height	=LayBtnHgt;

		SpeedButtonChecker->Height		=LayBtnHgt;
		btnMask->Height					=LayBtnHgt;

		SpeedButtonGridAll->Height		=LayBtnHgt;
		SpeedButtonGridTile->Height 	=LayBtnHgt;
		SpeedButtonGridAtr->Height  	=LayBtnHgt;
		SpeedButtonGridBlock->Height	=LayBtnHgt;
		SpeedButtonGridScreen->Height	=LayBtnHgt;

		SpeedButtonTiles->Top		=LayBtnTop;
		SpeedButtonPal->Top	    	=LayBtnTop+LayBtnHgt+2;
		SpeedButtonTypeIn->Top		=LayBtnTop+(LayBtnHgt+2)*2;
		SpeedButtonSelTiles->Top	=LayBtnTop;
		btnSelectedOnlyInverted->Top	=LayBtnTop;

		SpeedButtonChecker->Top		=LayBtnTop+LayBtnHgt+2;
		btnMask->Top				=LayBtnTop+LayBtnHgt+2;

		SpeedButtonGridAll->Top		=LayBtnTop+(LayBtnHgt+2)*2;
		SpeedButtonGridTile->Top 	=LayBtnTop+(LayBtnHgt+2)*2;
		SpeedButtonGridAtr->Top  	=LayBtnTop+(LayBtnHgt+2)*2;
		SpeedButtonGridBlock->Top	=LayBtnTop+(LayBtnHgt+2)*2;
		SpeedButtonGridScreen->Top  =LayBtnTop+(LayBtnHgt+2)*2;

		LabelApplyPatterns->Top		=LayTxt1Top;
		LabelApplyAttr->Top			=LayTxt2Top;
		LabelTypeIn->Top			=LayTxt3Top;
		//LabelSelOnly->Top			=LayTxt1Top;
		//LabelAttrChecker->Top		=LayTxt2Top;
		//LabelGrid->Top				=LayTxt3Top;

		StaticTextFontOffset->Top	=TypeInOffsTop;
		StaticTextFontOffset->Left	=TypeInOffsLeft;

		SpeedButtonGridTile->Left		=Grid1Left;
		SpeedButtonGridAtr->Left		=Grid2Left;
		SpeedButtonGridBlock->Left		=Grid4Left;


		SpeedButtonMarqTile->Top		=BtnMarqTop;

		SpeedButtonDrawTile->Top		=BtnDrawTop;

		GroupBoxSpriteList->Visible		=true;
		GroupBoxMetaSprite->Width		=(GroupMetaSprWdt/2)*uiScale;
		if(uiScale==3) GroupBoxMetaSprite->Width=GroupBoxMetaSprite->Width+8;

		if(uiScale==4) GroupBoxMetaSprite->Width=GroupBoxMetaSprite->Width+16;



		GroupBoxMetaSprite->Height		=GroupMetaSprHgt;
		GroupBoxMetaSprite->Left		=GroupMetaSprLeft;
		SpeedButtonSpriteGrid->Left		=BtnSprGridLeft;
		SpeedButtonClearMetaSprite->Left=BtnClrMetSprLeft;
		SpeedButtonSpriteSnap->Left		=BtnSprSnapLeft;
		SpeedButtonSprite8x16->Left 	=BtnSpr8x16Left;
		//ButtonForceView->Left			=BtnSpr8x16Left;
		SpeedButtonMetaSpriteHFlip->Left=BtnSprHFlipLeft;
		SpeedButtonMetaSpriteVFlip->Left=BtnSprVFlipLeft;
		SpeedButtonMetaSpriteRotate->Left=BtnSprRotLeft;

		SpeedButtonClearMetaSprite->Width=BtnClrMetSprWdt;
		SpeedButtonSpriteSnap->Width	 =BtnSprSnapWdt;
		SpeedButtonSprite8x16->Width 	 =BtnSpr8x16Wdt;
		PanelSpriteToolbar->Width		=PnlSprToolbarWdt;
		PanelSpriteToolbar->Height		=235;
		GroupBoxSpriteList->Width		=237;

	}
	//UpdateAll(); //did this originally, but because of the fix above, at least this cuts out some redundance.
	pal_validate();

	CorrectView();
	DrawPalettes();

	cueUpdateMetasprite=true;
	cueStats=true;

}

void __fastcall TFormMain::FormCreate(TObject *Sender)
{
	FILE *file;
	int i,pp;
	unsigned char buf[192];
	AnsiString dir,name,spr;

	CF_CHR=RegisterClipboardFormat("NESST_CF_CHR");
	CF_NAM=RegisterClipboardFormat("NESST_CF_NAM");
	CF_META=RegisterClipboardFormat("NESST_CF_META");
	CF_PAL=RegisterClipboardFormat("NESST_CF_PAL");
	CF_SUBPAL=RegisterClipboardFormat("NESST_CF_SUBPAL");
	CF_METATILE=RegisterClipboardFormat("NEXXT_CF_METATILE");

	for(i=0;i<4;i++) bBrushMask[i]=true;
	for(i=0;i<256;i++){
		collisionID_R[i]=0xff;
		collisionID_G[i]=0xff;
		collisionID_B[i]=0xff;
	}


	spr="Ctrl + M: Create metasprite from selection\nDrop tile: Add sprite\nLeft click: Select sprite\nRight drag: Move sprite\nCtrl + left drag: Move anchor";
	LabelMetaSpriteHint->Caption=spr;

	FormCaption=Application->Title; //Caption;

	BlockDrawing(false);
	TimerNTstrip->Enabled=false;
	palBank=0;

	memcpy(bgPal,&bgPalDefault_NEXXT2[16],sizeof(bgPal));
	memcpy(bgPalUndo,&bgPalDefault_NEXXT2[16],sizeof(bgPalUndo));
	memcpy(bgPalCheckpoint,&bgPalDefault_NEXXT2[16],sizeof(bgPalCheckpoint));
	memcpy(bgPalUndoCheckPoint,&bgPalDefault_NEXXT2[16],sizeof(bgPalUndoCheckPoint));

	nameTableWidth=32;
	nameTableHeight=30;

	nameTableViewX=0;
	nameTableViewY=0;

	//BROKE STUDIO
	for(i=0;i<256;i++)
	{
		metaSpriteNames[i]="Metasprite"+IntToStr(i);
	}

	//

	for(unsigned int j=0;j<chrBanks;j++){chrBankLabels.push_back("Unlabeled");}
	for(unsigned int j=0;j<undo_chrBanks;j++){undo_chrBankLabels.push_back("Unlabeled");}
	for(unsigned int j=0;j<checkpoint_chrBanks;j++){checkpoint_chrBankLabels.push_back("Unlabeled");}
	for(unsigned int j=0;j<undo_checkpoint_chrBanks;j++){undo_checkpoint_chrBankLabels.push_back("Unlabeled");}

	for( int j=0;j<metatileSets_2x2;j++){metatileSetLabels_2x2.push_back("Unlabeled");}
	for( int j=0;j<undo_metatileSets_2x2;j++){undo_metatileSetLabels_2x2.push_back("Unlabeled");}
	for( int j=0;j<checkpoint_metatileSets_2x2;j++){checkpoint_metatileSetLabels_2x2.push_back("Unlabeled");}
	for( int j=0;j<undo_checkpoint_metatileSets_2x2;j++){undo_checkpoint_metatileSetLabels_2x2.push_back("Unlabeled");}

	for( int j=0;j<metatileSets_4x4;j++){metatileSetLabels_4x4.push_back("Unlabeled");}
	for( int j=0;j<undo_metatileSets_4x4;j++){undo_metatileSetLabels_4x4.push_back("Unlabeled");}
	for( int j=0;j<checkpoint_metatileSets_4x4;j++){checkpoint_metatileSetLabels_4x4.push_back("Unlabeled");}
	for( int j=0;j<undo_checkpoint_metatileSets_4x4;j++){undo_checkpoint_metatileSetLabels_4x4.push_back("Unlabeled");}

	for( int j=0;j<metatileSets_8x8;j++){metatileSetLabels_8x8.push_back("Unlabeled");}
	for( int j=0;j<undo_metatileSets_8x8;j++){undo_metatileSetLabels_8x8.push_back("Unlabeled");}
	for( int j=0;j<checkpoint_metatileSets_8x8;j++){checkpoint_metatileSetLabels_8x8.push_back("Unlabeled");}
	for( int j=0;j<undo_checkpoint_metatileSets_8x8;j++){undo_checkpoint_metatileSetLabels_8x8.push_back("Unlabeled");}

	ImageName->Picture=new TPicture();
	ImageName->Picture->Bitmap=new Graphics::TBitmap();
	ImageName->Picture->Bitmap->PixelFormat=pf24bit;

	ImageTiles->Picture=new TPicture();
	ImageTiles->Picture->Bitmap=new Graphics::TBitmap();
	ImageTiles->Picture->Bitmap->PixelFormat=pf24bit;

	ImageMetaSprite->Picture=new TPicture();
	ImageMetaSprite->Picture->Bitmap=new Graphics::TBitmap();
	ImageMetaSprite->Picture->Bitmap->PixelFormat=pf24bit;

	ImageBigTiles->Picture=new TPicture();
	ImageBigTiles->Picture->Bitmap=new Graphics::TBitmap();
	ImageBigTiles->Picture->Bitmap->PixelFormat=pf24bit;

	BufCheckerStripes=new Graphics::TBitmap();
	BufCheckerStripes->PixelFormat=pf24bit;
	BufCheckerStripes->SetSize(16,16);

	BufCheckerClassic=new Graphics::TBitmap();
	BufCheckerClassic->PixelFormat=pf24bit;
	BufCheckerClassic->SetSize(4,4);


	BufBmpName=new Graphics::TBitmap();
	BufBmpName->PixelFormat=pf4bit;
	BufBmpTiles=new Graphics::TBitmap();
	BufBmpTiles->PixelFormat=pf4bit;

	LayBtnWdt		   =SpeedButtonTiles->Width;
	LayBtnHgt		   =SpeedButtonTiles->Height;
	LayBtnTop		   =SpeedButtonTiles->Top;
	LayBtnLeft		   =SpeedButtonTiles->Left;

	LayTxt1Top			=LabelApplyPatterns->Top;
	LayTxt1Left			=LabelApplyPatterns->Left;
	LayTxt2Top			=LabelApplyAttr->Top;
	LayTxt3Top			=LabelTypeIn->Top;
	//GridTxtLeft			=LabelGrid->Left;
	TypeInOffsTop		=StaticTextFontOffset->Top;
	TypeInOffsLeft		=StaticTextFontOffset->Left;

	Grid1Left			=SpeedButtonGridTile->Left;
	Grid2Left			=SpeedButtonGridAtr->Left;
	Grid4Left			=SpeedButtonGridBlock->Left;

	BtnMarqLeft			=SpeedButtonMarqTile->Left;
	BtnMarqTop			=SpeedButtonMarqTile->Top;
	BtnMarqWdt			=SpeedButtonMarqTile->Width;
	BtnMarqHgt			=SpeedButtonMarqTile->Height;
	BtnDrawLeft			=SpeedButtonDrawTile->Left;
	BtnDrawTop			=SpeedButtonDrawTile->Top;
	BtnDrawWdt			=SpeedButtonDrawTile->Width;
	BtnDrawHgt			=SpeedButtonDrawTile->Height;

	GroupSprListWdt		=GroupBoxSpriteList->Width;
	GroupSprListHgt		=GroupBoxSpriteList->Height;
	GroupMetaSprWdt		=GroupBoxMetaSprite->Width;
	GroupMetaSprHgt		=GroupBoxMetaSprite->Height;
	GroupMetaSprLeft	=GroupBoxMetaSprite->Left;

	FormOriginalWidth          =Width;
	FormOriginalHeight         =Height;
	PanelEditAreaOriginalWidth =PanelEditArea->Width;
	PanelEditAreaOriginalHeight=PanelEditArea->Height;
	PanelToolbarOriginalWidth  =PanelToolbar->Width;
	PanelToolbarOriginalHeight =PanelToolbar->Height;
	PanelTilesetOriginalHeight =PanelTileset->Height;
	GroupBoxTilesOriginalHeight=GroupBoxTiles->Height;
	PageControlEditorOriginalHeight=PageControlEditor->Height;
	GroupBoxMetaSpriteOriginalWidth=GroupBoxMetaSprite->Width;
	PanelSpriteViewOriginalHeight=PanelSpriteView->Height;

	GroupBoxLayoutOriginalLeft=GroupBoxLayout->Left;
	GroupBoxLayoutOriginalTop=GroupBoxLayout->Top;
	GroupBoxViewOriginalLeft=GroupBoxView->Left;
	GroupBoxViewOriginalTop=GroupBoxView->Top;

	BtnTileViewLeft		=ButtonTileView->Left;

	BtnSprGridLeft		=SpeedButtonSpriteGrid->Left;
	BtnClrMetSprLeft	=SpeedButtonClearMetaSprite->Left;
	BtnSprSnapLeft		=SpeedButtonSpriteSnap->Left;
	BtnSpr8x16Left	 	=SpeedButtonSprite8x16->Left;
	BtnForceViewLeft	=ButtonForceView->Left;
	BtnSprHFlipLeft		=SpeedButtonMetaSpriteHFlip->Left;
	BtnSprVFlipLeft		=SpeedButtonMetaSpriteVFlip->Left;
	BtnSprRotLeft		=SpeedButtonMetaSpriteRotate->Left;

	BtnClrMetSprWdt		=SpeedButtonClearMetaSprite->Width;
	BtnSprSnapWdt		=SpeedButtonSpriteSnap->Width;
	BtnSpr8x16Wdt		=SpeedButtonSprite8x16->Width;
	PnlSprToolbarWdt	=PanelSpriteToolbar->Width;

	FormMain->Left=(Screen->Width-FormMain->Width)/2;
	FormMain->Top=(Screen->Height-FormMain->Height)/2;


	tileActive=0;
	bankActive=0;
	metaSpriteActive=0;

	spriteGridX=64;
	spriteGridY=64;
	undoSpriteGridX=64;
	undoSpriteGridY=64;
	checkpointSpriteGridX	   =  64;
	checkpointSpriteGridY      =  64;
	undoCheckpointSpriteGridX  =  64;
	undoCheckpointSpriteGridY  =  64;

	spriteActive=0;


	memset(nameTable  				,0  ,sizeof(nameTable));
	memset(undoNameTable  			,0  ,sizeof(undoNameTable));
	memset(checkpointNameTable  	,0  ,sizeof(checkpointNameTable));
	memset(undoCheckpointNameTable  ,0  ,sizeof(undoCheckpointNameTable));

	memset(attrTable  				,0  ,sizeof(attrTable));
	memset(undoAttrTable  			,0  ,sizeof(undoAttrTable));
	memset(checkpointAttrTable  	,0  ,sizeof(checkpointAttrTable));
	memset(undoCheckpointAttrTable  ,0  ,sizeof(undoCheckpointAttrTable));

	memset(chr        			,0  ,chrBanks*4096);
	memset(undoChr        		,0  ,undo_chrBanks*4096);
	memset(checkpointChr    	,0  ,checkpoint_chrBanks*4096);
	memset(undoCheckpointChr    ,0  ,undo_checkpoint_chrBanks*4096);

	memset(tileProperties        			,0  ,chrBanks*256);
	memset(tilePropertiesUndo        		,0  ,undo_chrBanks*256);
	memset(checkpointTileProperties    		,0  ,checkpoint_chrBanks*256);
	memset(checkpointTilePropertiesUndo     ,0  ,undo_checkpoint_chrBanks*256);


	memset(chrCopy    ,0  ,sizeof(chrCopy));
	memset(propCopy	  ,0  ,sizeof(propCopy));

	memset(metaSprites				,255,sizeof(metaSprites));
	memset(undoMetaSprites			,255,sizeof(undoMetaSprites));
	memset(checkpointMetaSprites	,255,sizeof(checkpointMetaSprites));
	memset(undoCheckpointMetaSprites,255,sizeof(undoCheckpointMetaSprites));

	memset(metaSpriteCopy,255,sizeof(metaSpriteCopy));
	memset(metaSpritesBuf,255,sizeof(metaSpritesBuf));


	memset(chr_LinkBuf        			,0  ,chrBanks_LinkBuf*4096);
	memset(undoChr_LinkBuf        		,0  ,undo_chrBanks_LinkBuf*4096);
	memset(undoCheckpointChr_LinkBuf    ,0  ,undo_checkpoint_chrBanks_LinkBuf*4096);
	memset(checkpointChr_LinkBuf        ,0  ,checkpoint_chrBanks_LinkBuf*4096);

	memset(tileProperties_LinkBuf        			,0  ,chrBanks_LinkBuf*256);
	memset(tilePropertiesUndo_LinkBuf        		,0  ,undo_chrBanks_LinkBuf*256);
	memset(checkpointTileProperties_LinkBuf    		,0  ,checkpoint_chrBanks_LinkBuf*256);
	memset(checkpointTilePropertiesUndo_LinkBuf     ,0  ,undo_checkpoint_chrBanks_LinkBuf*256);


	//set up bank offsets A...D
	for(int b=0;b<4;b++)
		for (int i=0; i < 256; i++) {
			bankViewTable[b*256 + i] = b*4096;
			undo_bankViewTable[b*256 + i] = b*4096;
			checkpoint_bankViewTable[b*256 + i] = b*4096;
			undo_checkpoint_bankViewTable[b*256 + i] = b*4096;
	}


	propCHRlabel[0] ="solid";
	propCHRlabel[1] ="dangerous";
	propCHRlabel[2] ="unlabeled2";
	propCHRlabel[3] ="unlabeled3";
	propCHRlabel[4] ="unlabeled4";
	propCHRlabel[5] ="unlabeled5";
	propCHRlabel[6] ="escape bit";
	propCHRlabel[7] ="global condition";

	dir=ParamStr(0).SubString(0,ParamStr(0).LastDelimiter("\\/"));
	globalDir=dir;
	file=fopen((dir+"nes.pal").c_str(),"rb");

	if(file)
	{
		if(get_file_size(file)==192)
		{
			Externalnespal1->Checked=true;
			fread(buf,192,1,file);
			fclose(file);

			pp=0;

			for(i=0;i<64;i++)
			{
				basePalette[i]=(buf[pp+2]<<16)|(buf[pp+1]<<8)|buf[pp];
				pp+=3;
			}
		}
	}
	else
	{
		NESSTclassic1->Checked=true;       //todo: maybe allow for a few internal palette preference choices.
		pp=0;

		for(i=0;i<64;i++)
		{
			basePalette[i]=(palette[pp+2]<<16)|(palette[pp+1]<<8)|palette[pp];
			pp+=3;
		}
	}

	file=fopen((dir+"default_subpals.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==64)
		 {
			fread(buf,64,1,file);
			fclose(file);
			memcpy(bgPal,buf,64);
			memcpy(bgPalUndo,buf,64);
			memcpy(bgPalCheckpoint,buf,64);
			memcpy(bgPalUndoCheckPoint,buf,64);
		 }
	}


	file=fopen((dir+"default_subpal0.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(bgPal,buf,16);
			memcpy(bgPalUndo,buf,16);
			memcpy(bgPalCheckpoint,buf,16);
			memcpy(bgPalUndoCheckPoint,buf,16);
		 }
	}
	file=fopen((dir+"default_subpal1.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(&bgPal[16],buf,16);
			memcpy(&bgPalUndo[16],buf,16);
			memcpy(&bgPalCheckpoint[16],buf,16);
			memcpy(&bgPalUndoCheckPoint[16],buf,16);
		 }
	}
	file=fopen((dir+"default_subpal2.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(&bgPal[32],buf,16);
			memcpy(&bgPalUndo[32],buf,16);
			memcpy(&bgPalCheckpoint[32],buf,16);
			memcpy(&bgPalUndoCheckPoint[32],buf,16);
		 }
	}
	file=fopen((dir+"default_subpal3.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(&bgPal[48],buf,16);
			memcpy(&bgPalUndo[48],buf,16);
			memcpy(&bgPalCheckpoint[48],buf,16);
			memcpy(&bgPalUndoCheckPoint[48],buf,16);
		 }
	}
	

	ppuMask=0;
	palette_calc();

	nameSelection.left=-1;
	nameSelection.top=-1;
	nameCopyWidth=-1;
	nameCopyHeight=-1;

	chrSelection.left=0;
	chrSelection.right=0;
	chrSelection.right=1;
	chrSelection.bottom=1;
	chrCopyWidth=-1;
	chrCopyHeight=-1;
	chrCopyRect=true;

	for(i=0;i<256;i++) chrSelected[i]=0;

	chrSelected[tileActive]=1;
	chrSelectRect=true;
	//create 8x16 vs 8x8 indirection table
	int num;
	AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
	bool btiletab = PageControlEditor->ActivePage==TabSheetTile?true:false;


	if(((SpeedButtonSprite8x16->Down==true)&&(ButtonForceView->Down==true))){
		SetViewTable(1);
		AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
		GroupBoxTiles->Caption=tmp_LinkCHRstr+"Tileset (8x16 sprite mode)";
	}
	else {
		for(i=0;i<256;i++) {tileViewTable[i]=i;}
        if(btiletab) TabSheetTile->Caption=tmp_LinkCHRstr+"[F3] Tiles";
		else GroupBoxTiles->Caption=tmp_LinkCHRstr+"Tileset";

	}
	//parameterize this in config sometime?
	SetLatestCHR(0,1024*4, true); //changed from 8k to 4k for now

	tileXC=-1;
	tileYC=-1;
	nameXC=-1;
	nameYC=-1;
	palHover=-1;
	palColHover=-1;
	colHover=-1;
	spriteHover=-1;
	spriteDrag=-1;
	CHREditorHover=-1;

	dir=reg_load_str(regWorkingDirectory,"");
	openByFileDone=false;

	if(ParamStr(1)!="")
	{
		name=RemoveExt(ParamStr(1));

		if(!ImportBMP(name+".bmp",false,false,-1,256,false,false))
		{
			if(!LoadSession(name+".nss"))
			{
				OpenAll(name);
			}
		}

		dir=ParamStr(1);

	//bandaid for bug with yet unknown cause:
	//1 user had a file that'd load improperly when opened by association.
	//resetting nametable selection prevents it.
	nameSelection.left=-1;
	nameSelection.top=-1;
	}

	SetCurrentDirectory(dir.c_str());
	openByFileDone=true;

	SetUndo();

	unsavedChanges=false;
	//Savesession1->Enabled=false;      //no reason not to let the user save if save knows to redirect to save as.
	SharedBGcol->Checked=true;
	SafeColours->Checked=true;


	//we've initialized some startup values beforehand, so if the attempted loadConfig fails,
	//we'll create a factory config.

	if(!LoadConfig())
	{
		uiScale=FAC_GUISCALE;
		bgPalCur=FAC_COL;
		palActive=FAC_PAL;
		typeInFontOffset=0;
		typeInASCIIOffset=32;

		prefStartScale2x=true;
		prefStartCol3=true;
		prefStartSubpal0=true;

		prefStartGridShow=false;
		prefStartGrid1=false;
		prefStartGrid2=true;
		prefStartGrid4=false;
		prefStartGrid32x30=false;

		prefStartGridPixelCHR=true;
		prefStartGridTilesCHR=false;
		prefStartGridMidpointsCHR=false;

		prefStartShowCHR=false;
		prefStartShowMM=false;

		prefStartShowBrushes=false;
		prefStartShowBucket=false;
		prefStartShowLines=false;


		iWarnMsprCyan=2;
		bWarnMsprCyan=false;
		bWarnMsprYellow=true;
		bWarnMsprOrange=false;
		bWarnMsprRed=true;

		bExportPalFilename=true;
		bExportPalSet=true;

		iGlobalAlpha=FAC_ALPHA;
		iRadioOpenSave=1;
		iShowFilepath=2;
		bSnapToScreen=false;

		bImportCHRWrap=true;
		bImportCHRCarry=false;
		bImportCHRSkip=false;

        bNotify_linkedmodewarning=true;

		SaveConfig();
	}
	if(prefStartScale2x) 	 	uiScale = 2;
	else if(prefStartScale3x)	uiScale = 3;
	else if(prefStartScale4x)	uiScale = 4;
	else						uiScale = 2;

	if(prefStartCol0) 	 		bgPalCur = 0;
	else if(prefStartCol1)		bgPalCur = 1;
	else if(prefStartCol2)		bgPalCur = 2;
	else if(prefStartCol3)		bgPalCur = 3;
	else 						bgPalCur = 3;

	if(prefStartSubpal0) 	 	palActive = 0;
	else if(prefStartSubpal1)	palActive = 1;
	else if(prefStartSubpal2)	palActive = 2;
	else if(prefStartSubpal3)	palActive = 3;
	else 						palActive = 0;


	//relative init to sensible presets.
	if(bgPalCur==3) bgPalCurOld=0; else bgPalCurOld=3;
	if(palActive==1) palActiveOld=0;  else palActiveOld=1;


	metaSpriteBankName=RemoveExt(ExtractFileName(SaveDialogSession->FileName)); //
	UpdateUIScale();
}
//---------------------------------------------------------------------------

int check_palette_mouse(int X,int Y)
{
	int i,x,y,s;

	s=2;//uiScale;
	y=0;
	x=8*s;

	//scale adjusters
	int rc = 0 -96 + 48*uiScale;
	int w=0 -24 +12*uiScale;


	//check BG palettes

	for(i=0;i<4;i++)
	{
		if(Y>=y&&Y<y+20+rc/4)
		{
			if(X>=x+ 0 && X<x+20+w) return 0x00|i;
			if(X>=x+20+w && X<x+40+w*2) return 0x10|i;
			if(X>=x+40+w*2 && X<x+60+w*3) return 0x20|i;
			if(X>=x+60+w*3 && X<x+80+w*4) return 0x30|i;
		}

		x+=(64-8)*s+rc;


		if(x>(64*s+rc))
		{
			x=8*s;
			y=16*s+rc/4;
		}
	}

	//check the main palette
	w= 0 -16 +8*uiScale;
	int off = rc/2;
	if(X>=0&&X<(16*(16+w)) && Y>= off+(32*s) && Y<(off+(32*s+4*(16+w))))
	{
		return 0x100| (X/(16+w) + (Y-(32*s +off))/(16+w)*16);
	}

	return -1;
}




void __fastcall TFormMain::PaintBoxPalMouseDown(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int i,n,col,off,entry;


	n=check_palette_mouse(X,Y);

	if(Shift.Contains(ssLeft))
	{
		if(n>=0)
		{
			if(n&0x100)
			{
				SetUndo();

				if(sharedCol0)     //checks the rule of universal backdrop colour
				{
					entry=palActive*4+bgPalCur;
					if(!(entry&3)|(Shift.Contains(ssAlt))&(!Shift.Contains(ssCtrl))) entry=0;
					bgPal[palBank*16+entry]=n&0xff;
				}
				else
				{
					bgPal[palBank*16+palActive*4+bgPalCur]=n&0xff;
				}
				if(Shift.Contains(ssCtrl))
				{
					bgPalCur++;

					bgPalCur=bgPalCur&3;
					if (bgPalCur==0)
					{
						if(sharedCol0) bgPalCur=1;
						if(Shift.Contains(ssShift))
						{
							palActive++;
							palActive=palActive&3;
						}
					}
					UpdateAll();

				}
				else if(Shift.Contains(ssShift))
						{
							palActive++;
							palActive=palActive&3; UpdateAll();
						}
			}
			else
			{
				if((palActive!=n&3)&&(AutostoreLastUsed->Checked)) palActiveOld=palActive;
				palActive=n&3;

				Pal0->Checked=false;
				Pal1->Checked=false;
				Pal2->Checked=false;
				Pal3->Checked=false;

				if (palActive==0) Pal0->Checked=true;
				if (palActive==1) Pal1->Checked=true;
				if (palActive==2) Pal2->Checked=true;
				if (palActive==3) Pal3->Checked=true;

				if((bgPalCur!=n>>4)&&(AutostoreLastUsed->Checked)) bgPalCurOld=bgPalCur;
				bgPalCur=n>>4;

				Colour001->Checked=false;
				Colour011->Checked=false;
				Colour101->Checked=false;
				Colour111->Checked=false;

				if (bgPalCur==0) Colour001->Checked=true;
				if (bgPalCur==1) Colour011->Checked=true;
				if (bgPalCur==2) Colour101->Checked=true;
				if (bgPalCur==3) Colour111->Checked=true;



				if(clickC)
				{
					cf_palRotation=0;  //reset

					DWORD Len=4;
					HGLOBAL hDst=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,Len);

					if (hDst)
					{
						LPSTR gDst=(LPSTR)GlobalLock(hDst);
						if (gDst)
						{
							if (OpenClipboard(Handle))
							{
								EmptyClipboard();
								memcpy(gDst,&bgPal[palBank*16+palActive*4],4);

								SetClipboardData(CF_SUBPAL,hDst);
								CloseClipboard();
							}
						}
					}
				}


				if(clickV)
				{
					OpenClipboard(Handle);

					HGLOBAL hClipBuf=GetClipboardData(CF_SUBPAL);

					if (hClipBuf)
					{
						LPSTR gSrc=(LPSTR)GlobalLock(hClipBuf);

						if (gSrc!=NULL)
						{
							SetUndo();
							memcpy(&bgPal[palBank*16+palActive*4],gSrc,4);
							GlobalUnlock(gSrc);
						}
					}
					HGLOBAL hClipBuf2=GetClipboardData(CF_PAL);

					if (hClipBuf2)
					{
						LPSTR gSrc=(LPSTR)GlobalLock(hClipBuf2);

						if (gSrc!=NULL)
						{
							SetUndo();
							memcpy(&bgPal[palBank*16+palActive*4],gSrc+cf_palRotation,4);
							GlobalUnlock(gSrc);
							cf_palRotation+=4;
							if (cf_palRotation>12) cf_palRotation=0;
						}
					}
					CloseClipboard();
					UpdateAll();
				}
			}
		}
	}

	if(PageControlEditor->ActivePage==TabSheetSprite)
	{
		for(i=0;i<ListBoxSpriteList->Items->Count;++i)
		{
			if(ListBoxSpriteList->Selected[i])
			{
				off=metaSpriteActive*64*4+i*4;

				if(metaSprites[off]<255) metaSprites[off+2]=(metaSprites[off+2]&~3)+palActive;
			}
		}

		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	}

	if(Shift.Contains(ssRight))
	{
		if(n>=0)
		{
			if(n&0x100)
			{
				palDragColor=n&0xff;
				acceptPalDrag=false;
				if(Shift.Contains(ssCtrl))
				{
					bgPalCur++;

					bgPalCur=bgPalCur&3;
					if (bgPalCur==0)
					{
						if(sharedCol0) bgPalCur=1;
						if(Shift.Contains(ssShift))
						{
							palActive++;
							palActive=palActive&3;
						}
					}
					UpdateAll();

				}
				else if(Shift.Contains(ssShift))
						{
							palActive++;
							palActive=palActive&3; UpdateAll();
						}
			}
			else
			{
			palDragColor=bgPal[palBank*16+(n&3)*4+(n>>4)];
			palDragVal=n;
			acceptPalDrag=true;

			}
			PaintBoxPal->BeginDrag(false,-1);
		}
	}

	pal_validate();

	DrawPalettes();
	UpdateTiles(true);
	if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;
	UpdateNameTable(-1,-1,true);

	if (FormNavigator != NULL) FormNavigator->Draw(false,false,false);  //bookmark: maybe replace with colour remapper rather than redrawer. maybe change working format to 8bit indexed altogether.
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;

}


int __fastcall TFormMain::GetNameTableFrame(void)
{
	AnsiString str;
	char strid[16];
	int frame;

	str=RemoveExt(OpenDialogName->FileName);
	str=str.SubString(str.Length()-3,4);

	strcpy(strid,str.c_str());

	if(strid[0]!='_') return -1;

	if(strid[1]<'0'||strid[1]>'9') return -1;
	if(strid[2]<'0'||strid[2]>'9') return -1;
	if(strid[3]<'0'||strid[3]>'9') return -1;

	frame=(strid[1]-'0')*100+(strid[2]-'0')*10+(strid[3]-'0');

	return frame;
}



void __fastcall TFormMain::ChangeNameTableFrame(int dir)
{
	unsigned char *buf;
	AnsiString filename;
	char namestr[1024];
	int frame;

	frame=GetNameTableFrame();

	if(frame<0) return;

	frame+=dir;

	if(frame<0) frame=0;
	if(frame>999) frame=999;

	filename=RemoveExt(OpenDialogName->FileName);
	filename=filename.SubString(1,filename.LastDelimiter("_"));

	sprintf(namestr,"%s%3.3i.nam",filename.c_str(),frame);

	buf=(unsigned char*)malloc(name_size()+attr_size());

	memcpy(buf,nameTable,name_size());
	memcpy(buf+name_size(),attrTable,attr_size());

	if(OpenNameTable(namestr,true)) //exploit 'open all' do not throw a warning
	{
		//save previous frame

		save_data((RemoveExt(OpenDialogName->FileName)+".nam").c_str(),buf,name_size()+attr_size(),SAVE_FORMAT_BIN,false);

		//remember new file name

		OpenDialogName->FileName=RemoveExt(namestr);
		SaveDialogName->FileName=OpenDialogName->FileName;
		SaveDialogMap ->FileName=OpenDialogName->FileName;
	}

	free(buf);
}

//---------------------------------------------------------------------------


void __fastcall TFormMain::ImageTilesMouseDown(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int i,off;
    int set=bankActive/16;
	int tx=X/(8*uiScale);
	int ty=Y/(8*uiScale);
	bool bQuick = FormLineDetails->btnQuick->Down;

	if(MCHRFindDoubles->Checked)
	{
		MCHRFindDoubles->Checked=false;
		MCHRFindDoubles->Caption="Find &duplicates";
	}

	if(MCHRFindUnused->Checked)
	{
		MCHRFindUnused->Checked=false;
		MCHRFindUnused->Caption="Find &unused";
	}

	//---

	//added here in addition to keydown and keyup.
    //there was a context that wouldm't register properly without.
	bBufCtrl=Shift.Contains(ssCtrl)?true:false;
	bBufShift=Shift.Contains(ssShift)?true:false;
	bBufAlt=Shift.Contains(ssAlt)?true:false;
	if (bBufCtrl || bBufShift || bBufAlt)
	{
		//this is for updating when drag has been accepted.
		if (bImageNameAccepted) cueUpdateNametableNoRepaint=true;
		else if (bImageTileAccepted){
			cueUpdateTiles=true;
            if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;
		}
		//this is for any other case. Probably redundant for now.

		//else{cueUpdateNametable=true; cueUpdateTiles=true;}  //include if thereÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s a legitimate use.
	}
	//---
	
	txDown=tx;    //used for relative positioning when dragging selection, as well as performing positive selections to the left/up.
	tyDown=ty;
	//int tile=tx+ty*16;
	bOutsideSel=false;
	//if(abs(chrSelection.left-chrSelection.right)>1||abs(chrSelection.top-chrSelection.bottom)>1)
		//{
			if((chrSelection.right<=tx)
				|(chrSelection.left>tx)
				|(chrSelection.bottom<=ty)
				|(chrSelection.top>ty))
					bOutsideSel=true;
			if(!chrSelectRect&&tileActive!=ty*16+tx) bOutsideSel=true;

		//}

	if(Sender==ImageTiles&&PageControlEditor->ActivePage==TabSheetTile) return;

	if(IsBlockDrawing()) return;

	if(!(X>=0&&X<(128*uiScale)&&Y>=0&&Y<(128*uiScale))) return;

	tileSelRectWdt=1;
	tileSelRectHgt=1;

	isLastClickedMetaSprite=false;
	isLastClickedSpriteList=false;
	isLastClickedNametable=false;
	isLastClickedTiles=true;
	

	//handle draw mode
	if(((SpeedButtonDrawTile->Down)&&(!Shift.Contains(ssShift)))
		||((SpeedButtonDrawTile->Down)&&((Shift.Contains(ssShift)&&Shift.Contains(ssCtrl)))))
	{
		if(!Shift.Contains(ssRight)) SetUndo();

		lineDrawing = ((btnLine->Down)&& (!Shift.Contains(ssCtrl)) );
		if(lineDrawing && !bQuick) FormLineDetails->btnMove->Enabled=true;


		if((!lineDrawing) || !bQuick){
			for(int tile=0;tile<256;tile++){
				int pp=tileViewTable[tile]*16+bankViewTable[tile+set];
				memcpy(chrBuf+tileViewTable[tile]*16,chr+pp,16);
			}
			memcpy (chrBufFill, chrBuf, 4096);
		}
        if(bQuick || (Shift.Contains(ssCtrl)&&!bQuick))
		{
		   if(lineUpX<0) {
				//memcpy (chrBuf, chr, 8192);
				for(int tile=0;tile<256;tile++){
					int pp=tileViewTable[tile]*16+bankViewTable[tile+set];
					memcpy(chrBuf+tileViewTable[tile]*16,chr+pp,16);
				}
		   }
		   else {
				//memcpy (chr, chrBuf, 8192);       //chrQuickLine
				for(int tile=0;tile<256;tile++){
					int pp=tileViewTable[tile]*16+bankViewTable[tile+set];
					memcpy(chr+pp,chrBuf+tileViewTable[tile]*16,16);
				}
		   }
           for(int tile=0;tile<256;tile++){
				int pp=tileViewTable[tile]*16+bankViewTable[tile+set];
				memcpy(chrBufFill+tileViewTable[tile]*16,chr+pp,16);
			}

		}
        if(bQuick)
		{
			bOldLine=true;   //user wants to retouch the position of old line
			 if(lineUpX<0) lineUpX= X;
			 if(lineUpY<0) lineUpY= Y;
			lineDownX=lineUpX;
			lineDownY=lineUpY;
		}
        else{

			bOldLine=false;
			if(bBufVK_3) memcpy(arr_linePlotCHR2,arr_linePlotCHR,sizeof(arr_linePlotCHR2));
			else memset (arr_linePlotCHR2,0,sizeof(arr_linePlotCHR2));
			lineDownX=X;
			lineDownY=Y;
		}
		lineToolAlt=0;
		lineToolX=0;
		lineToolY=0;
        
		bForbidPaint=false;
		lineDrawing = ((btnLine->Down)&&(!Shift.Contains(ssCtrl)));
		if(!lineDrawing) {lineUpX= -1; lineUpY= -1;}
		if(FormLineDetails->CheckResetLineNudge){
			lineOffX=0;
			lineOffY=0;
		}
		if(FormLineDetails->btnResetLine->Down)
		{
			lineToolRoster=0;
			

			for (int i = 0; i < 3; i++) {
				if (lineRosterEnable[lineToolRoster]==false) lineToolRoster++;
			}
			if (lineToolRoster > 3) lineToolRoster = 3;
		}




		if(bSmudge ||(bBufCtrl && bBufAlt))   //smudge or noncontiguous fill
		{
			int py = (Y/uiScale)%8; 	//only used by fill.
			int px = (X/uiScale)%8; 	//which pixel in 8x1 sliver
			if(FormCHREditor->btnQuant->Down){px=px&14;py=py&14;}

			int pp=tileViewTable[tx+ty*16]*16+bankViewTable[set+tx+ty*16]+py;
			iSmudge=(((chr[pp]<<px)&128)>>7)|(((chr[pp+8]<<px)&128)>>6);
		}
        bFillFirstIteration=true;
		ImageTilesMouseMove(Sender,Shift,X,Y);
	}
	else{lineUpX= -1; lineUpY= -1;}


	//handle layout mode (+ shift modifier in draw mode)
	if((SpeedButtonMarqTile->Down)||((Shift.Contains(ssShift))&&(!Shift.Contains(ssCtrl)))){
		//multi-select method
		if(Shift.Contains(ssCtrl)&&Shift.Contains(ssLeft))
		{
			bMultiSelectRemoveMode=chrSelected[Y/(8*uiScale)*16+X/(8*uiScale)];
			bool bTmp = bMultiSelectRemoveMode;
			chrSelected[Y/(8*uiScale)*16+X/(8*uiScale)]=Shift.Contains(ssLeft)?!bTmp:bTmp;
			//chrSelected[Y/(8*uiScale)*16+X/(8*uiScale)]=(!chrSelected[Y/(8*uiScale)*16+X/(8*uiScale)]);
			chrSelectRect=false;

			UpdateTiles(true);
			if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
			if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
		}
		//tile + box select method
		else if(Shift.Contains(ssLeft)||(bOutsideSel))
		{

			SelectTile(ty*16+tx);
			bool b=FormCHREditor->btn2x2mode->Down;
			if(b)
			{
				if((tileActive&15)==15) tileActive--;
				if(tileActive>=0xF0) tileActive-=16;

			}
		    SetTile(tileActive);
			UpdateTiles(true);
			if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
			if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
		}
		else if(Shift.Contains(ssRight)||(!bOutsideSel)){


			chrSelBuf.left		=	chrSelection.left;
			chrSelBuf.top		=	chrSelection.top;
			chrSelBuf.right		=	chrSelection.right;
			chrSelBuf.bottom  	=	chrSelection.bottom;

			destRect.left		=	chrSelection.left;
			destRect.top		=	chrSelection.top;
			destRect.right		=	chrSelection.right;
			destRect.bottom  	=	chrSelection.bottom;

		UpdateTiles(true);
		if(FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
		if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
		}
				
		if(SpeedButtonTypeIn->Down)
		{
		
			NameTableTypeIn(tileViewTable[tileActive]);
		}
		else
		{
			int exceptionSize =FormCHREditor->btn2x2mode->Down? 2:1;
			if(   abs(chrSelection.left-chrSelection.right)!=exceptionSize
				||abs(chrSelection.top-chrSelection.bottom)!=exceptionSize
				||Shift.Contains(ssCtrl)
                ||Shift.Contains(ssShift)
				)
			{
				nameSelection.left=-1;
				nameSelection.top=-1;

				UpdateTiles(true);
			}
		}
		//---- QuickPaste for CHR
		if(Shift.Contains(ssLeft)) if(clickV)
		{
			PasteCHR(0);
			//UpdateStats();
			cueStats=true;
			return;
		}
		//----

		UpdateNameTable(-1,-1,true);
		if((FormNavigator->Visible)&&(SpeedButtonSelTiles->Down)) FormNavigator->Draw(false,false,false);

		//UpdateStats();
		cueStats=true;

		if(PageControlEditor->ActivePage==TabSheetName)
		{
			if(Shift.Contains(ssRight)&&!Shift.Contains(ssShift))
			{
				if(bOutsideSel) SetTile(ty*16+tx);
				ImageTiles->BeginDrag(false,-1);
			}
		}
		if(PageControlEditor->ActivePage==TabSheetTile&&Sender==ImageBigTiles)
		{
			if(Shift.Contains(ssRight)&&!Shift.Contains(ssShift))
			{
				if(bOutsideSel) SetTile(ty*16+tx);
				ImageBigTiles->BeginDrag(false,-1);
			}
		}
		if(PageControlEditor->ActivePage==TabSheetSprite)
		{
			if(Shift.Contains(ssRight)&&!Shift.Contains(ssShift))
			{
				if(bOutsideSel) SetTile(ty*16+tx);
				ImageTiles->BeginDrag(false,-1);
                
			}

			if(!Shift.Contains(ssRight))
			{
				if(spriteActive>=0)
				{
					off=metaSpriteActive*64*4+spriteActive*4;

					if(metaSprites[off]<255)
					{
						SetUndo();

						if(!SpeedButtonSprite8x16->Down)
						{
							metaSprites[off+1]=tileActive;
						}
						else
						{
							//metaSprites[off+1]=(tileActive&0xfe)|(metaSprites[off+1]&1);
							metaSprites[off+1]=(tileViewTable[tileActive]&0xfe)|(metaSprites[off+1]&1);
						}

						//UpdateMetaSprite();
						cueUpdateMetasprite=true;
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonGridAllClick(TObject *Sender)
{
	if(((TSpeedButton*)Sender)->GroupIndex==10)
	{
		//repurposed as auto grids on/off. structure kept for easy reversal.
		//SpeedButtonGridTile->Down=SpeedButtonGridAll->Down;
		//SpeedButtonGridAtr->Down=SpeedButtonGridAll->Down;
		//SpeedButtonGridBlock->Down=SpeedButtonGridAll->Down;

	   /*if(!SpeedButtonGridTile->Down&&!SpeedButtonGridAtr->Down&&!SpeedButtonGridBlock->Down)
		{
			SpeedButtonGridTile->Down=true;
			SpeedButtonGridBlock->Down=true;
		}*/
	}
	else
	{

		//SpeedButtonGridAll->Down=(SpeedButtonGridTile->Down||SpeedButtonGridAtr->Down||SpeedButtonGridBlock->Down);
	}
	//SpeedButtonGridAll->Caption=SpeedButtonGridAll->Down?"on":"off";

	//if(!SpeedButtonGridTile->Down||!SpeedButtonGridAtr->Down||!SpeedButtonGridBlock->Down) SpeedButtonGridAll->Down=false;
	//if(SpeedButtonGridTile->Down&&SpeedButtonGridAtr->Down&&SpeedButtonGridBlock->Down) SpeedButtonGridAll->Down=true;

	GridOnOff1->Checked=SpeedButtonGridAll->Down;
	GridOnOff2->Checked=SpeedButtonGridAll->Down;

	Screen32x301->Checked=SpeedButtonGridScreen->Down;


	Tilegrid1->Checked=SpeedButtonGridTile->Down;
	Tilegrid2->Checked=SpeedButtonGridAtr->Down;
	Tilegrid4->Checked=SpeedButtonGridBlock->Down;

	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHROpenClick(TObject *Sender)
{
	if(!OpenDialogChr->Execute()) return;

	BlockDrawing(true);

	if(OpenCHR(OpenDialogChr->FileName, false))
	{
		OpenDialogChr->FileName=RemoveExt(OpenDialogChr->FileName);

		SaveDialogChr->FileName=OpenDialogChr->FileName;
	}


	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PaintBoxNamePaint(TObject *Sender)
{
	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
}
//---------------------------------------------------------------------------

bool __fastcall TFormMain::TestAutoDraw(void) //int nYC, int nXC
{
  //char freeDrawCHRbuf[16];
	int t=nameTable[nameYC*nameTableWidth+nameXC]; //?????
    int set=bankActive/16;
	int i,j,k;
	int usedOnce=0;				//used to see if we can draw on current tile
	int isTilenameVacant;			//1/2 condition to assign a new tilename to the place
	bool empty;			//2/2 condition to assign a new tilename to the place
	int tileExamined=255;

	for(j=0;j<nameTableWidth*nameTableHeight;++j)
		if(nameTable[j]==t) {++usedOnce; if (usedOnce>1) break;}

	for(j=0;j<256*64*4;j+=4) 
		if(metaSprites[j+1]==t) {++usedOnce; if (usedOnce>1) break;}

	if (usedOnce==1) return true;  //true
	else 	//if tile has only one usage, we can just draw on it as usual
	{
		/* todo: find a match between a proposed new tile and the existing set

		//memcpy (freeDrawCHRbuf, chr[t], 16)
		//-do the draw action to the freedrawbuf
		//-match buf with tileset. (find duplicate)

		*/

		


		for (i=0;i<256;++i)
		{


			empty=true;

			for (j=0;j<16;++j)
			{
				if (chr[bankViewTable[set+j]+(tileExamined*16)+j]>0)
				{
					empty=false;
					break;
				}
			}

			if (empty==true) //examined tile has no content
			{
				isTilenameVacant=0; 	
				
				

				for(k=0;k<nameTableWidth*nameTableHeight;k++) // check BG usage
					{
						if (isTilenameVacant>0) break;
						if(nameTable[k]==tileExamined) ++isTilenameVacant;
					}

				for(k=0;k<256*64*4;k+=4) // check sprite usage
					{
						if (isTilenameVacant>0) break;
						if(metaSprites[k+1]==tileExamined) ++isTilenameVacant;
					}
				if (isTilenameVacant==0)
				{
					nameTable[nameYC*nameTableWidth+nameXC]=tileExamined;

					return true;
				}
			}
			--tileExamined;
		}

		//no acceptable solution found

		LabelStats->Caption="Warning: Could not find an available (empty and unused) tile.\nTip: removing metasprites, duplicates and unused tiles may help.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
		return false;
	}

}


void __fastcall TFormMain::ImageNameMouseDown(TObject *Sender, TMouseButton Button,
TShiftState Shift, int X, int Y)
{
	int set = bankActive/16;
	bBufCtrl=Shift.Contains(ssCtrl)?true:false;
	bBufShift=Shift.Contains(ssShift)?true:false;
	bBufAlt=Shift.Contains(ssAlt)?true:false;

	int i,nx,ny;
	if(IsBlockDrawing()) return;
	bOutsideSel=false;
	//mouseDraggedNTSel=false;
	//mouseDraggedTileSel=false;
	nx=X/(8*uiScale)+nameTableViewX;
	ny=Y/(8*uiScale)+nameTableViewY;
	nxDown=nx;    //used for relative positioning when dragging selection, as well as performing positive selections to the left/up.
	nyDown=ny;
	if(nameXC<0||nameXC>=nameTableWidth||nameYC<0||nameYC>=nameTableHeight) return;
    FormNavigator->Map1->Picture->Bitmap->Assign(bufBmp);

	isLastClickedMetaSprite=false;
	isLastClickedSpriteList=false;
	isLastClickedNametable=true;
	isLastClickedTiles=false;

	if(Shift.Contains(ssLeft)) if(MouseTypeIn(X,Y)) return;

	if(Shift.Contains(ssLeft)&&(clickV))
	{
        SetUndo();
		//set selection
		nameSelection.left  =nx;
		nameSelection.top   =ny;
		nameSelection.right =nameSelection.left;
		nameSelection.bottom=nameSelection.top;

		for(i=0;i<256;++i) chrSelected[i]=0;

		chrSelected[tileActive]=1;
		chrSelectRect=true;
		PasteMap();
		//deselect
		nameSelection.left=-1;
		nameSelection.top =-1;

		cueStats=true;
		//FormNavigator->Draw(false,false);
		//FormNavigator->UpdateLines(false);
		FormNavigator->CueLinesTimer->Enabled=true;
		return;
	}

	//draw mode stuff
	if(((SpeedButtonDrawTile->Down)&&(!Shift.Contains(ssShift)))
		||((SpeedButtonDrawTile->Down)&&((Shift.Contains(ssShift)&&Shift.Contains(ssCtrl)))))
	{
		if(!Shift.Contains(ssRight)) SetUndo();


		for(int tile=0;tile<256;tile++){
			memcpy(chrBuf+tile*16,chr+tileViewTable[tile]*16 + bankViewTable[set*256+tile],16);
		}

		memcpy (chrBufFill, chrBuf, 4096); //preps a reference point for inc/dec fill
		

		/*
		memcpy (chrBuf, chr, 8192); //preps a referencepoint for brushstrokes
		memcpy (chrBufFill, chr, 8192); //preps a reference point for inc/dec fill
		*/
		TimerNTstrip->Enabled=true;

		if(bSmudge ||(bBufCtrl && bBufAlt))   //smudge or noncontiguous fill
		{
			int py = (Y/uiScale)%8; 	//only used by fill.
			int px = (X/uiScale)%8; 	//which pixel in 8x1 sliver
			if(FormCHREditor->btnQuant->Down){px=px&14;py=py&14;}

			int pp=nameTable[nyDown*nameTableWidth+nxDown]*16 +py;
			int bp=bankViewTable[set + nameTable[nyDown*nameTableWidth+nxDown]];
			iSmudge=(((chr[bp+pp]<<px)&128)>>7)|(((chr[bp+pp+8]<<px)&128)>>6);
		}

		ImageNameMouseMove(Sender,Shift,X,Y);
		return;
	}

	if (Shift.Contains(ssRight)) { //prep context for inside/outside selection
		SelectTile(nameTable[nameYC*nameTableWidth+nameXC]);
		SelectPalette(AttrGet(nameXC,nameYC,false,false));
		UpdateTiles(true);
        if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;
		if((nameSelection.right<=nx)
				|(nameSelection.left>nx)
				|(nameSelection.bottom<=ny)
				|(nameSelection.top>ny))
		{
					bOutsideSel=true;
					nameSelection.left=-1;
					nameSelection.top =-1;



		}
		else{
			if(!Shift.Contains(ssShift) && nameSelection.left != -1 && nameSelection.top != -1) ImageName->BeginDrag(false,-1);

			nameSelBuf.left		=	nameSelection.left;
			nameSelBuf.top		=	nameSelection.top;
			nameSelBuf.right	=	nameSelection.right;
			nameSelBuf.bottom  	=	nameSelection.bottom;
			//these are probably redundant
			destRect.left		=	nameSelection.left;
			destRect.top		=	nameSelection.top;
			destRect.right		=	nameSelection.right;
			destRect.bottom  	=	nameSelection.bottom;

			

		}
		UpdateNameTable(-1,-1,true);
		//FormNavigator->Draw(false,false);
		FormNavigator->UpdateLines(true);
	}

	else if(Shift.Contains(ssShift)&&Shift.Contains(ssLeft))   //begin selection
	{
		nameSelection.left  =nx;
		nameSelection.top   =ny;
		nameSelection.right =nameSelection.left+1;
		nameSelection.bottom=nameSelection.top +1;

		
		/*
		chrSelection.right =chrSelection.left+1;
		chrSelection.bottom=chrSelection.top +1;

		for(i=0;i<256;++i) chrSelected[i]=0;

		chrSelected[tileActive]=1;
		chrSelectRect=true;

		tileSelRectWdt=1;
		tileSelRectHgt=1;
        */
		UpdateTiles(true);
		UpdateNameTable(-1,-1,true);
		//FormNavigator->Draw(false,false);
		FormNavigator->UpdateLines(false);
	}

	else
	{
		if(Shift.Contains(ssLeft)) SetUndo();        //place tile

		ImageNameMouseMove(Sender,Shift,X,Y);

		if(nameSelection.left>=0||nameSelection.top>=0)      
		{
			nameSelection.left=-1;
			nameSelection.top =-1;

			UpdateNameTable(-1,-1,true);
			//FormNavigator->UpdateLines(false);
			FormNavigator->CueLinesTimer->Enabled=true;
			//int dx,dy,dw,dh;
			//GetSelection(chrSelection,dx,dy,dw,dh);
			//FormNavigator->DrawRange(dx,dy,dw,dh);
			//FormNavigator->Draw(false,false);
		}
	}




	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageNameMouseMove(TObject *Sender, TShiftState Shift,
int X, int Y)
{
	int i,j,dx,dy,xc,yc;
	int xmin,ymin,xmax,ymax;
	 int set=bankActive/16;
    int ix,iy; //pixel modifiers for thick pen
	int gx,gy,g; //guards for thick pen
	int iThick=FormCHREditor->btnThick->Down?4:1;

	TRect r;
	if(IsBlockDrawing()) return;



	xc=X/(8*uiScale);
	yc=Y/(8*uiScale);

	nameXC=xc+nameTableViewX;
	nameYC=yc+nameTableViewY;

	if(xc<0||xc>=viewPortWidth||yc<0||yc>=viewPortHeight||nameXC<0||nameXC>=nameTableWidth||nameYC<0||nameYC>=nameTableHeight)
	{
		nameXC=-1;
		nameYC=-1;

		cueStats=true;
		return;
	}

	if(Shift.Contains(ssLeft)) if(MouseTypeIn(X,Y)) return;
	if(Shift.Contains(ssLeft)) if(clickV)return;
	if(!FormMain->Active) return;

	///test
	if(Sender==ImageTiles&&PageControlEditor->ActivePage!=TabSheetTile) return;
	if(IsBlockDrawing()) return;

	if(
		(!Shift.Contains(ssShift)||(Shift.Contains(ssShift)&&Shift.Contains(ssCtrl)))
			&&(SpeedButtonDrawTile->Down&(X>=0&&X<(256*uiScale)&&Y>=0&&Y<(256*uiScale))))
	{
		int px,py,pp,extpp,mask,tempPal;

		if(Shift.Contains(ssLeft))
		{
			if((SpeedButtonAutocreate->Down==true)&&(!TestAutoDraw())) return;


			py = (Y/uiScale)%8; 	//only used by fill.
			px = (X/uiScale)%8; 	//which pixel in 8x1 sliver
			//pp = (Y/uiScale)%8 	+(X/(8*uiScale))*16 	+(Y/(8*uiScale))*256;
			//	^-which sliver		^-which column	 		^-which row
			if(FormCHREditor->btnQuant->Down){px=px&14;py=py&14;}
			for(int i=0; i<iThick; i++)
				{
                if(FormCHREditor->btnThick->Down)
				{
					if(bBrushMask[i]==false) continue;
					//todo: if introducing more brush sizes, use an xy array for their masks instead of this.
					if(i==0){ ix=0; iy=0;}
					if(i==1){ ix=1; iy=0;}
					if(i==2){ ix=0; iy=1;}
					if(i==3){ ix=1; iy=1;}

					py 	= (Y/uiScale)%8;      //which sliver
					px 	= (X/uiScale)%8;      //which pixel in 8x1 sliver

					if(FormCHREditor->btnQuant->Down){
						px=px&6;
						py=py&6;
					}
                    if(px+ix<8) px=px+ix;
					if(py+iy<8) py=py+iy;

					//"guard dog" for brush field crossing the tile border
					gx=((X/uiScale)%8); gx+=ix;
					gy=((Y/uiScale)%8); gy+=iy;

					g=7;

					if(!FormCHREditor->btnQuant->Down){
						if((i==1||3)&&gx>g) continue;
						if((i==2||3)&&gy>g) continue;
						//if((i==1||3)&&gx>g) {tx++; px-=7;}
						//if((i==2||3)&&gy>g) {ty++; py-=7;}
					}
				}
				int set = bankActive/16;
				extpp = nameTable[nameYC*nameTableWidth+nameXC]*16 + bankViewTable[set + nameTable[nameYC*nameTableWidth+nameXC]];  //the value held by nameTable[] may be altered by TestAutoDraw()
				int bufpp = nameTable[nameYC*nameTableWidth+nameXC]*16;
				pp = extpp + py;
				int bpp = bufpp + py;
				mask=128>>px;

				tempPal=(((chrBuf[bpp]<<px)&128)>>7)|(((chrBuf[bpp+8]<<px)&128)>>6);
				fillPal=(((chrBufFill[bpp]<<px)&128)>>7)|(((chrBuf[bpp+8]<<px)&128)>>6);
				if(Applytopen2->Checked){
					if(FormCHREditor->Protect0->Down && tempPal==0) continue;
					if(FormCHREditor->Protect1->Down && tempPal==1) continue;
					if(FormCHREditor->Protect2->Down && tempPal==2) continue;
					if(FormCHREditor->Protect3->Down && tempPal==3) continue;
				}
				if(IncDecPerclick1->Checked==false)
				{
					tmpContinousIncDecTimer++;
					if (tmpContinousIncDecTimer>continousIncDecDuration)
					{
						tmpContinousIncDecTimer=0;

						for(int tile=0;tile<256;tile++){
						memcpy(chrBuf+tile*16,chr+tileViewTable[tile]*16 + bankViewTable[set+tile],16);
						}
						//memcpy (chrBuf, chr, 8192);
					}

				}

				if(!Shift.Contains(ssCtrl))
				{
					if (penActive==0&&(!Shift.Contains(ssShift)))
					{
                        if(!bSmudge){
								if(ButtonBitmaskLo->Down||!FormMain->Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((bgPalCur&1)<<7)>>px);
								if(ButtonBitmaskHi->Down||!FormMain->Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((bgPalCur&2)<<6)>>px);
							}
							else{
								if(ButtonBitmaskLo->Down||!FormMain->Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((iSmudge&1)<<7)>>px);
								if(ButtonBitmaskHi->Down||!FormMain->Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((iSmudge&2)<<6)>>px);
							}
					}
					if (penActive==1&&(!Shift.Contains(ssShift)))
					{
						if(!Shift.Contains(ssAlt))tempPal++;
						else tempPal--;

						if(IncDecCap1->Checked==true)
						{
							if (tempPal >3) tempPal = 3;
							if (tempPal <0) tempPal = 0;
						}


						if(FormCHREditor->ButtonBitmaskLo->Down||!Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((tempPal&1)<<7)>>px);
						if(FormCHREditor->ButtonBitmaskHi->Down||!Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((tempPal&2)<<6)>>px);

					}
					if (penActive==2&&(!Shift.Contains(ssShift)))
					{
						if(!Shift.Contains(ssAlt))tempPal--;
						else tempPal++;

						if(IncDecCap1->Checked==true)
						{
							if (tempPal >3) tempPal = 3;
							if (tempPal <0) tempPal = 0;
						}

						if(FormCHREditor->ButtonBitmaskLo->Down||!Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((tempPal&1)<<7)>>px);
						if(FormCHREditor->ButtonBitmaskHi->Down||!FormMain->Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((tempPal&2)<<6)>>px);
					}
				}
				else
				{
					FormCHREditor->Fill(Shift,px,py,-1,-1,extpp,bufpp,true); //tempPal,fillPal
				}
			}   //<--this loop end works for drawing just one tile. This method (nametable painting) currently doesnÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´t allow brushes crossing tile borders

			//remove doubles to current tile
			if(SpeedButtonAutocreate->Down)       
			{
				bool clear[256];  int a;
				int curtile=nameTable[nameYC*nameTableWidth+nameXC];
				//for(i=curtile+1;i<256;i++)
				for(i=256; i-- > curtile+1;)
				{
					if(!memcmp(&chr[bankViewTable[set+curtile]+curtile*16],&chr[bankViewTable[set+i]+i*16],16))
					{
						//clear[i]=true;
						//for (int k=0;k<16;k++) chr[16*i+bankActive+k]=0;
						//for(j=0;j<nameTableWidth*nameTableHeight;j++) if(nameTable[j]==i) nameTable[j]=curtile;
						//for(j=0;j<256*64*4;j+=4) if(metaSprites[j+1]==i) metaSprites[j+1]=curtile;

						for (int k=0;k<16;k++) chr[16*curtile+bankViewTable[set+curtile]+k]=0;
						for(j=0;j<nameTableWidth*nameTableHeight;j++) if(nameTable[j]==curtile) nameTable[j]=i;
						for(j=0;j<256*64*4;j+=4) if(metaSprites[j+1]==curtile) metaSprites[j+1]=i;
					}
				}
				//for(;a<4096;a++) if(clear[a/16]) chr[bankActive+a]=0;
			}
			
			//UpdateNameTable(-1,-1,true);
			//cueUpdateNametable=true;
			cueUpdateTiles=true;

			cueUpdateMetasprite=true;
			cueUpdateNTstrip=true;
			cueRemoveDoubles=true;
			//int nx=nameXC+nameTableViewX;
			//int ny=nameYC+nameTableViewY;

			int xl=(X/uiScale&0xFFFFFFF8)*uiScale;
			int yl=(Y/uiScale&0xFFFFFFF8)*uiScale;
            int cgX=CHRCollisionGranularityX;
			int cgY=CHRCollisionGranularityY;

			DrawTile(
				ImageName->Picture,xl,yl,
				nameTable[nameYC*nameTableWidth+nameXC],
				nameTable[((nameYC*nameTableWidth)/(nameTableWidth*cgY))*(nameTableWidth*cgY) + (nameXC/cgX)*cgX],
				AttrGet(nameXC,nameYC,false,false),nameXC,nameYC,
				false,false,uiScale,false,false,false,false,false);
			//cueUpdateNT=true;
			//UpdateNameTable(-1,-1,true);

		}

		if(Shift.Contains(ssRight))
		{

			py = (Y/uiScale)%8; 	//only used by fill.
			px = (X/uiScale)%8; 	//which pixel in 8x1 sliver
			//pp = (Y/uiScale)%8 		+(X/(8*uiScale))*16 	+(Y/(8*uiScale))*256;
			//	^-which sliver		^-which column	 		^-which row
			extpp = nameTable[nameYC*nameTableWidth+nameXC]*16 + bankViewTable[set + nameTable[nameYC*nameTableWidth+nameXC]];
			pp = extpp + py;
			bgPalCur=(((chr[pp]<<px)&128)>>7)|(((chr[pp+8]<<px)&128)>>6);
			
			FormMain->DrawPalettes();
		}
	cueStats=true;
	return;
	}

	//layout mode begins:

	if(Shift.Contains(ssShift))          
	{
		if(Shift.Contains(ssLeft))
		{



			if(nameXC<nxDown)   {nameSelection.left=nameXC+1-(nameXC<nameSelection.right?1:0);
				nameSelection.right=nxDown+1;
				}
			if(nameXC>=nxDown) {nameSelection.right =nameXC+(nameXC>=nameSelection.left?1:0);
				nameSelection.left=nxDown;
				}
			if(nameYC<nyDown)  {nameSelection.top=nameYC-(nameYC>=nameSelection.bottom ?1:0);
				nameSelection.bottom=nyDown+1;
			    }
			if(nameYC>=nyDown) {nameSelection.bottom=nameYC+(nameYC>=nameSelection.top ?1:0);
				nameSelection.top=nyDown;				  
				}
			//UpdateNameTable(-1,-1,true);
			//cueUpdateNametable=true;
			NameLinesTimer->Enabled=true;
			//TODO: bug. selection not persistent on Nav until mouse hovers
			mouseDraggedNTSel=true;
		}

		else if(Shift.Contains(ssRight))
		{
			if(bOutsideSel){
				nameSelection.left=-1;
				nameSelection.top =-1;
                NameLinesTimer->Enabled=true;
				//cueUpdateNametable=true;
				//UpdateNameTable(-1,-1,true);
			}
			else if(!Shift.Contains(ssCtrl))
			{
				nameSelection.left=nameSelBuf.left+nameXC-nxDown;
				nameSelection.right=nameSelBuf.right+nameXC-nxDown;
				nameSelection.top=nameSelBuf.top+nameYC-nyDown;
				nameSelection.bottom=nameSelBuf.bottom+nameYC-nyDown;

				int cAlignHeight = (viewPortHeight-nameTableHeight);
				int cAlignWidth = (viewPortWidth-nameTableWidth);
				if  (cAlignHeight<0) cAlignHeight=0;
				if  (cAlignWidth<0) cAlignWidth=0;
				

				for (i=0; i<32; i++)  //long enough loop - felt safer than while
				{
					if(nameSelection.left+nameTableViewX<0-cAlignWidth/2)   	{	nameSelection.left++;
													nameSelection.right++;}

					if( nameSelection.right-nameTableViewX>viewPortWidth-cAlignWidth/2)
												{	nameSelection.left--;
													nameSelection.right--;}

					if(nameSelection.top+nameTableViewY<0-cAlignHeight/2)   	{	nameSelection.top++;
													nameSelection.bottom++;}

					if(nameSelection.bottom-nameTableViewY>viewPortHeight-cAlignHeight/2)
												{	nameSelection.top--;
													nameSelection.bottom--;}
				}

				//cueUpdateNametable=true;
				NameLinesTimer->Enabled=true;
				cueUpdateTiles=true;
			}
		}
	    cueStats=true;
		return;
	}




	if(Shift.Contains(ssLeft)&&nameSelection.left<0&&nameSelection.top<0)
	{
		if((FormMetatileTool->Visible==true) && (FormMetatileTool->btnClonestamp->Down==true))
		{
			bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
			bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
			bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;
			int d, tmpD;
			if(bTab2x2) d=2;
			if(bTab4x4) d=4;
			if(bTab8x8) d=8;
			int tmpNameXC;
			int tmpNameYC;
			if(FormMetatileTool->btnCloneSnap->Down==true){

				int tmpYoff = 0;

				if (((nameYC / 30) % 2 == 1) && (SpeedButtonGridScreen->Down || FormMetatileTool->chkAlignScr->Checked)){
					 if(bTab4x4) {tmpYoff = 2; }
					 if(bTab8x8) {tmpYoff = 6; }
				}

				tmpNameXC = (nameXC)/d;
				tmpNameYC = (nameYC - tmpYoff)/d;

				tmpNameYC = tmpNameYC* d + tmpYoff;
				tmpNameXC = tmpNameXC* d;



			}
			else{
				tmpNameXC = nameXC;
				tmpNameYC = nameYC;
			}
			if(bTab2x2){
				for(i=0;i<d;i++){
					for(j=0;j<d;j++){

						dx=tmpNameXC+j;
						dy=tmpNameYC+i;

						if(dx>=0&&dx<nameTableWidth&&dy>=0&&dy<nameTableHeight)
						{
							if(SpeedButtonTiles->Down) nameTable[dy*nameTableWidth+dx]=metatileSet_2x2_id[(mtClickID*d*d)+i*d+j];

							if(SpeedButtonPal->Down)
							{
								if(FormMetatileTool->btnUseAttr) AttrSet(dx,dy,metatileSet_2x2_pal[(mtClickID*d*d) + ((i*d)/(d*2))*(d*2) + (j/2)*2],false);
								else AttrSet(dx,dy,palActive,false);
							}
							UpdateNameTable(dx,dy,false);
						}
					}
				}
			}

			if(bTab4x4){
				tmpD=d;
				if((FormMetatileTool->btnCloneSnap->Down) && (SpeedButtonGridScreen->Down || FormMetatileTool->chkAlignScr->Checked))
				{
					if ((tmpNameYC % 30 == 28)) {tmpD=2;}
				}
				for(i=0;i<tmpD;i++){
					for(j=0;j<d;j++){

						dx=tmpNameXC+j;
						dy=tmpNameYC+i;

						if(dx>=0&&dx<nameTableWidth&&dy>=0&&dy<nameTableHeight)
						{
							if(SpeedButtonTiles->Down) nameTable[dy*nameTableWidth+dx]=metatileSet_4x4_id[(mtClickID*d*d)+i*d+j];
							if(SpeedButtonPal->Down)
							{
								if(FormMetatileTool->btnUseAttr) AttrSet(dx,dy,metatileSet_4x4_pal[(mtClickID*d*d) + ((i*d)/(d*2))*(d*2) + (j/2)*2],false);
								else AttrSet(dx,dy,palActive,false);
							}


							UpdateNameTable(dx,dy,false);
						}
					}
				}
			}
			if(bTab8x8){
tmpD=d;
				if((FormMetatileTool->btnCloneSnap->Down) && (SpeedButtonGridScreen->Down || FormMetatileTool->chkAlignScr->Checked))
				{
					if ((tmpNameYC % 30 == 24)) {tmpD=6;}
				}

				for(i=0;i<tmpD;i++){
					for(j=0;j<d;j++){

						dx=tmpNameXC+j;
						dy=tmpNameYC+i;

						if(dx>=0&&dx<nameTableWidth&&dy>=0&&dy<nameTableHeight)
						{
							if(SpeedButtonTiles->Down) nameTable[dy*nameTableWidth+dx]=metatileSet_8x8_id[(mtClickID*d*d)+i*d+j];
							if(SpeedButtonPal->Down)
							{
								if(FormMetatileTool->btnUseAttr) AttrSet(dx,dy,metatileSet_8x8_pal[(mtClickID*d*d) + ((i*d)/(d*2))*(d*2) + (j/2)*2],false);
								else AttrSet(dx,dy,palActive,false);
							}
							UpdateNameTable(dx,dy,false);
						}
					}
				}
			}

		}
		//traditional tile place behaviour below.
		else if(chrSelectRect)    //place tiles/attrs with chr box selection
		{
			for(i=0;i<chrSelection.bottom-chrSelection.top;++i)
			{
				for(j=0;j<chrSelection.right-chrSelection.left;++j)
				{
					dx=nameXC+j;
					dy=nameYC+i;

					if(dx>=0&&dx<nameTableWidth&&dy>=0&&dy<nameTableHeight)
					{
						if(SpeedButtonTiles->Down) nameTable[dy*nameTableWidth+dx]=tileViewTable[((chrSelection.top+i)<<4)+chrSelection.left+j];

						if(SpeedButtonPal->Down) AttrSet(dx,dy,palActive,false);

						UpdateNameTable(dx,dy,false);
					}
				}
			}
		}
		else    //place tiles / attrs with chr multi selection
		{
			xmin=16;
			ymin=16;
			xmax=0;
			ymax=0;

			//determine area
			for(i=0;i<16;++i)
			{
				for(j=0;j<16;++j)
				{
					if(!chrSelected[i*16+j]) continue;

					if(j<xmin) xmin=j;
					if(j>xmax) xmax=j;
					if(i<ymin) ymin=i;
					if(i>ymax) ymax=i;
				}
			}
			//place tiles
			for(i=ymin;i<=ymax;++i)
			{
				for(j=xmin;j<=xmax;++j)
				{
					if(!chrSelected[i*16+j]) continue;

					dx=nameXC+j-xmin;
					dy=nameYC+i-ymin;

					if(dx>=0&&dx<nameTableWidth&&dy>=0&&dy<nameTableHeight)
					{
						if(SpeedButtonTiles->Down) nameTable[dy*nameTableWidth+dx]=tileViewTable[i*16+j];

						if(SpeedButtonPal->Down) AttrSet(dx,dy,palActive,false);

						UpdateNameTable(dx,dy,false);
					}
				}
			}
		}

		UpdateNameTable(nameXC,nameYC,true);
	}
	

	else if(Shift.Contains(ssRight))
	{


			SelectTile(tileViewTable[nameTable[nameYC*nameTableWidth+nameXC]]);
			SelectPalette(AttrGet(nameXC,nameYC,false,false));

			UpdateTiles(true);
            if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;
			UpdateNameTable(-1,-1,true);
			if(FormNavigator->Visible) FormNavigator->Draw(false,false,false);

	}

	//if (!(X % (4*uiScale))||!(Y % (4*uiScale))) //obsolete.
	cueStats=true;

}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MOpenNameTableClick(TObject *Sender)
{
	BlockDrawing(true);
	
	if(OpenDialogName->Execute())
	{
		if(OpenNameTable(OpenDialogName->FileName, false))
		{
			OpenDialogName->FileName=RemoveExt(OpenDialogName->FileName);
			SaveDialogName->FileName=OpenDialogName->FileName;
			SaveDialogMap ->FileName=OpenDialogName->FileName;
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveNameTableCClick(TObject *Sender)
{
	AnsiString name,filter;
	unsigned char* buf;
	int size;
    
	BlockDrawing(true);

	SaveDialogName->FileName=RemoveExt(SaveDialogName->FileName);
	SaveDialogName->Title="Save nametable as C header";

	filter="C header (*.h)|*.h|Any file (*.*)|*.*";

	SaveDialogName->Filter=filter;

	if(SaveDialogName->Execute())
	{
		name=RemoveExt(SaveDialogName->FileName)+".h";

		if(FileExists(name))
		{
			if(Application->MessageBox(("File "+name+" already exists. Overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES)
			{
				BlockDrawing(false);
				return;
			}
		}

		buf=(unsigned char*)malloc(name_size()+attr_size());
		size=0;

		if(MSaveIncName->Checked)
		{
			memcpy(buf,nameTable,name_size());
			size+=name_size();
		}

		if(MSaveIncAttr->Checked)
		{
			memcpy(buf+size,attrTable,attr_size());
			size+=attr_size();
		}

		name=save_data(name.c_str(),buf,size,SAVE_FORMAT_H,MSaveRLE->Checked);

		free(buf);

		if(name!="") Application->MessageBox(name.c_str(),"Error",MB_OK);
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveNameTableBINClick(TObject *Sender)
{
	AnsiString name,filter;
	unsigned char* buf;
	int size; int f;
	bool b=false;

	BlockDrawing(true);

	SaveDialogName->FileName=RemoveExt(SaveDialogName->FileName);
	SaveDialogName->Title="Save as screen (nametable)";

	if(!MSaveRLE->Checked) {filter="Nametable (*.nam)|*.nam|Binary (*.bin)|*.bin|Assembly text (*.asm)|.asm|CA65 text (*.s)|*.s|C header (*.h)|*.h|RLE packed Nametable (*.nrle)|*.nrle|RLE asm text(*.asm)|.asm|RLE CA65 text(*.s)|*.s|RLE C header(*.h)|*.h";
		filter+="||Any file (*.*)|*.*";
	}
	else {
		filter="RLE packed Nametable (*.nrle)|*.nrle|RLE asm text(*.asm)|.asm|RLE CA65 text(*.s)|*.s|RLE C header(*.h)|*.h";
    	SaveDialogName->Title="Save as NESlib RLE packed screen (nametable)";
	}


	SaveDialogName->Filter=filter;

	if(SaveDialogName->Execute())
	{
		name=RemoveExt(SaveDialogName->FileName);

		if(!MSaveRLE->Checked){
			switch(SaveDialogName->FilterIndex)
			{
				case 1: name+=".nam";				f=SAVE_FORMAT_BIN; break;
				case 2: name+=".bin"; 				f=SAVE_FORMAT_BIN; break;
				case 3: name+=".asm";				f=SAVE_FORMAT_ASM; break;
				case 4: name+=".s";					f=SAVE_FORMAT_ASM; break;
				case 5: name+=".h";					f=SAVE_FORMAT_H;   break;
				case 6: name+=".nrle";	b=true;		f=SAVE_FORMAT_BIN; break;
				case 7: name+=".asm";	b=true;		f=SAVE_FORMAT_ASM; break;
				case 8: name+=".s";		b=true;		f=SAVE_FORMAT_ASM; break;
				case 9: name+=".h";		b=true;     f=SAVE_FORMAT_H;   break;

				default: name+=!MSaveRLE->Checked?".nam":".nrle"; f=SAVE_FORMAT_BIN;
			}
		}
		else {
             switch(SaveDialogName->FilterIndex)
			{
				case 1: name+=".nrle";	b=true;		f=SAVE_FORMAT_BIN; break;
				case 2: name+=".asm";	b=true;		f=SAVE_FORMAT_ASM; break;
				case 3: name+=".s";		b=true;		f=SAVE_FORMAT_ASM; break;
				case 4: name+=".h";		b=true;     f=SAVE_FORMAT_H;   break;

				default: name+=!MSaveRLE->Checked?".nam":".nrle"; f=SAVE_FORMAT_BIN;
		}}

		if(FileExists(name))
		{
			if(Application->MessageBox(("File "+name+" already exists. Overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES)
			{
				BlockDrawing(false);
				return;
			}
		}

		buf=(unsigned char*)malloc(1024);   //1 .nam
		size=0;
		int pp=0;
		int x = nameTableViewX;
        int y = nameTableViewY;
		int h = 30, w = 32;
		//if(nameTableHeight<=30) y=-1;

		if(nameTableHeight<32)
		{
			y = y + (32-nameTableHeight)/2;
		}
		int yBuf = y;

		if(MSaveIncName->Checked)
		{
			//memcpy(buf,nameTable,name_size());
			for(int i=0;i<h;i++)
			{
				for(int j=0;j<w;j++)
				{
					buf[pp]=nameTable[y*nameTableWidth+x+j];

				pp++;
				}
			y++;
			}
			size+=960;
		}

		if(MSaveIncAttr->Checked)
		{
			//memcpy(buf+size,attrTable,attr_size());
			y=yBuf;
			pp=0;
			for(int i=0;i<h;i++)
			{
				for(int j=0;j<w;j+=4)
				{
					//pp=y/4*((nameTableWidth+3)/4)+(x+j)/4;
					buf[size+pp]=attrTable[y/4*((nameTableWidth+3)/4)+(x+j)/4];
					//attrTable[y*nameTableWidth+x+j]
					//AttrGet(x+j,y,false,true);
					//attrTable[y/4*((nameTableWidth+3)/4)+(x+j)/4];
					//   //

				pp++;
				}
			y+=4;

			}
			size+=64;
		}

        if (f==SAVE_FORMAT_ASM) {
			if (byte1->Checked) {byte1bool=true; db1bool=false;}
			if (db1->Checked)   {byte1bool=false; db1bool=true;}
		}
		name=save_data(name.c_str(),buf,size,f,(MSaveRLE->Checked||b));

		free(buf);

		if(name!="") Application->MessageBox(name.c_str(),"Error",MB_OK);
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPaletteOpenClick(TObject *Sender)
{
	BlockDrawing(true);

	if(OpenDialogPal->Execute())
	{
		if(OpenPalette(OpenDialogPal->FileName, false))
		{
			OpenDialogPal->FileName=RemoveExt(OpenDialogPal->FileName);

			SaveDialogPal->FileName=OpenDialogPal->FileName;
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPaletteSaveClick(TObject *Sender)
{
	FILE *file;
	unsigned char pal[16];
	int i;
	AnsiString name;

	BlockDrawing(true);

	if(SaveDialogPal->Execute())
	{
		name=RemoveExt(SaveDialogPal->FileName)+".pal";

		if(!OverwritePrompt(name))
		{
			BlockDrawing(false);
			return;
		}

		pal_validate();

		for(i=0;i<4;i++)
		{
			pal[i+ 0]=bgPal[palBank*16+0*4+i];
			pal[i+ 4]=bgPal[palBank*16+1*4+i];
			pal[i+ 8]=bgPal[palBank*16+2*4+i];
			pal[i+12]=bgPal[palBank*16+3*4+i];
		}

		file=fopen(name.c_str(),"wb");

		if(file)
		{
			fwrite(pal,16,1,file);
			fclose(file);
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::PaintBoxPalPaint(TObject *Sender)
{
	DrawPalettes();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHREditorClick(TObject *Sender)
{
	FormCHREditor->Visible^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormKeyDown(TObject *Sender, WORD &Key,
TShiftState Shift)
{

bool bDoUpdate;

	bBufCtrl=Shift.Contains(ssCtrl)?true:false;     //these are chiefly used for the new drag mode actions, but could be used to condense and detangle many mouse events.
	bBufShift=Shift.Contains(ssShift)?true:false;   //idea - let these help status readouts in finer context?
	bBufAlt=Shift.Contains(ssAlt)?true:false;
	if(Key==VK_OEM_3) bBufVK_3=true;



	if (bBufCtrl || bBufShift || bBufAlt)
	{
		//this is for updating when drag has been accepted.
		if (bImageNameAccepted) cueUpdateNametableNoRepaint=true;
		else if (bImageTileAccepted){
			cueUpdateTiles=true;
            if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;
         }
		//this is for any other case. Probably redundant for now.

		//else{cueUpdateNametable=true; cueUpdateTiles=true;}  //include if thereÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s a legitimate use.
	}


	if(!Shift.Contains(ssCtrl)&&!Shift.Contains(ssShift))
	{
		bool b=false;
		if(Key==VK_F1) {FormMain->PageControlEditor->ActivePageIndex=0; b=true;}
		if(Key==VK_F2) {FormMain->PageControlEditor->ActivePageIndex=1; b=true;}
		if(Key==VK_F3) {FormMain->PageControlEditor->ActivePageIndex=2; b=true;}
		if (b) {FormMain->PageControlEditorChange(Sender);}
        //*/

		if(Key==VK_PAUSE)
		{
			FormMain->AlphaBlend^=true;
			if (!Shift.Contains(ssShift))
			{
				FormCHREditor->AlphaBlend^=true;
				FormManageMetasprites->AlphaBlend^=true;
			}
		}


		if(!SpeedButtonTypeIn->Down||(nameSelection.left<0&&nameSelection.top<0)){
        	if(PageControlEditor->ActivePage==TabSheetName)
			{
				if(Key=='T') {bIgnoreKey=true; TypeInModeOnOff1Click(TypeInModeOnOff1);}
			}
		}
		if(!SpeedButtonTypeIn->Down)
		{
			if(PageControlEditor->ActivePage==TabSheetSprite)
			{

			   if(Key=='H') FlipMetaSprites(true,false,true);
			   if(Key=='V') FlipMetaSprites(false,true,true);
			   if(Key=='P') SBPriorityToggle1Click(Sender);
			}

			else     //hotkeys that are disabled when in sprite editing mode
			{
                if(PageControlEditor->ActivePage==TabSheetName)
				{
					if(Key=='V') clickV=true;
					if(Key=='C') clickC=true;

					if(Key=='H') Tilegrid1Click(Screen32x301);
					if(Key=='N')if(!lineDrawing) {  FormMetatileTool->btnClonestamp->Down^=true;
						if(FormMetatileTool->btnClonestamp->Down) 	LabelStats->Caption="Metatile clonestamp ON.";
						else			   			   LabelStats->Caption="Metatile clonestamp OFF.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}
				}
					if(Key=='J') Tilegrid1Click(Tilegrid1);
					if(Key=='K') Tilegrid1Click(Tilegrid2);
					if(Key=='L') Tilegrid1Click(Tilegrid4);

			}
			if(PageControlEditor->ActivePage==TabSheetTile)
			{



				if(!lineDrawing)
				{
					if(Key=='T') {FormLineDetails->btnSmear->Down^=true;
						LineCoating1->Checked=FormLineDetails->btnSmear->Down;
						int i = iLinePresetIndex;
						bLinePreset_modeCoat[i]=FormLineDetails->btnSmear->Down;
						if(FormLineDetails->btnSmear->Down) 	LabelStats->Caption="Coat mode ON.";
						else									LabelStats->Caption="Coat mode OFF.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}
					if(Key=='K') {
						FormLineDetails->btnQuick->Down^=true;
						LineQuickmultiline1->Checked=FormLineDetails->btnQuick->Down;
						int i = iLinePresetIndex;
						bLinePreset_modeQuick[i]=FormLineDetails->btnQuick->Down;
						if(FormLineDetails->btnQuick->Down) 	LabelStats->Caption="Quick multiline ON.";
						else			   						LabelStats->Caption="Quick multiline OFF.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}

					/*  moved to hotkeyed menu actions
					if(Key=='5') {Protect0->Down^=true; FormCHREditor->Protect0->Down = Protect0->Down;
						if(Protect0->Down) 	LabelStats->Caption="Colour 0 Protected.";
						else			   	LabelStats->Caption="Colour 0 Overwritable.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}

					if(Key=='6') {Protect1->Down^=true; FormCHREditor->Protect1->Down = Protect1->Down;
						if(Protect1->Down) 	LabelStats->Caption="Colour 1 Protected.";
						else			   	LabelStats->Caption="Colour 1 Overwritable.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}

					if(Key=='7') {Protect2->Down^=true; FormCHREditor->Protect2->Down = Protect2->Down;
						if(Protect2->Down) 	LabelStats->Caption="Colour 2 Protected.";
						else			   	LabelStats->Caption="Colour 2 Overwritable.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}

					if(Key=='8') {Protect3->Down^=true; FormCHREditor->Protect3->Down = Protect3->Down;
						if(Protect3->Down) 	LabelStats->Caption="Colour 3 Protected.";
						else			   	LabelStats->Caption="Colour 3 Overwritable.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}
					*/
					if(Key=='H') FormCHREditor->MirrorHorizontal();
					if(Key=='V') FormCHREditor->MirrorVertical();
					if(Key=='R') FormCHREditor->SpeedButtonRotateCWClick(Sender);//{Flip90(false); Rotate4tiles(false);}
					if(Key=='G') {btnSmudge->Down=true; FormMain->btnSmudge->Down=true; bSmudge=true;}
					if(Key=='B') {FormCHREditor->btnThick->Down^=true; btnThick->Down = FormCHREditor->btnThick->Down;
						if(btnThick->Down) 	LabelStats->Caption="Toggled to Brush mode.";
						else			   	LabelStats->Caption="Toggled to Pen mode.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}

					if(Key=='N') {FormCHREditor->btnLine->Down^=true; btnLine->Down = FormCHREditor->btnLine->Down;
						if(btnThick->Down) 	LabelStats->Caption="Line mode ON.";
						else			   	LabelStats->Caption="Line mode OFF.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;
					}

					if(Key=='U') {FormCHREditor->btnQuant->Down^=true; btnQuant->Down = FormCHREditor->btnQuant->Down;
						if(btnQuant->Down) 	LabelStats->Caption="Quantized pixel placement ON.";
						else			   	LabelStats->Caption="Quantized pixel placement OFF.";
						StatusUpdateWaiter->Enabled=true; holdStats=true;

					}

					if(Key=='M') FormBrush->Visible^=true;

					if(Key=='F') FormBucketToolbox->ToggleFillMode();
					if(Key=='C') FormBucketToolbox->ToggleFloodMode();

				}
			}
			else{
                if(!lineDrawing)
				{
					if(Key=='R') Red1Click(Red1);
					if(Key=='G') Red1Click(Green1);
					if(Key=='B') Red1Click(Blue1);
					if(Key=='M') Red1Click(Gray1);
				}
			}
			//if(Key=='F' && !(nameSelection.left<0&&nameSelection.top<0)) FillMap();
			if(!lineDrawing)
			{
				if(Key=='X') Toggletileset1Click(Toggletileset1);
				if(Key=='A') Attributes1Click(Attributes1);
				if(Key=='S') SelectedOnly1Click(SelectedOnly1);
				if(Key=='W') ApplyTiles1Click(ApplyTiles1);
				if(Key=='D') Tilegrid1Click(GridOnOff1);
				if(Key=='E') ApplyAttributes1Click(ApplyAttributes1);
			}
            
		}
	}
	//*/
	if(!(nameSelection.left<0&&nameSelection.top<0))     //Photoshop alias
	{
		if(Shift.Contains(ssAlt))
		{
			if(Key==VK_BACK) FillMap(false);
		}

	}
	if(Shift.Contains(ssCtrl))
	{
		if((Key=='1')&&(uiScale!=1)) {uiScale=1;   UpdateUIScale();}
	}

	if(SpeedButtonTypeIn->Down&&PageControlEditor->ActivePage==TabSheetName)
	{
		switch(Key)
			{
			case VK_ESCAPE:
				SpeedButtonTypeIn->Down=false;
				StaticTextFontOffset->Visible=false;
				TypeInModeOnOff1->Checked=false;
				break;
			}
		if(nameSelection.left>=0)
		{
			switch(Key)
			{
			case VK_BACK:
			case VK_LEFT:   --nameSelection.left; break;

			case VK_RIGHT:  ++nameSelection.left; break;

			case VK_UP:    	--nameSelection.top;  break;

			case VK_RETURN: nameSelection.left=returnCarriagePos;
							if(!Shift.Contains(ssShift)) ++nameSelection.top;
							else --nameSelection.top;
							break;

			case VK_DOWN:  	++nameSelection.top;  break;
			}

			if(nameSelection.left<0) nameSelection.left=0;
			if(nameSelection.left>=nameTableWidth) nameSelection.left=nameTableWidth-1;
			if(nameSelection.top<0) nameSelection.top=0;
			if(nameSelection.top>=nameTableHeight) nameSelection.top=nameTableHeight-1;

			nameSelection.right =nameSelection.left+1;
			nameSelection.bottom=nameSelection.top +1;

            //bookmark: should these be moved or cued?
			UpdateNameTable(-1,-1,false);
			FormNavigator->Draw(false,false,false);
			cueStats=true;
		}

		return;
	}
	else
	{
		if(Key==VK_ESCAPE) {
			if( FormNavigator->Active) {
				FormNavigator->Close();
			}
		}
    }

	if(!Shift.Contains(ssCtrl))
	{
		if(Key==VK_OEM_4||Key==VK_OEM_COMMA) {SpeedButtonPrevMetaSpriteClick(Sender);cueStats=true;}// [
		if(Key==VK_OEM_6||Key==VK_OEM_PERIOD) {SpeedButtonNextMetaSpriteClick(Sender);cueStats=true;}// ]

		
		if(Key==(int)MapVirtualKey(0x27, 1)) FormBrush->ChangePreset(-1);
		if(Key==(int)MapVirtualKey(0x28, 1)) FormBrush->ChangePreset(+1);
		if(Key==(int)MapVirtualKey(0x2B, 1)) FormBrush->ChangePreset(+7);


		if(Key=='Q') FormCHREditor->Show();
		if(Key==VK_NUMPAD7) FormCHREditor->TileChange(-1,-1);
		if(Key==VK_NUMPAD8) FormCHREditor->TileChange( 0,-1);
		if(Key==VK_NUMPAD9) FormCHREditor->TileChange(+1,-1);

		if(Key==VK_NUMPAD4) FormCHREditor->TileChange(-1,0);
		if(Key==VK_NUMPAD5) MCHREditorClick(Sender);
		if(Key==VK_NUMPAD6) FormCHREditor->TileChange(+1,0);

		if(Key==VK_NUMPAD1) FormCHREditor->TileChange(-1,+1);
		if(Key==VK_NUMPAD2) FormCHREditor->TileChange( 0,+1);
		if(Key==VK_NUMPAD3) FormCHREditor->TileChange(+1,+1);
	}

	if(PageControlEditor->ActivePage==TabSheetName)
	{
		if(Shift.Contains(ssCtrl))
		{
			if(Key==VK_OEM_4) {ChangeNameTableFrame(-1); cueStats=true;}// [
			if(Key==VK_OEM_6) {ChangeNameTableFrame(1); cueStats=true;}// ]

		}

		if(!Shift.Contains(ssAlt))
		{
			if(!Shift.Contains(ssCtrl))
			{
				bDoUpdate=false;
				if(Key==VK_LEFT)  {nameTableViewX-=4; bDoUpdate=true;}
				if(Key==VK_RIGHT) {nameTableViewX+=4; bDoUpdate=true;}
				if(Key==VK_UP)    {nameTableViewY-=4; bDoUpdate=true;}
				if(Key==VK_DOWN)  {nameTableViewY+=4; bDoUpdate=true;}

				if(bDoUpdate){
					CorrectView();
					UpdateNameTable(-1,-1,true);
					FormNavigator->CueLinesTimer->Enabled=true;
					//FormNavigator->Draw(false,false,false);
					cueStats=true;
					}
			}
			else
			{
				if(Key==VK_LEFT)  NameTableScrollLeft (Shift.Contains(ssShift));
				if(Key==VK_RIGHT) NameTableScrollRight(Shift.Contains(ssShift));
				if(Key==VK_UP)    NameTableScrollUp   (Shift.Contains(ssShift));
				if(Key==VK_DOWN)  NameTableScrollDown (Shift.Contains(ssShift));

			}
		}
		else
		{
			if(nameSelection.left>=0)
			{
				bDoUpdate=false;
				if(Key==VK_LEFT)
				{
					if(nameSelection.left>0)
					{
						--nameSelection.left;
						--nameSelection.right;
						bDoUpdate=true;
					}
				}

				if(Key==VK_RIGHT)
				{
					if(nameSelection.right<nameTableWidth)
					{
						++nameSelection.left;
						++nameSelection.right;
						bDoUpdate=true;
					}
				}

				if(Key==VK_UP)
				{
					if(nameSelection.top>0)
					{
						--nameSelection.top;
						--nameSelection.bottom;
						bDoUpdate=true;
					}
				}

				if(Key==VK_DOWN)
				{
					if(nameSelection.bottom<nameTableHeight)
					{
						++nameSelection.top;
						++nameSelection.bottom;
						bDoUpdate=true;
					}
				}

				if(bDoUpdate) {
					UpdateNameTable(-1,-1,false);
					FormNavigator->CueLinesTimer->Enabled=true;
					//FormNavigator->Draw(false,false,false);
					cueStats=true;
					}
			}
		}
	}
	else
	{
        int sprMov;
		if(Shift.Contains(ssShift)) sprMov=8; else sprMov=1;
		if(Key==VK_LEFT)  MoveSprite(-sprMov, 0);
		if(Key==VK_RIGHT) MoveSprite( sprMov, 0);
		if(Key==VK_UP)    MoveSprite( 0,-sprMov);
		if(Key==VK_DOWN)  MoveSprite( 0, sprMov);
		cueStats=true;
	}

	if (GetKeyState(VK_CAPITAL)&&(!SpeedButtonTypeIn->Down))
	{
		if (bAllowLockMessage) {
			AnsiString lockbuf;
			lockbuf=LabelStats->Caption;
			lockbuf+="\nStats locked. Press [CAPS LOCK] to unlock.";
			LabelStats->Caption=lockbuf;
			bAllowLockMessage=false;
			cueStats=true;
		}
	}
	


	//cueStats=true;

}

//---------------------------------------------------------------------------



void __fastcall TFormMain::ImageTilesDblClick(TObject *Sender)
{
	/*   //old behaviour - left in case i change my mind.
	if(SpeedButtonDrawTile->Down)
	{
	if (FormCHREditor->Visible) return;
	}
	FormCHREditor->Show();
	*/
	if(SpeedButtonDrawTile->Down) return;
	if(SpeedButtonTypeIn->Down) return;

	FormCHREditor->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormDestroy(TObject *Sender)
{

	delete BufCheckerStripes;
	delete BufCheckerClassic;

	delete BufBmpName;
	delete BufBmpTiles;

	free (chr);
	free (undoChr);
	free (checkpointChr);
	free (undoCheckpointChr);
	free (tmp_chr);




	free (metatileSet_2x2_id);
	free (undo_metatileSet_2x2_id);
	free (checkpoint_metatileSet_2x2_id);
	free (undo_checkpoint_metatileSet_2x2_id);

	free (metatileSet_2x2_pal);
	free (undo_metatileSet_2x2_pal);
	free (checkpoint_metatileSet_2x2_pal);
	free (undo_checkpoint_metatileSet_2x2_pal);

	free (metatileSet_2x2_props);
	free (undo_metatileSet_2x2_props);
	free (checkpoint_metatileSet_2x2_props);
	free (undo_checkpoint_metatileSet_2x2_props);


	free (metatileSet_4x4_id);
	free (undo_metatileSet_4x4_id);
	free (checkpoint_metatileSet_4x4_id);
	free (undo_checkpoint_metatileSet_4x4_id);

	free (metatileSet_4x4_pal);
	free (undo_metatileSet_4x4_pal);
	free (checkpoint_metatileSet_4x4_pal);
	free (undo_checkpoint_metatileSet_4x4_pal);

	free (metatileSet_4x4_props);
	free (undo_metatileSet_4x4_props);
	free (checkpoint_metatileSet_4x4_props);
	free (undo_checkpoint_metatileSet_4x4_props);


	free (metatileSet_8x8_id);
	free (undo_metatileSet_8x8_id);
	free (checkpoint_metatileSet_8x8_id);
	free (undo_checkpoint_metatileSet_8x8_id);

	free (metatileSet_8x8_pal);
	free (undo_metatileSet_8x8_pal);
	free (checkpoint_metatileSet_8x8_pal);
	free (undo_checkpoint_metatileSet_8x8_pal);

	free (metatileSet_8x8_props);
	free (undo_metatileSet_8x8_props);
	free (checkpoint_metatileSet_8x8_props);
	free (undo_checkpoint_metatileSet_8x8_props);

	/*
	free(mtUsage_2x2);
	free(mtUsage_4x4);
	free(mtUsage_8x8);
    */
	free(mtContent_2x2);
	free(mtContent_4x4);
	free(mtContent_8x8);

	//----metatile unpacking space for merging outside sets.
	free(metatileBuf_2x2_id);
	free(metatileBuf_2x2_pal);
	free(metatileBuf_2x2_props);

	free(metatileBuf_4x4_id);
	free(metatileBuf_4x4_pal);
	free(metatileBuf_4x4_props);

	free(metatileBuf_8x8_id);
	free(metatileBuf_8x8_pal);
	free(metatileBuf_8x8_props);



	char path[MAX_PATH];
	int len;

	GetCurrentDirectory(len,path);
	reg_save_str(regWorkingDirectory,path);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageNameMouseLeave(TObject *Sender)
{

	//TODO: look into maxing the selection on the given axis here
	//hereÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s some copypasta to remember variable names by
	/*
	if(mouseDraggedNTSel==true)
	{

	nameSelection.right =nameXC+(nameXC>=nameSelection.left?1:0);
	nameSelection.bottom=nameYC+(nameYC>=nameSelection.top ?1:0);
	UpdateNameTable(-1,-1,true);
	oldNameXC=nameXC;
	oldNameYC=nameYC;
	}
    */

	nameXC=-1;
	nameYC=-1;
	bMouseOverNam=false;	    //used by DrawTile to identify the target of auto screen grid.
	mouseDraggedNTSel=false;    //redundancies
	mouseDraggedTileSel=false;
	UpdateNameTable(-1,-1,true);
	//UpdateStats();
	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageTilesMouseLeave(TObject *Sender)
{
	tileXC=-1;
	tileYC=-1;

	
		bDrawDestShadow=false;
		cueUpdateTiles=true;
	
	mouseDraggedNTSel=false; 	//redundancies
	mouseDraggedTileSel=false;
	
	//UpdateStats();
	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageTilesMouseMove(TObject *Sender,
TShiftState Shift, int X, int Y)
{
	int i,j,tx,ty;
	int xs,ys,wdt,hgt;
	int set=bankActive/16;
    int bpp,bufpp;
	tx=X/(8*uiScale);
	ty=Y/(8*uiScale);
	main_tilesetScrollX=X;
	main_tilesetScrollY=Y;

	int ix,iy; //pixel modifiers for thick pen
	int gx,gy,g; //guards for thick pen
	int iThick=FormCHREditor->btnThick->Down?iBrushSize[iBrushPresetIndex]:1;

	if(Sender==ImageTiles&&PageControlEditor->ActivePage==TabSheetTile) return;
	if(IsBlockDrawing()) return;

	if(
		(!Shift.Contains(ssShift)
			||(Shift.Contains(ssShift)&&Shift.Contains(ssCtrl))
			||(Shift.Contains(ssShift)&&lineDrawing))
			&&(SpeedButtonDrawTile->Down&(X>=0&&X<(128*uiScale)&&Y>=0&&Y<(128*uiScale))))
	{
		int px,py,pp,extpp,mask,tempPal;

		py = (Y/uiScale)%8; 	//also used separately by fill.
		px = (X/uiScale)%8; 	//which pixel in 8x1 sliver
		//pp = py			 		+(X/(8*uiScale))*16 	+(Y/(8*uiScale))*256;
		//	^-which sliver		^-which column	 		^-which row

		extpp = tileViewTable[(X/(8*uiScale)) +(Y/(8*uiScale))*16]*16 + bankViewTable[set+(X/(8*uiScale)) +(Y/(8*uiScale))*16];  //used for fill
		bufpp = tileViewTable[(X/(8*uiScale)) +(Y/(8*uiScale))*16]*16;  //used for fill

		pp	   = extpp+py;  //used for pen
		bpp	   = tileViewTable[(X/(8*uiScale)) +(Y/(8*uiScale))*16]*16+py; //used for pen

		if(FormCHREditor->btnQuant->Down){px=px&14;pp=pp&0x1FFE;}
		mask=128>>px;


		if(Shift.Contains(ssLeft))
		{

		if(lineDrawing)
		{
				 FormCHREditor->Line(Shift,lineDownX,lineDownY,X,Y,1);
		}
		else
		{
			//if there is a selection larger than 1x1, only allow drawing within it.
			if(abs(chrSelection.left-chrSelection.right)>1||abs(chrSelection.top-chrSelection.bottom)>1)
			{
				if(chrSelectRect
					&((chrSelection.right<=tx)
					|(chrSelection.left>tx)
					|(chrSelection.bottom<=ty)
					|(chrSelection.top>ty)))
					return;
			}	

			for(int br_y=0; br_y<iThick; br_y++)
			 {
				for(int br_x=0; br_x<iThick; br_x++)
				{
					if(FormCHREditor->btnThick->Down)
					{
						if((*ptr_tableBrush[iBrushPresetIndex])[br_x][br_y]==false) continue;

						ix=br_x;
						iy=br_y;
						int pre_x = X/uiScale;
						int pre_y = Y/uiScale;
						if(FormCHREditor->btnQuant->Down){  //pixel pass
							int qsx = iBrushSnapSize_x[iBrushPresetIndex];
							int qsy = iBrushSnapSize_y[iBrushPresetIndex];

							pre_x=((pre_x/ qsx)*qsx);
							pre_y=((pre_y/ qsy)*qsy);
						}
						py 	= (pre_y)%8;      //which sliver
						px 	= (pre_x)%8;      //which pixel in 8x1 sliver
						tx	= pre_x/(8);      //which column
						ty	= pre_y/(8);	  //which row


						//guard values for crossing borders
						px+=ix;
						py+=iy;

						gx=px;
						gy=py;
						g=8;

						//if(!FormCHREditor->btnQuant->Down){}
						tx+=gx/g;
						ty+=gy/g;

						px&=7;
						py&=7;

						//pp = py+tx*16+ty*256; //selects to which char to operate.
						pp = tileViewTable[tx+ty*16]*16+bankViewTable[set+tx+ty*16]+py;
						bpp = tileViewTable[tx+ty*16]*16+py;
						mask=128>>px;         //used for selecting bits in char.
					}

					tempPal=(((chrBuf[bpp]<<px)&128)>>7)|(((chrBuf[bpp+8]<<px)&128)>>6);
					fillPal=(((chrBufFill[bpp]<<px)&128)>>7)|(((chrBufFill[bpp+8]<<px)&128)>>6);
					if(Applytopen2->Checked){
						if(FormCHREditor->Protect0->Down && tempPal==0) continue;
						if(FormCHREditor->Protect1->Down && tempPal==1) continue;
						if(FormCHREditor->Protect2->Down && tempPal==2) continue;
						if(FormCHREditor->Protect3->Down && tempPal==3) continue;
					}
					if(IncDecPerclick1->Checked==false)
					{
						tmpContinousIncDecTimer++;
						if (tmpContinousIncDecTimer>continousIncDecDuration){
							tmpContinousIncDecTimer=0;
							//memcpy (chrBuf, chr, 8192);   //FIX
							for(int tile=0;tile<256;tile++){
								memcpy(chrBuf+tile*16,chr+tileViewTable[tile]*16 + bankViewTable[set+tile],16);
							}

						}
					}

					if(!Shift.Contains(ssCtrl)){
						if (penActive==0&&(!Shift.Contains(ssShift))){
							if(!bSmudge){
								if(ButtonBitmaskLo->Down||!FormMain->Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((bgPalCur&1)<<7)>>px);
								if(ButtonBitmaskHi->Down||!FormMain->Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((bgPalCur&2)<<6)>>px);
							}
							else{
								if(ButtonBitmaskLo->Down||!FormMain->Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((iSmudge&1)<<7)>>px);
								if(ButtonBitmaskHi->Down||!FormMain->Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((iSmudge&2)<<6)>>px);
							}
						}
						if (penActive==1&&(!Shift.Contains(ssShift))){
							if(!Shift.Contains(ssAlt))tempPal++;
							else tempPal--;

							if(IncDecCap1->Checked==true){
								if (tempPal >3) tempPal = 3;
								if (tempPal <0) tempPal = 0;
							}
							if(FormCHREditor->ButtonBitmaskLo->Down||!Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((tempPal&1)<<7)>>px);
							if(FormCHREditor->ButtonBitmaskHi->Down||!Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((tempPal&2)<<6)>>px);
						}
						if (penActive==2&&(!Shift.Contains(ssShift))){
							if(!Shift.Contains(ssAlt))tempPal--;
							else tempPal++;

							if(IncDecCap1->Checked==true){
								if (tempPal >3) tempPal = 3;
								if (tempPal <0) tempPal = 0;
							}
							if(FormCHREditor->ButtonBitmaskLo->Down||!Applytopen1->Checked) chr[pp]=(chr[pp]&~mask)|(((tempPal&1)<<7)>>px);
							if(FormCHREditor->ButtonBitmaskHi->Down||!FormMain->Applytopen1->Checked) chr[pp+8]=(chr[pp+8]&~mask)|(((tempPal&2)<<6)>>px);
						}
					}
					else
					{
						//extpp=tileViewTable[tx+ty*16]*16+bankViewTable[set+tx+ty*16];

						FormCHREditor->Fill(Shift,px,py,tx,ty,extpp,bufpp,false); //tempPal,fillPal
					}
				}
			 }
			}
			cueUpdateTiles=true;
			cueUpdateMetasprite=true;
			cueUpdateNTstrip=true;
		}

		if(Shift.Contains(ssRight))
		{
			bgPalCur=(((chr[pp]<<px)&128)>>7)|(((chr[pp+8]<<px)&128)>>6);
			FormMain->DrawPalettes();
		}

	//recalc stuff for the status readout.
	tx=X/(8*uiScale);
	ty=Y/(8*uiScale);
	tileXC=tx;
	tileYC=ty;
	cueStats=true;
	return;
	}


	if(SpeedButtonTypeIn->Down) return;

	if(!lineDrawing){
	if(Shift.Contains(ssLeft)&&!(Shift.Contains(ssShift)||Shift.Contains(ssCtrl)))
	{
		//this enables the user to "scan" tiles with left click + drag
		if(X>=0&&X<(128*uiScale)&&Y>=0&&Y<(128*uiScale))
		{
			tx=X/(8*uiScale);
			ty=Y/(8*uiScale);

			SetTile(tx+ty*16);
			cueUpdateNametable=true;

			cueCHRdrawAll=true;
		}
		else return; 
	}

	if((!Shift.Contains(ssShift))||(X>=0&&X<(128*uiScale)&&Y>=0&&Y<(128*uiScale)))
	{
		tx=X/(8*uiScale);
		ty=Y/(8*uiScale);
		tileXC=tx;
		tileYC=ty;

		if(Shift.Contains(ssShift)&&!Shift.Contains(ssCtrl))
		{
			//drag selection
			if(Shift.Contains(ssRight)&&Shift.Contains(ssShift)&&!bOutsideSel)
			{
		
				chrSelection.left=chrSelBuf.left+tx-txDown;
				chrSelection.right=chrSelBuf.right+tx-txDown;
				chrSelection.top=chrSelBuf.top+ty-tyDown;
				chrSelection.bottom=chrSelBuf.bottom+ty-tyDown;

				for (i=0; i<16; i++)  //long enough loop - felt safer than while
				{
					if(chrSelection.left<0)   	{	chrSelection.left++;
													chrSelection.right++;}
					if(chrSelection.right>0x10)	{	chrSelection.left--;
													chrSelection.right--;}
					if(chrSelection.top<0)   	{	chrSelection.top++;
													chrSelection.bottom++;}
					if(chrSelection.bottom>0x10){	chrSelection.top--;
													chrSelection.bottom--;}
				}
				cueUpdateTiles=true;
				cueUpdateNametable=true;


			}

			//box selection
			if(Shift.Contains(ssLeft))
				{
					mouseDraggedTileSel=true;
					mouseDraggedNTSel=true;


					if(tx<txDown) {chrSelection.left=tx+1-(tx<chrSelection.right?1:0);
								  chrSelection.right=txDown+1;
								  }
					if(tx>=txDown) {chrSelection.right =tx+(tx>=chrSelection.left?1:0);
								  chrSelection.left=txDown;

								  }

					if(ty<tyDown)  {chrSelection.top=ty-(ty>=chrSelection.bottom ?1:0);
								   chrSelection.bottom=tyDown+1;

								   }
					if(ty>=tyDown) {chrSelection.bottom=ty+(ty>=chrSelection.top ?1:0);
								   chrSelection.top=tyDown;
								  
								   }
				   tileActive=chrSelection.top*16+chrSelection.left;



					//note: this is also performed in mousedown
					int exceptionSize =FormCHREditor->btn2x2mode->Down? 2:1;
					if(   abs(chrSelection.left-chrSelection.right)!=exceptionSize
						||abs(chrSelection.top-chrSelection.bottom)!=exceptionSize)
                    {
						nameSelection.left=-1;
						nameSelection.top =-1;

						UpdateNameTable(-1,-1,true);  //bookmark: should this be cued?
					}
					

				
					
		}  //
		for(i=0;i<256;i++) chrSelected[i]=false;

					//some of this is probably unwarranted now that there is no inverted selection weirdness anymore. Keeping it for redundancy.
					//--------------
					xs=chrSelection.left<chrSelection.right ?chrSelection.left:chrSelection.right;
					ys=chrSelection.top <chrSelection.bottom?chrSelection.top :chrSelection.bottom;

					//first check for negatives, later on we derive absolutes.
					wdt=(chrSelection.right -chrSelection.left);
					hgt=(chrSelection.bottom-chrSelection.top);

					//these are used by the new scroll/wrap tile routines, as well as the new drag-swap.
					tileSelRectWdt=abs(wdt);
					tileSelRectHgt=abs(hgt);

					//overwrite. similar to std::max except i did this instead.

					if (wdt<0)tileSelRectWdt=0;
					if (hgt<0)tileSelRectHgt=0;

					//these are used for the rest of nesst vanilla code below
					wdt=abs(wdt);
					hgt=abs(hgt);


					for(i=0;i<hgt;i++)
						{
						for(j=0;j<wdt;j++)
						{
							chrSelected[(i+ys)*16+j+xs]=true;
						}
					}
					chrSelectRect=true;
		}

		//multi-select by dragging
		if(Shift.Contains(ssCtrl)&&(Shift.Contains(ssLeft)||Shift.Contains(ssRight)))
		{
			bool bTmp = bMultiSelectRemoveMode;
			chrSelected[ty*16+tx]=Shift.Contains(ssLeft)?!bTmp:bTmp;
			chrSelectRect=false;

			if(SpeedButtonSelTiles->Down) UpdateNameTable(-1,-1,true);
		}
	 }
	 else
	 {
		tileXC=-1;
		tileYC=-1;
	 }

	 if (wdt!=oldCHRRectWdt || hgt!=oldCHRRectHgt)
	 {
		UpdateTiles(false);

	 }

	 FormCHRbit->BitBtnTimer->Enabled=true;
	 cueStats=true;

	 oldCHRRectWdt=wdt;
	 oldCHRRectHgt=hgt;
	 }

}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MCHRClearClick(TObject *Sender)
{
	int i;

   //if(Application->MessageBox("Do you really want to clear 8K CHR?","Confirm",MB_YESNO)==ID_YES)
   //{
		SetUndo();
		int set=bankActive/16;
		for(i=0;i<4096;i++) chr[bankViewTable[set + i/16]+i]=0;
		UpdateAll();
		/*
		UpdateTiles(true);
		UpdateNameTable(-1,-1,true);
		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
		*/
		LabelStats->Caption="Tiles in current tileview cleared! Undo set.";
		StatusUpdateWaiter->Enabled=true;

		holdStats=true;
	//}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonChrBank1Click(TObject *Sender)
{
	int tag;
	TSpeedButton *speedButton = dynamic_cast<TSpeedButton*>(Sender);
	if (speedButton){
		tag = speedButton->Tag;
	}


	bankActive=tag*4096;

	FormBankCHR->btnA->Down=SpeedButtonChrBank1->Down;
	FormBankCHR->btnB->Down=SpeedButtonChrBank2->Down;
	FormBankCHR->btnC->Down=SpeedButtonChrBank3->Down;
	FormBankCHR->btnD->Down=SpeedButtonChrBank4->Down;

	UpdateAll();
	/*
	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false);
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	FormBankCHR->Update();*/
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRRemoveDoublesClick(TObject *Sender)
{
	//FG: this, along with the other remove/sort actions, were modified to toggle between do/undo for the sake of fluidity.
	if(MCHRRemoveDoubles->Checked)
	{
		Undo();
		MCHRRemoveDoubles->Checked=false;
		MCHRRemoveDoubles->Caption="&Remove duplicates";
	}
	else
	{
		RemoveDoublesUnused(false);
		MCHRRemoveDoubles->Checked=true;
		MCHRRemoveDoubles->Caption="Undo &remove duplicates";
		LabelStats->Caption="Duplicates removed. \n\nTip: Press again to undo.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PaintBoxPalMouseMove(TObject *Sender,
TShiftState Shift, int X, int Y)
{
	int n;

	palHover=-1;
	palColHover=-1;
	colHover=-1;

	n=check_palette_mouse(X,Y);

	if(n>=0)
	{
		if(n&0x100)
		{
			colHover=n&0xff;
		}
		else
		{
			palHover=n&3;
			palColHover=n>>4;
		}
	}

	//UpdateStats();
	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PaintBoxPalMouseLeave(TObject *Sender)
{
	palHover=-1;
	palColHover=-1;
	colHover=-1;
	//UpdateStats();
	cueStats=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MCHRInterleaveClick(TObject *Sender)
{
	InterleaveCHR(true);

	LabelStats->Caption="CHR Interleaved.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRDeinterleaveClick(TObject *Sender)
{
	InterleaveCHR(false);

	LabelStats->Caption="CHR De-interleaved.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSwapColorsClick(TObject *Sender)
{
	bool swap[256*4];
	int i,j,k,pp,col,bit;
	int ib, is, bankOff;
	unsigned char paltemp[4];

   //prep buffer so we can perform previews
     for(int set=0;set<4;set++){
		for(int tile=0;tile<256;tile++){
			memcpy(chrBuf+set*4096+tile*16,chr+bankViewTable[set*256 + tile]+tile*16,16);
		}
	}
   //	memcpy (chrBuf, chr, 4096*chrBanks);
	memcpy (palBuf, bgPal, 4*16);

	FormSwapColors->ShowModal();


	// restore.
	for(int set=0;set<4;set++){
		for(int tile=0;tile<256;tile++){
			memcpy(chr+bankViewTable[set*256 + tile]+tile*16,chrBuf+set*4096+tile*16,16);
		}
	}
	//memcpy (chr, chrBuf, 4096*chrBanks);
	memcpy (bgPal, palBuf, 4*16);

	if(FormSwapColors->Swap)
	{
		SetUndo();

		for(i=0;i<256*4;i++) swap[i]=false;

        if(!FormSwapColors->RadioPatternNone->Checked)
		{

			if(FormSwapColors->Selection)
			{
				for(int set=0;set<4;set++){
					if(bankActive/4096 == set ){
						for(i=0;i<256;i++) swap[(set*256)+i]=chrSelected[i];
					}
				}
			}
			else
			{
				if(FormSwapColors->WholeCHR)
				{   //all 4 views
					for(i=0;i<256*4;i++) swap[i]=true;
				}
				else
				{   //current view
					for(i=0;i<256;i++) swap[(bankActive/4096*256)+i]=true;
				}
			}
			//do per views
			for(int set=0;set<4;set++){
				for(i=0;i<256;i++)
				{
					if(swap[i])
					{
						pp=bankViewTable[set*256+i]+i*16;

						for(j=0;j<8;j++)
						{
							for(k=0;k<8;k++)
							{
								bit=1<<k;
								col=((chr[pp]&bit)?1:0)|((chr[pp+8]&bit)?2:0);
								col=FormSwapColors->Map[col];
								chr[pp]=(chr[pp]&~bit)|((col&1)<<k);
								chr[pp+8]=(chr[pp+8]&~bit)|((col>>1)<<k);
							}
							pp++;
						}
					}
				}
			}
		}

		if(FormSwapColors->RemapPalette)
		{
			if(FormSwapColors->RadioPalOne->Checked) {ib=iSwap_WhichSubpal; is=1; bankOff=palBank;}
					else if(FormSwapColors->RadioPalAll->Checked) {ib=0; is=4*4; bankOff=0;}
					else {ib=0;is=4;bankOff=palBank;}
					for(i=ib;i<is+ib;i++)
					{
						for(j=0;j<4;j++) paltemp[j]=bgPal[bankOff*16+i*4+FormSwapColors->Map[j]];
						for(j=0;j<4;j++) bgPal[bankOff*16+i*4+j]=paltemp[j];
					}
		}
		if(sharedCol0)
		{
			//this overrides the results of pal_validate by overwriting its input

			col=bgPal[palBank*16+iSwap_Pal0Subpal*4+0];

			bgPal[palBank*16+0*4+0]=col;
			bgPal[palBank*16+1*4+0]=col;
			bgPal[palBank*16+2*4+0]=col;
			bgPal[palBank*16+3*4+0]=col;
		}
		
	//WorkCHRToBankCHR();
	}
    pal_validate();

	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);
	if(FormNavigator->Visible) FormNavigator->Draw(false,false,false);
	DrawPalettes();
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;


}

//---------------------------------------------------------------------------

void __fastcall TFormMain::MAddOffsetClick(TObject *Sender)
{
	int i,j,x,y,w,h,off;

	FormNameOffset->Mode=0;
	FormNameOffset->ShowModal();

	if(FormNameOffset->MakeOffset)
	{
		if(FormNameOffset->SelectedOnly)
		{
			x=-1;

			if(nameSelection.left>=0&&nameSelection.top>=0)
			{
				x=nameSelection.left;
				y=nameSelection.top;
				w=nameSelection.right-nameSelection.left;
				h=nameSelection.bottom-nameSelection.top;
			}
		}
		else
		{
			x=0;
			y=0;
			w=nameTableWidth;
			h=nameTableHeight;
		}

		if(x>=0)
		{
			SetUndo();

			for(i=0;i<h;++i)
			{
				for(j=0;j<w;++j)
				{
					off=(y+i)*nameTableWidth+(x+j);

					if(nameTable[off]>=FormNameOffset->From&&nameTable[off]<=FormNameOffset->To) nameTable[off]+=FormNameOffset->Offset;
				}
			}

			UpdateNameTable(-1,-1,true);
			FormNavigator->Draw(false,false,false);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMaskBClick(TObject *Sender)
{
	ppuMask^=0x80;
	ppuMaskSet[palBank]=ppuMask;
	UpdateRGBM();
	palette_calc();
	UpdateTiles(true);
    if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	DrawPalettes();
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMaskGClick(TObject *Sender)
{
	ppuMask^=0x40;
	ppuMaskSet[palBank]=ppuMask;
	UpdateRGBM();
	palette_calc();
	UpdateTiles(true);
    if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	DrawPalettes();
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMaskRClick(TObject *Sender)
{
	ppuMask^=0x20;
	ppuMaskSet[palBank]=ppuMask;
	UpdateRGBM();
	palette_calc();
	UpdateTiles(true);
    if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	DrawPalettes();
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMaskMClick(TObject *Sender)
{
	ppuMask^=0x01;
	ppuMaskSet[palBank]=ppuMask;
	UpdateRGBM();
	palette_calc();
	UpdateTiles(true);
    if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	DrawPalettes();
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MExportNametableBMPClick(TObject *Sender)
{
	TPicture *picture;
	Graphics::TBitmap *bmp;
	int i,j,x,y;
	AnsiString str;

	BlockDrawing(true);

	str=RemoveExt(SaveDialogName->FileName);

	if(str=="") str="nametable";

	SaveDialogImage->FileName=str+".bmp";

	if(SaveDialogImage->Execute())
	{
		picture=new TPicture();
		bmp=new Graphics::TBitmap();
		bmp->SetSize(nameTableWidth*8,nameTableHeight*8);
		bmp->PixelFormat=pf4bit;
		SetBMPPalette(bmp);
		picture->Bitmap=bmp;

		y=0;

		for(i=0;i<nameTableHeight;i++)
		{
			x=0;

			for(j=0;j<nameTableWidth;j++)
			{
				DrawExportTile16(picture,x,y,nameTable[i*nameTableWidth+j],AttrGet(j,i,false,false),false);

				x+=8;
			}

			y+=8;
		}

		picture->SaveToFile(SaveDialogImage->FileName);

		delete bmp;
		delete picture;
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MExportTilesetBMPClick(TObject *Sender)
{
	TPicture *picture;
	Graphics::TBitmap *bmp;
	int i,x,y;
	AnsiString str;

	BlockDrawing(true);

	str=RemoveExt(SaveDialogChr->FileName);

	if(str=="") str="tileset";

	SaveDialogImage->FileName=str+".bmp";

	if(SaveDialogImage->Execute())
	{
		picture=new TPicture();
		bmp=new Graphics::TBitmap();
		bmp->SetSize(128,128);
		bmp->PixelFormat=pf4bit;
		SetBMPPalette(bmp);
		picture->Bitmap=bmp;

		x=0;
		y=0;

		for(i=0;i<256;i++)
		{
			DrawExportTile16(picture,x,y,i,palActive,false);

			x+=8;

			if(x>=128)
			{
				x=0;
				y+=8;
			}
		}

		//test
		unsigned short picFormat;
		unsigned int cbHandle;
		HPALETTE hPal;

		picture->SaveToClipboardFormat(picFormat, cbHandle, hPal);
		Clipboard()->SetAsHandle(picFormat, cbHandle);

		picture->SaveToFile(SaveDialogImage->FileName);

		delete bmp;
		delete picture;
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------



void __fastcall TFormMain::SpeedButtonCheckerClick(TObject *Sender)
{
	Attributes1->Checked = SpeedButtonChecker->Down;
	UpdateNameTable(-1,-1,true);
	if(FormNavigator->Visible) FormNavigator->Draw(false,false,false);
	/*LabelStats->Caption=LabelStats->Caption
		+"\n\nCheck!";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;*/
}
//---------------------------------------------------------------------------




void __fastcall TFormMain::MSaveNameTableASMClick(TObject *Sender)
{
	AnsiString name,filter;
	unsigned char* buf;
	int size;

	BlockDrawing(true);

	SaveDialogName->FileName=RemoveExt(SaveDialogName->FileName);
	SaveDialogName->Title="Save nametable as assembly code";

	filter="Assembly code (*.asm)|*.asm|CA65 assembly code (*.s)|*.s|Any file (*.*)|*.*";

	SaveDialogName->Filter=filter;

	if(SaveDialogName->Execute())
	{
		name=RemoveExt(SaveDialogName->FileName)+(SaveDialogName->FilterIndex!=2?".asm":".s");

		if(FileExists(name))
		{
			if(Application->MessageBox(("File "+name+" already exists. Overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES)
			{
				BlockDrawing(false);
				return;
			}
		}

		buf=(unsigned char*)malloc(name_size()+attr_size());
		size=0;

		if(MSaveIncName->Checked)
		{
			memcpy(buf,nameTable,name_size());
			size+=name_size();
		}

		if(MSaveIncAttr->Checked)
		{
			memcpy(buf+size,attrTable,attr_size());
			size+=attr_size();
		}

		//hotfix that could probably be refactored away, but this works.
		if (byte1->Checked) {byte1bool=true; db1bool=false;}
		if (db1->Checked)   {byte1bool=false; db1bool=true;}

		name=save_data(name.c_str(),buf,size,SAVE_FORMAT_ASM,MSaveRLE->Checked);

		free(buf);

		if(name!="") Application->MessageBox(name.c_str(),"Error",MB_OK);
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRFillNumbersClick(TObject *Sender)
{
	int set=bankActive/16;
	int i,j,k,pp;
	int x,y,w,h;
	bool skip[256]={false};
	bool doWhole;
	int cnt=0;

	//these allow nonselected tiles to be skipped.
	GetSelection(chrSelection,x,y,w,h);
	if(chrSelectRect)
	{
		if(w>1||h>1)
		{
			for(i=0;i<16;++i)
			{
				for(j=0;j<16;++j)
				{
					skip[i*16+j]=(i>=y&&i<y+h&&j>=x&&j<x+w)?false:true;
				}
			}
		}
	}
	else
	{
		for(i=0;i<256;++i)
		{
			skip[i]= chrSelected[i]?false:true;
			if (chrSelected[i]) cnt++;
		}
	}

	if (cnt>0 || (chrSelectRect && (w>1||h>1))) doWhole=false;
	else doWhole=true;

	SetUndo();

	int tile=0;
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			if(!doWhole && skip[j+i*16]) {tile++; continue;}
			for(k=0;k<16;k++)
			{
				int y=tileViewTable[j+i*16]/16;
				int x=tileViewTable[j+i*16]&15;
					chr[tileViewTable[tile]*16+bankViewTable[set+tile]+k]
					=
					 (smallnums_data[(y<<4)+k]&0xf0)
					|(smallnums_data[(x<<4)+k]&0x0f);
			}
			tile++;
		}
	}
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRFindDoublesClick(TObject *Sender)
{
	if(MCHRFindDoubles->Checked)
	{
		for(int i=0;i<256;++i) chrSelected[i]=0;
		SelectTile(tileActive);
		MCHRFindDoubles->Checked=false;
		MCHRFindDoubles->Caption="Find &duplicates";
		UpdateAll();
	}
	else
	{
		FindDoublesUnused(false);
		MCHRFindDoubles->Checked=true;
		MCHRFindDoubles->Caption="Deselect Find &duplicates";

		LabelStats->Caption="Duplicates found. \n\nTip: Press again to deselect.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}
}
//---------------------------------------------------------------------------



int           *import_name;
unsigned char *import_chr;
int *import_chr_sort;
int *import_chr_freq;

unsigned char *import_chr_temp;
unsigned char *import_bmp;
int           *import_stat;

int import_chr_size;
int import_name_size;

int import_bmp_width;
int import_bmp_height;

int import_name_width;
int import_name_height;

int import_stat_size;



//compare two tiles by counting number of matching pixels

inline int tile_lossy_compare( unsigned char* tile1ext, unsigned char* tile2ext)
{
	int i,j,col11,col21,col12,col22,mask,match;

	int col0match,col1match,col2match,col3match;
	int rowMatch[8] = {0};
	int columnMatch[8] = {0};

	//match=0;
	int weight0=1;
	int weight1=1;
	int weight2=1;
	int weight3=1;

	// CPU cycles attempt 1 - precompute mask values for row loop
	int maskTable[8];
	for (i = 0, mask = 0x01; i < 8; ++i, mask <<= 1)
	{
		maskTable[i] = mask;
	}
	// CPU cycles savings attempt 2 - copy tiles to compare to local memory, to reduce global access
	char tile1[16];
	char tile2[16];
	memcpy (tile1,tile1ext,16);
	memcpy (tile2,tile2ext,16);

	for(i=0;i<8;++i)  //look at a byte (8x1 sliver)
	{
		//old - checks bitplanes
		/*
		col11=tile1[i];
		col21=tile2[i];
		col12=tile1[i+8];
		col22=tile2[i+8];
		*/

		//new checks indexes. seems worse on test materials, possibly bugged. disabled for now.

		col0match  = ~(( ~tile1[i]  & ~tile1[i+8])  		  ^ ( ~tile2[i] & ~tile2[i+8]));
		col1match  = ~((  tile1[i]  & (tile1[i] ^ tile1[i+8])) ^ (  tile2[i]  & (tile2[i] ^ tile2[i+8])));
		col2match  = ~((  tile1[i+8]& (tile1[i] ^ tile1[i+8])) ^ (  tile2[i+8]& (tile2[i] ^ tile2[i+8])));
		col3match  = ~((  tile1[i]  &  tile1[i+8])  			  ^ (  tile2[i] &  tile2[i+8]));

		
		//mask=0x01;
		match=0;
		for(j=0;j<8;++j) //score each bit of a sliver.
		{
			mask = maskTable[j];
			//old - checks bitplanes

			//if(!((col11^col21)&mask)) ++match;
			//if(!((col12^col22)&mask)) ++match;

			// new, disabled

			if (col0match & mask) ++match;
			if (col1match & mask) ++match;
			if (col2match & mask) ++match;
			if (col3match & mask) ++match;


			//mask<<=1;
		}

		rowMatch[i]=match*match;
	}
	int rowScore=0;
	for(i=0;i<8;i++)
	{
		rowScore+=rowMatch[i];
	}
	//rowScore=rowScore*rowScore;
	//score columns next
	//..................



	//mask=0x01;

	for(j=0;j<8;++j)
	{
	match=0;

		for(i=0;i<8;++i)
		{
			mask = maskTable[i];
			//col0
			match+=((~(( ~tile1[i]  & ~tile1[i+8])
					 ^ ( ~tile2[i]  & ~tile2[i+8])))&mask);

			//col1
			match  += ((~((  tile1[i]  & (tile1[i] ^ tile1[i+8]))
						^ (  tile2[i]  & (tile2[i] ^ tile2[i+8]))))&mask);

			//col2
			match  += ((~((  tile1[i+8]& (tile1[i] ^ tile1[i+8]))
						^ (  tile2[i+8]& (tile2[i] ^ tile2[i+8]))))&mask);

			//col3
			match  += ((~((  tile1[i]  &  tile1[i+8])
						^ (  tile2[i] &  tile2[i+8])))&mask);

		}
	//mask<<=1;

	columnMatch[i]=match*match;
	}
	int columnScore=0;
	for(i=0;i<8;i++)
	{
		columnScore+=columnMatch[i];
	}
	//columnScore=columnScore*columnScore;

	int finalScore=columnScore+rowScore;
	return finalScore;     //match
}

class Thread_lossyTileComp : public TThread{
	private:
		int start_tile;
		int end_tile;
		int tile_count;
		int* max_array;
		int* tile1_array;
		int* tile2_array;
		int thread_index;
		int local_max;
		int local_tile1;
		int local_tile2;
	public:
		Thread_lossyTileComp(int start, int end, int count, int* max_arr,  int* tile1_arr,  int* tile2_arr, int index)
		: TThread(false), start_tile(start), end_tile(end),
		tile_count(count), max_array(max_arr), tile1_array(tile1_arr), tile2_array(tile2_arr), thread_index(index), local_max(0), local_tile1(0), local_tile2(0)
		{}

	void __fastcall Execute(){
		for (int i = start_tile; i < end_tile - 1; ++i){
			for (int j = i + 1; j < tile_count; ++j){
				int n = tile_lossy_compare(&import_chr[i * 16], &import_chr[j * 16]) * 65536 / import_stat[j];

				if (n > local_max){
					local_max = n;
					local_tile1 = i;
					local_tile2 = j;
				}
			}
		}
		//output array entries to pick a final winner once all threads are done
		max_array[thread_index] = local_max;
		tile1_array[thread_index] = local_tile1;
		tile2_array[thread_index] = local_tile2;
	}
};

int __fastcall TFormMain::ImportConvertTiles(int wdt,int hgt,int xoff,int yoff,int thresh)
{
	unsigned char tile[16];
	int i,j,k,l,x,y,c1,c2,col,pp,ptr,tile_count,pixel_count;
	bool add;

	memset(import_chr ,0,import_chr_size);
	memset(import_name,0,import_name_size);

	ptr=0;
	tile_count=0;

	for(i=0;i<hgt/8;++i)
	{
		for(j=0;j<wdt/8;++j)
		{
			memset(tile,0,sizeof(tile));

			pp=0;

			y=((i<<3)-yoff);

			pixel_count=0;

			for(k=0;k<8;++k)
			{
				x=((j<<3)-xoff);

				for(l=0;l<8;++l)
				{
					if(x<0) x+=wdt;
					if(x>wdt) x-=wdt;
					if(y<0) y+=hgt;
					if(y>hgt) y-=hgt;

					col=import_bmp[y*import_bmp_width+x];

					c1=(col&1)?(1<<(7-l)):0;
					c2=(col&2)?(1<<(7-l)):0;

					tile[pp+0]|=c1;
					tile[pp+8]|=c2;

					if(c1|c2) ++pixel_count;

					++x;
				}

				++pp;
				++y;
			}

			add=false;


			if(thresh<0||(!pixel_count||pixel_count>thresh)) add=true;
			if(pixel_count==0 || pixel_count<=thresh)
			{
				import_name[i*import_name_width+j]=nullTile;
				add=false;
			}


			if(add)
			{
				for(k=0;k<tile_count;++k)  // look for match in existing tiles
				{
					if(memcmp(tile,&import_chr[k*16],16)==0)
					{
						import_name[i*import_name_width+j]=k;
						add=false; //found match, don't lay down new chr tile.
						break;
					}
				}
			}


			if(add)
			{
				if(tile_count == nullTile) {ptr+=16; ++tile_count;} //skip ahead preserving the null tile.


				memcpy(&import_chr[ptr],tile,16);
				ptr+=16;
				import_name[i*import_name_width+j]=tile_count;

				++tile_count;

				if(ptr>=import_chr_size) break;
			}
		}
	}

	return tile_count;
}


struct matchStruct {
	unsigned char match;
	int tile1;
	int tile2;
};

enum {
	IMPORT_NORMAL=0,
	IMPORT_SQUEEZE,
	IMPORT_MATCH
};

bool __fastcall TFormMain::ImportBMP(AnsiString filename,int mode,bool shift,int thresh,int maxtiles,bool noAttr, bool noPal)
{
	FILE *file;
	unsigned char *data;
	int set=bankActive/16;
	int i,j,k,l,x,y,n,size,wdt,hgt,pp,ps,off,ptr,bpp;
	int cnt,fi,min,max,id,rx,gx,bx,pitch;
	int tile_count,tiles_min,tile1,tile2,shift_x,shift_y,best_x,best_y;
	int attr[4];
	AnsiString str;

	int infoheader;

	file=fopen(filename.c_str(),"rb");

	if(!file) return false;

	size=get_file_size(file);
	data=(unsigned char*)malloc(size);
	fread(data,size,1,file);
	fclose(file);

	infoheader=data[0x0e]; //added because some bitmaps may have nonstandard length headers.
	bpp=data[28];

	if(data[0]!='B'||data[1]!='M'||(bpp!=4&&bpp!=8)||data[29]!=0||read_dword(&data[30]))
	{
		Application->MessageBox("Wrong BMP format, only uncompressed 4- or 8bit images supported","Error",MB_OK);
		free(data);
		return false;
	}

	off=read_dword(&data[10]);
	wdt=read_dword(&data[18]);
	hgt=read_dword(&data[22]);

	if(wdt<8||wdt>8*NAME_MAX_WIDTH||labs(hgt)<8||labs(hgt)>8*NAME_MAX_HEIGHT)
	{
		Application->MessageBox(("BMP should not be smaller than 8x8 or larger than "+IntToStr(NAME_MAX_WIDTH*8)+"x"+IntToStr(NAME_MAX_HEIGHT)+" pixels").c_str(),"Error",MB_OK);
		free(data);
		return false;
	}

	//find similar colors in the NES palette

	if(!noPal) //switch for skipping overwrite of palette
	{
		for(i=0;i<16;i++)
		{
			min=0x01000000;
			id=0;

			for(j=0;j<64;j++)
			{
				//should be up to user. pal_validate() also defaults black to $0F according to config.
				//if(j==0x1d||j==0x2d) continue;

				bx=((outPalette[j]>>16)&255)-data[i*4+infoheader+0x0e+0]; //54 was
				gx=((outPalette[j]>>8 )&255)-data[i*4+infoheader+0x0e+1]; //55
				rx=((outPalette[j]    )&255)-data[i*4+infoheader+0x0e+2]; //56
				//swapped assignment order from rx,gx,bx to bx,gx,rx since that's what bmp:s do.

				//old
				//fi=30*rx*rx+59*gx*gx+11*bx*bx;

				//new
				int redmean=(((outPalette[j])&255) + data[i*4+infoheader+0x0e+2])/2;
				if(redmean<128) fi=2*rx*rx + 4*gx*gx + 3*bx*bx;
				else            fi=3*rx*rx + 4*gx*gx + 2*bx*bx;

				if(fi<min)
				{
					min=fi;
					id=j;
				}
			}

			if(id==0x20) id=0x30;

			if((i!=4&&i!=8&&i!=12) || sharedCol0==false ) bgPal[palBank*16+(i>>2)*4+(i&3)]=id;
		}

		pal_validate();
	}

	//put bitmap into an array to make it easier to work with

	import_bmp_width =wdt;
	import_bmp_height=hgt;

	import_bmp=(unsigned char*)malloc(import_bmp_width*import_bmp_height*sizeof(unsigned char));

	memset(import_bmp,0,import_bmp_width*import_bmp_height*sizeof(unsigned char));

	switch(bpp)
	{
	case 4:
		{
			pitch=wdt&~7;

			if(wdt&7) pitch+=8;

			pitch>>=1;

			for(i=0;i<hgt;++i)
			{
				ps=off+(hgt-1-i)*pitch;

				for(j=0;j<wdt;++j)
				{
					if(!(j&1))
					{
						import_bmp[i*import_bmp_width+j]=data[ps]>>4;
					}
					else
					{
						import_bmp[i*import_bmp_width+j]=data[ps++]&15;
					}
				}
			}
		}
		break;

	case 8:
		{
			pitch=wdt&~3;

			if(wdt&3) pitch+=4;

			for(i=0;i<hgt;++i)
			{
				ps=off+(hgt-1-i)*pitch;

				for(j=0;j<wdt;++j) import_bmp[i*import_bmp_width+j]=data[ps++];
			}
		}
		break;
	}

	free(data);

	//convert graphics into tiles

	if(wdt&7) wdt=(wdt&~7)+8;
	if(hgt&7) hgt=(hgt&~7)+8;

	import_name_width =wdt/8;
	import_name_height=hgt/8;

	import_name_size=import_name_width*import_name_height*sizeof(int);
	import_chr_size =import_bmp_width *import_bmp_height *sizeof(unsigned char);

	import_name=(int*)malloc(import_name_size);
	import_chr =(unsigned char*)malloc(import_chr_size);

	tiles_min=import_name_width*import_name_height;

	best_x=0;
	best_y=0;

	if(shift)
	{
		for(shift_y=0;shift_y<8;++shift_y)
		{
			for(shift_x=0;shift_x<8;++shift_x)
			{
				tile_count=ImportConvertTiles(wdt,hgt,shift_x,shift_y,thresh);

				if(tile_count<tiles_min)
				{
					tiles_min=tile_count;
					best_x=shift_x;
					best_y=shift_y;
				}
			}
		}
	}

	tile_count=ImportConvertTiles(wdt,hgt,best_x,best_y,thresh);
	importBMP_tile_count=tile_count; //for external display purposes
	importBMP_tile_count_reduced=tile_count;  //establishing baseline value before lossy compress
/*
		if(mode==IMPORT_NORMAL&&tile_count>maxtiles)
		{
			Application->MessageBox(("Too many unique tiles (256+"+IntToStr(tile_count-256)+"="+IntToStr(tile_count)+").\nExcess tiles will be put on pattern table B").c_str(),"Warning",MB_OK);
		}
*/

	if(mode==IMPORT_SQUEEZE)//&&(tile_count>maxtiles))
	{
		str=FormImportBMP->Caption;

		import_stat_size=tile_count*sizeof(int);
		import_stat=(int*)malloc(import_stat_size);

		//tile sorter begins here
		//if (FormLossyDetails->RadioFreq->Checked==true)
		//{
			FormImportBMP->Caption="Wait... ["+IntToStr(tile_count)+" tiles]";
			unsigned int temp=0;
			//import_chr_sort =(int*)malloc(import_chr_size/16);
			//import_chr_freq =(int*)malloc(import_chr_size/16);
			import_chr_sort =(int*)malloc(tile_count*sizeof(int));
			import_chr_freq =(int*)malloc(tile_count*sizeof(int));
			import_chr_temp =(unsigned char*)malloc(tile_count*16);


			memset(import_chr_freq,0,sizeof(import_chr_freq));


			//get use frequency
			for(i=0;i<import_name_height*import_name_width;i++)
				++import_chr_freq[import_name[i]];


			//list sort table
			for(i=0;i<tile_count;i++) import_chr_sort[i]=i;

			//sort chr assignment
			for(i=0;i<tile_count-1;i++)
			{
				for(j=0;j<tile_count-i-1;j++)
				{
					if(import_chr_freq[j]<import_chr_freq[j+1])
					{
						temp=import_chr_freq[j];
						import_chr_freq[j]=import_chr_freq[j+1];
						import_chr_freq[j+1]=temp;

						temp=import_chr_sort[j];
						import_chr_sort[j]=import_chr_sort[j+1];
						import_chr_sort[j+1]=temp;
					}
				}
			}

			//repair NT
			for(i=0;i<import_name_height*import_name_width;i++)
			{
				for(j=0;j<tile_count;j++)
				{
					if(import_chr_sort[j]==import_name[i])
					{
						import_name[i]=j;
						break;
					}
				}
			}

			//copy chr according to sort assignment
			memcpy(import_chr_temp,import_chr,tile_count*16);
			//memcpy(import_chr,import_chr_temp,sizeof(import_chr));

			for(i=0;i<tile_count;i++)
			{
				int index = import_chr_sort[i];
				memcpy(&import_chr[i*16],&import_chr_temp[index*16],16);
			}

			skip:


			free(import_chr_sort);
			free(import_chr_freq);
			free(import_chr_temp);

		//}
		//tile combinator begins here

		SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);

			int num_cpus = sysinfo.dwNumberOfProcessors;
			int num_threads = num_cpus*1.5; // number of threads to spawn. 1.5 seemed like a fine compromise.
			num_threads = 1>num_threads? 1:num_threads;
			num_threads = 24<=num_threads? 24:num_threads;

			const int tiles_per_thread = (tile_count-1)/num_threads + 1; // number of tiles each thread will handle
			int* max_array = new int[num_threads];
			int* tile1_array = new int[num_threads];
			int* tile2_array = new int[num_threads];

		//FormImportBMP->ButtonCancel->Enabled=false;
		FormImportBMP->ButtonSwap->Enabled=false;

		while(tile_count>maxtiles)
		{
			FormImportBMP->Caption="Wait... ["+IntToStr(tile_count)+" tiles]";

			//count tile use

			memset(import_stat,0,import_stat_size);
			for(i=0;i<import_name_width*import_name_height;++i) ++import_stat[import_name[i]];


			if(tile_count<32){
			//find a pair of tiles with max similarity

			max=0;
			tile1=0;
			tile2=0;


			for(i=0;i<tile_count-1;++i)
			{
				for(j=i+1;j<tile_count;++j)
				{
					n=tile_lossy_compare(&import_chr[i*16],&import_chr[j*16])*65536/import_stat[j];

					if(n>max)
					{
						max=n;
						tile1=i;
						tile2=j;
					}
				}
			}

			}

			else{
			//test begin-----


			std::vector<Thread_lossyTileComp*> thread_list;

			//int tiles_per_thread = tile_count / num_threads;
			for (int i = 0; i < num_threads; ++i)
			{
				int start_tile = i * tiles_per_thread;
				int end_tile = (i + 1) * tiles_per_thread;
				if (i == num_threads - 1) end_tile = tile_count; // Last thread takes any remaining tiles

				Thread_lossyTileComp* thread = new Thread_lossyTileComp(start_tile, end_tile, tile_count, max_array, tile1_array, tile2_array, i);
				thread->Resume();
				thread_list.push_back(thread);
			}

		// Wait for threads to complete
		for (std::vector<Thread_lossyTileComp*>::iterator it = thread_list.begin(); it != thread_list.end(); ++it)
		{
			(*it)->WaitFor();
		}

		// Find the maximum value across all threads
		int global_max = 0;
		for (int i = 0; i < num_threads; ++i)
		{
			if (max_array[i] > global_max)
			{
				global_max = max_array[i];
				tile1 	   = tile1_array[i];
				tile2	   = tile2_array[i];
			}
		}

		//delete arrays removed from loop
			//test end.....
		}


			//remove one of tiles

			for(i=0;i<import_name_width*import_name_height;++i)
			{
				if(import_name[i]==tile2) import_name[i]=tile1;
				else if(import_name[i]>tile2) --import_name[i];
			}
			memcpy(&import_chr[tile2*16],&import_chr[tile2*16+16],(tile_count-tile2-1)*16);
			--tile_count;
		}
		delete[] max_array;
		delete[] tile1_array;
		delete[] tile2_array;
		free(import_stat);

		FormImportBMP->Caption=str;
		//FormImportBMP->ButtonCancel->Enabled=true;
		FormImportBMP->ButtonSwap->Enabled=true;

	}

	if(mode!=IMPORT_MATCH)
	{

		int tilecap = maxtiles<tile_count? maxtiles:tile_count;
		tile_count = tilecap;
		//memcpy(chr+bankActive,import_chr,tile_count*16);


		for(int t=0;t<tile_count;t++){
			   memcpy(chr+bankViewTable[(set + t)%1024]+t*16,import_chr+t*16,16);
		}


		for(i=0;i<import_name_width*import_name_height;++i)
		{
			n=import_name[i];

		   //	if(n>maxtiles-1) n=maxtiles-1; //old
			if(n>maxtiles-1) n=nullTile;
			nameTable[i]=n;
		}
	}

	if(mode==IMPORT_MATCH)
	{
		for(i=0;i<import_name_width*import_name_height;++i)
		{
			tile1=import_name[i];

			max=0;
			tile2=0;

			for(j=0;j<maxtiles;++j)
			{
				n=tile_lossy_compare(&import_chr[tile1*16],&chr[bankViewTable[(set + j)%1024]+j*16]);

				if(n>max)
				{
					max=n;
					tile2=j;
				}
			}

			nameTable[i]=tile2;
		}

	}

	free(import_chr);
	free(import_name);

	//try to set proper attributes

	if(mode!=IMPORT_MATCH)
	{
		nameTableWidth =import_name_width;
		nameTableHeight=import_name_height;

		for(i=0;i<import_bmp_height;i+=16)
		{
			for(j=0;j<import_bmp_width;j+=16)
			{
				for(k=0;k<4;++k) attr[k]=0;

				for(k=i;k<i+16;++k)
				{
					for(l=j;l<j+16;++l)
					{
						if(import_bmp[k*import_bmp_width+l]&3) ++attr[import_bmp[k*import_bmp_width+l]>>2];
					}
				}

				id=0;
				max=0;

				for(k=0;k<4;++k)
				{
					if(attr[k]>max)
					{
						max=attr[k];
						id=k;
					}
				}

				if(!noAttr) AttrSet(j>>3,i>>3,id,false);
			}
		}
	}
	free(import_bmp);
	importBMP_tile_count_reduced = tile_count; //for external display purposes
    nameTableWidth =import_name_width;
	nameTableHeight=import_name_height;
	return true;
}



void __fastcall TFormMain::MImportBMPNametableClick(TObject *Sender)
{
	int mode;

	BlockDrawing(true);

	OpenDialogImport->Title="Import BMP to canvas";
	OpenDialogImport->Filter="Windows bitmap files (*.bmp)|*.bmp|All files (*.*)|*.*";
	OpenDialogImport->DefaultExt="bmp";

	if(OpenDialogImport->Execute())
	{
		SetUndo();

		if(!MImportLossy->Checked) mode=IMPORT_NORMAL; else mode=IMPORT_SQUEEZE;

		if(ImportBMP(OpenDialogImport->FileName,mode,MImportBestOffsets->Checked,MImportThreshold->Checked?8:-1,256,MImportNoColorData->Checked,MImportNoColorData->Checked))
		{
			UpdateAll();
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MImportNESClick(TObject *Sender)
{
	FILE *file;
	unsigned char *data;
	unsigned char buf[4096];
	BlockDrawing(true);
	
	OpenDialogImport->Title="Import NES file";
	OpenDialogImport->Filter="NES cartridge dump|*.nes|All files (*.*)|*.*";
	OpenDialogImport->DefaultExt="nes";

	if(OpenDialogImport->Execute())
	{
		FormBank->Caption="Import 4K CHR bank from NES file";
		FormBank->Label2->Caption="To Set:";
		FormBank->FileName=OpenDialogImport->FileName;
		FormBank->ShowModal();

		if(FormBank->OK)
		{
			file=fopen(OpenDialogImport->FileName.c_str(),"rb");

			if(file)
			{
				fseek(file,16+FormBank->PRG*16384+FormBank->Bank*4096,SEEK_SET);
				fread(buf,4096,1,file);
				fclose(file);
			}
			int set=0;
			if(FormBank->btnA->Down) set=0;
			if(FormBank->btnB->Down) set=1*256;
			if(FormBank->btnB->Down) set=2*256;
			if(FormBank->btnB->Down) set=3*256;

			for(int t=0;t<256;t++){
				memcpy(chr+bankViewTable[set+t]+t*16, buf+t*16,16);

			}

			UpdateAll();/*
			UpdateTiles(true);
			UpdateNameTable(-1,-1,true);
			FormNavigator->Draw(false,false);
			//UpdateMetaSprite();
			cueUpdateMetasprite=true; */
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MExportNESClick(TObject *Sender)
{
	FILE *file;
	unsigned char *nes;
	int size;

	BlockDrawing(true);
	
	if(SaveDialogExportNES->Execute())
	{
		FormBank->Caption="Export 4K CHR set to NES file";
		FormBank->Label2->Caption="From Set:";
		FormBank->FileName=SaveDialogExportNES->FileName;
		FormBank->ShowModal();

		if(FormBank->OK)
		{
			file=fopen(SaveDialogExportNES->FileName.c_str(),"rb");

			if(file)
			{
				fseek(file,0,SEEK_END);
				size=ftell(file);
				fseek(file,0,SEEK_SET);

				nes=(unsigned char*)malloc(size);
				fread(nes,size,1,file);
				fclose(file);

				file=fopen((SaveDialogExportNES->FileName+".bak").c_str(),"wb");

				if(!file)
				{
					free(nes);
					BlockDrawing(false);
					return;
				}

				fwrite(nes,size,1,file);
				fclose(file);

				//memcpy(nes+16+FormBank->PRG*16384+FormBank->Bank*4096,chr,4096);
                int set=0;
				if(FormBank->btnA->Down) set=0;
				if(FormBank->btnB->Down) set=1*256;
				if(FormBank->btnB->Down) set=2*256;
				if(FormBank->btnB->Down) set=3*256;

				for(int t=0;t<256;t++){
					memcpy(nes+16+FormBank->PRG*16384+FormBank->Bank*4096+t*16, chr+bankViewTable[set+t]+t*16,16);
				}

				file=fopen(SaveDialogExportNES->FileName.c_str(),"wb");

				if(file)
				{
					fwrite(nes,size,1,file);
					fclose(file);
				}
				
				free(nes);
			}
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MImportBMPTilesetClick(TObject *Sender)
{
	FILE *file;
	unsigned char *data;
	int i,j,k,l,size,wdt,hgt,pp,ps,off,ptr,bpp;
	unsigned char tile[16];

	BlockDrawing(true);

	//if(Sender==MImportBMPTileset)

	OpenDialogImport->Title="Import BMP as tileset";
	OpenDialogImport->Filter="Windows bitmap files (*.bmp)|*.bmp|All files (*.*)|*.*";
	OpenDialogImport->DefaultExt="bmp";

	if(OpenDialogImport->Execute())
    {
		file=fopen(OpenDialogImport->FileName.c_str(),"rb");

		if(file)
		{
			size=get_file_size(file);
			data=(unsigned char*)malloc(size);
			fread(data,size,1,file);
			fclose(file);



			if(data[0]==0x89&&data[1]==0x50&&data[2]==0x4E&&data[3]==0x47)
				{
					Application->MessageBox(
						"Please use 4bpp or 8bpp BMP.",
						"Error: PNG not supported",MB_OK);
					free(data);
					BlockDrawing(false);
					return;
				}


			bpp=data[28];

			if(data[0]!='B'||data[1]!='M'||(bpp!=4&&bpp!=8)||data[29]!=0||read_dword(&data[30]))
			{
				Application->MessageBox(
						"Wrong BMP format: only 4bpp & 8bpp files w/o RLE are supported",
						"Error",MB_OK);
				free(data);
				BlockDrawing(false);
				return;
			}

			off=read_dword(&data[10]);
			wdt=read_dword(&data[18]);
			hgt=read_dword(&data[22]);

			if(wdt>128||(hgt>128&&hgt<-128)||(wdt&7)||(hgt&7))
			{
				Application->MessageBox("BMP should be 128x128 pixels or smaller, & dimensions a multiple of 8","Error",MB_OK);
				free(data);
				BlockDrawing(false);
				return;
			}

			ptr=bankActive;

			if(!(wdt==128&&hgt==128)) ptr+=tileActive*16;
			
			for(i=0;i<hgt/8;i++)
			{
				for(j=0;j<wdt/8;j++)
				{
					memset(tile,0,sizeof(tile));
					pp=0;

					//test (not working right now)
					/*
					if(bpp==1)
					{
						for(k=0;k<8;k++)
						{
							ps=off+(hgt-1-i*8-k)*(wdt/4)+j;
							for(l=0;l<8;l++)
							{
								tile[pp]|=(data[ps])?1<<(7-l):0; 
								ps++;
							}
							pp++;
						}
					}
					*/
					if(bpp==4)
					{
						for(k=0;k<8;k++)
						{
							/*if(hgt>0) ps=off+(i*8+k)*128+j*4; else */
							ps=off+(hgt-1-i*8-k)*(wdt/2)+j*4;
							for(l=0;l<8;l++)
							{
								tile[pp]|=(data[ps]&(l&1?0x01:0x10))?1<<(7-l):0;
								tile[pp+8]|=(data[ps]&(l&1?0x02:0x20))?1<<(7-l):0;

								if(l&1) ps++;
							}

							pp++;
						}
					}
					else
					{
						for(k=0;k<8;k++)
						{
							/*if(hgt>0) ps=off+(i*8+k)*128+j*4; else */
							ps=off+(hgt-1-i*8-k)*wdt+j*8;
							for(l=0;l<8;l++)
							{
								tile[pp]|=(data[ps]&1)?1<<(7-l):0;
								tile[pp+8]|=(data[ps]&2)?1<<(7-l):0;

								ps++;
							}

							pp++;
						}
					}

					memcpy(&chr[ptr+j*16],tile,16);
				}

				ptr+=16*16;
			}

			free(data);
		}

		UpdateTiles(true);
		//WorkCHRToBankCHR();
		UpdateNameTable(-1,-1,true);
		FormNavigator->Draw(false,false,false);
		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSwapBanksClick(TObject *Sender)
{
	unsigned char temp[4096];
	uint32_t propt[512];

    //int bank=curViewCHRbankCanvas2;

	memcpy(temp,chr,4096);
	memcpy(chr,chr+4096,4096);
	memcpy(chr+4096,temp,4096);

	memcpy(propt					,tileProperties				,256*sizeof(uint32_t));
	memcpy(tileProperties			,&tileProperties[256]		,256*sizeof(uint32_t));
	memcpy(&tileProperties[256]		,propt						,256*sizeof(uint32_t));

	UpdateAll();
	/*UpdateTiles(true);
	//WorkCHRToBankCHR();
	UpdateNameTable(-1,-1,true);
	if (FormNavigator->Visible) FormNavigator->Draw(false,false);
	if (FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
	if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;*/
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MOpenAllClick(TObject *Sender)
{
	BlockDrawing(true);

	if(OpenDialogAll->Execute())
	{
		OpenAll(RemoveExt(OpenDialogAll->FileName));
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveAllClick(TObject *Sender)
{
	BlockDrawing(true);

	SaveCHR(0,4016*4,true);

	if(SaveDialogName->FileName=="") SaveDialogName->FileName=SaveDialogChr->FileName;
	if(SaveDialogMap ->FileName=="") SaveDialogMap ->FileName=SaveDialogChr->FileName;

	if(nameTableWidth==32&&nameTableHeight==30)
	{
		MSaveNameTableBINClick(Sender);
	}
	else
	{
		MSaveMapClick(Sender);
	}

	if(SaveDialogPal->FileName=="") SaveDialogPal->FileName=SaveDialogName->FileName;

	MPaletteSaveClick(Sender);

	MSaveMetaSpriteBankClick(Sender);

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MLoadSessionClick(TObject *Sender)
{
	
	 BlockDrawing(true);
	if(OpenDialogSession->Execute())
	{
        FormMain->Caption="Loading...";
		LabelStats->Caption="Loading...";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
		

		if(!LoadSession(OpenDialogSession->FileName)) Application->MessageBox("Can't load session","Error",MB_OK);
		else unsavedChanges=false;
	}
    UpdateStats();
	UpdateAll();
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveSessionClick(TObject *Sender)
{
	BlockDrawing(true);

	if(SaveDialogSession->Execute())
	{
		SaveSession(SaveDialogSession->FileName);
	}

    bool bByFiletype	 = FormPipelineHelper->btnType->Down;
	bool bByAsset		 = FormPipelineHelper->btnAsset->Down;
	bool bByName		 = FormPipelineHelper->btnName->Down;

	if (FormPipelineHelper->chkAutoExpMetatilesBMP->Checked==true)
	{


		//make it directly in in workdir
		if(FormPipelineHelper->RadioButton1->Checked) {
			AnsiString str2 = SaveDialogSession->FileName;
			MetatilesToBMP(str2);
		}
		//make it in subfolder of workdir
		else if (FormPipelineHelper->RadioButton2->Checked){
			AnsiString str2 = ExtractFileDir(SaveDialogSession->FileName);
			//naming style
			AnsiString str3;
			if(bByFiletype)str3="Autoexport Bitmaps";
			else if(bByAsset)str3="Autoexport Metatiles";
			else if(bByName) str3= "Autoexport " + RemoveExt(ExtractFileName(SaveDialogSession->FileName));

			str2=CheckAndCreateFolder(str3.c_str(), false);
			if(bByName)str2 +="metatiles";
			else       str2 += RemoveExt(ExtractFileName(SaveDialogSession->FileName))+"_metatiles";
			//ShowMessage(str2);
			MetatilesToBMP(str2);

		}
		//use/create home subfolder.
		else{
                AnsiString str2 = SaveDialogSession->FileName;
				AnsiString str3 = "Autoexports";
				str2=CheckAndCreateFolder(str3.c_str(), true);
				str2+= RemoveExt(ExtractFileName(SaveDialogSession->FileName))+"_metatiles";
				MetatilesToBMP(str2);
		}
	}


	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MExportPaletteBMPClick(TObject *Sender)
{
	TPicture *picture;
	Graphics::TBitmap *bmp;
	int i;
	unsigned char* dst;
	AnsiString str;

	BlockDrawing(true);
	
	str=RemoveExt(SaveDialogPal->FileName);

	if(str=="") str="palette";

	SaveDialogImage->FileName=str+".bmp";

	if(SaveDialogImage->Execute())
	{
		picture=new TPicture();
		bmp=new Graphics::TBitmap();
		bmp->SetSize(16,1);
		bmp->PixelFormat=pf4bit;
		SetBMPPalette(bmp);
		picture->Bitmap=bmp;

		dst=(unsigned char*)picture->Bitmap->ScanLine[0];

		for(i=0;i<16;i+=2) *dst++=((i+1)|(i<<4));

		picture->SaveToFile(SaveDialogImage->FileName);

		delete bmp;
		delete picture;
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonTypeInClick(TObject *Sender)
{

	EnableDisableTypeConflictShortcuts(false); //bool=true forces disable, unconditionally.
	bool TypeIn = SpeedButtonTypeIn->Down;
	if(TypeIn)
	{
		if(nameSelection.left>=0)
		{
			nameSelection.right =nameSelection.left+1;
			nameSelection.bottom=nameSelection.top +1;
			returnCarriagePos= nameSelection.left;
			UpdateNameTable(-1,-1,true);
		}
	}

	StaticTextFontOffset->Caption="$"+IntToHex(typeInFontOffset,2);

	StaticTextFontOffset->Visible=TypeIn;
	TypeInModeOnOff1->Checked=TypeIn;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormKeyPress(TObject *Sender, char &Key)
{
	if(SpeedButtonTypeIn->Down)
	{
		if(Key>=32 && !bIgnoreKey)
		{
			NameTableTypeIn(Key-typeInASCIIOffset);
			cueUpdateNametable=true;
		}
	}

}
//---------------------------------------------------------------------------


void __fastcall TFormMain::PageControlEditorChange(TObject *Sender)
{
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	ActiveControl=NULL;//to prevent ListBoxSpriteList grab focus while Nametable tab is active
	AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
	if(PageControlEditor->ActivePage==TabSheetTile){
		TabSheetTile->Caption=tmp_LinkCHRstr+"[F3] Tiles";
		GroupBoxTiles->Caption="Screen (view only)";
		Fill1->Enabled=false;
		FormMain->Fill1->ShortCut=TextToShortCut("(None)");
	}

	else
	{
		if(SpeedButtonTypeIn->Down==false)FormMain->Fill1->ShortCut=TextToShortCut("F");

		AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
		GroupBoxTiles->Caption=tmp_LinkCHRstr+"Tileset";
		if(PageControlEditor->ActivePage==TabSheetSprite)
		{
			if(GroupBoxSpriteList->Visible) ActiveControl=ListBoxSpriteList;
			Fill1->Enabled=false;
		}
		if(PageControlEditor->ActivePage==TabSheetName)
		{
			Fill1->Enabled=true;
		}
	}
	if(uiScale>1 && PageControlEditor->ActivePage==TabSheetTile)
	{	GroupBoxTileControl->Visible=true;}
	else
	{	GroupBoxTileControl->Visible=false;	}

	UpdateNameTable(-1,-1,true);        //true or false?
	if(FormNavigator->Visible)FormNavigator->Draw(false,false,false);
	UpdateTiles(true);  //true or false?
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageMetaSpriteDragOver(TObject *Sender,
TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	if(Source->InheritsFrom(__classid(TImage))) Accept=true; else Accept=false;
	bDrawDestShadow=false;
	cueUpdateTiles=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageMetaSpriteDragDrop(TObject *Sender,
TObject *Source, int X, int Y)
{
	int i,pp,tile;



	bool baOdd = ((bankActive/4096)%2) == 1;
	/*
	bool baOdd=false;
	if(bankActive==0) baOdd=btnVRAM_A->Down? true:false;
	if(bankActive==1*4096) baOdd=btnVRAM_B->Down? true:false;
	if(bankActive==2*4096) baOdd=btnVRAM_C->Down? true:false;
	if(bankActive==3*4096) baOdd=btnVRAM_D->Down? true:false;
	*/

	SetUndo();

	pp=metaSpriteActive*64*4;

	for(i=0;i<64;++i)
	{
		if(metaSprites[pp]==255)
		{
			tile=tileViewTable[tileActive];

			if(SpeedButtonSprite8x16->Down) tile=(tileViewTable[tileActive]&0xfe)+(baOdd?1:0);

			metaSprites[pp+0]=Y/uiScale;
			metaSprites[pp+1]=tile;
			metaSprites[pp+2]=palActive;
			metaSprites[pp+3]=X/uiScale;

			if(SpeedButtonSpriteSnap->Down) SpriteSnap(i);

			SelectSprite(i,false);

			break;
		}

		pp+=4;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageMetaSpriteEndDrag(TObject *Sender,
TObject *Target, int X, int Y)
{
	if(Target) cueUpdateMetasprite=true; //UpdateMetaSprite();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPrevMetaSpriteClick(TObject *Sender)
{
	if(metaSpriteActive)
	{
		--metaSpriteActive;

		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
		//UpdateStats();
		cueStats=true;
	}
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonNextMetaSpriteClick(TObject *Sender)
{
	if(metaSpriteActive<255)
	{
		++metaSpriteActive;

		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
		//UpdateStats();
		cueStats=true;
	}
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonClearMetaSpriteClick(TObject *Sender)
{
	int i,off;
    SetUndo();
	//if(Application->MessageBox("Metasprite will be cleared out!","CLEAR METASPRITE: Are you sure?",MB_YESNO)==IDYES)
	//{
		off=metaSpriteActive*64*4;

		for(i=0;i<64*4;i+=4) metaSprites[off+i]=255;

		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	//}

	//todo: this if statement was done in preparation of new copy/cut/paste routines for individual sprites.
	//todo: fix problem with "Sender"
		//if(Sender==Cut1Click)LabelStats->Caption="MetaSprite cut.";
		//else LabelStats->Caption="MetaSprite cleared.";

		//StatusUpdateWaiter->Enabled=true;
		//holdStats=true;
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteDelClick(TObject *Sender)
{
	int i,j,off,cnt;

	//if(Application->MessageBox("Sprite(s) will be deleted!","DELETE SPRITE(S): Are you sure?",MB_YESNO)==IDYES)
	//{
		SetUndo();

		cnt=0;

		for(i=0;i<ListBoxSpriteList->Items->Count;++i)
		{
			if(ListBoxSpriteList->Selected[i])
			{
				off=metaSpriteActive*64*4+i*4;

				for(j=0;j<4;++j) metaSprites[off+j]=255;

				++cnt;
			}
		}

		for(i=0;i<cnt;++i) squeeze_sprites();

		for(i=0;i<ListBoxSpriteList->Items->Count;++i) ListBoxSpriteList->Selected[i]=false;

		//UpdateMetaSprite();
		cueUpdateMetasprite=true;

		//todo: this if statement was done in preparation of new copy/cut/paste routines for individual sprites.
		//todo: fix problem with "sender"
		//if(Sender==Cut1Click)LabelStats->Caption="Sprite object(s) cut.";
		//else LabelStats->Caption="Sprite object(s) cleared.";

		//StatusUpdateWaiter->Enabled=true;
		//holdStats=true;

		isLastClickedMetaSprite=false;
		isLastClickedSpriteList=true;
	//}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ListBoxSpriteListClick(TObject *Sender)
{
    cueUpdateMetasprite_noListUpdate=true;
	SelectSprite(ListBoxSpriteList->ItemIndex,true);
	isLastClickedMetaSprite=false;
	isLastClickedSpriteList=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonFrameSelectedClick(TObject *Sender)
{
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteUpClick(TObject *Sender)
{
	unsigned char temp[4];
	int off=metaSpriteActive*64*4;
	int lastSelectedItem;


	if(!ListBoxSpriteList->Selected[0])
	{
		SetUndo();

		//get the _lowest_ selected item
		for(int i=ListBoxSpriteList->Items->Count-1;i>0;--i)
			if(ListBoxSpriteList->Selected[i]) {lastSelectedItem=i; break;}


		//do the up shift of one or multiple items
		for(int i=0;i<ListBoxSpriteList->Items->Count;++i)
		{
			if(ListBoxSpriteList->Selected[i])
			{
				memcpy( temp            ,&metaSprites[i*4-4+off],4);
				memcpy(&metaSprites[i*4-4+off],&metaSprites[i*4  +off],4);
				memcpy(&metaSprites[i*4  +off], temp            ,4);

			if (i<ListBoxSpriteList->Items->Count&&i>0) //second condition is a redundancy but we keep it
				ListBoxSpriteList->Selected[i-1]=ListBoxSpriteList->Selected[i];
			}
		}
		 --spriteActive;
	ListBoxSpriteList->Selected[lastSelectedItem]=false;
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	}
	/*
    //original routine
	off=metaSpriteActive*64*4+spriteActive*4;

	if(spriteActive>0)
	{
		if(metaSprites[off]<255)
		{
			memcpy( temp              ,&metaSprites[off-4],4);
			memcpy(&metaSprites[off-4],&metaSprites[off  ],4);
			memcpy(&metaSprites[off  ], temp              ,4);

			--spriteActive;

			//UpdateMetaSprite();
			cueUpdateMetasprite=true;
		}
	}

	SelectSprite(spriteActive,false);
	*/
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteDownClick(TObject *Sender)
{
	unsigned char temp[4];
	int off=metaSpriteActive*64*4;
	int firstSelectedItem;
	int max = ListBoxSpriteList->Items->Count-1;

	if(!ListBoxSpriteList->Selected[max])
	{
		SetUndo();

		//get the _lowest_ selected item
		for(int i=0;i<max;++i)
			if(ListBoxSpriteList->Selected[i]) {firstSelectedItem=i; break;}


		//do the down shift of one or multiple items
		for(int i=max;i>=0;--i)
		{
			if(ListBoxSpriteList->Selected[i])
			{
				memcpy( temp            ,&metaSprites[i*4+off],4);
				memcpy(&metaSprites[i*4+off],&metaSprites[i*4+4+off],4);
				memcpy(&metaSprites[i*4+4+off], temp            ,4);

			if (i<max) //
				ListBoxSpriteList->Selected[i+1]=ListBoxSpriteList->Selected[i];
			}


		}
		++spriteActive;
		ListBoxSpriteList->Selected[firstSelectedItem]=false;
		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	}


	/*
	//original routine
	off=metaSpriteActive*64*4+spriteActive*4;

	if(spriteActive<63)
	{
	SetUndo();
		if(metaSprites[off+4]<255)
		{
			memcpy( temp              ,&metaSprites[off  ],4);
			memcpy(&metaSprites[off  ],&metaSprites[off+4],4);
			memcpy(&metaSprites[off+4], temp              ,4);

			++spriteActive;

			//UpdateMetaSprite();
			cueUpdateMetasprite=true;
		}
	}

	SelectSprite(spriteActive,false);*/
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ListBoxSpriteListKeyDown(TObject *Sender, WORD &Key,
TShiftState Shift)
{
int max = ListBoxSpriteList->Items->Count-1;

	if (!Shift.Contains(ssCtrl))
	{
		FormKeyDown(Sender,Key,Shift);
		Key=0;
	}
	else //quickfix for selecting multiple items downwards. i should probably refactor sprite selection at some point but this works.
	{
		if(Key==VK_DOWN&&Shift.Contains(ssShift))
		if(ListBoxSpriteList->Items->Count)
		if(!ListBoxSpriteList->Selected[max])
		{
			++spriteActive;
			if(spriteActive>=0) ListBoxSpriteList->Selected[spriteActive]=true;

			Key=0;
			//UpdateMetaSprite();
			cueUpdateMetasprite=true;
		}
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageMetaSpriteMouseDown(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int i,off;

	if(IsBlockDrawing()) return;

	if(Shift.Contains(ssLeft)&&!Shift.Contains(ssCtrl))
	{
		i=GetSpriteID(X,Y);

		if(i>=0)
		{
			SelectSprite(i,false);
		}
	}
	if(Shift.Contains(ssLeft)&&Shift.Contains(ssCtrl)&&!Shift.Contains(ssRight))
	{
       //begin dragging anchor
	   SetUndo();

	}

	if(Shift.Contains(ssRight)&&!Shift.Contains(ssLeft))
	{
		i=GetSpriteID(X,Y);

		if(i>=0)
		{
			SelectSprite(i,false);
		}

		off=metaSpriteActive*64*4+spriteActive*4;

		if(metaSprites[off]<255)
		{
			SetUndo();

			spriteDragX=metaSprites[off+3]-X/uiScale;
			spriteDragY=metaSprites[off+0]-Y/uiScale;
		}

		spriteDrag=i;
	}

	ImageMetaSpriteMouseMove(Sender,Shift,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageMetaSpriteMouseLeave(TObject *Sender)
{
	spriteHover=-1;
	spriteDrag=-1;
	//UpdateStats();
	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageMetaSpriteMouseMove(TObject *Sender,
TShiftState Shift, int X, int Y)
{
	int x,y,off;

	if(IsBlockDrawing()) return;

	if(Shift.Contains(ssRight)&&spriteDrag>=0)
	{
		off=metaSpriteActive*64*4+spriteDrag*4;

		if(metaSprites[off]<255)
		{
			x=X/uiScale+spriteDragX;
			y=Y/uiScale+spriteDragY;

			if(x<0)   x=0;
			if(x>120) x=120;
			if(y<0)   y=0;
			if(y>120) y=120;

			metaSprites[off+3]=x;
			metaSprites[off+0]=y;

			//UpdateMetaSprite();
			cueUpdateMetasprite=true;
		}
	}

	if(Shift.Contains(ssLeft)&&Shift.Contains(ssCtrl))
	{
		spriteGridX=(X/uiScale)&~7;
		spriteGridY=(Y/uiScale)&~7;

		//original was 8 but that's sort of a mistake.
		if(spriteGridX<0) spriteGridX=0;
		if(spriteGridY<0) spriteGridY=0;

		if(spriteGridX>=128) spriteGridX=128;
		if(spriteGridY>=128) spriteGridY=128;
		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	}

	spriteHover=GetSpriteID(X,Y);

	//UpdateStats();
	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageMetaSpriteMouseUp(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(SpeedButtonSpriteSnap->Down)
	{
		if(spriteActive>=0) SpriteSnap(spriteActive);

		//UpdateMetaSprite();
		cueUpdateMetasprite=true;

	}

	spriteDrag=-1;
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MOpenMetaSpriteBankClick(TObject *Sender)
{
	BlockDrawing(true);
	
	if(OpenDialogMetaSpriteBank->Execute())
	{
		if(OpenMetaSprites(OpenDialogMetaSpriteBank->FileName, false))
		{
			OpenDialogMetaSpriteBank->FileName=RemoveExt(OpenDialogMetaSpriteBank->FileName);

			SaveDialogMetaSpriteBank->FileName=OpenDialogMetaSpriteBank->FileName;
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveMetaSpriteBankClick(TObject *Sender)
{
	FILE *file;
	unsigned char data[2];
	AnsiString name;

	BlockDrawing(true);

	if(SaveDialogMetaSpriteBank->Execute())
	{
		name=RemoveExt(SaveDialogMetaSpriteBank->FileName)+".msb";

		if(!OverwritePrompt(name))
		{
			BlockDrawing(false);
			return;
		}

		file=fopen(name.c_str(),"wb");

		if(file)
		{
			data[0]=spriteGridX;
			data[1]=spriteGridY;

			fwrite(data,2,1,file);
			fwrite(metaSprites,256*64*4,1,file);
			fclose(file);
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MMetaSpritePutBankToClipboardCClick(TObject *Sender)
{
	char *str,buf[1024];
	char strBuf[256];

	int i,j,pp,off,size,str_size;
    bool ask=false;
	if(AskBankName1->Checked) ask=true;

	AnsiString name;
    //BROKE STUDIO
	//FG:	Name is already implied by session filename
	//FormName->EditName->Text=metaSpriteBankName;

	//
	if(ask)
	{
		FormName->Caption="Bank label";
		FormName->EditName->Text=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
		FormName->ShowModal();
		name=RemoveExt(ExtractFileName(FormName->EditName->Text));
		//name=FormName->EditName->Text;
		FormName->Caption="Metasprite label"; //return to normal
	}
	else name=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
	//BROKE STUDIO
	metaSpriteBankName=name;
	//
	str_size=65536;

	str=(char*)malloc(str_size);

	str[0]=0;

	for(i=0;i<256;++i)
	{
		off=i*64*4;

		if(metaSprites[off]<255)
		{
			//old implied method
			//sprintf(buf,"const unsigned char %s_%i_data[]={\n",name.c_str(),i);
			//BROKE STUDIO's method. This might need to be a preferences setting or something.
			sprintf(buf,"const unsigned char %s_%s_data[]={\n",name.c_str(),metaSpriteNames[i]);
			//
			int sprCnt = 0;
			int offBuf = off;
			for(int k=0;k<64;++k) if(metaSprites[offBuf]<255) {++sprCnt; offBuf+=4;}

			if(sprCnt>=10) sprintf(strBuf," %d", sprCnt);
			else 		   sprintf(strBuf,"  %d", sprCnt);
			strcat(strBuf,",\n");

			strcat(str,buf);   // new sprite declaration
			if(Spritecountheader1->Checked) strcat(str,strBuf); // add sprite count

			for(j=0;j<64;++j)			//lay down a line
			{
				if(metaSprites[off]==255) continue;

				if(j) strcat(str,",\n");
				if(!(j%4)) strcat(str,"\n");

				sprintf(buf,"\t%s,%s,0x%2.2x,%i",num_to_3char(metaSprites[off+3]-spriteGridX),num_to_3char(metaSprites[off+0]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);

				strcat(str,buf);

				if(metaSprites[off+2]&OAM_B2) strcat(str,"|B2");
				if(metaSprites[off+2]&OAM_B3) strcat(str,"|B3");
				if(metaSprites[off+2]&OAM_B4) strcat(str,"|B4");
				if(metaSprites[off+2]&OAM_FLIP_H) strcat(str,"|OAM_FLIP_H");
				if(metaSprites[off+2]&OAM_FLIP_V) strcat(str,"|OAM_FLIP_V");
				if(metaSprites[off+2]&OAM_PRIO) strcat(str,"|OAM_PRIO");

				off+=4;
			}

			if(Nflagterminator1->Checked)	 strcat(str,",\n\t0x80\n\n};\n\n");
			if(FFterminator1->Checked)		 strcat(str,",\n\t0xff\n\n};\n\n");
			if(Single00terminator1->Checked) strcat(str,",\n\t0\n\n};\n\n");
			if(Double00terminator1->Checked) strcat(str,",\n\t0,0\n\n};\n\n");
			if(Spritecountheader1->Checked)  strcat(str,"\n\n};\n\n");

			//strcat(str,",\n\t128");
			//strcat(str,"\n\n};\n\n");
		}

		if((int)strlen(str)>=str_size/2)
		{
			str_size+=65536;
			str=(char*)realloc(str,str_size);
		}
	}

	sprintf(buf,"const unsigned char* const %s_list[]={\n\n",name.c_str());

	strcat(str,buf);

	pp=0;

	for(i=0;i<256;++i)
	{
		off=i*64*4;

		if(metaSprites[off]<255)
		{
			if(pp) strcat(str,",\n");
            //BROKE STUDIO
			sprintf(buf,"\t%s_%s_data",name.c_str(),metaSpriteNames[i]);
			//
            //same as earlier in this routine; this might need to be a preferences setting
			//sprintf(buf,"\t%s_%i_data",name.c_str(),i);

			strcat(str,buf);

			size=1;

			for(j=0;j<64;++j)
			{
				if(metaSprites[off]==255) break;

				off+=4;
				size+=4;
			}

			pp+=size;
		}
	}

	strcat(str,"\n\n};\n\n");

	Clipboard()->SetTextBuf(str);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MMetaSpritePutToClipboardCClick(TObject *Sender)
{
	char str[65536],buf[1024];
	int i,j,x,y,off;

	off=metaSpriteActive*64*4;
	bool ask=false;
	AnsiString name;
	if(AskMetaName1->Checked) ask=true;
	if(ask)
	{
        FormName->EditName->Text=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
		FormName->ShowModal();
		name=RemoveExt(ExtractFileName(FormName->EditName->Text));
	}
	else name=RemoveExt(ExtractFileName(SaveDialogSession->FileName));


	sprintf(buf, "  %d", ListBoxSpriteList->Items->Count);
	strcat(buf,",\n");

	if(metaSprites[off]<255)
	{
		if(ask)sprintf(str,"const unsigned char %s[]={\n",name.c_str());
		else sprintf(str,"const unsigned char %s[]={\n",metaSpriteNames[metaSpriteActive]);
		if(Spritecountheader1->Checked)
		{
			strcat(str,"\t");
			strcat(str,buf);
		}
		for(i=0;i<64;++i)
		{
			if(i) strcat(str,",\n");

			if(metaSprites[off]==255) break;
			if(!(i%4)) strcat(str,"\n");
			sprintf(buf,"\t%s,%s,0x%2.2x,%i",num_to_3char(metaSprites[off+3]-spriteGridX),num_to_3char(metaSprites[off]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);

			strcat(str,buf);

			if(metaSprites[off+2]&OAM_B2) strcat(str,"|B2");
			if(metaSprites[off+2]&OAM_B3) strcat(str,"|B3");
			if(metaSprites[off+2]&OAM_B4) strcat(str,"|B4");
			if(metaSprites[off+2]&OAM_FLIP_H) strcat(str,"|OAM_FLIP_H");
			if(metaSprites[off+2]&OAM_FLIP_V) strcat(str,"|OAM_FLIP_V");
			if(metaSprites[off+2]&OAM_PRIO) strcat(str,"|OAM_PRIO");
			off+=4;
		}

		if(Nflagterminator1->Checked)	 strcat(str,"\t0x80\n};");
		if(FFterminator1->Checked)		 strcat(str,"\t0xff\n};");
		if(Single00terminator1->Checked) strcat(str,"\t0\n};");
		if(Double00terminator1->Checked) strcat(str,"\t0,0\n};");
		if(Spritecountheader1->Checked)  strcat(str,"};");

		strcat(str,"\n\n");
		Clipboard()->SetTextBuf(str);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteHFlipClick(TObject *Sender)
{
	bool tmpBufCtrl		=(GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	bool tmpBufShift 	=(GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	//bool tmpBufAlt		=(GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

	if((tmpBufCtrl)&&(!tmpBufShift)){
		FlipMetaSprites_justPosition(true,false);}

	else if((!tmpBufCtrl)&&(tmpBufShift)){
		FlipMetaSprites(true,false,false);}

	else {FlipMetaSprites(true,false,true);}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteVFlipClick(TObject *Sender)
{
	bool tmpBufCtrl		=(GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	bool tmpBufShift 	=(GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	//bool tmpBufAlt		=(GetAsyncKeyState(VK_MENU) & 0x8000) != 0;


	if((tmpBufCtrl)&&(!tmpBufShift)){
		FlipMetaSprites_justPosition(false,true);}

	else if((!tmpBufCtrl)&&(tmpBufShift)){
		FlipMetaSprites(false,true,false);}

	else {FlipMetaSprites(false,true,true);}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMetaSpriteCopyClick(TObject *Sender)
{
	MetaSpriteCopy();
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMetaSpritePasteClick(TObject *Sender)
{
	MetaSpritePaste();
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMetaSpriteHFlipClick(TObject *Sender)
{
	int i,x,off;

	SetUndo();

	off=metaSpriteActive*64*4;

	for(i=0;i<64;++i)
	{
		if(metaSprites[off]==255) break;

		x=-8-(metaSprites[off+3]-spriteGridX);

		metaSprites[off+3]=spriteGridX+x;
		metaSprites[off+2]^=OAM_FLIP_H;

		off+=4;
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMetaSpriteVFlipClick(TObject *Sender)
{
	int i,y,off;

	SetUndo();
	
	off=metaSpriteActive*64*4;

	for(i=0;i<64;++i)
	{
		if(metaSprites[off]==255) break;

		y=-8-(metaSprites[off+0]-spriteGridY);

		metaSprites[off+0]=spriteGridY+y;
		metaSprites[off+2]^=OAM_FLIP_V;

		off+=4;
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRFindUnusedClick(TObject *Sender)
{
	if(MCHRFindUnused->Checked)
	{
		for(int i=0;i<256;++i) chrSelected[i]=0;
		SelectTile(tileActive);
		MCHRFindUnused->Checked=false;
		MCHRFindUnused->Caption="Find &unused";
        UpdateAll();
	}
	else
	{
		FindDoublesUnused(true);
		MCHRFindUnused->Checked=true;
		MCHRFindUnused->Caption="Deselect Find &unused";

	   LabelStats->Caption="Unused found. \n\nTip: Press again to Deselect.";
	   StatusUpdateWaiter->Enabled=true;
	   holdStats=true;


	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRRemoveUnusedClick(TObject *Sender)
{
	//FG: this, along with the other remove/sort actions, were modified to toggle between do/undo for the sake of fluidity.
	if(MCHRRemoveUnused->Checked)
	{
		Undo();
		MCHRRemoveUnused->Checked=false;
		MCHRRemoveUnused->Caption="Re&move unused";
	}
	else
	{
		RemoveDoublesUnused(true);
		MCHRRemoveUnused->Checked=true;
		MCHRRemoveUnused->Caption="Undo re&move unused";
    	LabelStats->Caption="Unused removed. \n\nTip: Press again to undo.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSprite8x16Click(TObject *Sender)
{
	SpeedButtonSpriteBank->Enabled=SpeedButtonSprite8x16->Down;

	//create 8x16 vs 8x8 indirection table

	int t;
	bool b=((SpeedButtonSprite8x16->Down==true)&&(ButtonForceView->Down==true));
	AnsiString str;
	AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
	if(b)
	{
		 if(Normal1->Checked) 				t = Normal1->Tag;
		 if(N8x16spritemode1->Checked)  	t = N8x16spritemode1->Tag;

		 if(Sortedbyfrequency1->Checked)  	t = Sortedbyfrequency1->Tag;
		 if(Sortedbydensity1->Checked)  	t = Sortedbydensity1->Tag;
		 if(Sortedbydetail1->Checked)  		t = Sortedbydetail1->Tag;
		 if(Sortedbyedge1->Checked)  		t = Sortedbyedge1->Tag;
		 if(Sortedbycurcol1->Checked)  		t = Sortedbycurcol1->Tag;

		 if(N4x12x21->Checked)  			t = N4x12x21->Tag;
		 if(N4x12x2topbottom1->Checked)  	t = N4x12x2topbottom1->Tag;
		 if(N16tiles4x41->Checked)  		t = N16tiles4x41->Tag;

		 tileViewTagBuf = t;
		 SetViewTable(1);
		 str=tmp_LinkCHRstr+"Tileset (8x16 sprite mode)";

	}
	else
	{
		if(Normal1->Checked) 			str = "Tileset";
		if(N8x16spritemode1->Checked)  	str = "Tileset (8x16 mode)";

		if(Sortedbyfrequency1->Checked) str = "Tileset (by frequency)";
		if(Sortedbydensity1->Checked)  	str = "Tileset (by density)";
		if(Sortedbydetail1->Checked)  	str = "Tileset (by detail)";
		if(Sortedbyedge1->Checked)  	str = "Tileset (by edge content)";
		if(Sortedbycurcol1->Checked)  	str = "Tileset (by selected colour)";

		if(N4x12x21->Checked)  			str = "Tileset (by 4x1 -> 2x2)";
		if(N4x12x2topbottom1->Checked)  str = "Tileset (by 4x1 -> 2x2 top-down)";
		if(N16tiles4x41->Checked)  		str = "Tileset (by 16x1 -> 4x4)";


		SetViewTable(tileViewTagBuf);
		//revert to the checked option
	}

	GroupBoxTiles->Caption=str;

	//redo selection box
	bool b8x16=(SpeedButtonSprite8x16->Down);
	if(spriteActive>=0)
	{
		int off=metaSpriteActive*64*4+spriteActive*4;

		if(metaSprites[off]<255)
		{

			if(b8x16)	SelectTile8x16   (metaSprites[off+1]);
			else		SelectTile       (metaSprites[off+1]);
		}
	}




	//UpdateMetaSprite();
	UpdateTiles(true);
	cueUpdateMetasprite=true;
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteBankClick(TObject *Sender)
{
	int i,off;

	SetUndo();
	
	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			off=metaSpriteActive*64*4+i*4;

			if(metaSprites[off]<255) metaSprites[off+1]^=1;
		}
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	isLastClickedMetaSprite=false;
	isLastClickedSpriteList=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteGridClick(TObject *Sender)
{
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MMetaSpritePutToClipboardAsmClick(TObject *Sender)
{
	char str[65536],buf[1024];
	int i,j,x,y,off,sprCnt;
    bool ask=false;
	if(AskMetaName1->Checked) ask=true;
	AnsiString name;
	off=metaSpriteActive*64*4;
	sprCnt=ListBoxSpriteList->Items->Count;

    if(ask)
	{
		FormName->EditName->Text=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
		FormName->ShowModal();
		name=RemoveExt(ExtractFileName(FormName->EditName->Text));
	}
	else name=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
    //^Redundant by now, i expect. just waiting until all changes are done, and then i'll probably forget about this line.
	if(byte1->Checked)
		if(sprCnt>=10)	sprintf(buf, "\n\t.byte  %d", sprCnt);
		else            sprintf(buf, "\n\t.byte   %d", sprCnt);
	if(db1->Checked)
		if(sprCnt>=10)  sprintf(buf, "\n\t.db  %d", sprCnt);
		else		    sprintf(buf, "\n\t.db   %d", sprCnt);

	if(metaSprites[off]<255)
	{
		//sprintf(str,"metasprite:\n");
		if (ask) sprintf(str,"%s:\n", name.c_str());
		else sprintf(str,"%s:\n",metaSpriteNames[metaSpriteActive]);
		if(Spritecountheader1->Checked) strcat(str,buf);
		for(i=0;i<64;++i)
		{
			strcat(str,"\n");

			if(metaSprites[off]==255) break;

			if(!(i%4)) strcat(str,"\n");

			if(byte1->Checked) {
				if(SignOther->Checked){sprintf(buf,"\t.byte %s,%s,$%2.2x,%i",num_to_3char(metaSprites[off+3]-spriteGridX),		num_to_3char(metaSprites[off]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);}
			}   else{				   sprintf(buf,"\t.byte %s,%s,$%2.2x,%i",num_to_4char_ca65(metaSprites[off+3]-spriteGridX), num_to_4char_ca65(metaSprites[off]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);}
			if(db1->Checked){
				if(SignOther->Checked){sprintf(buf,"\t.db %s,%s,$%2.2x,%i",	 num_to_3char(metaSprites[off+3]-spriteGridX),num_to_3char(metaSprites[off]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);}
				else{				   sprintf(buf,"\t.db %s,%s,$%2.2x,%i",	 num_to_4char_ca65(metaSprites[off+3]-spriteGridX),num_to_4char_ca65(metaSprites[off]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);}

			}
			strcat(str,buf);

			if(metaSprites[off+2]&OAM_B2) strcat(str,"|B2");
			if(metaSprites[off+2]&OAM_B3) strcat(str,"|B3");
			if(metaSprites[off+2]&OAM_B4) strcat(str,"|B4");
			if(metaSprites[off+2]&OAM_FLIP_H) strcat(str,"|OAM_FLIP_H");
			if(metaSprites[off+2]&OAM_FLIP_V) strcat(str,"|OAM_FLIP_V");
			if(metaSprites[off+2]&OAM_PRIO) strcat(str,"|OAM_PRIO");

			off+=4;
		}
		if(!(Spritecountheader1->Checked||Noterminator1->Checked))
		{
			if(byte1->Checked) strcat(str,"\n\t.byte ");
			if(db1->Checked) strcat(str,"\n\t.db ");
		}
		if(Nflagterminator1->Checked)	 strcat(str,"$80\n");
		if(FFterminator1->Checked)		 strcat(str,"$ff\n");
		if(Single00terminator1->Checked) strcat(str,"  0\n");
		if(Double00terminator1->Checked) strcat(str,"  0,  0\n");

		strcat(str,"\n\n");

		Clipboard()->SetTextBuf(str);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MMetaSpritePutBankToClipboardAsmClick(
TObject *Sender)
{
	char str[65536],buf[1024],hdrBuf[256];
	int i,j,off;
	bool ask=false;
	if(AskBankName1->Checked) ask=true;
	AnsiString name;

	if(ask)
	{
        FormName->EditName->Text=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
		FormName->ShowModal();
		name=RemoveExt(ExtractFileName(FormName->EditName->Text));
	}
	else //name=SaveDialogSession->FileName;
	name=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
	//
	str[0]=0;

	for(i=0;i<256;++i)
	{
		off=i*64*4;

		if(metaSprites[off]<255)
		{
			//sprintf(buf,"%s_%i_data:\n\n",name.c_str(),i);
			sprintf(buf,"%s_%s_data:\n\n",name.c_str(),metaSpriteNames[i]);
			strcat(str,buf); //lay down ID

			//in case we want a header byte
			int sprCnt = 0;
			int offBuf = off;
			for(int k=0;k<64;++k) if(metaSprites[offBuf]<255) {++sprCnt; offBuf+=4;}

			if(byte1->Checked)
				if(sprCnt>=10)	sprintf(hdrBuf, "\n\t.byte  %d\n", sprCnt);
				else            sprintf(hdrBuf, "\n\t.byte   %d\n", sprCnt);
			if(db1->Checked)
				if(sprCnt>=10)  sprintf(hdrBuf, "\n\t.db  %d", sprCnt);
				else		    sprintf(hdrBuf, "\n\t.db   %d", sprCnt);
			if(Spritecountheader1->Checked) strcat(str,hdrBuf);

			for(j=0;j<64;++j)
			{
				if(metaSprites[off]==255) continue;

				if(j) strcat(str,"\n");

				if(!(j%4)) strcat(str,"\n");

				if(byte1->Checked) sprintf(buf,"\t.byte %s,%s,$%2.2x,%i",num_to_3char(metaSprites[off+3]-spriteGridX),num_to_3char(metaSprites[off+0]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);
				if(db1->Checked)   sprintf(buf,"\t.db %s,%s,$%2.2x,%i",num_to_3char(metaSprites[off+3]-spriteGridX),num_to_3char(metaSprites[off+0]-spriteGridY),metaSprites[off+1],metaSprites[off+2]&3);


				strcat(str,buf);

				/*
				//todo:
				//unfinished option to cast as binary written constant

				char undefBits = metaSprites[off+2]&(OAM_B2|OAM_B3|OAM_B4)

				if (undefBits)
				{
					strcat(str,"|%");
					strcat(str,IntToStrAsBin(undefBits);
				}
				*/

				if(metaSprites[off+2]&OAM_B2) strcat(str,"|B2");
				if(metaSprites[off+2]&OAM_B3) strcat(str,"|B3");
				if(metaSprites[off+2]&OAM_B4) strcat(str,"|B4");
				if(metaSprites[off+2]&OAM_FLIP_H) strcat(str,"|OAM_FLIP_H");
				if(metaSprites[off+2]&OAM_FLIP_V) strcat(str,"|OAM_FLIP_V");
				if(metaSprites[off+2]&OAM_PRIO) strcat(str,"|OAM_PRIO");

				off+=4;
			}
			if(!(Spritecountheader1->Checked||Noterminator1->Checked))
			{
				if(byte1->Checked) strcat(str,"\n\t.byte ");
				if(db1->Checked) strcat(str,"\n\t.db ");
			}
			if(Nflagterminator1->Checked)	 strcat(str,"$80\n");
			if(FFterminator1->Checked)		 strcat(str,"$ff\n");
			if(Single00terminator1->Checked) strcat(str,"  0\n");
			if(Double00terminator1->Checked) strcat(str,"  0,  0\n");

			strcat(str,"\n\n");
		}
	}

	strcat(str,(name+"_pointers:\n").c_str());

	for(i=0;i<256;++i)
	{
		off=i*64*4;

		if(metaSprites[off]<255)
		{
			strcat(str,"\n");

			//if(byte1->Checked) sprintf(buf,"\t.word %s_%i_data",name.c_str(),i);
			//if(db1->Checked)   sprintf(buf,"\t.dw %s_%i_data",name.c_str(),i);
			if(byte1->Checked) sprintf(buf,"\t.word %s_%s_data",name.c_str(),metaSpriteNames[i]);
			if(db1->Checked)   sprintf(buf,"\t.dw %s_%s_data",name.c_str(),metaSpriteNames[i]);

			strcat(str,buf);

			for(j=0;j<64;++j)
			{
				if(metaSprites[off]==255) break;

				off+=4;
			}
		}
	}

	strcat(str,"\n\n");

	Clipboard()->SetTextBuf(str);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::StaticTextFontOffsetMouseDown(TObject *Sender,
TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(Shift.Contains(ssLeft)) prevMouseY=Y;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::StaticTextFontOffsetMouseMove(TObject *Sender,
TShiftState Shift, int X, int Y)
{
	int diff;

	if(Shift.Contains(ssLeft))
	{
		diff=abs(prevMouseY-Y);

		if(Shift.Contains(ssShift))
		{
			//typeInFontOffset+=1*(delta); //hold shift to fine adjust
			typeInFontOffset=1*(diff/2);
		}
		else
		{
			//typeInFontOffset+=16*(delta); //original behaviour
			typeInFontOffset=16*(diff/32); //original behaviour

		}

		if(typeInFontOffset<0x00) typeInFontOffset=0x00;
		if(typeInFontOffset>0xff) typeInFontOffset=0x00;   //was f0

		StaticTextFontOffset->Caption="$"+IntToHex(typeInFontOffset,2);

		//prevMouseY=Y;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ImageTilesDragOver(TObject *Sender, TObject *Source,
int X, int Y, TDragState State, bool &Accept)
{
	bool b=FormCHREditor->btn2x2mode->Down;
	bool bSmallWdt=chrSelection.right-chrSelection.left<2?1:0;
	bool bSmallHgt=chrSelection.bottom-chrSelection.top<2?1:0;

	Accept=false;

	cueStats=true;

	if((PageControlEditor->ActivePage==TabSheetName
		||PageControlEditor->ActivePage==TabSheetSprite)
		&&Source==ImageTiles)
	{
		if(X>=0&&X<(128*uiScale)&&Y>=0&&Y<(128*uiScale))
		{
			tileXC=X/(8*uiScale);
			tileYC=Y/(8*uiScale);
		}
		Accept=true;
	}

	if(PageControlEditor->ActivePage==TabSheetTile&&Source==ImageBigTiles)
	{
		if(X>=0&&X<(256*uiScale)&&Y>=0&&Y<(256*uiScale))
		{
			tileXC=X/(16*uiScale);
			tileYC=Y/(16*uiScale);
		}
		Accept=true;
	}


	if (Accept==true)
	{
		bImageNameAccepted=false;
		bImageTileAccepted=true;
		if (!bOutsideSel&&(!bSmallWdt||!bSmallHgt))
		{
			destRect.left	=chrSelection.left		+tileXC-txDown;
			destRect.right	=chrSelection.right		+tileXC-txDown;
			destRect.top	=chrSelection.top		+tileYC-tyDown;
			destRect.bottom	=chrSelection.bottom	+tileYC-tyDown;

			for (int i=0; i<16; i++)  //long enough loop
				{
					if(destRect.left<0)   	{	destRect.left++;
												destRect.right++;}
					if(destRect.right>0x10)	{	destRect.left--;
												destRect.right--;}
					if(destRect.top<0)   	{	destRect.top++;
												destRect.bottom++;}
					if(destRect.bottom>0x10){	destRect.top--;
												destRect.bottom--;}
				}

		}
        //old behaviour. Now redundancies but letÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s keep them.
		else if(b)
		{
            destRect.left=tileXC;
			destRect.top=tileYC;
			if(tileXC>=15) destRect.left=14; 
			if(tileYC>=15) destRect.top=14;
			destRect.right=tileXC+2;
			destRect.bottom=tileYC+2;
		}
		else
		{
			destRect.left=tileXC;
			destRect.top=tileYC;
			destRect.right=tileXC+1;
			destRect.bottom=tileYC+1;
		}
		bDrawDestShadow=true;
		cueUpdateTiles=true;
	}

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageTilesDragDrop(TObject *Sender, TObject *Source,
int X, int Y)
{
	//If same src and dest, exit & donÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´t set Undo.
	if(destRect.left==chrSelection.left&&destRect.top==chrSelection.top) return;

	//REMOVE THIS when swaps work in 8x16 mode!!
   //	if(SpeedButtonSprite8x16->Down) return;


	//int set = bankActive/16;
	uint32_t propt[sizeof(tileProperties)];
	int propt_el = 1*sizeof(uint32_t);


	unsigned char tempchr[16];
	int tile,ps,pd, ba, offset;


	bool b2x2 = FormCHREditor->btn2x2mode->Down;

	bool bForceActive = RepairActiveTab1->Checked;
	bool bNT = RepairNT1->Checked;
	bool bMS = RepairMSPR1->Checked;

	bool bClone = ( bBufCtrl && !bBufShift &&  bBufAlt);
	bool bSwap	= (!bBufCtrl && !bBufShift && !bBufAlt);
	bool bMove	= ( bBufCtrl && !bBufShift && !bBufAlt);

	int w=1;   //init to single tile
	int h=1;

	if (!bOutsideSel) //if grabbed from inside selection, retain size
	{
		w=destRect.right-destRect.left;
		h=destRect.bottom-destRect.top;
	}
	else if (b2x2) { //last case, 2x2 edit mode.
		w=2;
		h=2;
	}


	if (bForceActive) {  //overrules the other options if "force: work on active tab only" is set.
		bMS=PageControlEditor->ActivePage==TabSheetSprite?true:false;
		bNT=!bMS;
	}

	SetUndo();

	X/=(8*uiScale*2);
	Y/=(8*uiScale*2);

	if(X<0||X>=(8*uiScale)||Y<0||Y>=(8*uiScale)) return;


	tile=destRect.top*16+destRect.left;

	//ps=tileActive*16+bankActive;
	//pd=tile*16+bankActive;
	ps=tileActive;
	pd=tile;
	ba=bankActive;

	const int tw=16;   //tileset table width

	int xSource = tileViewTable[tileActive&(tw-1)];
	int ySource = tileViewTable[tileActive/tw];

	if(nullTile==tileActive)  //TODO_02_2023: check if correct assignment in 8x16 mode
	{
		nullTile=tile;
		TextNullTile->Caption="   null tile: $"+IntToHex(nullTile,2)+" ";
	}

	for(int sy=0; sy<h*tw; sy+=tw) {
		for(int sx=0; sx<w; sx++) {
			if (ySource>=Y) {
				if   (xSource>X) offset=sx+(sy);
				else 			 offset=(w-1)-sx+(sy);
			}
			else {
				if 	 (xSource>X) offset=sx+((h-1)*tw)-(sy);
				else 			 offset=(w-1)-sx+((h-1)*tw)-(sy);
			}

			int bvtd = bankViewTable[ba/16 +pd+offset];
			int bvts = bankViewTable[ba/16 +ps+offset];
			int bvtsprop = bvts/16;
			int bvtdprop = bvtd/16;
			int	bvtn = bankViewTable[ba/16 +nullTile];
			int bvtnprop = bvtn/16;
			//swap
			if(bSwap){
				//memcpy(tempchr			   , &chr[pd+offset]	,t);
				//memcpy(&chr		[pd+offset], &chr[ps+offset]	,t);
				//memcpy(&chr		[ps+offset], tempchr			,t);
				memcpy(tempchr			   , &chr[bvtd+tileViewTable[pd+offset]*16]	,16);
				memcpy(&chr		[bvtd+tileViewTable[pd+offset]*16], &chr[bvts+tileViewTable[ps+offset]*16]	,16);
				memcpy(&chr		[bvts+tileViewTable[ps+offset]*16], tempchr			,16);

				if(RepairProps->Checked==true){
					memcpy(propt			   ,&tileProperties[bvtdprop+tileViewTable[pd+offset]]	,propt_el);
					memcpy(&tileProperties		[bvtdprop+tileViewTable[pd+offset]], &tileProperties[bvtsprop+tileViewTable[ps+offset]]	,propt_el);
					memcpy(&tileProperties		[bvtsprop+tileViewTable[ps+offset]], propt			,propt_el);
				}


			}

			//move
			if(bMove){
				memcpy(&chr		[bvtd+tileViewTable[pd+offset]*16], &chr[bvts+tileViewTable[ps+offset]*16]	,16);
				memcpy(&chr		[bvts+tileViewTable[ps+offset]*16], &chr[bvtn+tileViewTable[nullTile]*16]		,16);

				if(RepairProps->Checked==true){
					memcpy(&tileProperties		[bvtdprop+tileViewTable[pd+offset]], &tileProperties[bvtsprop+tileViewTable[ps+offset]]	,propt_el);
					memcpy(&tileProperties		[bvtsprop+tileViewTable[ps+offset]], &tileProperties[bvtnprop+tileViewTable[nullTile]]	,propt_el);

				}
			}

			//clone
			if(bClone){
				memcpy(&chr		[bvtd+tileViewTable[pd+offset]*16], &chr[bvts+tileViewTable[ps+offset]*16]	,16);

				if(RepairProps->Checked==true){
					memcpy(&tileProperties		[bvtdprop+tileViewTable[pd+offset]], &tileProperties[bvtsprop+tileViewTable[ps+offset]]	,propt_el);

				}
			}

			if(bNT && (bSwap || bMove)){
				for(int i=0;i<nameTableWidth*nameTableHeight;++i)  //nametable reindex
				{
					//if(nameTable[i]==tileActive+offset/t) nameTable[i]=tile+offset/t;
					//else if(nameTable[i]==tile+offset/t) nameTable[i]=tileActive+offset/t;
					if(nameTable[i]==tileViewTable[ps+offset]) nameTable[i]=tileViewTable[pd+offset];
					else if(nameTable[i]==tileViewTable[pd+offset]) nameTable[i]=tileViewTable[ps+offset];
				}
			}
			if(bMS && (bSwap || bMove)){
				for (int i = 0; i < 64*256; i++) //metasprite bank reindex
				{
					if		(metaSprites[(i*4)+1]==tileViewTable[ps+offset]) metaSprites[(i*4)+1]=tileViewTable[pd+offset];
					else if (metaSprites[(i*4)+1]==tileViewTable[pd+offset])	   metaSprites[(i*4)+1]=tileViewTable[ps+offset];
				}
			}
		}
	}


	//SetTile(tile);
	SelectTile(tile); //destination is the new active tile.
	chrSelection.right=chrSelection.left+w;  //restore selection width/height (SetTile clobbers it).
	chrSelection.bottom=chrSelection.top+h;
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave1KCursorClick(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(tileActive*16,1024,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave2KCursorClick(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(tileActive*16,2048,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave4KClick(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0,4096,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave8KClick(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0,8192,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSaveSelectionClick(TObject *Sender)
{
	FILE *file;
	int i,size;
	AnsiString name;

	BlockDrawing(true);
	
	if(SaveDialogChr->Execute())
	{
		name=RemoveExt(SaveDialogChr->FileName)+".chr";

		size=0;

		for(i=0;i<256;++i) if(chrSelected[i]) size+=16;

		file=fopen(name.c_str(),"rb");

		if(file)
		{
			fseek(file,0,SEEK_END);
			i=ftell(file);
			fclose(file);

			if(Application->MessageBox((size==i?"Overwrite?":"Previous file has different size! Overwrite?"),"Confirm",MB_YESNO)!=IDYES)
			{
				BlockDrawing(false);
				return;
			}
		}

		file=fopen(name.c_str(),"wb");

		if(file)
		{
			for(i=0;i<256;i++)
			{
				if(chrSelected[i]) fwrite(chr+i*16+bankViewTable[bankActive/16+i],16,1,file);
			}

			fclose(file);

			SetLatestCHR(0,size,true);
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutSelectedBlockToClipboardCClick(TObject *Sender)
{
	CopyMapCodeC(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutSelectedBlockToClipboardASMClick(TObject *Sender)
{
	CopyMapCodeASM();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutMetaSpriteToClipboardCNoFlipClick(TObject *Sender)
{
	CopyMetaSpriteCodeC(false);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutMetaSpriteToClipboardCHFlipClick(TObject *Sender)
{
	CopyMetaSpriteCodeC(true);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSaveLatestClick(TObject *Sender)
{
	BlockDrawing(true);

	switch(CHRLatestSize)
	{
	case 1024: SaveCHR(CHRLatestOffset,1024,true); break;
	case 2048: SaveCHR(CHRLatestOffset,2048,true); break;
	case 4096: SaveCHR(CHRLatestOffset,4096,true); break;
	case 8192: SaveCHR(CHRLatestOffset,8192,true); break;
	default: MCHRSaveSelectionClick(Sender);
	}

	BlockDrawing(false);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutPaletteToClipboardAssemblyClick(TObject *Sender)
{
	char str[1024],buf[1024];
	int i,j;

	pal_validate();

	sprintf(str,"\tlda #$3f\n\tsta $2006\n\tlda #$00\n\tsta $2006\n\tldx #$%2.2x\n\tstx $2007\n",bgPal[palBank*16+0]);

	for(i=0;i<4;i++)
	{
		if(i) strcat(str,"\tstx $2007\n");
		for(j=1;j<4;j++)
		{
			sprintf(buf,"\tlda #$%2.2x\n\tsta $2007\n",bgPal[palBank*16+i*4+j]);
			strcat(str,buf);
		}
	}

	Clipboard()->SetTextBuf(str);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutPaletteToClipboardASMClick(TObject *Sender)
{
	char str[1024],buf[1024];
	int i,j;

	//if(byte1->Checked) strcpy(str,".byte ");
	//if(db1->Checked)   strcpy(str,".db ");
	//sprintf(str,"palette:\n");
	AnsiString name;
	AnsiString bank;
	name=RemoveExt(ExtractFileName(SaveDialogSession->FileName));

	switch (palBank) {
	case 0: bank="a"; break;
	case 1: bank="b"; break;
	case 2: bank="c"; break;
	case 3: bank="d"; break;

	default: bank="a";
	}

	if((bExportPalFilename)&&(bExportPalSet))
		sprintf(str,"palette_%s_%s:\n",name.c_str(),bank.c_str());
	else if (bExportPalFilename)
		sprintf(str,"palette_%s:\n",name.c_str());
	else if (bExportPalSet)
		sprintf(str,"palette_%s:\n",bank.c_str());
	else
		sprintf(str,"palette:\n");





	for(i=0;i<4;i++)
	{
		if(byte1->Checked) sprintf(buf,".byte ");
		if(db1->Checked)   sprintf(buf,".db ");
        strcat(str,buf);
		for(j=0;j<4;j++)
		{
			//sprintf(buf,"$%2.2x%c",bgPal[palBank*16+i*4+j],i*4+j<15?',':'\n');
			sprintf(buf,"$%2.2x%c",bgPal[palBank*16+i*4+j],j<3?',':'\n');
			strcat(str,buf);
		}
	}

	Clipboard()->SetTextBuf(str);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutPaletteToClipboardCClick(TObject *Sender)
{
	char str[1024],buf[1024];
	int i,j;
	AnsiString name;
	AnsiString bank;
	name=RemoveExt(ExtractFileName(SaveDialogSession->FileName));

	switch (palBank) {
	case 0: bank="a"; break;
	case 1: bank="b"; break;
	case 2: bank="c"; break;
	case 3: bank="d"; break;

	default: bank="a";
	}

	
	//strcpy
	if((bExportPalFilename)&&(bExportPalSet))
		sprintf(str,"const unsigned char palette_%s_%s[16]={ ",name.c_str(),bank.c_str());
	else if (bExportPalFilename)
		sprintf(str,"const unsigned char palette_%s[16]={ ",name.c_str());
	else if (bExportPalSet)
		sprintf(str,"const unsigned char palette_%s[16]={ ",bank.c_str());
	else
		sprintf(str,"const unsigned char palette[16]={ ");

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			sprintf(buf,"0x%2.2x%c",bgPal[palBank*16+i*4+j],i*4+j<15?',':' ');

			strcat(str,buf);
		}
	}

	strcat(str,"};\n\n");

	Clipboard()->SetTextBuf(str);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormMouseWheel(TObject *Sender, TShiftState Shift,
int WheelDelta, TPoint &MousePos, bool &Handled)
{

	bool l=btnLine->Down;
	bool t=FormCHREditor->TimerScrollEvent->Enabled;
	bool bCtrl=Shift.Contains(ssCtrl)?true:false;
	//bool bShift=Shift.Contains(ssShift)?true:false;
	bool bAlt=Shift.Contains(ssAlt)?true:false;

	int tmpD=WheelDelta/2;
	int absl;
	if(l)
	{
		if(!bCtrl){
			if(bAlt)   //just alt pressed
			{
				if(tmpD<0) lineToolAlt++;
				else lineToolAlt--;
			}
			else if(t==false){     //nothing pressed
				lineToolAlt=0;
				lineToolX=0;
				lineToolY=0;
				if(tmpD<0) lineToolRoster--;
				else lineToolRoster++;

				for (int i = 0; i < 8; i++) {
					absl = abs(lineToolRoster);
					if (lineRosterEnable[absl]==false)
					{
						if(tmpD<0) lineToolRoster--;
						else lineToolRoster++;
					}
				}
				if (lineToolRoster > 8) lineToolRoster = -8;
				if (lineToolRoster < -8) lineToolRoster = 8;

                int tmpi = iLinePresetIndex;
				iLinePreset_rosterIndex[tmpi]=lineToolRoster;
			}
		}
		else  //ctrl is assumed
		{
           if(bAlt)   //ctrl + alt
			{
				if(tmpD<0) lineToolX++;
				else lineToolX--;
			}
			else     //just ctrl
			{
				if(tmpD<0) {lineToolY++; lineToolY_toggleperm++;}
				else 	   {lineToolY--; lineToolY_toggleperm--;}
			}
		}

		if(lineDrawing)	FormCHREditor->Line(Shift,lineDownX,lineDownY,main_tilesetScrollX,main_tilesetScrollY,1);
		//ImageTilesMouseMove(Sender,Shift,CHR_mmX,CHR_mmY);

		//FormLineDetails->Repaint();

		FormCHREditor->TimerScrollEvent->Enabled=true;
        cueUpdateMetasprite=true;
		cueUpdateNametable=true;
		cueUpdateTiles=true;
		cueCHRdraw=true;
	}
	else
	{


		if(WheelDelta<0)
		{
			if(Shift.Contains(ssCtrl)) palActive=(palActive-1)&3;
			else
			{
				//if(Shift.Contains(ssShift)) nameTableViewX+=1;
				if(Shift.Contains(ssShift)) nameTableViewX+=1;

				else nameTableViewY+=1;
			}

		}
		else
		{
			//bgPalCur=(bgPalCur+1)&3;
			if(Shift.Contains(ssCtrl)) palActive=(palActive+1)&3;
			else
			{
				if(Shift.Contains(ssShift)) nameTableViewX-=1;
				else nameTableViewY-=1;
				//UpdateNameTable(-1,-1,true);
			}
		}

		if(WheelDelta<-120&&(!Shift.Contains(ssCtrl)))
		{
			if(Shift.Contains(ssShift)) nameTableViewX+=7;
			else nameTableViewY+=7;
		}
		if(WheelDelta>120&&(!Shift.Contains(ssCtrl)))
		{
			if(Shift.Contains(ssShift)) nameTableViewX-=7;
			else nameTableViewY-=7;
		}
	
		pal_validate();
		CorrectView();

		DrawPalettes();
		//UpdateTiles(true);
		UpdateNameTable(-1,-1,true);
		cueUpdateTiles=true;
		cueUpdateMetasprite=true;
	}
	//UpdateStats();
	cueStats=true;
	Handled=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPutSelectedBlockToClipboardCRLEClick(
TObject *Sender)
{
	CopyMapCodeC(true);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MPutSelectionToMetaspriteClick(TObject *Sender)
{
	int i,j,x,y,w,h,sx,sy,tx,ty,ptr,spr,tile;
	bool bIsNT = false;
	int ox=64;      //middle of canvas
	int oy=64;
	bool is8x16 = SpeedButtonSprite8x16->Down;
	int ystep = is8x16? 16:8;
	int bank = SpeedButtonChrBank2->Down? 1:0;
	int start_selection;
	SetUndo();

	if(Anchor1->Checked)
		{
		ox=spriteGridX;
		oy=spriteGridY;
		}
	if(nameSelection.left>=0&&nameSelection.top>=0)
	{
		GetSelection(nameSelection,x,y,w,h);
		bIsNT = true;
	}
	else
	{
		GetSelection(chrSelection,x,y,w,h);
	}
		spr=0;
		ptr=metaSpriteActive*64*4;

		if(center2->Checked)	sy=oy-(h*8/2);
		if(top1->Checked)		sy=oy;
		if(bottom1->Checked)	sy=oy-(h*8);

		start_selection=MPutSelectionToMetaspriteMerge->Checked? ListBoxSpriteList->Items->Count:0;
		for(i=0;i<h;i++)
		{
			if(center1->Checked)	sx=ox-(w*8/2);
			if(left1->Checked)		sx=ox;
			if(right1->Checked)		sx=ox-(w*8);

			for(j=0;j<w;j++)
			{
				tx=x+j;
				ty=y+i;


				if (bIsNT) tile=nameTable[ty*nameTableWidth+tx];
				else 	   tile=tileViewTable[ty*16+tx];
				if(is8x16) tile=(tile & ~1)+bank;

				if(tile!=nullTile||!MPutSelectionToMetaspriteSkipZero->Checked)
				{
					if(MPutSelectionToMetaspriteMerge->Checked)
					{
						while(spr<64)
						{
							if(metaSprites[ptr]==255) break;
							ptr+=4;
							++spr;
						}
					}

					if(spr<64)
					{
						if(sy==255) metaSprites[ptr+0]=254; //safeguard against autodeleting sprite item
						else metaSprites[ptr+0]=sy;
						metaSprites[ptr+1]=tile;
						if (bIsNT)metaSprites[ptr+2]=AttrGet(tx,ty,false,false);
						else metaSprites[ptr+2]=palActive;
						metaSprites[ptr+3]=sx;

						ptr+=4;

						++spr;
					}
				}

				sx+=8;
			}

			if(is8x16) i++;
			sy+=ystep;
		}

		if(!MPutSelectionToMetaspriteMerge->Checked)
		{
			while(spr<64)
			{
				metaSprites[ptr]=255;
				ptr+=4;
				++spr;
			}
		}

		if(MPutSelectionToMetaspriteAutoInc->Checked) if(metaSpriteActive<255) ++metaSpriteActive;

		UpdateMetaSprite(true); //rebuild list so we can make selections.
		for(i=0; i<ListBoxSpriteList->Items->Count; ++i)                 ListBoxSpriteList->Selected[i]=false;
		for(i=start_selection; i<ListBoxSpriteList->Items->Count; ++i)   ListBoxSpriteList->Selected[i]=true;


		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	/*}
	else
	{
	   GetSelection(nameSelection,x,y,w,h);
	   LabelStats->Caption="WARNING: No metasprite was made:\nNeeds a selection on the map/nametable.";
	   StatusUpdateWaiter->Enabled=true;
	   holdStats=true;
	} */
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteAllClick(TObject *Sender)
{
	int i;

	for(i=0;i<ListBoxSpriteList->Items->Count;++i) ListBoxSpriteList->Selected[i]=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteNoneClick(TObject *Sender)
{
	int i;
    spriteActive=-1;
	for(i=0;i<ListBoxSpriteList->Items->Count;++i) ListBoxSpriteList->Selected[i]=false;

	ListBoxSpriteList->Invalidate();
    cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PaintBoxPalDragOver(TObject *Sender, TObject *Source,
int X, int Y, TDragState State, bool &Accept)
{
	//UpdateStats();
	cueStats=true;
	if(Source==PaintBoxPal) Accept=true; else Accept=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PaintBoxPalDragDrop(TObject *Sender, TObject *Source,
int X, int Y)
{
	int n,entry;

	n=check_palette_mouse(X,Y);

	if(n>=0&&!(n&0x100))
	{
		SetUndo();

		entry=(n&3)*4+(n>>4);

		if(!(entry&3)) entry=0;

		if(acceptPalDrag)
		{
			bgPal[palBank*16+(palDragVal&3)*4+(palDragVal>>4)]=bgPal[palBank*16+entry];
			acceptPalDrag=false;
		}
		bgPal[palBank*16+entry]=palDragColor;

		pal_validate();

		DrawPalettes();
		UpdateTiles(true);
        if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

		UpdateNameTable(-1,-1,true);
		FormNavigator->Draw(false,false,false);
		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MView2xClick(TObject *Sender)
{
	if((Sender==MView2x)&&(uiScale==2)) return;
	if((Sender==MView3x)&&(uiScale==3)) return;
	if((Sender==MView4x)&&(uiScale==4)) return;

	if(Sender==MView2x) uiScale=2;
	if(Sender==MView3x) uiScale=3;
	if(Sender==MView4x) uiScale=4;

	UpdateUIScale();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MReplaceTileClick(TObject *Sender)
{
	int i,j,x,y,w,h,off;

	FormNameOffset->Mode=1;
	FormNameOffset->ShowModal();

	if(FormNameOffset->MakeOffset)
	{
		if(FormNameOffset->SelectedOnly)
		{
			x=-1;

			if(nameSelection.left>=0&&nameSelection.top>=0)
			{
				x=nameSelection.left;
				y=nameSelection.top;
				w=nameSelection.right-nameSelection.left;
				h=nameSelection.bottom-nameSelection.top;
			}
		}
		else
		{
			x=0;
			y=0;
			w=nameTableWidth;
			h=nameTableHeight;
		}

		if(x>=0)
		{
			SetUndo();

			for(i=0;i<h;++i)
			{
				for(j=0;j<w;++j)
				{
					off=(y+i)*nameTableWidth+(x+j);

					if(nameTable[off]==FormNameOffset->From) nameTable[off]=FormNameOffset->Offset;
				}
			}

			UpdateNameTable(-1,-1,true);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::toggleCheckClick(TObject *Sender)
{
	((TMenuItem*)Sender)->Checked^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRFreqSortClick(TObject *Sender)
{
	//FG: this, along with the other remove/sort actions, were modified to toggle between do/undo for the sake of fluidity.
	if(MCHRFreqSort->Checked)
	{
		Undo();
		MCHRFreqSort->Checked=false;

		MCHRFreqSort->Caption="Sort by &frequency";
	}
	else
	{
		bool forceActive = ForceActiveTab1->Checked;
		bool sweepNT = IncludeNametables1->Checked;
		bool sweepMS = IncludeMetasprites1->Checked;

		int i,j,temp;
		static int sort[256],freq[256];
		static unsigned char chrt[256*16];
		static unsigned char propt[256];

		//int bank = curViewCHRbankCanvas2*256;
		int set=bankActive/16;

		SetUndo();

		memset(freq,0,sizeof(freq));

		if((forceActive)||!(sweepNT||sweepMS)) //if no sweep checkbox is set, also default to active.
		{
		   if(PageControlEditor->ActivePage==TabSheetSprite) {
                for(j=0;j<256*64*4;j+=4) if(metaSprites[j]!=0xFF) ++freq[metaSprites[j+1]];
			 }
		   else {
				for(i=0;i<nameTableHeight*nameTableWidth;++i) ++freq[nameTable[i]];
			}
		}
		else
		{
		   if(sweepNT) {
				for(i=0;i<nameTableHeight*nameTableWidth;++i) ++freq[nameTable[i]];
			}
		   if(sweepMS) {
				for(j=0;j<256*64*4;j+=4) if(metaSprites[j]!=0xFF) ++freq[metaSprites[j+1]];
			}
		}





		for(i=0;i<256;++i) sort[i]=i;

		for(i=0;i<256-1;++i)
		{
			for(j=0;j<256-i-1;++j)
			{
				if(freq[j]<freq[j+1])
				{
					temp=freq[j];
					freq[j]=freq[j+1];
					freq[j+1]=temp;

					temp=sort[j];
					sort[j]=sort[j+1];
					sort[j+1]=temp;
				}
			}
		}
		//repair NT
		for(i=0;i<nameTableWidth*nameTableHeight;++i)
		{
			for(j=0;j<256;++j)
			{
				if(sort[j]==nameTable[i])
				{
					nameTable[i]=j;
					break;
				}
			}
		}
		//repair MSP.
		for(int i=0;i<256*64*4;i+=4)
		{
			for(j=0;j<256;++j)
			{
				if(sort[j]==metaSprites[i+1]) {metaSprites[i+1]=j; break;}
			}
		}

		//perform chr sort & property sort
        for(i=0;i<256;++i)
		{
				memcpy(chrt+i*16,&chr[bankViewTable[set+i]+i*16],16);
				propt[i]=tileProperties[bankViewTable[set+i]/16+i];

		}
		//memcpy(chrt,&chr[bank],sizeof(chrt));
		//memcpy(propt,&tileProperties[bank],sizeof(propt));

		for(i=0;i<256;++i)
		{
			memcpy(&chr[bankViewTable[set+i] + i*16],&chrt[sort[i]*16],16);
			tileProperties[bankViewTable[set+i]+i]=propt[sort[i]];
			//memcpy(&tileProperties[bank+i],&propt[sort[i]],sizeof(uint32_t));
		}






		UpdateAll();
		MCHRFreqSort->Checked=true;
		MCHRFreqSort->Caption="Undo Sort by &frequency";
		LabelStats->Caption="Tiles sorted by Frequency. \n\nTip: Press again to undo.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;

	}



}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MNameTableNewClick(TObject *Sender)
{
	int x,y,pp,pal,mask;

	bool useNull;

		FormSetSize->ShowModal();
		if (!FormSetSize->Confirm) return;
		useNull = FormSetSize->CheckNullTile->Checked;

	
	//if(Application->MessageBox("Are you sure?","Confirm",MB_YESNO)!=IDYES) return;

	memcpy(tmpNameTable,nameTable,sizeof(nameTable));
	memcpy(tmpAttrTable,attrTable,sizeof(attrTable));

	SetUndo();

	int oldWdt=nameTableWidth;
	int oldHgt=nameTableHeight;


	nameTableWidth =FormSetSize->NewWidth;
	nameTableHeight=FormSetSize->NewHeight;


	int newWdt=nameTableWidth;
	int newHgt=nameTableHeight;

	bool isNarrow	= oldWdt > newWdt
					? true : false;

	bool isTall		= oldHgt < newHgt
					? true : false;

	ClearNametable(useNull);




	if(!FormSetSize->CheckBoxClear->Checked)
	{
		for(y=0;y<oldHgt;y++)
		{

			for(x=0;x<oldWdt;x++)
			{
				if(isNarrow && isTall) if (y==oldHgt-1 && x>=newWdt) continue;  //ignores an excess remainder on the following row.
				nameTable[y*newWdt+x]=tmpNameTable[y*oldWdt+x];

			}
		}

		for(y=0;y<oldHgt;y+=2)
		{
			for(x=0;x<oldWdt;x+=2)
			{
				if(isNarrow && isTall) if (y==oldHgt-1 && x>=newWdt) continue;
				pal=tmpAttrTable[y/4*((oldWdt+3)/4)+x/4];

				if(x&2) pal>>=2;
				if(y&2) pal>>=4;

				pp=y/4*((newWdt+3)/4)+x/4;

				mask=3;
				pal=pal&3;

				if(x&2)
				{
					pal<<=2;
					mask<<=2;
				}
				if(y&2)
				{
					pal<<=4;
					mask<<=4;
				}

				attrTable[pp]=(attrTable[pp]&(mask^255))|pal;
			}
		}

	}

	nameSelection.left=-1;
	nameSelection.top=-1;
	nameCopyWidth=-1;
	nameCopyHeight=-1;

	//SetUndo();
    if(FormNavigator->Visible) FormNavigator->Draw(false,true,false);
	UpdateAll();

	//if(FormNavigator->Height > GetSystemMetrics(SM_CYFULLSCREEN)) FormNavigator->Height  = GetSystemMetrics(SM_CYFULLSCREEN) + 10;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveMapClick(TObject *Sender)
{
	unsigned char meta[4];
	unsigned char* buf; //for RLE compressing maps.
	unsigned char* dst; //^
	bool bRLE;
	AnsiString name,filter;
	FILE *file;

	BlockDrawing(true);

	//
				
	//
	if(SaveDialogMap->Execute())
	{
		if(SaveDialogMap->FilterIndex==2||MSaveRLE->Checked) {bRLE=true; name=RemoveExt(SaveDialogMap->FileName)+".mrle";}
		else {bRLE=false; name=RemoveExt(SaveDialogMap->FileName)+".map";}

		if(FileExists(name))
		{
			if(Application->MessageBox(("File "+name+" already exists. Overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES)
			{
				BlockDrawing(false);
				return;
			}
		}

		meta[0]=nameTableWidth&255;
		meta[1]=nameTableWidth/256;
		meta[2]=nameTableHeight&255;
		meta[3]=nameTableHeight/256;

		file=fopen(name.c_str(),"wb");

		if(file)
		{
			//
			if(bRLE){
				buf=(unsigned char*)malloc(name_size()+attr_size());
				int size=0;

				if(MSaveIncName->Checked)
				{
					memcpy(buf,nameTable,name_size());
					size+=name_size();
				}

				if(MSaveIncAttr->Checked)
				{
					memcpy(buf+size,attrTable,attr_size());
					size+=attr_size();
				}
				//pack buf as RLE to dst
				dst=(unsigned char*)malloc(size*2);
				size=encode_rle(buf,size,dst);
				//handle if compression is impossible.
				if(size<0){
					Application->MessageBox("No unused values found (tiles and/or attributes), can't be saved as RLE due to limitations of the format.","Error",MB_OK);
					free(dst);
					free(buf);
					fclose(file);
					BlockDrawing(false);
					return;
				}
				//fwrite dst as RLE here
				fwrite(dst,size,1,file);
				free(dst);
				free(buf);
			}
			//
			else{
				fwrite(nameTable,name_size(),1,file);
				fwrite(attrTable,attr_size(),1,file);
			}
			fwrite(meta,sizeof(meta),1,file);
			fclose(file);
		}
	
    }
	BlockDrawing(false);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SpeedButtonSpriteDupClick(TObject *Sender)
{
	int i,j,len,src_ptr,dst_ptr;


	
	dst_ptr=metaSpriteActive*64*4;
	len=ListBoxSpriteList->Items->Count;

	if(len)SetUndo();

	for(i=0;i<len;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			src_ptr=metaSpriteActive*64*4+i*4;

			while(1)
			{
				if(metaSprites[dst_ptr]==255) break;

				dst_ptr+=4;
			}

			if(dst_ptr<metaSpriteActive*64*4+64*4)
			{
				for(j=0;j<4;++j) metaSprites[dst_ptr+j]=metaSprites[src_ptr+j];
				dst_ptr+=4;
			}
		}
	}

	spriteActive=len;

	//UpdateMetaSprite();
	//cueUpdateMetasprite=true;
	UpdateMetaSprite(true);
	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		ListBoxSpriteList->Selected[i]=(i<len)?false:true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPalBankAClick(TObject *Sender)
{
	MPalA->Checked=false;
	MPalB->Checked=false;
	MPalC->Checked=false;
	MPalD->Checked=false;

	if(SpeedButtonPalBankA->Down) {palBank=0; MPalA->Checked=true;}
	if(SpeedButtonPalBankB->Down) {palBank=1; MPalB->Checked=true;}
	if(SpeedButtonPalBankC->Down) {palBank=2; MPalC->Checked=true;}
	if(SpeedButtonPalBankD->Down) {palBank=3; MPalD->Checked=true;}

	ppuMask=ppuMaskSet[palBank];
	UpdateRGBM();
	pal_validate();
    palette_calc();

	DrawPalettes();
	UpdateTiles(true);
    if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MCHRDensitySortClick(TObject *Sender)
{
	int set=bankActive/16;
	if(MCHRDensitySort->Checked)
	{
		Undo();
		MCHRDensitySort->Checked=false;
		MCHRDensitySort->Caption="Sort by d&ensity";
	}
	else
	{
		 int i,j,k,ptr,mask,temp;
		static int sort[256],freq[256];
		static unsigned char chrt[256*16];
		static unsigned char propt[256];
		SetUndo();

		for(i=0;i<256;++i)
		{
			sort[i]=i;
			freq[i]=0;

			ptr=i*16+bankViewTable[set+i];

			for(j=0;j<8;++j)
			{
				mask=128;

				for(k=0;k<8;++k)
				{
					if((chr[ptr]&mask)||(chr[ptr+8]&mask)) ++freq[i];

					mask>>=1;
				}

				++ptr;
			}
		}

		for(i=0;i<256-1;++i)
		{
			for(j=0;j<256-i-1;++j)
			{
				if(freq[j]<freq[j+1])
				{
					temp=freq[j];
					freq[j]=freq[j+1];
					freq[j+1]=temp;

					temp=sort[j];
					sort[j]=sort[j+1];
					sort[j+1]=temp;
				}
			}
		}
		//repair NT
		for(i=0;i<nameTableWidth*nameTableHeight;++i)
		{
			for(j=0;j<256;++j)
			{
				if(sort[j]==nameTable[i])
				{
					nameTable[i]=j;
					break;
				}
			}
		}
		//repair MSP.
		for(int i=0;i<256*64*4;i+=4)
		{
			for(j=0;j<256;++j)
			{
				if(sort[j]==metaSprites[i+1]) {metaSprites[i+1]=j; break;}
			}
		}

		for(i=0;i<256;++i)
		{
				memcpy(chrt+i*16,&chr[bankViewTable[set+i]+i*16],16);
				propt[i]=tileProperties[bankViewTable[set+i]/16+i];

		}
		//memcpy(chrt,&chr[bankActive],sizeof(chrt));
		//memcpy(propt,&tileProperties[bankActive/16],sizeof(propt));
		for(i=0;i<256;++i)
		{

            memcpy(&chr[bankViewTable[set+i] + i*16],&chrt[sort[i]*16],16);
			tileProperties[bankViewTable[set+i]+i]=propt[sort[i]];
			
			//memcpy(&chr[bankActive+i*16],&chrt[sort[i]*16],16);
			//memcpy(&tileProperties[(bankActive/16)+i],&propt[sort[i]],sizeof(uint32_t));
		}

		UpdateAll();
		MCHRDensitySort->Checked=true;
		MCHRDensitySort->Caption="Undo Sort by d&ensity";
		LabelStats->Caption="Tiles sorted by Density. \n\nTip: Press again to undo.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSelectTilesFromMapClick(TObject *Sender)
{
	int i,j,x,y,w,h,sx,sy,tx,ty,ptr,spr,tile;

	if(nameSelection.left<0||nameSelection.top<0) return;

	chrSelectRect=false;

	for(i=0;i<256;i++) chrSelected[i]=false;

	GetSelection(nameSelection,x,y,w,h);

	for(i=0;i<h;i++)
	{
		for(j=0;j<w;j++)
		{
			tx=x+j;
			ty=y+i;

			tile=nameTable[ty*nameTableWidth+tx];


			chrSelected[ViewTablePos(tile)]=true;
		}
	}
    bOutsideSel=true;
	nameSelection.left=-1;
	nameSelection.top=-1;

	UpdateAll();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SpeedButtonMetaSpriteRotateClick(TObject *Sender)
{
	int i,x,y,off;

	SetUndo();
	
	off=metaSpriteActive*64*4;

	for(i=0;i<64;++i)
	{
		if(metaSprites[off]==255) break;

		y=-8-(metaSprites[off+3]-spriteGridX);
		x=(metaSprites[off+0]-spriteGridY);

		metaSprites[off+0]=spriteGridY+y;
		metaSprites[off+3]=spriteGridX+x;
		//metaSprites[off+2]^=(OAM_FLIP_H|OAM_FLIP_V);

		off+=4;
	}

	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MMetaSpriteManageClick(TObject *Sender)
{
	FormManageMetasprites->Visible^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MAddTileOffsetMetaSpritesClick(TObject *Sender)
{
	int i,j,off;

	FormNameOffset->Mode=0;
	FormNameOffset->ShowModal();

	if(FormNameOffset->MakeOffset)
	{
		SetUndo();

		for(i=0;i<256;++i)
		{
			for(j=0;j<64;++j)
			{
				off=i*64*4+j*4;

				if(metaSprites[off]<255) metaSprites[off+1]+=FormNameOffset->Offset;
			}
		}

		//UpdateMetaSprite();
		cueUpdateMetasprite=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MImportBMPMatchTilesClick(TObject *Sender)
{
	BlockDrawing(true);
	
	OpenDialogImport->Title="Import BMP to canvas; matched to existing tileset";
	OpenDialogImport->Filter="Windows bitmap files (*.bmp)|*.bmp|All files (*.*)|*.*";
	OpenDialogImport->DefaultExt="bmp";

	if(OpenDialogImport->Execute())
	{
		SetUndo();

		if(ImportBMP(OpenDialogImport->FileName,IMPORT_MATCH,0,0,256,false,false))
		{
			UpdateAll();
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRGenerate4x4Click(TObject *Sender)
{
	int i,j,k,bit,c1,c2,pp;

	//if(Application->MessageBox("Do you really want to fill 4K CHR with chunks?","Confirm",MB_YESNO)==ID_YES)
	//{
		SetUndo();
		int set=bankActive/16;

		for(i=0;i<16;i++)
		{
			for(j=0;j<16;j++)
			{
				pp=bankViewTable[set+j+i*16]+j*16 +i*256;

				bit=j|(i<<4);

				for(k=0;k<8;k++)
				{
					c1=0;
					c2=0;

					if(k<4)
					{
						if(bit&0x1) c1|=0x0f;
						if(bit&0x2) c2|=0x0f;
						if(bit&0x4) c1|=0xf0;
						if(bit&0x8) c2|=0xf0;
					}
					else
					{
						if(bit&0x10) c1|=0x0f;
						if(bit&0x20) c2|=0x0f;
						if(bit&0x40) c1|=0xf0;
						if(bit&0x80) c2|=0xf0;
					}

					chr[pp+0+k]=c1;
					chr[pp+8+k]=c2;
				}

				//pp+=16;
			}
		}
		  /*
		for(i=0;i<16;i++)
		{
			for(j=0;j<16;j++)
			{
				bit=j|(i<<4);

				for(k=0;k<8;++k)
				{
					c1=0;
					c2=0;

					if(k<4)
					{
						if(bit&0x01) c1|=0x03;
						if(bit&0x02) c1|=0x0c;
						if(bit&0x04) c1|=0x30;
						if(bit&0x08) c1|=0xc0;
					}
					else
					{
						if(bit&0x10) c1|=0x03;
						if(bit&0x20) c1|=0x0c;
						if(bit&0x40) c1|=0x30;
						if(bit&0x80) c1|=0xc0;
					}

					switch(i/4)
					{
					case 0: break;
					case 1: c2=c1; c1=0; break;
					case 2: c2=c1; c1=0xff; break;
					case 3: c2=0xff; break;
					}

					chr[pp+0+k]=c1;
					chr[pp+8+k]=c2;
				}

				pp+=16;
			}
		} */
		//memcpy(&chr[4096],chr,4096);
	  UpdateAll();
	  /*
	  UpdateTiles(true);
		//WorkCHRToBankCHR();
		UpdateNameTable(-1,-1,true);
		FormNavigator->Draw(false,false);
		cueUpdateMetasprite=true;
    	*/
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MImportBMPIntoNumberOfTilesClick(TObject *Sender)
{
	int mode;

	FormInputNumber->ShowModal();

	if(!FormInputNumber->Confirm) return;

	OpenDialogImport->Title="Import BMP file to canvas";
	OpenDialogImport->Filter="Windows bitmap files (*.bmp)|*.bmp|All files (*.*)|*.*";
	OpenDialogImport->DefaultExt="bmp";

	BlockDrawing(true);

	if(OpenDialogImport->Execute())
	{
		SetUndo();

		if(ImportBMP(OpenDialogImport->FileName,IMPORT_SQUEEZE,false,-1,FormInputNumber->Number,MImportNoColorData->Checked,MImportNoColorData->Checked))
		{
			UpdateAll();
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPaletteResetGrayscaleClick(TObject *Sender)
{
	//if(Application->MessageBox("Reset palette?","Confirm",MB_YESNO)!=IDYES) return;
	SetUndo();
	memcpy(&bgPal[palBank*16],&bgPalDefault[((TMenuItem*)Sender)->Tag*16],16);

	DrawPalettes();
	UpdateTiles(true);
    if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPaletteCopyClick(TObject *Sender)
{
	PaletteCopy();	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MPalettePasteClick(TObject *Sender)
{
	PalettePaste();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TimerBlockTimer(TObject *Sender)
{
	BlockDrawingFlag=false;
	TimerBlock->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave1KTile00Click(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0x00*16,1024,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave1KTile40Click(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0x40*16,1024,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave1KTile80Click(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0x80*16,1024,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave1KTileC0Click(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0xC0*16,1024,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave2KTile00Click(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0x00*16,2048,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCHRSave2KTile80Click(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0x80*16,2048,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MAddXYOffsetMetaSpritesClick(TObject *Sender)
{
	int i,j,off,from,to;

	FormMetaspriteOffset->ShowModal();

	if(FormMetaspriteOffset->MakeOffset)
	{
		SetUndo();

		if(FormMetaspriteOffset->SelectedOnly)
		{
			from=metaSpriteActive;
			to=from+1;
		}
		else
		{
			from=0;
			to=256;
		}

		for(i=from;i<to;++i)
		{
			off=i*4*64;

			for(j=0;j<64;++j)
			{
				if(metaSprites[off]<255)
				{
					metaSprites[off+3]+=FormMetaspriteOffset->OffsetX;
					metaSprites[off+0]+=FormMetaspriteOffset->OffsetY;

					off+=4;
				}
			}
		}

		//UpdateMetaSprite();
    	cueUpdateMetasprite=true;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::UndoRedo1Click(TObject *Sender)
{
	Undo();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Cut1Click(TObject *Sender)
{

	int i,off;
	AnsiString tmp;
    extern bool bMetaManagerEditLabelActive;
	if(FormCHRbit->Active==true)
	{
		if(Sender==Copy1)  FormCHRbit->HexToClip();
		if(Sender==Paste1) FormCHRbit->ClipToHexEdit();
	}
	else if(FormMTprops->Active==true)
	{
		if(Sender==Copy1)FormMTprops->HexToClip();
		if(Sender==Paste1) FormMTprops->ClipToHexEdit();
	}
	else if(FormEmphasisPalette->Active==true)
	{
		if(Sender==Copy1)FormEmphasisPalette->HexToClip();
		if(Sender==Paste1) FormEmphasisPalette->ClipToHexEdit();
	}

	else if(FormCHREditor->Active==true)
	{
		if(Sender==Cut1) {CopyCHR(true,true); tmp="CHR";}
		if(Sender==Copy1) CopyCHR(true,false);
		if(Sender==Paste1) PasteCHR(0);
	}
	else if((FormManageMetasprites->Active==true) && (bMetaManagerEditLabelActive==true))
	{
			if(Sender==Cut1)  FormManageMetasprites->MetaLabelToClip(true);
			if(Sender==Copy1) FormManageMetasprites->MetaLabelToClip(false);
			if(Sender==Paste1) FormManageMetasprites->ClipToMetaLabel();
	}
	else if((FormMetatileTool->Active==true))
	{
		if(Sender==Cut1) {CopyMetatiles(true,false); tmp="Metatiles";}  //tmp redundant, message is handled in-routine and given priority through the holdStats global bool.
		if(Sender==Copy1) CopyMetatiles(false,false);
		if(Sender==Paste1) PasteMetatiles(false);
	}
	else
	{
		if(PageControlEditor->ActivePage==TabSheetSprite&&isLastClickedMetaSprite)
		{
			if(Sender==Cut1)
			{
                    SetUndo();
					MetaSpriteCopy();

					off=metaSpriteActive*64*4;
					for(i=0;i<64*4;i+=4) metaSprites[off+i]=255;
					//UpdateMetaSprite();
					cueUpdateMetasprite=true;
					tmp="MetaSprite #"+num_to_3char(metaSpriteActive);
				}
				if(Sender==Copy1) MetaSpriteCopy() ;
				if(Sender==Paste1) MetaSpritePaste();

			}

		else if(PageControlEditor->ActivePage==TabSheetName||PageControlEditor->ActivePage==TabSheetTile
		||(PageControlEditor->ActivePage==TabSheetSprite&&!isLastClickedMetaSprite))
		{
			if(nameSelection.left<0&&nameSelection.top<0)
			{
				if(Sender==Cut1) {CopyCHR(true,true); tmp="CHR";}
				if(Sender==Copy1) CopyCHR(true,false);
				if(Sender==Paste1) PasteCHR(0);
			}
			else
			{
				if(Sender==Cut1) {CopyMap(true); tmp="Name(s)";}
				if(Sender==Copy1) CopyMap(false);
				if(Sender==Paste1) PasteMap();
			}
			if(PageControlEditor->ActivePage==TabSheetTile) UpdateNameTable(-1,-1,true);
		}
	}
    
	if ((Sender==Cut1)&&(holdStats==false))
	{
	LabelStats->Caption=LabelStats->Caption
		+"\n"
		+tmp
		+" was/were cut.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Quit1Click(TObject *Sender)
{
	FormMain->Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Tilegrid1Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	if(Sender==GridOnOff1||Sender==GridOnOff2)
	{
		SpeedButtonGridAll->Down^=true;
		GridOnOff1->Checked=SpeedButtonGridAll->Down;
		GridOnOff2->Checked=SpeedButtonGridAll->Down;
	}
	if(Sender==Screen32x301) {SpeedButtonGridScreen->Down^=true;
							Screen32x301->Checked=SpeedButtonGridScreen->Down;}

	if(Sender==Tilegrid1) {SpeedButtonGridTile->Down^=true;
							Tilegrid1->Checked=SpeedButtonGridTile->Down;}
	if(Sender==Tilegrid2) {SpeedButtonGridAtr->Down^=true;
							Tilegrid2->Checked=SpeedButtonGridAtr->Down;}
	if(Sender==Tilegrid4) {SpeedButtonGridBlock->Down^=true;
							Tilegrid4->Checked=SpeedButtonGridBlock->Down;}

	//((TMenuItem*)Sender)->Checked^=true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	UpdateTiles(true);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Colour0Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	Colour001->Checked=false;
	Colour011->Checked=false;
	Colour101->Checked=false;
	Colour111->Checked=false;

	if((bgPalCurOld!=bgPalCur)&&(AutostoreLastUsed->Checked)) bgPalCurOld=bgPalCur;
	if(Sender==Colour001) {
		bgPalCur=0;

	}
	if(Sender==Colour011) {
		bgPalCur=1;

	}
	if(Sender==Colour101) {
		bgPalCur=2;

	}
	if(Sender==Colour111) {
		bgPalCur=3;

	}

	penActive=0;
	PenMode1->Checked=false;
	PenMode2->Checked=false;
	((TMenuItem*)Sender)->Checked^=true;
	//FormMain->
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::pal0Click(TObject *Sender)
{

	Pal0->Checked=false;
	Pal1->Checked=false;
	Pal2->Checked=false;
	Pal3->Checked=false;

	if((palActiveOld!=palActive)&&(AutostoreLastUsed->Checked)) palActiveOld=palActive;
	if(Sender==Pal0) {
		palActive=0;

	}
	if(Sender==Pal1) {
		palActive=1;


	}
	if(Sender==Pal2) {
		palActive=2;

	}
	if(Sender==Pal3) {
		palActive=3;
	
	}

	((TMenuItem*)Sender)->Checked^=true;

	FormMain->UpdateAll();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::PenModeClick(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;


	if(Sender==PenMode1) {
		penActive=1;
		LabelStats->Caption="Special ink: Increment/'brighten'.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}
	if(Sender==PenMode2) {
		penActive=2;
		LabelStats->Caption="Special ink: Decrement/'darken'.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}
	((TMenuItem*)Sender)->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SafeColoursClick(TObject *Sender)
{
	((TMenuItem*)Sender)->Checked^=true;
	safeColsOnly^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SharedBGcolClick(TObject *Sender)
{
	((TMenuItem*)Sender)->Checked^=true;
	sharedCol0^=true;
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Magnify1Click(TObject *Sender)
{
	//not in use or finished.. decided to wait
	/*
	system("magnify /lens")
	else
	{
	system("taskkill /F /IM "magnify.exe" /T")
	}
	*/
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ApplyTiles1Click(TObject *Sender)
{
   if (FormCHREditor->Active) return;
   if(SpeedButtonTypeIn->Down) return;

   //ApplyTiles1Click->Checked^=true;
   ((TMenuItem*)Sender)->Checked^=true;
   SpeedButtonTiles->Down^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ApplyAttributes1Click(TObject *Sender)
{
   if (FormCHREditor->Active) return;
   if(SpeedButtonTypeIn->Down) return;

   ((TMenuItem*)Sender)->Checked^=true;
   SpeedButtonPal->Down^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Toggletileset1Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	//These conditionals are necessary to bypass doubling up
	//on the speedbuttons behaving like radio buttons per default for this group

	int start= -1;
	if(SpeedButtonChrBank1->Down==true) start=0;
	if(SpeedButtonChrBank2->Down==true) start=1;
	if(SpeedButtonChrBank3->Down==true) start=2;
	if(SpeedButtonChrBank4->Down==true) start=3;

	int cnt=0;
	if(SpeedButtonChrBank1->Flat==true) cnt++;
	if(SpeedButtonChrBank2->Flat==true) cnt++;
	if(SpeedButtonChrBank3->Flat==true) cnt++;
	if(SpeedButtonChrBank4->Flat==true) cnt++;

	if(cnt==4){
		//reset all to nonflat. (signifying they're in the x switch roster)
		SpeedButtonChrBank1->Flat=false;
		SpeedButtonChrBank2->Flat=false;
		SpeedButtonChrBank3->Flat=false;
		SpeedButtonChrBank4->Flat=false;

	}
	else if(cnt==3){
		//auto set the one after the one that is set.
		if     (SpeedButtonChrBank1->Flat==false) SpeedButtonChrBank2->Flat=false;
		else if(SpeedButtonChrBank2->Flat==false) SpeedButtonChrBank3->Flat=false;
		else if(SpeedButtonChrBank3->Flat==false) SpeedButtonChrBank4->Flat=false;
		else if(SpeedButtonChrBank4->Flat==false) SpeedButtonChrBank1->Flat=false;
	}

	if(start == -1){   //noone was checked (this should not ever happen), set view A.
		SpeedButtonChrBank1->Down=true;
	}
	else{
		if(start==0){
			if     (SpeedButtonChrBank2->Flat==false) SpeedButtonChrBank2->Down=true;
			else if(SpeedButtonChrBank3->Flat==false) SpeedButtonChrBank3->Down=true;
			else if(SpeedButtonChrBank4->Flat==false) SpeedButtonChrBank4->Down=true;
		}
		if(start==1){
			if     (SpeedButtonChrBank3->Flat==false) SpeedButtonChrBank3->Down=true;
			else if(SpeedButtonChrBank4->Flat==false) SpeedButtonChrBank4->Down=true;
			else if(SpeedButtonChrBank1->Flat==false) SpeedButtonChrBank1->Down=true;
		}
		if(start==2){
			if     (SpeedButtonChrBank4->Flat==false) SpeedButtonChrBank4->Down=true;
			else if(SpeedButtonChrBank1->Flat==false) SpeedButtonChrBank1->Down=true;
			else if(SpeedButtonChrBank2->Flat==false) SpeedButtonChrBank2->Down=true;
		}
		if(start==3){
			if     (SpeedButtonChrBank1->Flat==false) SpeedButtonChrBank1->Down=true;
			else if(SpeedButtonChrBank2->Flat==false) SpeedButtonChrBank2->Down=true;
			else if(SpeedButtonChrBank3->Flat==false) SpeedButtonChrBank3->Down=true;
		}
	}

	if(SpeedButtonChrBank1->Down==true) bankActive=0*4096;
	if(SpeedButtonChrBank2->Down==true) bankActive=1*4096;
	if(SpeedButtonChrBank3->Down==true) bankActive=2*4096;
	if(SpeedButtonChrBank4->Down==true) bankActive=3*4096;

	//old
	//if(SpeedButtonChrBank1->Down==true)SpeedButtonChrBank2->Down=true;
	//else SpeedButtonChrBank1->Down=true;

	//bankActive=SpeedButtonChrBank1->Down?0:4096;



	UpdateTiles(true);
    if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Attributes1Click(TObject *Sender)
{
	if (FormCHREditor->Active) return;
	if(SpeedButtonTypeIn->Down) return;
	((TMenuItem*)Sender)->Checked^=true;
	SpeedButtonChecker->Down^=true;
	SpeedButtonCheckerClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SelectedOnly1Click(TObject *Sender)
{

	if (FormCHREditor->Active) return;
 if(SpeedButtonTypeIn->Down) return;
 SpeedButtonSelTiles->Down^=true;
 SelectedOnly1->Checked=SpeedButtonSelTiles->Down;
 UpdateNameTable(-1,-1,true);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MASCIIneg20hClick(TObject *Sender)
{
   MASCIIneg20h->Checked=false;
   MASCIIneg30h->Checked=false;
   MASCIIneg40h->Checked=false;

   if(Sender==MASCIIneg20h)	typeInASCIIOffset=32;
   if(Sender==MASCIIneg30h)	typeInASCIIOffset=48;
   if(Sender==MASCIIneg40h)	typeInASCIIOffset=64;

   ((TMenuItem*)Sender)->Checked^=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MPalAClick(TObject *Sender)
{
	/*
	MPalA->Checked=false;
	MPalB->Checked=false;
	MPalC->Checked=false;
	MPalD->Checked=false;
	*/
	SpeedButtonPalBankA->Down=false;
	SpeedButtonPalBankB->Down=false;
	SpeedButtonPalBankC->Down=false;
	SpeedButtonPalBankD->Down=false;

	if(Sender==MPalA) {palBank=0; SpeedButtonPalBankA->Down=true;}
	if(Sender==MPalB) {palBank=1; SpeedButtonPalBankB->Down=true;}
	if(Sender==MPalC) {palBank=2; SpeedButtonPalBankC->Down=true;}
	if(Sender==MPalD) {palBank=3; SpeedButtonPalBankD->Down=true;}

	((TMenuItem*)Sender)->Checked^=true;

	pal_validate();
	DrawPalettes();
	UpdateTiles(true);
	if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------



void __fastcall TFormMain::PutcurrenttableonclipboardasBMP1Click(
	  TObject *Sender)
{
	TPicture *picture;
	Graphics::TBitmap *bmp;
	int i,x,y;
	unsigned short picFormat;
	unsigned int cbHandle;
	//AnsiString str;

	BlockDrawing(true);

	picture=new TPicture();
	bmp=new Graphics::TBitmap();
	bmp->SetSize(128,128);


	bmp->PixelFormat=pf4bit;
	SetBMPPalette(bmp);
	picture->Bitmap=bmp;


	x=0;
	y=0;

	for(i=0;i<256;i++)
	{
		DrawExportTile16(picture,x,y,i,palActive,false);

		x+=8;

		if(x>=128)
		{
			x=0;
			y+=8;
		}
	}



	HPALETTE hPal;

	picture->SaveToClipboardFormat(picFormat, cbHandle, hPal);
	Clipboard()->SetAsHandle(picFormat, cbHandle);

	delete bmp;
	delete picture;

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MCopyMapAsBMP(TObject *Sender)
{
TPicture *picture;
	Graphics::TBitmap *bmp;
	int i,j,x,y,w,h,pp;
	int xOff=0;
	int yOff=0;


	unsigned short picFormat;
	unsigned int cbHandle;


	if(nameSelection.left>=0&&nameSelection.top>=0&&(Sender!=CopymapasBMP1))
	{
		GetSelection(nameSelection,x,y,w,h);

		BlockDrawing(true);

		picture=new TPicture();
		bmp=new Graphics::TBitmap();
		bmp->SetSize(w*8,h*8);
		bmp->PixelFormat=pf4bit;
		SetBMPPalette(bmp);
		picture->Bitmap=bmp;

		pp=0;

		for(i=0;i<h;++i)
		{
			for(j=0;j<w;++j)
			{
				nameCopy[pp]=nameTable[y*nameTableWidth+x+j];
				attrCopy[pp]=AttrGet(x+j,y,false,false);
				DrawExportTile16(
					picture,
					j*8,
					i*8,
					nameCopy[pp],
					attrCopy[pp],
					false
					);
			++pp;
		}
		++y;
	}

	}
	else
	{
		w=nameTableWidth;
		h=nameTableHeight;

        BlockDrawing(true);

		picture=new TPicture();
		bmp=new Graphics::TBitmap();
		bmp->SetSize((w/*-xOff*/)*8,(h/*-yOff*/)*8);
		bmp->PixelFormat=pf4bit;
		SetBMPPalette(bmp);
		picture->Bitmap=bmp;

		y=0;

		for(i=yOff;i<h;i++)
		{
			x=0;

			for(j=xOff;j<w;j++)
			{
				DrawExportTile16(
					picture,
					x,
					y,
					nameTable[i*w+j],
					AttrGet(j,i,false,false),
					false
					);
				x+=8;
			}
		y+=8;
		}
	}

	HPALETTE hPal;

	picture->SaveToClipboardFormat(picFormat, cbHandle, hPal);
	Clipboard()->SetAsHandle(picFormat, cbHandle);

	delete bmp;
	delete picture;


	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
	  if(Shift.Contains(ssLeft))
	  {
		prevMouseY=Y;
		prevMouseX=X;
	  }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Red1Click(TObject *Sender)
{
	if(Sender==Red1) {
		SpeedButtonMaskRClick(Sender);
		}
	if(Sender==Blue1) {
		SpeedButtonMaskBClick(Sender);
		}
	if(Sender==Green1) {
		SpeedButtonMaskGClick(Sender);
		}
	if(Sender==Gray1) {
		SpeedButtonMaskMClick(Sender);
		}

	Red1->Checked=(ppuMask&0x20?true:false);
	Green1->Checked=(ppuMask&0x40?true:false);
	Blue1->Checked=(ppuMask&0x80?true:false);

	Gray1->Checked=(ppuMask&0x01?true:false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PerClickOrCont(TObject *Sender)
{

	IncDecPerclick1->Checked^=true;
	OverDistance1->Checked^=true;

	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::IncDecCap1Click(TObject *Sender)
{
	 IncDecCap1->Checked^=true;
	 IncDecWraparound1->Checked^=true;
}
//--------------------------------------------------------------------------


void __fastcall TFormMain::IncDecFlow1Click(TObject *Sender)
{
	//IncDecPerclick1->Checked=false;
	IncDecFlow1->Checked=false;
	IncDecFlow2->Checked=false;
	IncDecFlow3->Checked=false;
	IncDecFlow4->Checked=false;
	IncDecFlow5->Checked=false;

	((TMenuItem*)Sender)->Checked=true;

	if(Sender==IncDecFlow1) {continousIncDecDuration=1;}
	if(Sender==IncDecFlow2) {continousIncDecDuration=3;}
	if(Sender==IncDecFlow3) {continousIncDecDuration=5;}
	if(Sender==IncDecFlow4) {continousIncDecDuration=7;}
	if(Sender==IncDecFlow5) {continousIncDecDuration=9;}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Setcheckpoint1Click(TObject *Sender)
{
	size_t tmpSize;


	for(int i=0;i<256;i++)
	{
		metaSpriteNamesCheckpoint[i]	=metaSpriteNames[i];
		metaSpriteNamesCheckpointUndo[i]=metaSpriteNamesUndo[i];

		checkpoint_collisionIDlabel[i]		=collisionIDlabel[i];
		undo_checkpoint_collisionIDlabel[i]	=undo_collisionIDlabel[i];

	}
    for(int i=0;i<14;i++){
		memcpy(ptr_checkpoint_tableBrush[i],ptr_tableBrush[i],256*sizeof(int));
		memcpy(ptr_undo_checkpoint_tableBrush[i],ptr_undo_tableBrush[i],256*sizeof(int));

	}
     

	///put current state in checkpoint buffer
	memcpy(bgPalCheckpoint,bgPal,sizeof(bgPal));



	memcpy(checkpointNameTable,nameTable,name_size());
	memcpy(checkpointAttrTable,attrTable,attr_size());
	memcpy(checkpointMetaSprites,metaSprites,sizeof(checkpointMetaSprites));

	memcpy(checkpointTileProperties,tileProperties,sizeof(checkpointTileProperties));
	memcpy(checkpointTileProperties_LinkBuf,tileProperties_LinkBuf,sizeof(checkpointTileProperties_LinkBuf));

	memcpy(checkpointPropConditional,propConditional,sizeof(checkpointPropConditional));
	memcpy(checkpointPropCondition,propCondition,sizeof(checkpointPropCondition));

	checkpointNameTableWidth   =  nameTableWidth;
	checkpointNameTableHeight  =  nameTableHeight;

	checkpointSpriteGridX	   =  spriteGridX;
	checkpointSpriteGridY      =  spriteGridY;


	memcpy(checkpoint_bankViewTable,bankViewTable,sizeof(checkpoint_bankViewTable));
	memcpy(undo_checkpoint_bankViewTable,undo_bankViewTable,sizeof(undo_checkpoint_bankViewTable));

	//chr bank stuff pt1


	tmpSize = chrBanks*4096;
	if (tmpSize > checkpoint_chrBanks*4096) {
		checkpointChr = (char*) realloc(checkpointChr, tmpSize);
		checkpointTileProperties = (char*) realloc(checkpointTileProperties, tmpSize/16);
	}

	memcpy(checkpointChr, chr, tmpSize);
	memcpy(checkpointTileProperties, tileProperties, tmpSize/16);

	tmpSize = chrBanks_LinkBuf*4096;
	if (tmpSize > checkpoint_chrBanks_LinkBuf*4096){
		checkpointChr_LinkBuf = (char*) realloc(checkpointChr_LinkBuf, tmpSize);
		checkpointTileProperties_LinkBuf = (char*) realloc(checkpointTileProperties_LinkBuf, tmpSize/16);
	}

	memcpy(checkpointChr_LinkBuf,chr_LinkBuf,tmpSize);
	memcpy(checkpointTileProperties_LinkBuf,tileProperties_LinkBuf,tmpSize/16);

	checkpoint_chrBanks = chrBanks;
	checkpoint_chrBankLabels = chrBankLabels;

	checkpoint_chrBanks_LinkBuf = chrBanks_LinkBuf;
	checkpoint_chrBankLabels_LinkBuf = chrBankLabels_LinkBuf;

	//metatiles pt1
	tmpSize		 = metatileSets_2x2*META_2x2;
	if(tmpSize > (unsigned)checkpoint_metatileSets_2x2*META_2x2) {
		checkpoint_metatileSet_2x2_id = (char*) realloc(checkpoint_metatileSet_2x2_id, tmpSize);
		checkpoint_metatileSet_2x2_pal = (char*) realloc(checkpoint_metatileSet_2x2_pal, tmpSize);
		checkpoint_metatileSet_2x2_props = (char*) realloc(checkpoint_metatileSet_2x2_props, tmpSize);
	}


	memcpy(checkpoint_metatileSet_2x2_id, metatileSet_2x2_id, tmpSize);
	memcpy(checkpoint_metatileSet_2x2_pal, metatileSet_2x2_pal, tmpSize);
	memcpy(checkpoint_metatileSet_2x2_props, metatileSet_2x2_props, tmpSize);

	checkpoint_metatileSets_2x2 = metatileSets_2x2;

	tmpSize		 = metatileSets_4x4*META_4x4;

	if(tmpSize > (unsigned)checkpoint_metatileSets_4x4*META_4x4) {
		checkpoint_metatileSet_4x4_id = (char*) realloc(checkpoint_metatileSet_4x4_id, tmpSize);
		checkpoint_metatileSet_4x4_pal = (char*) realloc(checkpoint_metatileSet_4x4_pal, tmpSize);
		checkpoint_metatileSet_4x4_props = (char*) realloc(checkpoint_metatileSet_4x4_props, tmpSize);
	}

	memcpy(checkpoint_metatileSet_4x4_id, metatileSet_4x4_id, tmpSize);
	memcpy(checkpoint_metatileSet_4x4_pal, metatileSet_4x4_pal, tmpSize);
	memcpy(checkpoint_metatileSet_4x4_props, metatileSet_4x4_props, tmpSize);

	checkpoint_metatileSets_4x4 = metatileSets_4x4;



	tmpSize		 = metatileSets_8x8*META_8x8;

	if(tmpSize > (unsigned)checkpoint_metatileSets_8x8*META_8x8) {
		checkpoint_metatileSet_8x8_id = (char*) realloc(checkpoint_metatileSet_8x8_id, tmpSize);
		checkpoint_metatileSet_8x8_pal = (char*) realloc(checkpoint_metatileSet_8x8_pal, tmpSize);
		checkpoint_metatileSet_8x8_props = (char*) realloc(checkpoint_metatileSet_8x8_props, tmpSize);
    }
	memcpy(checkpoint_metatileSet_8x8_id, metatileSet_8x8_id, tmpSize);
	memcpy(checkpoint_metatileSet_8x8_pal, metatileSet_8x8_pal, tmpSize);
	memcpy(checkpoint_metatileSet_8x8_props, metatileSet_8x8_props, tmpSize);

	checkpoint_metatileSets_8x8 = metatileSets_8x8;





	//put undo buffer into "undo checkpoint" buffer
	memcpy(bgPalUndoCheckPoint,bgPalUndo,sizeof(bgPal));


	memcpy(undoCheckpointNameTable,undoNameTable,name_size());
	memcpy(undoCheckpointAttrTable,undoAttrTable,attr_size());
	memcpy(undoCheckpointMetaSprites,undoMetaSprites,sizeof(checkpointMetaSprites));

	//memcpy(checkpointTilePropertiesUndo,tilePropertiesUndo,tmpsize);
	//memcpy(checkpointTilePropertiesUndo_LinkBuf,tilePropertiesUndo_LinkBuf,));

	memcpy(checkpointPropConditionalUndo,propConditionalUndo,sizeof(checkpointPropConditionalUndo));
	memcpy(checkpointPropConditionUndo,propConditionUndo,sizeof(checkpointPropConditionUndo));


	undoCheckpointNameTableWidth  = undoNameTableWidth;
	undoCheckpointNameTableHeight = undoNameTableHeight;

	undoCheckpointSpriteGridX  =  undoSpriteGridX;
	undoCheckpointSpriteGridY  =  undoSpriteGridY;

	//chr bank stuff pt2

	tmpSize = undo_chrBanks*4096;
	if (tmpSize > undo_checkpoint_chrBanks*4096) {
		undoCheckpointChr = (char*) realloc(undoCheckpointChr, tmpSize);
		checkpointTilePropertiesUndo = (char*) realloc(checkpointTilePropertiesUndo, tmpSize/16);

	}
	memcpy(undoCheckpointChr, undoChr, tmpSize);
	memcpy(checkpointTilePropertiesUndo, tilePropertiesUndo, tmpSize/16);


	tmpSize = undo_chrBanks_LinkBuf*4096;
	if (tmpSize > undo_checkpoint_chrBanks_LinkBuf*4096) {
		undoCheckpointChr_LinkBuf = (char*) realloc(undoCheckpointChr_LinkBuf, tmpSize);
		checkpointTilePropertiesUndo_LinkBuf = (char*) realloc(checkpointTilePropertiesUndo_LinkBuf, tmpSize/16);
	}
	memcpy(undoCheckpointChr_LinkBuf,undoChr_LinkBuf,tmpSize);
	memcpy(checkpointTilePropertiesUndo_LinkBuf,tilePropertiesUndo_LinkBuf,tmpSize/16);

	undo_checkpoint_chrBanks = undo_chrBanks;
	undo_checkpoint_chrBankLabels = undo_chrBankLabels;
	undo_checkpoint_chrBanks_LinkBuf = undo_chrBanks_LinkBuf;
	undo_checkpoint_chrBankLabels_LinkBuf = undo_chrBankLabels_LinkBuf;

	//metatiles pt2
	tmpSize = undo_metatileSets_2x2*META_2x2;

	if(tmpSize > (unsigned)undo_checkpoint_metatileSets_2x2*META_2x2){
		undo_checkpoint_metatileSet_2x2_id = (char*) realloc(undo_checkpoint_metatileSet_2x2_id, tmpSize);
		undo_checkpoint_metatileSet_2x2_pal = (char*) realloc(undo_checkpoint_metatileSet_2x2_pal, tmpSize);
		undo_checkpoint_metatileSet_2x2_props = (char*) realloc(undo_checkpoint_metatileSet_2x2_props, tmpSize);
	}

	memcpy(undo_checkpoint_metatileSet_2x2_id, undo_metatileSet_2x2_id, tmpSize);
	memcpy(undo_checkpoint_metatileSet_2x2_pal, undo_metatileSet_2x2_pal, tmpSize);
	memcpy(undo_checkpoint_metatileSet_2x2_props, undo_metatileSet_2x2_props, tmpSize);


	undo_checkpoint_metatileSets_2x2 = undo_metatileSets_2x2;

	tmpSize = undo_metatileSets_4x4*META_4x4;

	if(tmpSize > (unsigned)undo_checkpoint_metatileSets_4x4*META_4x4) {
		undo_checkpoint_metatileSet_4x4_id = (char*) realloc(undo_checkpoint_metatileSet_4x4_id, tmpSize);
		undo_checkpoint_metatileSet_4x4_pal = (char*) realloc(undo_checkpoint_metatileSet_4x4_pal, tmpSize);
		undo_checkpoint_metatileSet_4x4_props = (char*) realloc(undo_checkpoint_metatileSet_4x4_props, tmpSize);
	}

	memcpy(undo_checkpoint_metatileSet_4x4_id, undo_metatileSet_4x4_id, tmpSize);
	memcpy(undo_checkpoint_metatileSet_4x4_pal, undo_metatileSet_4x4_pal, tmpSize);
	memcpy(undo_checkpoint_metatileSet_4x4_props, undo_metatileSet_4x4_props, tmpSize);

	undo_checkpoint_metatileSets_4x4 = undo_metatileSets_4x4;

	tmpSize = undo_metatileSets_8x8*META_8x8;

	if(tmpSize > (unsigned)undo_checkpoint_metatileSets_8x8*META_8x8) {
		undo_checkpoint_metatileSet_8x8_id = (char*) realloc(undo_checkpoint_metatileSet_8x8_id, tmpSize);
		undo_checkpoint_metatileSet_8x8_pal = (char*) realloc(undo_checkpoint_metatileSet_8x8_pal, tmpSize);
		undo_checkpoint_metatileSet_8x8_props = (char*) realloc(undo_checkpoint_metatileSet_8x8_props, tmpSize);
	}

	memcpy(undo_checkpoint_metatileSet_8x8_id, undo_metatileSet_8x8_id, tmpSize);
	memcpy(undo_checkpoint_metatileSet_8x8_pal, undo_metatileSet_8x8_pal, tmpSize);
	memcpy(undo_checkpoint_metatileSet_8x8_props, undo_metatileSet_8x8_props, tmpSize);


	undo_checkpoint_metatileSets_8x8 = undo_metatileSets_8x8;

}

//---------------------------------------------------------------------------

void __fastcall TFormMain::Reverttocheckpoint1Click(TObject *Sender)
{
	int i,tmp;
	size_t tmpSize;
	int tmp_maxAttr, tmp_maxName;

	int tmp_AttrSize	,	tmp_checkpointAttrSize;
	int tmp_NameSize	, 	tmp_checkpointNameSize;


	AnsiString tmpAnsi;

	for(i=0;i<256;i++)
	{
		tmpAnsi=metaSpriteNames[i];
		metaSpriteNames[i]=metaSpriteNamesCheckpoint[i];
		metaSpriteNamesCheckpoint[i]=tmpAnsi;

		tmpAnsi=metaSpriteNamesUndo[i];
		metaSpriteNamesUndo[i]=metaSpriteNamesCheckpointUndo[i];
		metaSpriteNamesCheckpointUndo[i]=tmpAnsi;
	}

	int tmpBrushTable[16][16];
	for(int i=0;i<14;i++){
		memcpy(tmpBrushTable,ptr_tableBrush[i],256*sizeof(int));
		memcpy(ptr_tableBrush[i],ptr_checkpoint_tableBrush[i],256*sizeof(int));
		memcpy(ptr_checkpoint_tableBrush[i],tmpBrushTable,256*sizeof(int));

		memcpy(tmpBrushTable,ptr_undo_tableBrush[i],256*sizeof(int));
		memcpy(ptr_undo_tableBrush[i],ptr_undo_checkpoint_tableBrush[i],256*sizeof(int));
		memcpy(ptr_undo_checkpoint_tableBrush[i],tmpBrushTable,256*sizeof(int));

	}


	//swap current and checkpoint states
	tmp_AttrSize = (nameTableWidth+3)/4*((nameTableHeight+3)/4);
	tmp_checkpointAttrSize = (checkpointNameTableWidth+3)/4*((checkpointNameTableHeight+3)/4);
	tmp_maxAttr = tmp_AttrSize > tmp_checkpointAttrSize
					? tmp_AttrSize : tmp_checkpointAttrSize;

	tmp_NameSize = nameTableWidth*nameTableHeight;
	tmp_checkpointNameSize = checkpointNameTableWidth*checkpointNameTableHeight;
	tmp_maxName = tmp_NameSize > tmp_checkpointNameSize
					? tmp_NameSize : tmp_checkpointNameSize;

	mem_exchange(bgPalCheckpoint,bgPal,sizeof(bgPal));

	//mem_exchange(checkpointChr,chr,8192);

	mem_exchange(checkpointNameTable,nameTable,tmp_maxName);   //name_size()
	mem_exchange(checkpointAttrTable,attrTable,tmp_maxAttr);   //attr_size()
	mem_exchange(checkpointMetaSprites,metaSprites,sizeof(checkpointMetaSprites));

	mem_32exchange(checkpoint_bankViewTable,bankViewTable,sizeof(checkpoint_bankViewTable));
	mem_32exchange(undo_checkpoint_bankViewTable,undo_bankViewTable,sizeof(undo_checkpoint_bankViewTable));


	mem_exchange(checkpointTileProperties,tileProperties,sizeof(tileProperties));
	mem_exchange(checkpointTileProperties_LinkBuf,tileProperties_LinkBuf,sizeof(tileProperties_LinkBuf));




	mem_exchange(checkpointPropConditional,propConditional,sizeof(propConditional));
	swap_bool_2d(checkpointPropCondition, propCondition, 4, 8);
	//mem_exchange(checkpointPropCondition,propCondition,sizeof(propCondition));




	tmp = nameTableWidth;   nameTableWidth = checkpointNameTableWidth;		checkpointNameTableWidth=tmp;
	tmp = nameTableHeight;  nameTableHeight = checkpointNameTableHeight;    checkpointNameTableHeight=tmp;

	tmp = spriteGridX;   spriteGridX = checkpointSpriteGridX;	 checkpointSpriteGridX=tmp;
	tmp = spriteGridY;   spriteGridY = checkpointSpriteGridY;    checkpointSpriteGridY=tmp;

	//chr bank stuff pt 1
	
	tmp_chrBankLabels	= chrBankLabels;               //vector swap
	chrBankLabels		= checkpoint_chrBankLabels;
	checkpoint_chrBankLabels	= tmp_chrBankLabels;

	tmp_chrBankLabels_LinkBuf	= chrBankLabels_LinkBuf;               //vector swap
	chrBankLabels_LinkBuf		= checkpoint_chrBankLabels_LinkBuf;
	checkpoint_chrBankLabels_LinkBuf	= tmp_chrBankLabels_LinkBuf;

	tmpSize = max(chrBanks*4096,checkpoint_chrBanks*4096);

	if (tmpSize > chrBanks*4096){
		chr = (char*)realloc(chr,tmpSize);
		tileProperties = (char*)realloc(tileProperties,tmpSize/16);
	}
	if (tmpSize > checkpoint_chrBanks*4096){
		checkpointChr 				= (char*)realloc(checkpointChr,tmpSize);
		checkpointTileProperties    = (char*)realloc(checkpointTileProperties,tmpSize/16);
	}
	mem_exchange(checkpointChr,	chr	,tmpSize);
	mem_exchange(checkpointTileProperties,	tileProperties	,tmpSize/16);


	tmpSize = max(chrBanks_LinkBuf*4096,checkpoint_chrBanks_LinkBuf*4096);

	if (tmpSize > chrBanks_LinkBuf*4096){
		chr_LinkBuf = (char*)realloc(chr_LinkBuf,tmpSize);
		tileProperties_LinkBuf = (char*)realloc(tileProperties_LinkBuf,tmpSize/16);
	}
	if (tmpSize > checkpoint_chrBanks_LinkBuf*4096){
		checkpointChr_LinkBuf 				= (char*)realloc(checkpointChr_LinkBuf,tmpSize);
		checkpointTileProperties_LinkBuf 	= (char*)realloc(checkpointTileProperties_LinkBuf,tmpSize/16);
	}
	mem_exchange(checkpointChr_LinkBuf,chr_LinkBuf,tmpSize);
	mem_exchange(checkpointTileProperties_LinkBuf,tileProperties_LinkBuf,tmpSize/16);

	tmp=chrBanks;
	chrBanks=checkpoint_chrBanks;
	checkpoint_chrBanks=tmp;


	//metatiles pt1
	tmpSize = max(metatileSets_2x2*META_2x2,checkpoint_metatileSets_2x2*META_2x2);
	if (tmpSize > (unsigned) metatileSets_2x2*META_2x2){
		metatileSet_2x2_id = (char*) realloc(metatileSet_2x2_id, tmpSize);
		metatileSet_2x2_pal = (char*) realloc(metatileSet_2x2_pal, tmpSize);
		metatileSet_2x2_props = (char*) realloc(metatileSet_2x2_props, tmpSize);
	}
	if (tmpSize > (unsigned) checkpoint_metatileSets_2x2*META_2x2){
		checkpoint_metatileSet_2x2_id = (char*) realloc(checkpoint_metatileSet_2x2_id, tmpSize);
		checkpoint_metatileSet_2x2_pal = (char*) realloc(checkpoint_metatileSet_2x2_pal, tmpSize);
		checkpoint_metatileSet_2x2_props = (char*) realloc(checkpoint_metatileSet_2x2_props, tmpSize);
	}
	mem_exchange(checkpoint_metatileSet_2x2_id,	metatileSet_2x2_id	 ,tmpSize);
	mem_exchange(checkpoint_metatileSet_2x2_pal,	metatileSet_2x2_pal	 ,tmpSize);
	mem_exchange(checkpoint_metatileSet_2x2_props,metatileSet_2x2_props,tmpSize);

	tmp=metatileSets_2x2;
	metatileSets_2x2=checkpoint_metatileSets_2x2;
	checkpoint_metatileSets_2x2=tmp;

	tmpSize = max(metatileSets_4x4*META_4x4,checkpoint_metatileSets_4x4*META_4x4);
	if (tmpSize > (unsigned) metatileSets_4x4*META_4x4){
		metatileSet_4x4_id = (char*) realloc(metatileSet_4x4_id, tmpSize);
		metatileSet_4x4_pal = (char*) realloc(metatileSet_4x4_pal, tmpSize);
		metatileSet_4x4_props = (char*) realloc(metatileSet_4x4_props, tmpSize);
	}
	if (tmpSize > (unsigned) checkpoint_metatileSets_4x4*META_4x4){
		checkpoint_metatileSet_4x4_id = (char*) realloc(checkpoint_metatileSet_4x4_id, tmpSize);
		checkpoint_metatileSet_4x4_pal = (char*) realloc(checkpoint_metatileSet_4x4_pal, tmpSize);
		checkpoint_metatileSet_4x4_props = (char*) realloc(checkpoint_metatileSet_4x4_props, tmpSize);
	}
	mem_exchange(checkpoint_metatileSet_4x4_id,	metatileSet_4x4_id	 ,tmpSize);
	mem_exchange(checkpoint_metatileSet_4x4_pal,	metatileSet_4x4_pal	 ,tmpSize);
	mem_exchange(checkpoint_metatileSet_4x4_props,metatileSet_4x4_props,tmpSize);

	tmp=metatileSets_4x4;
	metatileSets_4x4=checkpoint_metatileSets_4x4;
	checkpoint_metatileSets_4x4=tmp;

	tmpSize = max(metatileSets_8x8*META_8x8,checkpoint_metatileSets_8x8*META_8x8);
	if (tmpSize > (unsigned) metatileSets_8x8*META_8x8){
		metatileSet_8x8_id = (char*) realloc(metatileSet_8x8_id, tmpSize);
		metatileSet_8x8_pal = (char*) realloc(metatileSet_8x8_pal, tmpSize);
		metatileSet_8x8_props = (char*) realloc(metatileSet_8x8_props, tmpSize);
	}
	if (tmpSize > (unsigned) checkpoint_metatileSets_8x8*META_8x8){
		checkpoint_metatileSet_8x8_id = (char*) realloc(checkpoint_metatileSet_8x8_id, tmpSize);
		checkpoint_metatileSet_8x8_pal = (char*) realloc(checkpoint_metatileSet_8x8_pal, tmpSize);
		checkpoint_metatileSet_8x8_props = (char*) realloc(checkpoint_metatileSet_8x8_props, tmpSize);
	}
	mem_exchange(checkpoint_metatileSet_8x8_id,	metatileSet_8x8_id	 ,tmpSize);
	mem_exchange(checkpoint_metatileSet_8x8_pal,	metatileSet_8x8_pal	 ,tmpSize);
	mem_exchange(checkpoint_metatileSet_8x8_props,metatileSet_8x8_props,tmpSize);

	tmp=metatileSets_8x8;
	metatileSets_8x8=checkpoint_metatileSets_8x8;
	checkpoint_metatileSets_8x8=tmp;


	//swap UNDO checkpoint states
	//------------
	tmp_AttrSize = (undoNameTableWidth+3)/4*((undoNameTableHeight+3)/4);
	tmp_checkpointAttrSize = (undoCheckpointNameTableWidth+3)/4*((undoCheckpointNameTableHeight+3)/4);
	tmp_maxAttr = tmp_AttrSize > tmp_checkpointAttrSize
					? tmp_AttrSize : tmp_checkpointAttrSize;

	tmp_NameSize = undoNameTableWidth * undoNameTableHeight;
	tmp_checkpointNameSize = undoCheckpointNameTableWidth * undoCheckpointNameTableHeight;
	tmp_maxName = tmp_NameSize > tmp_checkpointNameSize
					? tmp_NameSize : tmp_checkpointNameSize;

	mem_exchange(bgPalUndoCheckPoint,bgPalUndo,sizeof(bgPal));

	//mem_exchange(undoCheckpointChr,undoChr,8192);


	mem_exchange(undoCheckpointNameTable,undoNameTable,tmp_maxName);      //name_size()
	mem_exchange(undoCheckpointAttrTable,undoAttrTable,tmp_maxAttr);    //attr_size()
	mem_exchange(undoCheckpointMetaSprites,undoMetaSprites,sizeof(checkpointMetaSprites));

	//mem_exchange(checkpointTilePropertiesUndo,tilePropertiesUndo,sizeof(tilePropertiesUndo));
	mem_exchange(checkpointPropConditionalUndo,propConditionalUndo,sizeof(propConditionalUndo));
	swap_bool_2d(checkpointPropConditionUndo, propConditionUndo, 4, 8);
	//mem_exchange(checkpointPropConditionUndo,propConditionUndo,sizeof(propConditionUndo));


	tmp = undoNameTableWidth;   undoNameTableWidth = undoCheckpointNameTableWidth;		undoCheckpointNameTableWidth=tmp;
	tmp = undoNameTableHeight;  undoNameTableHeight = undoCheckpointNameTableHeight;    undoCheckpointNameTableHeight=tmp;

	tmp = undoSpriteGridX;  undoSpriteGridX = undoCheckpointSpriteGridX;	undoCheckpointSpriteGridX=tmp;
	tmp = undoSpriteGridY;  undoSpriteGridY = undoCheckpointSpriteGridY;    undoCheckpointSpriteGridY=tmp;

	//chr bank stuff pt 2

	tmp_chrBankLabels	= undo_chrBankLabels;               //vector swap

	undo_chrBankLabels		= undo_checkpoint_chrBankLabels;
	undo_checkpoint_chrBankLabels	= tmp_chrBankLabels;

	tmpSize = max(undo_chrBanks*4096,undo_checkpoint_chrBanks*4096);

	if (tmpSize > undo_chrBanks*4096){
		undoChr 			= (char*)realloc(undoChr,tmpSize);
		tilePropertiesUndo 	= (char*)realloc(tilePropertiesUndo,tmpSize/16);
	}
	if (tmpSize > undo_checkpoint_chrBanks*4096) {
		undoCheckpointChr 			 = (char*)realloc(undoCheckpointChr,tmpSize);
		checkpointTilePropertiesUndo = (char*)realloc(checkpointTilePropertiesUndo,tmpSize/16);
	}
	mem_exchange(undoCheckpointChr,	undoChr	,tmpSize);
	mem_exchange(checkpointTilePropertiesUndo,	tilePropertiesUndo	,tmpSize/16);

	tmpSize = max(undo_chrBanks*4096,undo_checkpoint_chrBanks*4096);

	if (tmpSize > undo_chrBanks_LinkBuf*4096) {
		undoChr_LinkBuf 			= (char*)realloc(undoChr_LinkBuf,tmpSize);
		tilePropertiesUndo_LinkBuf 	= (char*)realloc(tilePropertiesUndo_LinkBuf,tmpSize/16);
	}
	if (tmpSize > undo_checkpoint_chrBanks_LinkBuf*4096){
		undoCheckpointChr_LinkBuf 			 = (char*)realloc(undoCheckpointChr_LinkBuf,tmpSize);
		checkpointTilePropertiesUndo_LinkBuf = (char*)realloc(checkpointTilePropertiesUndo_LinkBuf,tmpSize/16);
	}
	mem_exchange(undoCheckpointChr_LinkBuf,undoChr_LinkBuf,tmpSize);
    mem_exchange(checkpointTilePropertiesUndo_LinkBuf,tilePropertiesUndo_LinkBuf,tmpSize/16);


	tmp=undo_chrBanks;
	undo_chrBanks=undo_checkpoint_chrBanks;
	undo_checkpoint_chrBanks=tmp;

	tmp=undo_chrBanks_LinkBuf;
	undo_chrBanks_LinkBuf=undo_checkpoint_chrBanks_LinkBuf;
	undo_checkpoint_chrBanks_LinkBuf=tmp;


	//metatiles pt2
	tmpSize = max(undo_metatileSets_2x2*META_2x2,undo_checkpoint_metatileSets_2x2*META_2x2);
	if (tmpSize > (unsigned) undo_metatileSets_2x2*META_2x2){
		undo_metatileSet_2x2_id = (char*) realloc(undo_metatileSet_2x2_id, tmpSize);
		undo_metatileSet_2x2_pal = (char*) realloc(undo_metatileSet_2x2_pal, tmpSize);
		undo_metatileSet_2x2_props = (char*) realloc(undo_metatileSet_2x2_props, tmpSize);
	}
	if (tmpSize > (unsigned) undo_checkpoint_metatileSets_2x2*META_2x2){
		undo_checkpoint_metatileSet_2x2_id = (char*) realloc(undo_checkpoint_metatileSet_2x2_id, tmpSize);
		undo_checkpoint_metatileSet_2x2_pal = (char*) realloc(undo_checkpoint_metatileSet_2x2_pal, tmpSize);
		undo_checkpoint_metatileSet_2x2_props = (char*) realloc(undo_checkpoint_metatileSet_2x2_props, tmpSize);
	}
	mem_exchange(undo_checkpoint_metatileSet_2x2_id,	undo_metatileSet_2x2_id	 ,tmpSize);
	mem_exchange(undo_checkpoint_metatileSet_2x2_pal,	undo_metatileSet_2x2_pal	 ,tmpSize);
	mem_exchange(undo_checkpoint_metatileSet_2x2_props, undo_metatileSet_2x2_props,tmpSize);

	tmp=undo_metatileSets_2x2;
	undo_metatileSets_2x2=undo_checkpoint_metatileSets_2x2;
	undo_checkpoint_metatileSets_2x2=tmp;

	tmpSize = max(undo_metatileSets_4x4*META_4x4,undo_checkpoint_metatileSets_4x4*META_4x4);
	if (tmpSize > (unsigned) undo_metatileSets_4x4*META_4x4){
		undo_metatileSet_4x4_id = (char*) realloc(undo_metatileSet_4x4_id, tmpSize);
		undo_metatileSet_4x4_pal = (char*) realloc(undo_metatileSet_4x4_pal, tmpSize);
		undo_metatileSet_4x4_props = (char*) realloc(undo_metatileSet_4x4_props, tmpSize);
	}
	if (tmpSize > (unsigned) undo_checkpoint_metatileSets_4x4*META_4x4){
		undo_checkpoint_metatileSet_4x4_id = (char*) realloc(undo_checkpoint_metatileSet_4x4_id, tmpSize);
		undo_checkpoint_metatileSet_4x4_pal = (char*) realloc(undo_checkpoint_metatileSet_4x4_pal, tmpSize);
		undo_checkpoint_metatileSet_4x4_props = (char*) realloc(undo_checkpoint_metatileSet_4x4_props, tmpSize);
	}
	mem_exchange(undo_checkpoint_metatileSet_4x4_id,	undo_metatileSet_4x4_id	 ,tmpSize);
	mem_exchange(undo_checkpoint_metatileSet_4x4_pal,	undo_metatileSet_4x4_pal	 ,tmpSize);
	mem_exchange(undo_checkpoint_metatileSet_4x4_props, undo_metatileSet_4x4_props,tmpSize);

	tmp=undo_metatileSets_4x4;
	undo_metatileSets_4x4=undo_checkpoint_metatileSets_4x4;
	undo_checkpoint_metatileSets_4x4=tmp;

	tmpSize = max(undo_metatileSets_8x8*META_8x8,undo_checkpoint_metatileSets_8x8*META_8x8);
	if (tmpSize > (unsigned) undo_metatileSets_8x8*META_8x8){
		undo_metatileSet_8x8_id = (char*) realloc(undo_metatileSet_8x8_id, tmpSize);
		undo_metatileSet_8x8_pal = (char*) realloc(undo_metatileSet_8x8_pal, tmpSize);
		undo_metatileSet_8x8_props = (char*) realloc(undo_metatileSet_8x8_props, tmpSize);
	}
	if (tmpSize > (unsigned) undo_checkpoint_metatileSets_8x8*META_8x8){
		undo_checkpoint_metatileSet_8x8_id = (char*) realloc(undo_checkpoint_metatileSet_8x8_id, tmpSize);
		undo_checkpoint_metatileSet_8x8_pal = (char*) realloc(undo_checkpoint_metatileSet_8x8_pal, tmpSize);
		undo_checkpoint_metatileSet_8x8_props = (char*) realloc(undo_checkpoint_metatileSet_8x8_props, tmpSize);
	}
	mem_exchange(undo_checkpoint_metatileSet_8x8_id,	undo_metatileSet_8x8_id	 ,tmpSize);
	mem_exchange(undo_checkpoint_metatileSet_8x8_pal,	undo_metatileSet_8x8_pal	 ,tmpSize);
	mem_exchange(undo_checkpoint_metatileSet_8x8_props, undo_metatileSet_8x8_props,tmpSize);

	tmp=undo_metatileSets_8x8;
	undo_metatileSets_8x8=checkpoint_metatileSets_8x8;
	undo_checkpoint_metatileSets_8x8=tmp;






	//sanitize line drawing buffer
	memset (arr_linePlotCHR,0,sizeof(arr_linePlotCHR2));
	memset (arr_linePlotCHR2,0,sizeof(arr_linePlotCHR2));

	//memcpy (chrBuf, chr, 8192);
    int set=bankActive/16;
	for(int tile=0;tile<256;tile++){
	    int	pp=tileViewTable[tile]*16+bankViewTable[tile+set];
		memcpy(chrBuf+tile*16,chr+pp,16);
	}


	lineUpX= -1;
	lineUpY= -1;


	UpdateAll();


}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SpeedButtonSubpalCopyClick(TObject *Sender)
{
   cf_palRotation=0;  //reset subpalette paste rotation

   DWORD Len=4;
   HGLOBAL hDst=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,Len);

   if (hDst)
   {
		LPSTR gDst=(LPSTR)GlobalLock(hDst);
		if (gDst)
		{
			if (OpenClipboard(Handle))
			{
				EmptyClipboard();
				memcpy(gDst,&bgPal[palBank*16+palActive*4],4);

				SetClipboardData(CF_SUBPAL,hDst);
				CloseClipboard();
			}
		}
   }


}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSubpalPasteClick(TObject *Sender)
{


   OpenClipboard(Handle);

   HGLOBAL hClipBuf=GetClipboardData(CF_SUBPAL);

   if (hClipBuf)
   {
		LPSTR gSrc=(LPSTR)GlobalLock(hClipBuf);

		if (gSrc!=NULL)
		{
			SetUndo();

			memcpy(&bgPal[palBank*16+palActive*4],gSrc,4);


			GlobalUnlock(gSrc);
		}
   }

   CloseClipboard();
   UpdateAll();

}
//---------------------------------------------------------------------------


void __fastcall TFormMain::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
		clickV=false;
		clickC=false;
		bIgnoreKey=false;
		bBufCtrl=Shift.Contains(ssCtrl)?true:false;
		bBufShift=Shift.Contains(ssShift)?true:false;
		bBufAlt=Shift.Contains(ssAlt)?true:false;

		bBufVK_3=false;
        if(PageControlEditor->ActivePage==TabSheetTile)
		{
			if(Key=='G') {btnSmudge->Down=false; FormCHREditor->btnSmudge->Down=false; bSmudge=false;}
		}
		if (bBufCtrl || bBufShift || bBufAlt)
		{
			//this is for updating when drag has been accepted.
			if (bImageNameAccepted) cueUpdateNametableNoRepaint=true;
			else if (bImageTileAccepted) 	cueUpdateTiles=true;
			//this is for any other case. Probably redundant for now.

			//else{cueUpdateNametable=true; cueUpdateTiles=true;}  //include if thereÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s a legitimate use.
		}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SelectAll1Click(TObject *Sender)
{
	int i;
	if(PageControlEditor->ActivePage==TabSheetSprite)
	{
		for(i=0;i<ListBoxSpriteList->Items->Count;++i) ListBoxSpriteList->Selected[i]=true;
        UpdateAll();
		return;
	}

	if(nameSelection.left>=0||nameSelection.top>=0
		|| (isLastClickedNametable==true && isLastClickedTiles==false))
	{
		if(	(nameSelection.right-nameSelection.left>=32
			&& nameSelection.bottom-nameSelection.top>=30)
			&& !(	 nameSelection.left  ==0
				  && nameSelection.top   ==0
				  && nameSelection.right ==nameTableWidth
				  && nameSelection.bottom==nameTableHeight)
			)
		{
			nameSelection.left  =0;
			nameSelection.top   =0;
			nameSelection.right =nameTableWidth;
			nameSelection.bottom=nameTableHeight;
		}
		else
        {
			nameSelection.left  =nameTableViewX;
			nameSelection.top   =nameTableViewY;
			nameSelection.right =nameTableViewX+32;
			nameSelection.bottom=nameTableViewY+30;

            for (int i=0; i<32; i++)  //long enough loop
			// I need to special-case if the canvas is smaller than the viewport.
				{
					if(nameSelection.left<0)   	{	nameSelection.left++;
												nameSelection.right++;}
					if(nameSelection.right>nameTableWidth)
											{	nameSelection.left--;
												nameSelection.right--;}
					if(nameSelection.top<0)   	{	nameSelection.top++;
												nameSelection.bottom++;}
					if(nameSelection.bottom>nameTableHeight)
											{	nameSelection.top--;
												nameSelection.bottom--;}
				}


		}

	}
	else
	{
		for(i=0;i<256;i++) chrSelected[i]=true;
		chrSelection.top	= 0;
		chrSelection.left	= 0;
		chrSelection.right	= 16;
		chrSelection.bottom	= 16;
		tileSelRectWdt      = chrSelection.right-chrSelection.left;
		tileSelRectHgt		= chrSelection.bottom-chrSelection.top;

		if(MCHRFindDoubles->Checked)
		{
			MCHRFindDoubles->Checked=false;
			MCHRFindDoubles->Caption="Find &duplicates";
		}

		if(MCHRFindUnused->Checked)
		{
			MCHRFindUnused->Checked=false;
			MCHRFindUnused->Caption="Find &unused";
		}
	}
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Deselect1Click(TObject *Sender)
{
	int i;
	if(FormMetatileTool->Active || FormMTprops->Active)
	{
		 metaSelection_2x2.left= -1;
		 metaSelection_2x2.top= -1;
		 metaSelection_4x4.left= -1;
		 metaSelection_4x4.top= -1;
		 metaSelection_8x8.left= -1;
		 metaSelection_8x8.top= -1;
		 for(i=0;i<4;i++) metaSelected_2x2[i]=false;
		 for(i=0;i<16;i++) metaSelected_4x4[i]=false;
		 for(i=0;i<64;i++) metaSelected_8x8[i]=false;
	}
	else{
		if(PageControlEditor->ActivePage==TabSheetSprite)
		{
			for(i=0;i<ListBoxSpriteList->Items->Count;++i) ListBoxSpriteList->Selected[i]=false;
			UpdateAll();
			return;
		}
		if(nameSelection.left>=0||nameSelection.top>=0)
		{
			bOutsideSel=true;
			nameSelection.left  = -1;
			nameSelection.top   = -1;
			//nameSelection.right  = -1;
			//nameSelection.bottom   = -1;
		}
		else
		{
			for(i=0;i<256;i++) chrSelected[i]=false;
			chrSelected[0]=true;
			chrSelection.top	= 0;
			chrSelection.left	= 0;
			chrSelection.right	= 1;
			chrSelection.bottom	= 1;
			tileSelRectWdt      = chrSelection.right-chrSelection.left;
			tileSelRectHgt		= chrSelection.bottom-chrSelection.top;
		}
		if(MCHRFindDoubles->Checked)
		{
			MCHRFindDoubles->Checked=false;
			MCHRFindDoubles->Caption="Find &duplicates";
		}

		if(MCHRFindUnused->Checked)
		{
			MCHRFindUnused->Checked=false;
			MCHRFindUnused->Caption="Find &unused";
		}
	}
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TextNullTileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(Shift.Contains(ssLeft))
		{
			prevMouseY=Y;
			prevNullTile=nullTile;

			chrSelBuf.left		=	chrSelection.left;
			chrSelBuf.top		=	chrSelection.top;
			chrSelBuf.right		=	chrSelection.right;
			chrSelBuf.bottom  	=	chrSelection.bottom;

			chrSelection.left	=	nullTile%16;
			chrSelection.top   	=	nullTile/16;
			chrSelection.right 	=	chrSelection.left+1;
			chrSelection.bottom	=	chrSelection.top +1;
			chrSelectRect=true;
			UpdateTiles(true);
			UpdateNameTable(-1,-1,true);
			//UpdateStats();
			cueStats=true;

		}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TextNullTileMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	  int i,delta;

	if(Shift.Contains(ssLeft))
	{
		delta=prevMouseY-Y;


		nullTile+=1*(delta/2);



		if(nullTile<0x00) nullTile=0xff;
		if(nullTile>0xff) nullTile=0x00;

		TextNullTile->Caption="   null tile: $"+IntToHex(nullTile,2)+" ";

		prevMouseY=Y;
		chrSelection.left  =nullTile%16;
		chrSelection.top   =nullTile/16;
		chrSelection.right =chrSelection.left+1;
		chrSelection.bottom=chrSelection.top +1;
		chrSelectRect=true;

		UpdateTiles(true);
		//i think these were unneccessary
		//UpdateNameTable(-1,-1,true);
		//FormNavigator->Draw(false);

		cueStats=true;
        doUpdateMetatileNulls=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TextNullTileMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	   chrSelection.left	=	chrSelBuf.left;
	   chrSelection.top   	=	chrSelBuf.top;
	   chrSelection.right 	=	chrSelBuf.right;
	   chrSelection.bottom	=	chrSelBuf.bottom;
	   chrSelectRect=true;
	   UpdateTiles(true);
	   UpdateNameTable(-1,-1,true);
	   FormNavigator->Draw(false,false,false);

	   //set metatiles if empty
	   if(doUpdateMetatileNulls){
			bool madeChange=false;
				for(int i=0; i<metatileSets_2x2*64; i++) {
				if (AllValuesEqual(&metatileSet_2x2_id[i*4],   4, prevNullTile))
					{memset(&metatileSet_2x2_id[i*4],nullTile,4);
					madeChange=true;
				}
			}
			for(int i=0; i<metatileSets_4x4*64; i++) {
				if (AllValuesEqual(&metatileSet_4x4_id[i*16], 16, prevNullTile)) {
					memset(&metatileSet_4x4_id[i*16],nullTile,16);
					madeChange=true;
				}
			}
			for(int i=0; i<metatileSets_8x8*64; i++) {
				if (AllValuesEqual(&metatileSet_8x8_id[i*64], 64, prevNullTile))
					{memset(&metatileSet_8x8_id[i*64],nullTile,64);
					madeChange=true;
				}
			}
			if(madeChange) if (FormMetatileTool->Visible) FormMetatileTool->DrawTimer->Enabled=true;
			doUpdateMetatileNulls=false;
	   }

	   //UpdateStats();
	   cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Setcarriagereturnpoint1Click(TObject *Sender)
{
	if(nameSelection.left>=0&&nameSelection.top>=0) returnCarriagePos= nameSelection.left;
	else Application->MessageBox
		("A selected map position is required.",
		"Set carriage return point",
		MB_OK);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CopySelectionasBMP1Click(TObject *Sender)
{
	TPicture *picture;
	Graphics::TBitmap *bmp;
	int i,j,x,y,w,h,pp;
	AnsiString tmp;

	unsigned short picFormat;
	unsigned int cbHandle;

	if(nameSelection.left>=0&&nameSelection.top>=0)
		{GetSelection(nameSelection,x,y,w,h); tmp="name(s)";}
	else
		{GetSelection(chrSelection,x,y,w,h); tmp="character(s)";}
    BlockDrawing(true);

	picture=new TPicture();
	bmp=new Graphics::TBitmap();
	bmp->SetSize(w*8,h*8);
	bmp->PixelFormat=pf4bit;
	SetBMPPalette(bmp);
	picture->Bitmap=bmp;

	pp=0;

	if(nameSelection.left>=0&&nameSelection.top>=0)
	{

		for(i=0;i<h;++i)
		{
			for(j=0;j<w;++j)
			{
				nameCopy[pp]=nameTable[y*nameTableWidth+x+j];
				attrCopy[pp]=AttrGet(x+j,y,false,false);
				DrawExportTile16(
					picture,
					j*8,
					i*8,
					nameCopy[pp],
					attrCopy[pp],
					false
					);
			++pp;
		}
		++y;
	}

	}
	else
	{
		for(i=0;i<h;i++)
		{
			for(j=0;j<w;j++)
			{
				pp=bankActive+(x+j)*16+(y+i)*256;//+k;
				DrawExportTile16(picture,j*8,i*8,pp,palActive,true);
			}
		}
	}
	
	HPALETTE hPal;

	picture->SaveToClipboardFormat(picFormat, cbHandle, hPal);
	Clipboard()->SetAsHandle(picFormat, cbHandle);

	delete bmp;
	delete picture;


	BlockDrawing(false);


	LabelStats->Caption="Selected "+tmp+" put on clipboard as BMP.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Delete1Click(TObject *Sender)
{
//question - should this priority be uniform with how ctrl-x etc work?
if(PageControlEditor->ActivePage==TabSheetSprite)
{
	SpeedButtonSpriteDelClick(Sender);
	
    return;
}
if(nameSelection.left>=0&&nameSelection.top>=0) CopyMap(true);
else CopyCHR(false,true);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::StatusUpdateWaiterTimer(TObject *Sender)
{
	if(!openByFileDone) return;
	StatusUpdateWaiter->Enabled=false;
	if (cueStats==true) UpdateStats();


	holdStats=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ThrottleTimer(TObject *Sender)
{
	if(!openByFileDone) return;
	throttleOK = true;
	throttleOKselRect = true;
	throttleOKtileSelRect = true;
	
	if (cueStats==true&&holdStats==false) UpdateStats();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageNameMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	UpdateNameTable(-1,-1,true);
	//FormNavigator->Draw(false,false);
	int dx = nameTableViewX <  0 ?  0  : nameTableViewX;
	int dy = nameTableViewY <  0 ?  0  : nameTableViewY;
	int dw = nameTableWidth < 32 ?  nameTableWidth   : 32;
	int dh = nameTableHeight< 32 ?  nameTableHeight  : 32;

	FormNavigator->DrawRange(dx,dy,dw,dh,false);
	//FormNavigator->CueLinesTimer->Enabled=true;
    FormNavigator->UpdateLines(false);
	mouseDraggedNTSel=false;
	mouseDraggedTileSel=false;
	TimerNTstrip->Enabled=false;
	/* //debug
	int ax,ay,aw,ah;
	GetSelection(nameSelection,ax,ay,aw,ah);
	MessageBox(NULL, (String("ax: ") + IntToStr(ax) + "\nay: " + IntToStr(ay) + "\naw: " + IntToStr(aw) + "\nah: " + IntToStr(ah)).c_str(), "Integer Values", MB_OK | MB_ICONINFORMATION);
    */

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageTilesMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	//draw line stuff
    bool bSmear = FormLineDetails->btnSmear->Down;
  	//bool bMove  = FormLineDetails->btnMove->Down;
	//bool bReuse = FormLineDetails->btnReuse->Down;
	bool bQuick = FormLineDetails->btnQuick->Down;

		lineUpX=X;
		lineUpY=Y;
		mvOriginX=0;
		mvOriginY=0;
		lineDownX=0;
		lineDownY=0;
	if(lineDrawing){
		FormCHREditor->SpeedButtonHFlip->Enabled=true;
		FormCHREditor->SpeedButtonVFlip->Enabled=true;
		FormCHREditor->SpeedButtonRotateCW->Enabled=true;
		FormCHREditor->SpeedButtonRotateCCW->Enabled=true;

		FormLineDetails->btnMove->Down=false;
	}
	else{lineUpX= -1; lineUpY= -1;}

		
	if(bSmear || bQuick){memcpy (arr_linePlotCHR2,arr_linePlotCHR,sizeof(arr_linePlotCHR2));}

	FormLineDetails->btnMove->Enabled=false;
	lineDrawing = false;
	
	isMovingLine= false;
	bForbidPaint = false;




	//tileset stuff
	mouseDraggedNTSel=false;
	mouseDraggedTileSel=false;


	UpdateNameTable(-1,-1,true);
	if(FormNavigator->Visible) FormNavigator->Draw(false,false,false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TypeInModeOnOff1Click(TObject *Sender)
{
  SpeedButtonTypeIn->Down^=true;
  SpeedButtonTypeInClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ForceActiveTab1Click(TObject *Sender)
{
	((TMenuItem*)Sender)->Checked^=true;
	MCHRRemoveDoubles->Checked=false;
	MCHRRemoveUnused->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::IncludeNametables1Click(TObject *Sender)
{
   ((TMenuItem*)Sender)->Checked^=true;
	MCHRRemoveDoubles->Checked=false;
	MCHRRemoveDoubles->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::IncludeMetasprites1Click(TObject *Sender)
{
   ((TMenuItem*)Sender)->Checked^=true;
    MCHRRemoveDoubles->Checked=false;
	MCHRRemoveDoubles->Checked=false;
   //todo: maybe add a warning if both options are unchecked that default behaviour in this case is same as "force active".
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Savesession1Click(TObject *Sender)
{
	AnsiString str = SaveDialogSession->FileName;

	BlockDrawing(true);

	//-- save session

	if(str!="session") SaveSession(SaveDialogSession->FileName);
	else if(SaveDialogSession->Execute()) SaveSession(SaveDialogSession->FileName);

	//--

	//deal with auto exports from pipeline helper.
	//---
	bool bByFiletype	 = FormPipelineHelper->btnType->Down;
	bool bByAsset		 = FormPipelineHelper->btnAsset->Down;
	bool bByName		 = FormPipelineHelper->btnName->Down;

	if (FormPipelineHelper->chkAutoExpMetatilesBMP->Checked==true)
	{


		//make it directly in in workdir
		if(FormPipelineHelper->RadioButton1->Checked) {
			AnsiString str2 = SaveDialogSession->FileName;
			MetatilesToBMP(str2);
		}
		//make it in subfolder of workdir
		else if (FormPipelineHelper->RadioButton2->Checked){
			AnsiString str2 = ExtractFileDir(SaveDialogSession->FileName);
			//naming style
			AnsiString str3;
			if(bByFiletype)str3="Autoexport Bitmaps";
			else if(bByAsset)str3="Autoexport Metatiles";
			else if(bByName) str3= "Autoexport " + RemoveExt(ExtractFileName(SaveDialogSession->FileName));

			str2=CheckAndCreateFolder(str3.c_str(), false);
			if(bByName)str2 +="metatiles";
			else       str2 += RemoveExt(ExtractFileName(SaveDialogSession->FileName))+"_metatiles";
			//ShowMessage(str2);
			MetatilesToBMP(str2);

		}
		//use/create home subfolder.
		else{
                AnsiString str2 = SaveDialogSession->FileName;
				AnsiString str3 = "Autoexports";
				str2=CheckAndCreateFolder(str3.c_str(), true);
				str2+= RemoveExt(ExtractFileName(SaveDialogSession->FileName))+"_metatiles";
				MetatilesToBMP(str2);
		}
	}
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::sortonremoval1Click(TObject *Sender)
{
	sortonremoval1->Checked^=true;
	MCHRRemoveDoubles->Checked=false;
	MCHRRemoveDoubles->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Noterminator1Click(TObject *Sender)
{
   ((TMenuItem*)Sender)->Checked^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (unsavedChanges==true)
	{
		if(Application->MessageBox("Unsaved work will be lost!","QUIT: Are you sure?",MB_YESNO)==IDNO) Action = caNone;
	
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Externalnespal1Click(TObject *Sender)
{
	int i,pp;
	FILE *file;
	unsigned char buf[192];
	AnsiString dir, name;

	dir=ParamStr(0).SubString(0,ParamStr(0).LastDelimiter("\\/"));

	file=fopen((dir+"nes.pal").c_str(),"rb");
	if(file)
	{
		if(get_file_size(file)==192)
		{
			Externalnespal1->Checked=true;
			fread(buf,192,1,file);
			fclose(file);

			pp=0;

			for(i=0;i<64;i++)
			{
				basePalette[i]=(buf[pp+2]<<16)|(buf[pp+1]<<8)|buf[pp];
				pp+=3;
			}
		}
	}
	else
	{
		Application->MessageBox("CouldnÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´t find ext pal.\n Palette set to NESST classic.","Error",MB_OK);
		NESSTclassic1->Checked=true;       //todo: maybe allow for a few internal palette preference choices.
		pp=0;

		for(i=0;i<64;i++)
		{
			basePalette[i]=(palette[pp+2]<<16)|(palette[pp+1]<<8)|palette[pp];
			pp+=3;
		}
	}

	ppuMask=0;
	palette_calc();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::NESSTclassic1Click(TObject *Sender)
{
	 int pp,i;
	 NESSTclassic1->Checked=true;       //todo: maybe allow for a few internal palette preference choices.
		pp=0;

		for(i=0;i<64;i++)
		{
			basePalette[i]=(palette[pp+2]<<16)|(palette[pp+1]<<8)|palette[pp];
			pp+=3;
		}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ClipMetaSpriteAsBMP1Click(TObject *Sender)
{
	TPicture *picture;
	Graphics::TBitmap *bmp;
	BlockDrawing(true);

	picture=new TPicture();
	bmp=new Graphics::TBitmap();
	bmp->SetSize(128,128);
	bmp->PixelFormat=pf24bit; //todo: make the draw routine work for 4bpp instead
	SetBMPPalette(bmp);
	picture->Bitmap=bmp;

	//(TPicture *img,int spr_id,int scale,bool grid,bool frame_all,bool frame_none)
	DrawMetaSpriteExport(picture,metaSpriteActive,1,SpeedButtonSpriteGrid->Down,SpeedButtonFrameAll->Down,SpeedButtonFrameNone->Down);

	unsigned short picFormat;
	unsigned int cbHandle;
	HPALETTE hPal;

	try
	{
		picture->SaveToClipboardFormat(picFormat, cbHandle, hPal);
		Clipboard()->SetAsHandle(picFormat, cbHandle);
	}
	__finally
	{
		delete bmp;
		delete picture;
    }
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CopyMetasToCB(int i)
{
	TPicture *picture;
			Graphics::TBitmap *bmp;

			picture=new TPicture();
			bmp=new Graphics::TBitmap();
			bmp->SetSize(128,128);
			bmp->PixelFormat=pf24bit; //todo: make the draw routine work for 4bpp instead
			SetBMPPalette(bmp);
			picture->Bitmap=bmp;

			unsigned short picFormat;
			unsigned int cbHandle;
			HPALETTE hPal;

			try
			{
				DrawMetaSpriteExport(picture,i,1,SpeedButtonSpriteGrid->Down,SpeedButtonFrameAll->Down,SpeedButtonFrameNone->Down);

				picture->SaveToClipboardFormat(picFormat, cbHandle, hPal);
				Clipboard()->SetAsHandle(picFormat, cbHandle);
			}
			__finally
			{
				delete bmp;
				delete picture;
			}
}

void __fastcall TFormMain::CopyBankasBMP1Click(TObject *Sender)
{
	int i,off;
	int total=0;
	int cnt=0;

	for(i=0;i<256;++i)
	{
		off=i*64*4;

		if(metaSprites[off]<255)
		{
			++total;
		}
	}
	if(total>10)
	if(Application->MessageBox(("There are more than 10 metasprites to process.\nIt takes ~0.2s to process each."),"Confirm",MB_OKCANCEL)!=IDOK) return;
	Screen->Cursor = crHourGlass;
	for(i=0;i<256;++i)
	{
		off=i*64*4;

		if(metaSprites[off]<255)
		{
			++cnt;
			//if(Application->MessageBox("Keep putting metasprites to stack?","Confirm",MB_YESNO)!=IDYES) return;
			//templabel:
			//if(CBThrottle->OnTimer==false) goto templabel;

			Sleep(250);
			BlockDrawing(true);
			CopyMetasToCB(i);
			BlockDrawing(false);
		}
	}
	Screen->Cursor = crDefault;
	LabelStats->Caption=num_to_3char(cnt)+"/"+num_to_3char(total)
			+" metasprite(s) copied to the clipboard stack.\n"
			+"\tTo retrieve them, press win+v.";
	StatusUpdateWaiter->Enabled=true;
			holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SaveMetasAsBMP(int i, AnsiString str)
{

			AppendStr(str,+".bmp");

			TPicture *picture;
			Graphics::TBitmap *bmp;

			picture=new TPicture();
			bmp=new Graphics::TBitmap();
			bmp->SetSize(128,128);
			bmp->PixelFormat=pf24bit; //todo: make the draw routine work for 4bpp instead
			SetBMPPalette(bmp);
			picture->Bitmap=bmp;

			try
			{
				DrawMetaSpriteExport(picture,i,1,SpeedButtonSpriteGrid->Down,SpeedButtonFrameAll->Down,SpeedButtonFrameNone->Down);

				picture->SaveToFile(str);
			}
			__finally
			{
				delete bmp;
				delete picture;
			}
}


void __fastcall TFormMain::MetaspritebankasBMPSequence1Click(TObject *Sender)
{
	int i,cnt,off;
	AnsiString str,buf;

	cnt=0;

	for(i=0;i<256;++i)
	{
		off=i*64*4;
		if(metaSprites[off]<255) ++cnt;
	}
	if (cnt<1)
	{
		LabelStats->Caption="WARNING: No metasprite was found:\nNeeds at least 1 metasprite in list.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
		return;
	}

	BlockDrawing(true);
	str=RemoveExt(SaveDialogImage->FileName);

	if(str=="") str="sequence";

	SaveDialogImage->FileName=str+".bmp";
	buf=str;

	if(SaveDialogImage->Execute())

	for(int i=0;i<256;++i)
	{
		int off=i*64*4;

		if(metaSprites[off]<255)
		{
        	str=buf;
			if (i<100) AppendStr(str,"0");
			if (i<10)  AppendStr(str,"0");
			AppendStr(str,IntToStr(i));


			SaveMetasAsBMP(i,str);
		}
	}
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MetaspriteasBMP1Click(TObject *Sender)
{
	BlockDrawing(true);
	AnsiString str=RemoveExt(SaveDialogImage->FileName);

	if(str=="") str="metasprite";

	SaveDialogImage->FileName=str+".bmp";

	if(SaveDialogImage->Execute())

	SaveMetasAsBMP(metaSpriteActive,str);

	BlockDrawing(false);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SBPriorityToggle1Click(TObject *Sender)
{
	int i,off;
	bool tmp=true;
	SetUndo();
	for(i=0;i<ListBoxSpriteList->Items->Count;++i)
	{
		if(ListBoxSpriteList->Selected[i])
		{
			off=metaSpriteActive*64*4+i*4;

			if(metaSprites[off]<255)
			{

				if(Sender==SBPriorityToggle1) {metaSprites[off+2]^=OAM_PRIO; tmp=false;}
				if(Sender==SBB4) {metaSprites[off+2]^=OAM_B4; tmp=false;}
				if(Sender==SBB3) {metaSprites[off+2]^=OAM_B3; tmp=false;}   
				if(Sender==SBB2) {metaSprites[off+2]^=OAM_B2; tmp=false;}
				if(tmp) {metaSprites[off+2]^=OAM_PRIO; tmp=false;} //else it must be Prio bit
			}
		}
	}
		//note: NEXXT currently has no way to emulate the sort of clipping effect that may happen.
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Sprlistl1Click(TObject *Sender)
{
	GroupBoxMetaSprite->Align=alRight;
	((TMenuItem*)Sender)->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Sprlistc1Click(TObject *Sender)
{
	GroupBoxMetaSprite->Align=alLeft;
	((TMenuItem*)Sender)->Checked=true;
}
//---------------------------------------------------------------------------



void __fastcall TFormMain::Fill1Click(TObject *Sender)
{
	if(!(PageControlEditor->ActivePage==TabSheetName)) return; //redunancy
	if(!(nameSelection.left<0&&nameSelection.top<0)) FillMap(false);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Generalsettings1Click(TObject *Sender)
{
  FormPreferences->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MetaSpriteTimerTimer(TObject *Sender)
{

   if(!openByFileDone) return;

   bool tmp =! cueUpdateMetasprite_noListUpdate;
   cueUpdateMetasprite_noListUpdate=false;


   if (cueUpdateMetasprite==true)
	{
		if(PageControlEditor->ActivePage==TabSheetSprite)UpdateMetaSprite(tmp);
		if(FormManageMetasprites)
			if(FormManageMetasprites->Visible)
				{
				//DrawMetaSprite(FormManageMetasprites->ImageMetaSprite,metaSpriteActive,1,true,false,true);
				FormManageMetasprites->UpdateActive();
				}
	 }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Drawontilesetmodeonoff1Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	if(SpeedButtonDrawTile->Down) {SpeedButtonMarqTile->Down=true; cueStats=true; return;}
	SpeedButtonDrawTile->Down^=true;
	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageBigTilesMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
   bbigTileMouseDown = true;
   TimerNTstrip->Enabled=true;
   ImageTilesMouseDown(Sender,Button,Shift,X/2,Y/2);

}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ImageBigTilesMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
   bigTilesHover=0; //this can be repurposed later for identifying pixels and memory
					  //for now it just acts to give stats intel.
   cueStats=true;
   ImageTilesMouseMove(Sender,Shift,X/2,Y/2);

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageBigTilesMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	bbigTileMouseDown = false;
	TimerNTstrip->Enabled=false;

    bool bSmear = FormLineDetails->btnSmear->Down;
  	//bool bMove  = FormLineDetails->btnMove->Down;
	//bool bReuse = FormLineDetails->btnReuse->Down;
	bool bQuick = FormLineDetails->btnQuick->Down;

		lineUpX=X/2;
		lineUpY=Y/2;
		mvOriginX=0;
		mvOriginY=0;
		lineDownX=0;
		lineDownY=0;
	if(lineDrawing){
		FormCHREditor->SpeedButtonHFlip->Enabled=true;
		FormCHREditor->SpeedButtonVFlip->Enabled=true;
		FormCHREditor->SpeedButtonRotateCW->Enabled=true;
		FormCHREditor->SpeedButtonRotateCCW->Enabled=true;

		FormLineDetails->btnMove->Down=false;
	}
	else{lineUpX= -1; lineUpY= -1;}

		
	if(bSmear || bQuick){memcpy (arr_linePlotCHR2,arr_linePlotCHR,sizeof(arr_linePlotCHR2));}

	FormLineDetails->btnMove->Enabled=false;
	lineDrawing = false;
	
	isMovingLine= false;
	bForbidPaint = false;



	UpdateNameTable(-1,-1,true);
	if (FormNavigator->Visible) FormNavigator->Draw(false,false,false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageBigTilesMouseLeave(TObject *Sender)
{
	bigTilesHover=-1;
	cueStats=true;
	ImageTilesMouseLeave(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageBigTilesDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	ImageTilesDragDrop(Sender,Source,X/2,Y/2);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageBigTilesDragOver(TObject *Sender,
	  TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	Accept=false;


	ImageTilesDragOver(Sender, Source, X, Y, State, Accept);

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TimerNTstripTimer(TObject *Sender)
{
	 if(!openByFileDone) return;
	if (cueUpdateNTstrip==true)
	{
		UpdateNameStrip(false,0);
		UpdateNameStrip(true,3);
		UpdateNameStrip(false,7);
		UpdateNameStrip(true,11);
		UpdateNameStrip(false,15);
		UpdateNameStrip(true,19);
		UpdateNameStrip(false,23);
		UpdateNameStrip(true,27);
		cntNTstrip++;
		if (cntNTstrip>=4)
		{
			cueUpdateNTstrip==false;
			cntNTstrip=0;
			//if(PageControlEditor->ActivePage==TabSheetTile?true:false) ImageTiles->Repaint();
			//else ImageName->Repaint();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnCHReditClick(TObject *Sender)
{
	FormCHREditor->Visible^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::LabelMetaSpriteDblClick(TObject *Sender)
{
	FormName->Caption="Rename metasprite";
	FormName->EditName->Text=metaSpriteNames[metaSpriteActive];
	FormName->ShowModal();
	if(bKeyEscape) return;

	metaSpriteNames[metaSpriteActive]=FormName->EditName->Text;
	UpdateMetaSpriteLabel();
	if(FormManageMetasprites->Visible) FormManageMetasprites->UpdateActive();

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Renameallmetasprites1Click(TObject *Sender)
{
	FormName->Caption="Rename all metasprites";
	FormName->EditName->Text=RemoveExt(ExtractFileName(SaveDialogSession->FileName));
	FormName->ShowModal();
	if(bKeyEscape) return;
	for(int i=0;i<256;i++) metaSpriteNames[i]=FormName->EditName->Text+IntToStr(i);
	FormName->Caption="Metasprite name"; //return to normal
	UpdateMetaSpriteLabel();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonAutocreateClick(TObject *Sender)
{
   //SpeedButtonAutocreate->Down^=true;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Clearemphasis1Click(TObject *Sender)
{
	   ppuMask=0;
	   UpdateRGBM();
	   palette_calc();
	   UpdateTiles(true);
       if (FormBankCHR != NULL) if(FormBankCHR->Visible)FormBankCHR->DrawTimer->Enabled = true;

	   UpdateNameTable(-1,-1,true);
	   DrawPalettes();
	   cueUpdateMetasprite=true;
	   LabelStats->Caption="PPU mask cleared!";
	   StatusUpdateWaiter->Enabled=true;
	   holdStats=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Putemphasissetonallpalettesets1Click(TObject *Sender)
{
	for (int i = 0; i <= 3; i++) ppuMaskSet[i]=ppuMask;
	LabelStats->Caption="PPU mask cloned to all palette sets!";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnHuePlusClick(TObject *Sender)
{

   int n,entry=palActive*4;
   SetUndo();
   for (int i = 1; i <= 3; i++) {
	  n=bgPal[palBank*16+entry+i];
	  if (((n&0x0F)>0x00)&&((n&0x0F)<0x0C)) bgPal[palBank*16+entry+i]+=1;
	  else if ((n&0x0F)==0x0C) bgPal[palBank*16+entry+i]-=11;
	}
   UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnHueMinusClick(TObject *Sender)
{
  int n,entry=palActive*4;

  SetUndo();
  for (int i = 1; i <= 3; i++) {
	  n=bgPal[palBank*16+entry+i];
	  if (((n&0x0F)>0x01)&&((n&0x0F)<=0x0C)) bgPal[palBank*16+entry+i]-=1;
	  else if ((n&0x0F)==0x01) bgPal[palBank*16+entry+i]+=11;
	}
   UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnHueTiltMinusClick(TObject *Sender)
{
	int n,entry=palActive*4;
	SetUndo();

	n=bgPal[palBank*16+entry+1];
	if (((n&0x0F)>0x01)&&((n&0x0F)<=0x0C)) bgPal[palBank*16+entry+1]-=1;
	else if ((n&0x0F)==0x01) bgPal[palBank*16+entry+1]+=11;

	n=bgPal[palBank*16+entry+3];
	if (((n&0x0F)>0x00)&&((n&0x0F)<0x0C)) bgPal[palBank*16+entry+3]+=1;
	else if ((n&0x0F)==0x0C) bgPal[palBank*16+entry+3]-=11;
   UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnHueTiltPlusClick(TObject *Sender)
{
	int n,entry=palActive*4;
	SetUndo();

	n=bgPal[palBank*16+entry+3];
	if (((n&0x0F)>0x01)&&((n&0x0F)<=0x0C)) bgPal[palBank*16+entry+3]-=1;
	else if ((n&0x0F)==0x01) bgPal[palBank*16+entry+3]+=11;

	n=bgPal[palBank*16+entry+1];
	if (((n&0x0F)>0x00)&&((n&0x0F)<0x0C)) bgPal[palBank*16+entry+1]+=1;
	else if ((n&0x0F)==0x0C) bgPal[palBank*16+entry+1]-=11;
   UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSubpalCopyMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="Button [copy subpalette]:\tPuts a single subpalette on the clipboard.\n\nTips:\t-Holding [C] and clicking on a subpalette is equivalent.\n\t-[Shift-C] puts entire subpalette set on the clipboard.";
	//StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSubpalCopyMouseLeave(TObject *Sender)
{
   LabelStats->Caption="---";
   holdStats=false;
   if(uiScale>1 && PageControlEditor->ActivePage==TabSheetTile)
	{	GroupBoxTileControl->Visible=true;}
	else
	{	GroupBoxTileControl->Visible=false;	}

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSubpalPasteMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="Button [paste subpalette]:\tPastes to the active subpalette from the clipboard.\n\nTips:\t-Holding [V] and clicking on a subpalette is equivalent.\n\t-[Shift-V] pastes entire subpalette set.";
	//StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnBrightPlusClick(TObject *Sender)
{

	int n,entry=palActive*4;

	SetUndo();
	for (int i = 1; i <= 3; i++) {
	  n=bgPal[palBank*16+entry+i];
	  if ((n&0xF0)<0x30) bgPal[palBank*16+entry+i]+=16;
	  else if (((n&0xF0)==0x30)&&((n&0x0F)!=0x00)) bgPal[palBank*16+entry+i]=0x20;
	  //else if ((n&0x0F)==0x0C) bgPal[palBank*16+entry+i]-=11;
	}
   UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnBrightMinusClick(TObject *Sender)
{
	int n,entry=palActive*4;

	SetUndo();
	for (int i = 1; i <= 3; i++) {
	  n=bgPal[palBank*16+entry+i];
	  if ((n&0xF0)>=0x10) bgPal[palBank*16+entry+i]-=16;
	  else if ((n&0xF0)<0x10) bgPal[palBank*16+entry+i]=0x0F;
	}
   UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnHueMinusMouseEnter(TObject *Sender)
{
	AnsiString str;
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	if(Sender==btnHueMinus)
		str="Button [hue shift left]:\tShifts the hue of the Active Subpalette left. Grays are ignored.";
	if(Sender==btnHuePlus)
		str="Button [hue shift right]:\tShifts the hue of the Active Subpalette right. Grays are ignored.";
	if(Sender==btnHueTiltMinus)
		str="Button [hue tilt]:\tTilts the hue of the Active Subpalette (<-#1; #3->). Grays are ignored.";
	if(Sender==btnHueTiltPlus)
		str="Button [hue tilt]:\tTilts the hue of the Active Subpalette (<-#3; #1->). Grays are ignored.";
	if(Sender==btnBrightMinus)
		str="Button [value shift down]:\tShifts the value of the Active Subpalette down.";
	if(Sender==btnBrightPlus)
		str="Button [value shift up]:\tShifts the value of the Active Subpalette up.";

	str+="\n\nTips:\t-Set a checkpoint [Shift+Ctrl-Z] before you start manipulating palettes with these buttons.\n\t-Revert to last checkpoint with [Shift-Z].";

	LabelStats->Caption=str;
	//StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMaskBMouseEnter(TObject *Sender)
{
   if(uiScale<3) GroupBoxTileControl->Visible=false;
   LabelStats->Caption="PPUMASK buttons: These set the bits for [R]ed, [G]reen, [B]lue emphasis modes and the [M]onochrome mode.\n\nTips:\t-[Ctrl-E]clears the PPUMASK of the Active Subpalette Set.\n\t-[Shift+Ctrl+E]clones the current PPUMASK to all sets.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonAutocreateMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="If in Draw mode, when active, drawing freely on the screen or map \nautomatically asigns vacant tiles to the canvas.\nFreedraw + autocreation is best done sparingly when working with a tile budget.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMarqTileMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="The standard editing mode for the main window.\n\nOn screen/map: select [Shift+click or drag], place [click], sample [r click], seek [r drag], quickpaste [v+click].\nOn the tileset: single select [click], box select [shift+click], multi select [ctrl+click], swap [r drag].";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonDrawTileMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="Draw mode allows you to draw on the screen / tileset w. the mouse tools normally available in the CHR Editor.\n\nTips:\t-This is not intended to replace a full-fledged canvas editor like Aseprite or Pro Motion, but can be useful.\n\t-Be aware that free drawing in excess leads to huge tilecounts.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnCHReditMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="Opens CHR editor. ItÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s the main utility for detailed tile work.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TextNullTileMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="The 'null tile' is the tile that replaces deleted/cut content on the map.\n[Double click] to set the null tile to the current active tile.\n[Click+hold] to highlight current null tile. [Click+drag up/down] to change null tile.\nYou may also [r click+drag] a tile on the tileset to swap the null tile, along with its contents.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonChrBank1MouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	//LabelStats->Caption="When down, the screen/map references to the main (A) tileset, which is shown and editable.\n\n[X] toggles between main (A) and spare (B) tilesets.";

    TSpeedButton *hoveredBtn = dynamic_cast<TSpeedButton*>(Sender);

	if (hoveredBtn)
	{

		//int tag = hoveredBtn->Tag;
		AnsiString id = hoveredBtn->Caption;


		FormMain->LabelStats->Caption="Tileset "+id+":\tWhen down, assets like nametables, maps, sprites && metatiles refer to this set as their tile source.\nSets are in turn mapped to the first 4 banks by default, but can be remapped to contents from any bank.\nPressing [x] steps through which set is viewed. Right-click a button to temporarily remove it from the [x] roster.";
	 }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonChrBank2MouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="When down, the screen/map references to the spare (B) tileset, which is shown and editable.\n\n[X] toggles between main (A) and spare (B) tilesets.";
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SpeedButtonSelTilesMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="[S] Hides all non-selected tiles.\n\n Good for identifying the position and amount of selected tiles on the screen/map";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonCheckerMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="[A] Disables tile view and instead shows where attributes are placed.\n[Right-click] to open attribute checker options.\nGood for when attribute boundaries are unclear or for identifying 'rogue' attributes.\n";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonGridAllMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="[D] turns grid visibility on/off.\n\n[H,J,K,L] toggle grid resolutions.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonGridTileMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="These buttons [H: 32x30, J: 1x1, K: 2x2, L: 4x4] toggle grid resolutions.\n\nTip:\t[D] turns grid visibility on/off.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonTilesMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="[W] Toggles placing tiles on/off when clicking on the screen/map canvas.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPalMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="[E] Toggles placing palette attributes on/off when clicking on the screen/map canvas.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonTypeInMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="[T] Turns on 'Type In' mode, where you place tiles with your keyboard; each key corresponding to\nits ASCII value; +/- offset. [Esc] exits this mode. [Ctrl-T] toggles mode on/off. [Shift+Ctrl+T] Sets return point.\nTips:\t-If you select [shift+click] a cell before entering type in mode, a return [Enter] will reference it.\n\t-[Shift+Enter] acts as a negative return, moving to beginning of line and 1 line up.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::StaticTextFontOffsetMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="[Drag] to change ASCII offset by tileset row.\n[Shift+Drag] to adjust fine offset.\n\nYou may also check the Edit menu for setting an ASCII 'base offset', which applies independently from this control.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPalBankAMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="This button group switches between 4 subpalette sets.\You may use [Shift + 5-8] to do this more quickly.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CHReditortoolbartopClick(TObject *Sender)
{
	FormCHREditor->GroupBox2->Align=alTop;
	FormCHREditor->GroupBox1->Align=alBottom;
	((TMenuItem*)Sender)->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CHReditortoolbarbottomClick(TObject *Sender)
{
	FormCHREditor->GroupBox2->Align=alBottom;
	FormCHREditor->GroupBox1->Align=alTop;
	((TMenuItem*)Sender)->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::NTtimerTimer(TObject *Sender)
{
	if(!openByFileDone) return;

	if(cueUpdateNametable==true) {
		UpdateNameTable(-1,-1,true);
		if(FormNavigator->Visible) FormNavigator->CueChunkDraw->Enabled=true; //FormNavigator->CueDrawTimer->Enabled=true;   //performance thief when scaled. todo: divide and conquer.
		}
	else if(cueUpdateNametableNoRepaint==true)
	{
		UpdateNameTable(-1,-1,false);
		//FormNavigator->Draw(false);
		cueUpdateNametableNoRepaint=false;
		cueUpdateNametable=false;
	}
	cueUpdateNametable=false;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TileTimerTimer(TObject *Sender)
{
	 if(!openByFileDone) return;
	if(cueUpdateTiles==true) UpdateTiles(false);
	cueUpdateTiles=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CHRtimerTimer(TObject *Sender)
{
	if(!openByFileDone) return;
	if(cueCHRdraw==true) FormCHREditor->Draw(false);
	if(cueCHRdrawAll==true) FormCHREditor->Draw(true);
	cueCHRdraw=false;
	cueCHRdrawAll=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::N2x2tileeditmode1Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	FormCHREditor->btn2x2mode->Down^=true;
	FormCHREditor->btn2x2modeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::clearprotection1Click(TObject *Sender)
{
	FormCHREditor->Protect0->Down=false;
	FormCHREditor->Protect1->Down=false;
	FormCHREditor->Protect2->Down=false;
	FormCHREditor->Protect3->Down=false;

	Protect0->Down=false;
	Protect1->Down=false;
	Protect2->Down=false;
	Protect3->Down=false;

	LabelStats->Caption="Colour protections cleared.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Invertprotection1Click(TObject *Sender)
{
	FormCHREditor->Protect0->Down^=true;
	FormCHREditor->Protect1->Down^=true;
	FormCHREditor->Protect2->Down^=true;
	FormCHREditor->Protect3->Down^=true;

	Protect0->Down^=true;
	Protect1->Down^=true;
	Protect2->Down^=true;
	Protect3->Down^=true;

	LabelStats->Caption="Colour protection inverted.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SaveDialogMapTypeChange(TObject *Sender)
{
	//if(SaveDialogMap->FilterIndex==2) SaveDialogMap->FileName+=".rle";
	//else SaveDialogMap->FileName+=".map";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Lightboxmodetransparentform1Click(TObject *Sender)
{
	FormMain->AlphaBlend^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::LightboxmodeCHRtransparent1Click(TObject *Sender)
{
	FormCHREditor->AlphaBlend^=true;
	//FormManageMetasprites->AlphaBlend^=true;
	FormCHREditor->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::VisitWebClick(TObject *Sender)
{
	ShellExecute(NULL, "open", "https://frankengraphics.itch.io/nexxt", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Recallcolour1Click(TObject *Sender)
{
    if(SpeedButtonTypeIn->Down) return;
	int tmp;

	//bool b=(FormCHREditor->Active||FormBrush->Active||SpeedButtonDrawTile->Down);



		tmp=bgPalCur;
		bgPalCur=bgPalCurOld;
		bgPalCurOld=tmp;

		Colour001->Checked=false;
		Colour011->Checked=false;
		Colour101->Checked=false;
		Colour111->Checked=false;

		if (bgPalCur==0) Colour001->Checked=true;
		if (bgPalCur==1) Colour011->Checked=true;
		if (bgPalCur==2) Colour101->Checked=true;
		if (bgPalCur==3) Colour111->Checked=true;


	/*
	//maybe explore: find a good context where this can be done without feeling disruptive.
	if(!b)
	{
	   tmp=palActive;
	   palActive=palActiveOld;
	   palActiveOld=tmp;

	   Pal0->Checked=false;
	   Pal1->Checked=false;
	   Pal2->Checked=false;
	   Pal3->Checked=false;

	   if (palActive==0) Pal0->Checked=true;
	   if (palActive==1) Pal1->Checked=true;
	   if (palActive==2) Pal2->Checked=true;
	   if (palActive==3) Pal3->Checked=true;
	}
    */
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TogglePenBrush1Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	FormCHREditor->btnThick->Down^=true;
	btnThick->Down=FormCHREditor->btnThick->Down;
	TogglePenBrush1->Checked=FormCHREditor->btnThick->Down;

	if(btnThick->Down) LabelStats->Caption="Brush mode ON.";
	else			   LabelStats->Caption="Pen mode OFF.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Quantized1Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	FormCHREditor->btnQuant->Down^=true;
	btnQuant->Down=FormCHREditor->btnQuant->Down;
	Quantized1->Checked=FormCHREditor->btnQuant->Down;
	if(btnQuant->Down) LabelStats->Caption="Quantized pen/brush mode ON.";
	else			   LabelStats->Caption="Quantized pen/brush mode OFF.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Brushmask1Click(TObject *Sender)
{
	FormBrush->Visible^=true;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::About1Click(TObject *Sender)
{
	AboutBox->Show();	
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Swaptablesinselection1Click(TObject *Sender)
{
    FormSwapBanks->Show();

	/* legacied
	unsigned char temp[4096];
	uint32_t propt[512];

	int xTiles;  //number of tiles to process
	int yTiles;
	int pp = tileActive*16;
	int ta = tileActive;

	int i,j;
	int i16;
	SetUndo();

	if(chrSelectRect)
	{
		xTiles=tileSelRectWdt;
		yTiles=tileSelRectHgt;

		int x32=xTiles*sizeof(uint32_t);

		for (i=0; i < yTiles; i++) memcpy(temp+pp+i*256		,chr+pp+i*256		,xTiles*16);
		for (i=0; i < yTiles; i++) memcpy(chr+pp+i*256		,chr+4096+pp+i*256 ,xTiles*16);
		for (i=0; i < yTiles; i++) memcpy(chr+4096+pp+i*256	,temp+pp+i*256		,xTiles*16);

		for (i=0; i < yTiles; i++) {
				memcpy(&propt[ta+i*16]				,&tileProperties[ta+i*16]		,x32);}
		for (i=0; i < yTiles; i++)
				memcpy(&tileProperties[ta+i*16]		,&tileProperties[256+ta+i*16] 	,x32);
		for (i=0; i < yTiles; i++)
				memcpy(&tileProperties[256+ta+i*16]	,&propt[ta+i*16]					,x32);

	}
	else
	{
    	for(i=0;i<256;i++)
		{
			if(chrSelected[i])
			{
				memcpy(temp				,chr+i*16		,16);
				memcpy(chr+i*16			,chr+4096+i*16	,16);
				memcpy(chr+4096+i*16	,temp			,16);

				memcpy(propt					,&tileProperties[i]		,sizeof(uint32_t));
				memcpy(&tileProperties[i]		,&tileProperties[256+i]	,sizeof(uint32_t));
				memcpy(&tileProperties[256+i]	,propt					,sizeof(uint32_t));
			}
		}
	}
	UpdateTiles(true);
	//WorkCHRToBankCHR();
	UpdateNameTable(-1,-1,true);
	if (FormNavigator->Visible) FormNavigator->Draw(false,false);
	if (FormCHRbit->Visible) FormCHRbit->UpdateBitButtons(false);
	if(FormMTprops->Visible)FormMTprops->UpdateBitButtons_tileClick(false);
	cueUpdateMetasprite=true;
	*/
}
//---------------------------------------------------------------------------





void __fastcall TFormMain::ImageTilesEndDrag(TObject *Sender, TObject *Target,
      int X, int Y)
{
	bDrawDestShadow=false;
    bImageNameAccepted=false;
	bImageTileAccepted=false;
	cueUpdateTiles=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ImageNameDragOver(TObject *Sender, TObject *Source,
	  int X, int Y, TDragState State, bool &Accept)
{
	if(!FormMain->Active) return;
	Accept=false;
	if(PageControlEditor->ActivePage==TabSheetName&&Source==ImageName) if(!bOutsideSel) Accept=true;
	X=X/(8*uiScale);
	Y=Y/(8*uiScale);
	if(Accept==true)
	{
		bImageNameAccepted=true;
		bImageTileAccepted=false;
		if (!bOutsideSel)  //decided at mouse down-time
		{


			destRect.left	=nameSelection.left		+X-nxDown;
			destRect.right	=nameSelection.right	+X-nxDown;
			destRect.top	=nameSelection.top		+Y-nyDown;
			destRect.bottom	=nameSelection.bottom	+Y-nyDown;

			int cAlHgt = (viewPortHeight-nameTableHeight)/2;
			int cAlWdt = (viewPortWidth-nameTableWidth)/2;
			if  ( cAlHgt<0)cAlHgt=0;
			if  (cAlWdt<0) cAlWdt=0;

			for (int i=0; i<32; i++)  //long enough loop
			// I need to special-case if the canvas is smaller than the viewport.
				{
					if(destRect.left<0+cAlWdt)   	{	destRect.left++;
												destRect.right++;}
					if(destRect.right>viewPortWidth-cAlWdt)
											{	destRect.left--;
												destRect.right--;}
					if(destRect.top<0+cAlHgt)   	{	destRect.top++;
												destRect.bottom++;}
					if(destRect.bottom>viewPortHeight-cAlHgt)
											{	destRect.top--;
												destRect.bottom--;}
				}

		}
		else
		{
			destRect.left=X;
			destRect.top=X;
			destRect.right=Y+1;
			destRect.bottom=Y+1;
		}
		bDrawDestShadow=true;

		//cueUpdateNametableNoRepaint=true;
		cueUpdateTiles=false;
		NameLinesTimer->Enabled=true;
		//UpdateNameTable(-1,-1,true);
        cueStats=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageNameEndDrag(TObject *Sender, TObject *Target,
      int X, int Y)
{
	bDrawDestShadow=false;
	bImageNameAccepted=false;
	bImageTileAccepted=false;
	//cueUpdateNametable=true;
	UpdateNameTable(-1,-1,true);
	//FormNavigator->Draw(false,false);
	FormNavigator->UpdateLines(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageNameDragDrop(TObject *Sender, TObject *Source,
	  int X, int Y)
{
	bool bClone = ( bBufCtrl &&  !bBufShift && bBufAlt);
	bool bSwap	= (!bBufCtrl && !bBufShift && !bBufAlt);
	bool bMove	= ( bBufCtrl && !bBufShift && !bBufAlt);
	bool bOverride = false;

	X/=(8*uiScale);
	Y/=(8*uiScale);
	if(X<0||X>=(16*uiScale)||Y<0||Y>=(16*uiScale)) return;
	if(!FormMain->Active) return;
	int w=1;
	int h=1;


	if (!bOutsideSel) //if grabbed from inside selection, retain size. ImageNameDragDrop is only call while this is true, but in case a context changes it..
	{
		w=destRect.right-destRect.left;
		h=destRect.bottom-destRect.top;
	}
	//these are used by the preserve all tilenames; try preserve order method.
	unsigned char tempName;
	unsigned char tempAttr;

	//this array is used by the preserve structure; overwrite tilenames method.

	unsigned char tempTable[sizeof(w*h)];

	int dst = (destRect.top  + nameTableViewY ) * nameTableWidth
			 +  destRect.left+ nameTableViewX;



	int xSrc = nameSelection.left;//+ cAlWdt;//;
	int ySrc = nameSelection.top;//+ cAlHgt;// ;
	int xDst = destRect.left+nameTableViewX;
	int yDst = destRect.top+nameTableViewY;


	int src	= (ySrc  ) * nameTableWidth
			+ xSrc   ;

	bool bDiagonal=(xSrc!=xDst && ySrc!=yDst)?true:false;

	if(dst==src) return;
	if((SpeedButtonPal->Down==false) && (SpeedButtonTiles->Down==false))
		{bOverride=true;}

	//begin swap
	SetUndo();



	//--method 1: the "preserve all tilenames" fashioned from the CHR swap routine,
	//-----------
	// but if moving diagonally, structure is not intact which is important for the nametable.
	//diagonals therefore use a slightly better method below.
    //method 2 could replace this with minor modification now that it's in, but don't fix what works. 
	
	int offset;  	//gets loaded with an expression for sorting order.

	//bool xOdd = xDst%2?1:0;  //not in use.
	//bool yOdd = yDst%2?1:0;

	//----select sorting method
	for(int sy=0; sy<h*nameTableWidth; sy+=nameTableWidth) {
		for(int sx=0; sx<w; sx++) {
			if (ySrc==yDst) {
				if      (xSrc>xDst)  {offset=sx+(sy);}  //dir: east
				else if (xSrc==xDst) {return;}		   	//reduncancy; center; added for the note.
				else    			 {offset=((w-1))-sx+((h-1)*nameTableWidth)-(sy);} 		// west
			}
			else if (ySrc>yDst) {
				if      (xSrc==xDst) {offset=sx+(sy); }  //dir: north
				else if (xSrc>xDst)  {offset=sx+(sy); }  //dir: north-east
				else 	    		 {offset=((w-1))-sx+(sy);}   	//dir: north-west
			}
			else {
				if 	    (xSrc==xDst) {offset=sx+((h-1)*nameTableWidth)-(sy); } //south
				else if (xSrc>xDst) {offset=sx+((h-1)*nameTableWidth)-(sy); }  //south-east
				else    {offset=((w-1))-sx+((h-1)*nameTableWidth)-(sy); }      //south-west
			}

		 //-swap 1 tile for another.
		 if(!bDiagonal && (SpeedButtonTiles->Down || bOverride))  //the extra conditionals are because the loop does an extra row & line for attributes.
		 {
			//swap
			if(bSwap){
				memcpy(&tempName				   , &nameTable[dst+offset],1);
				memcpy(&nameTable	[dst+offset], &nameTable[src+offset] ,1);
				memcpy(&nameTable	[src+offset]  , &tempName			 ,1);
			}
			//move
			if(bMove){
				//memcpy(&tempName				   , &nameTable[dst+offset],1);
				memcpy(&nameTable	[dst+offset], &nameTable[src+offset] ,1);
				memcpy(&nameTable	[src+offset]  , &nullTile			 ,1);
			}
			//clone
			if(bClone){
				//memcpy(&tempName				   , &nameTable[dst+offset],1);
				memcpy(&nameTable	[dst+offset], &nameTable[src+offset] ,1);
				//memcpy(&nameTable	[src+offset]  , &tempName			 ,1);
			}

		 }
		}
	}


	if(bDiagonal  && (SpeedButtonTiles->Down || bOverride)){
	//--method 2: a simple copypaste swap with buffer.
	//----------
	//It seems to preserve the structure better in some cases.
	int offset;
	//-swap rows of tilenames

		if(bSwap){
			for (int i = 0; i < h; i++) {
				if (ySrc>yDst) offset=i*nameTableWidth;
				if (ySrc<yDst) offset=(h-1-i)*nameTableWidth;
				if (ySrc==yDst) return; //redundancy
				memcpy(&nameCopy[src+offset]	, &nameTable[src+offset],w);
			}
			for (int i = 0; i < h; i++) {
				if (ySrc>yDst) offset=i*nameTableWidth;
				if (ySrc<yDst) offset=(h-1-i)*nameTableWidth;
				if (ySrc==yDst) return; //redundancy
				memcpy(&nameTable	[src+offset], &nameTable[dst+offset],w);
			}
			for (int i = 0; i < h; i++) {
				if (ySrc>yDst) offset=i*nameTableWidth;
				if (ySrc<yDst) offset=(h-1-i)*nameTableWidth;
				if (ySrc==yDst) return; //redundancy
				memcpy(&nameTable	[dst+offset], &nameCopy[src+offset]	,w);
			}
		}
		if(bClone){
			for (int i = 0; i < h; i++) {
				if (ySrc>yDst) offset=i*nameTableWidth;
				if (ySrc<yDst) offset=(h-1-i)*nameTableWidth;
				if (ySrc==yDst) return; //redundancy
				memcpy(&nameTable	[dst+offset], &nameTable[src+offset],w);
			}
		}
		if(bMove)
		{
			for (int i = 0; i < h; i++) {
				if (ySrc>yDst) offset=i*nameTableWidth;
				if (ySrc<yDst) offset=(h-1-i)*nameTableWidth;
				if (ySrc==yDst) return; //redundancy
				memcpy(&nameCopy[dst+offset]	, &nameTable[src+offset],w);
			}
			for (int i = 0; i < h; i++) {
				if (ySrc>yDst) offset=i*nameTableWidth;
				if (ySrc<yDst) offset=(h-1-i)*nameTableWidth;
				if (ySrc==yDst) return; //redundancy
				//memcpy(&nameTable	[src+offset], nullTile	,w);
				for(int j = 0; j < w; j++) nameTable[src+offset+j]=nullTile;
			}
			for (int i = 0; i < h; i++) {
				if (ySrc>yDst) offset=i*nameTableWidth;
				if (ySrc<yDst) offset=(h-1-i)*nameTableWidth;
				if (ySrc==yDst) return; //redundancy
				memcpy(&nameTable	[dst+offset], &nameCopy[dst+offset],w);
			}
		}
	}

	//swap attributes
	if(SpeedButtonPal->Down||bOverride)
	{
	  int i,j,xo,yo;//,pp;
	  int n=1;

	  for (i=0;i<h;i++){
			for(j=0;j<w;++j){
				if (ySrc>yDst) yo=i;	if (ySrc<=yDst) yo=(h-n-i);
				if (xSrc>xDst) xo=j;	if (xSrc<=xDst) xo=(w-n-j);
				AttrSet(xo, yo, AttrGet(xSrc+xo,ySrc+yo,false,false),true);
			}
	  }
	  if(bSwap)
	  {
		for (i=0;i<h;i++){
			for(j=0;j<w;++j){
				if (ySrc>yDst) yo=i;	if (ySrc<=yDst) yo=(h-n-i);
				if (xSrc>xDst) xo=j;	if (xSrc<=xDst) xo=(w-n-j);
				AttrSet(xSrc+xo, ySrc+yo, AttrGet(xDst+xo,yDst+yo,false,false),false);
			}
		}
	  }
	  for (i=0;i<h;i++){
			for(j=0;j<w;++j){
				if (ySrc>yDst) yo=i;	if (ySrc<=yDst) yo=(h-n-i);
				if (xSrc>xDst) xo=j;	if (xSrc<=xDst) xo=(w-n-j);
				AttrSet(xDst+xo, yDst+yo, AttrGet(xo,yo,true,false),false);
			}
		}
	  
	}
    int dx,dy,dw,dh;
	FormNavigator->Map1->Picture->Bitmap->Assign(bufBmp);           
	GetSelection(nameSelection,dx,dy,dw,dh);
	FormNavigator->DrawRange(dx,dy,dw,dh,false);

	nameSelection.left 		= destRect.left 	+ nameTableViewX;
	nameSelection.top 		= destRect.top 		+ nameTableViewY;
	nameSelection.right 	= destRect.right 	+ nameTableViewX;
	nameSelection.bottom 	= destRect.bottom 	+ nameTableViewY;

	GetSelection(nameSelection,dx,dy,dw,dh);
	FormNavigator->DrawRange(dx,dy,dw,dh,false);


}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ImageBigTilesDblClick(TObject *Sender)
{
	//if(PageControlEditor->ActivePage==TabSheetTile)
	ImageTilesDblClick(Sender);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CHRpixelgrid1Click(TObject *Sender)
{
    TRect r;
	r.left=0;	r.top=0;	r.right=272+132;	r.Bottom=272+132;

	FormCHREditor->PaintBoxChr->Canvas->Brush->Color=FormCHREditor->GroupBox1->Color;
	FormCHREditor->PaintBoxChr->Canvas->FillRect(r);
	FormCHREditor->Draw(true);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MapNavigator1Click(TObject *Sender)
{
	//FormNavigator->Draw();
	//FormNavigator->FormActivate(Sender);

	if (FormNavigator->WindowState==wsMinimized) FormNavigator->WindowState=wsNormal;
	else FormNavigator->Visible^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonGridScreenMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
 LabelStats->Caption="[H] Toggles a screen-sized (32x30 tiles) grid resolution on/off.\n\nTip:\t[D] turns grid visibility on/off.";
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ImageNameMouseEnter(TObject *Sender)
{
	bMouseOverNam=true;
	UpdateNameTable(-1,-1,true);
	cueStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::AlwaysNavigator1Click(TObject *Sender)
{
	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::TextNullTileDblClick(TObject *Sender)
{
	char tmp = (char)nullTile;
	nullTile=tileActive;

	//set metatiles if empty
	bool madeChange=false;
	for(int i=0; i<metatileSets_2x2*64; i++) {
		if (AllValuesEqual(&metatileSet_2x2_id[i*4],   4, tmp))
			{memset(&metatileSet_2x2_id[i*4],nullTile,4);
			madeChange=true;
		}
	}
	for(int i=0; i<metatileSets_4x4*64; i++) {
		if (AllValuesEqual(&metatileSet_4x4_id[i*16], 16, tmp)) {
			memset(&metatileSet_4x4_id[i*16],nullTile,16);
			madeChange=true;
		}
	}
	for(int i=0; i<metatileSets_8x8*64; i++) {
		if (AllValuesEqual(&metatileSet_8x8_id[i*64], 64, tmp))
			{memset(&metatileSet_8x8_id[i*64],nullTile,64);
			madeChange=true;
		}
	}
	if(madeChange) if (FormMetatileTool->Visible) FormMetatileTool->DrawTimer->Enabled=true;

	TextNullTile->Caption="   null tile: $"+IntToHex(nullTile,2)+" ";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileClick(TObject *Sender)
{
   UpdateMenu();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::includenames1Click(TObject *Sender)
{
	MSaveIncName->Checked=includenames1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveIncNameClick(TObject *Sender)
{
   includenames1->Checked=MSaveIncName->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveIncAttrClick(TObject *Sender)
{
   includeattributes1->Checked=MSaveIncAttr->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::includeattributes1Click(TObject *Sender)
{
   MSaveIncAttr->Checked=includeattributes1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::forceNESlibRLEpacking1Click(TObject *Sender)
{
  MSaveRLE->Checked=forceNESlibRLEpacking1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MSaveRLEClick(TObject *Sender)
{
  forceNESlibRLEpacking1->Checked=MSaveRLE->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::NewNEXXTinstance1Click(TObject *Sender)
{

	ShellExecute(NULL, "Open", ParamStr(0).c_str(), "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Inverttileselection1Click(TObject *Sender)
{
	for(int i=0;i<256;++i) chrSelected[i]^=1;
	chrSelectRect=false;
    nameSelection.left=-1;
	nameSelection.top=-1;
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PPUdump1Click(TObject *Sender)
{
	FILE *file;
	unsigned char *tmp;
	int size;
    AnsiString ext;



	BlockDrawing(true);
	
	OpenDialogImport->Title="Import PPU RAM dump file";
	OpenDialogImport->Filter="NES PPU dump|*.bin|Nintendulator PPU dump|*.ppumem|All files (*.*)|*.*";
	OpenDialogImport->DefaultExt="bin";

	if(OpenDialogImport->Execute())
	{
		//FormBank->Caption="Import 8K CHR bank from NES file";
		//FormBank->FileName=OpenDialogImport->FileName;
		//FormBank->ShowModal();

		//if(FormBank->OK)

			file=fopen(OpenDialogImport->FileName.c_str(),"rb");
			ext=GetExt(OpenDialogImport->FileName);

			if(file)
			{
				fseek(file,0,SEEK_END);
				size=ftell(file);


				if (size<=0x4000 ) //full ppu address range dump or up until first mirror of palette ram.
				{
					 if (size>=0x2000 || ext=="ppumem") 	//patterns
					 {
						fseek(file,0,SEEK_SET);
						fread(chrBuf,8192,1,file);
					 }
					 if (size>=0x3000 || ext=="ppumem") 	//nametables
					 {
						nameTableWidth=64;
						nameTableHeight=60;
						tmp=(unsigned char*)malloc(0x1000);
						//load them as map.
						fseek(file,0x2000,SEEK_SET);
						fread(tmp,0x1000,1,file);
                        int v = 64;
						for (int i = 0; i < 30; i++) {
							memcpy(nameTable		   +i*v ,tmp			+i*32	,32);
							memcpy(nameTable		+32+i*v ,tmp +1024		+i*32	,32);
							memcpy(nameTable +960*2	   +i*v ,tmp +1024*2	+i*32	,32);
							memcpy(nameTable +960*2	+32+i*v ,tmp +1024*3	+i*32	,32);
						}
						for (int y = 0; y < 30; y+=2) {
							for (int x = 0; x < 32; x+=2) {
							//2 upper screens' attributes
							memcpy(attrTable+		(y/4*((nameTableWidth+3)/4)+x/4),tmp+960        +(y/4*((32+3)/4)+x/4),1);
							memcpy(attrTable+8+		(y/4*((nameTableWidth+3)/4)+x/4),tmp+960+1024   +(y/4*((32+3)/4)+x/4),1);
							//2 lower screens' attributes -put them in tmp to adjust later
							memcpy(attrCopy+128+	(y/4*((nameTableWidth+3)/4)+x/4),tmp+960+1024*2 +(y/4*((32+3)/4)+x/4),1);
							memcpy(attrCopy+128+8+	(y/4*((nameTableWidth+3)/4)+x/4),tmp+960+1024*3 +(y/4*((32+3)/4)+x/4),1);


						}
                        //correct lower screen attributes one half attribute strip up.
						for(int y=0;y<30;y++)
						{
							for(int x=0;x<64;x++)
							{
								AttrSet(x,30+y,AttrGet(x,32+y,true,false),false);
								
							}
						}


					 }

					 //palettes
					 if (ext=="ppumem")
					 {
						fseek(file,0x3100,SEEK_SET);
						fread(bgPal,32,1,file);
					 }
					 else if (size>=0x3F20)
					 {
						fseek(file,0x3F00,SEEK_SET);
						fread(bgPal,32,1,file);
					 }

					 //PPU OAM goes here when thereÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´s something actually using raw oam.
					 /*
						 if (ext=="ppumem")
						 {
							fseek(file,0x3000,SEEK_SET);
							fread(ppuOAM,256,1,file);
						 }
                     */

				}

				fclose(file);
			}
            int set=bankActive/16;
			for(int tile=0;tile<512;tile++){
				int pp=tileViewTable[tile]*16+bankViewTable[(tile+set)%(256*4)];
				memcpy(chr+pp,chrBuf+tile*16,16);
			}


			UpdateAll();
		}
	}
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::NameLinesTimerTimer(TObject *Sender)
{
     if(!openByFileDone) return;

	NameLinesTimer->Enabled=false;
	TimerNTstrip->Enabled=false;
 	cueUpdateNametable=false;
	

    bool bBigTile=(PageControlEditor->ActivePage==TabSheetTile?true:false)&&!bForceNameScale;
	TRect r;


	ImageName->Picture->Bitmap->Assign(BufBmpName);
	if(nameSelection.left>=0&&nameSelection.top>=0)
	{
		r.left  =nameSelection.left  -nameTableViewX;
		r.right =nameSelection.right -nameTableViewX;
		r.top   =nameSelection.top   -nameTableViewY;
		r.bottom=nameSelection.bottom-nameTableViewY;

		if(!bBigTile) DrawSelection(ImageName,r,uiScale,false,false);
        if(FormNavigator->Visible) FormNavigator->UpdateLines(true);
	}
}
//---------------------------------------------------------------------------




void __fastcall TFormMain::ImportBitmap1Click(TObject *Sender)
{
	bool swap[256*4];
	int i,j,k,pp,col,bit;
	int ib, is, bankOff;
	unsigned char paltemp[4];

	bool bNoAttr;
	bool bNoPal;
	bool bDensityThres;
	bool bMaxTiles;
	bool bLossy;
	bool bBestOffsets;
	int mode;
	int iMT;
	int iPT;

	BlockDrawing(true);

	OpenDialogImport->Title="Import BMP";
	OpenDialogImport->Filter="Windows bitmap files (*.bmp)|*.bmp|All files (*.*)|*.*";
	OpenDialogImport->DefaultExt="bmp";

	if(OpenDialogImport->Execute())
	{
		SetUndo();
        for(int set=0;set<4;set++){
			for(int tile=0;tile<256;tile++){
				memcpy(chrBuf+set*4096+tile*16,chr+bankViewTable[set*256 + tile]+tile*16,16);
			}               
		}
		//memcpy (chrBuf, chr, 4096*2);    //prep buffer so we can perform previews
		memcpy (palBuf, bgPal, 4*16);
		memcpy (tmpNameTable, nameTable, NAME_MAX_SIZE);
		memcpy (tmpAttrTable, attrTable, ATTR_MAX_SIZE);
		buf_nameWidth = nameTableWidth;
		buf_nameHeight = nameTableHeight;

		FormImportBMP->CheckLossy->Checked=false;
		FormImportBMP->ShowModal();

		if(!FormImportBMP->Swap)
		{
            for(int set=0;set<4;set++){
				for(int tile=0;tile<256;tile++){
					memcpy(chr+bankViewTable[set*256 + tile]+tile*16,chrBuf+set*4096+tile*16,16);
				}
			}
			//memcpy (chr, chrBuf, 4096*2);
			memcpy (bgPal, palBuf, 4*16);
			memcpy (nameTable, tmpNameTable, NAME_MAX_SIZE);
			memcpy (attrTable, tmpAttrTable, ATTR_MAX_SIZE);
			nameTableWidth = buf_nameWidth;
			nameTableHeight =buf_nameHeight;
		}
		
		if(FormImportBMP->Swap && !FormImportBMP->CheckBox1->Checked)
		{
            for(int set=0;set<4;set++){
				for(int tile=0;tile<256;tile++){
					memcpy(chr+bankViewTable[set*256 + tile]+tile*16,chrBuf+set*4096+tile*16,16);
				}
			}
			//memcpy (chr, chrBuf, 4096*2);  // restore.
			memcpy (bgPal, palBuf, 4*16);
			memcpy (nameTable, tmpNameTable, NAME_MAX_SIZE);
			memcpy (attrTable, tmpAttrTable, ATTR_MAX_SIZE);
			nameTableWidth = buf_nameWidth;
			nameTableHeight =buf_nameHeight;




			bNoAttr = FormImportBMP->CheckNoAttr->Checked;
			bNoPal = FormImportBMP->CheckNoPal->Checked;
			bDensityThres =FormImportBMP->CheckDensityThres->Checked;
			bLossy = FormImportBMP->CheckLossy->Checked;
			bBestOffsets = FormImportBMP->CheckBestOffsets->Checked;
			bMaxTiles = FormImportBMP->CheckMaxTiles->Checked;
			iMT = StrToInt(FormImportBMP->EditMaxTiles->Text);
			iPT = StrToInt(FormImportBMP->EditPxThres->Text);
			if(bLossy) mode=IMPORT_SQUEEZE;
			else mode=IMPORT_NORMAL;
			if(FormImportBMP->RadioMatched->Checked) mode=IMPORT_MATCH;

			//perform final import.
			if(ImportBMP(OpenDialogImport->FileName,mode,bBestOffsets,bDensityThres?iPT:-1,bMaxTiles?iMT:256,bNoAttr,bNoPal))
			{

			//perform palette+pattern swaps
			for(i=0;i<256*4;i++) swap[i]=false;
			if(FormImportBMP->Selection)
			{
				for(i=0;i<256;i++) swap[(bankActive/4096*256)+i]=chrSelected[i];
			}
			else
			{
				if(FormImportBMP->WholeCHR)
				{
					for(i=0;i<256*4;i++) swap[i]=true;
				}
				else
				{
					for(i=0;i<256;i++) swap[(bankActive/4096*256)+i]=true;
				}
			}

			for(i=0;i<256*4;i++)
			{
				if(swap[i])
				{
					pp=i*16;

					for(j=0;j<8;j++)
					{
						for(k=0;k<8;k++)
						{
							bit=1<<k;
							col=((chr[pp]&bit)?1:0)|((chr[pp+8]&bit)?2:0);
							col=FormImportBMP->Map[col];
							chr[pp]=(chr[pp]&~bit)|((col&1)<<k);
							chr[pp+8]=(chr[pp+8]&~bit)|((col>>1)<<k);
						}
						pp++;
					}
				}
			}

			if(FormImportBMP->RemapPalette)
			{
				if(FormImportBMP->RadioPalOne->Checked) {ib=iSwap_WhichSubpal; is=1; bankOff=palBank;}
					else if(FormImportBMP->RadioPalAll->Checked) {ib=0; is=4*4; bankOff=0;}
					else {ib=0;is=4;bankOff=palBank;}
					for(i=ib;i<is+ib;i++)
					{
						for(j=0;j<4;j++) paltemp[j]=bgPal[bankOff*16+i*4+FormImportBMP->Map[j]];
						for(j=0;j<4;j++) bgPal[bankOff*16+i*4+j]=paltemp[j];
					}
			}
			if(sharedCol0)
			{
			//this overrides the results of pal_validate by overwriting its input

				col=bgPal[palBank*16+iSwap_Pal0Subpal*4+0];

				bgPal[palBank*16+0*4+0]=col;
				bgPal[palBank*16+1*4+0]=col;
				bgPal[palBank*16+2*4+0]=col;
				bgPal[palBank*16+3*4+0]=col;
			}
			            /*
                pal_validate();
                
							UpdateTiles(true);
                			UpdateNameTable(-1,-1,true);
                			FormNavigator->Draw(false,false);
                			DrawPalettes();
                			//UpdateMetaSprite();
                			cueUpdateMetasprite=true;    
            */
		}
	}
	}

	pal_validate();
	CorrectView();
	UpdateTiles(true);

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	DrawPalettes();
	cueUpdateMetasprite=true;
	FormMain->CorrectView();
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Findclosestmatch1Click(TObject *Sender)
{
		int n,j;
		int set=bankActive/16;
		int max=0;
		int	tile1=0;
		int	tile2=0;
		int tile3=0;

		for(j=0;j<255;j++)
		{
			n=tile_lossy_compare(&chr[bankViewTable[set+tileActive]+(tileActive*16)],&chr[bankViewTable[set+j]+(j*16)]);

			if(n>max && tileActive!=j)
			{
				max=n;
				tile1=tile2;
				tile2=tile3;
				tile3=j;
			}
		}
	   chrSelected[ViewTablePos(tile1)]=1;
	   chrSelected[ViewTablePos(tile2)]=1;
	   chrSelected[ViewTablePos(tile3)]=1;

	   //turn off rect selection; instead displaying multi selection
	   chrSelectRect=false;
	   nameSelection.left=-1;
	   nameSelection.top=-1;

	   //update visuals
	   UpdateTiles(true);
	   UpdateNameTable(-1,-1,true);
	   if(FormNavigator->Visible) FormNavigator->UpdateLines(true);
	   UpdateStats();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SortbyDifference1Click(TObject *Sender)
{
	int set=bankActive/16;
	if(SortbyDifference1->Checked)
	{
		Undo();
		MCHRDensitySort->Checked=false;
		MCHRDensitySort->Caption="Sort by d&ifference";
	}
	else
	{
		int i,j,k,ptr,mask,temp;
		static int sort[256],freq[256];
		static unsigned char chrt[256*16];
		static unsigned char propt[256];
		SetUndo();

		for(i=0;i<256;++i)
		{
			sort[i]=i;
			freq[i]=0;

			ptr=i*16+bankViewTable[set+i];

			//tiles that have contents get a base point over tiles that don't.
			//this tidies up solid tiles with a single colour over empty tiles.
			freq[i]+=GetTileHasContents(&chr[ptr]);

			//vertical
			freq[i]+=count_diff_bytes(&chr[ptr],8,0xFF);
			freq[i]+=count_diff_bytes(&chr[ptr+8],8,0xFF);

			//horizontal
			for(j=0;j<16;++j)
			{

				//horizontal difference
				freq[i]+=count_diff_bits(chr[ptr]);
				++ptr;
			}
		}

		for(i=0;i<256-1;++i)
		{
			for(j=0;j<256-i-1;++j)
			{
				if(freq[j]<freq[j+1])
				{
					temp=freq[j];
					freq[j]=freq[j+1];
					freq[j+1]=temp;

					temp=sort[j];
					sort[j]=sort[j+1];
					sort[j+1]=temp;
				}
			}
		}
		//repair NT
		for(i=0;i<nameTableWidth*nameTableHeight;++i)
		{
			for(j=0;j<256;++j)
			{
				if(sort[j]==nameTable[i])
				{
					nameTable[i]=j;
					break;
				}
			}
		}
		//repair MSP.
		for(int i=0;i<256*64*4;i+=4)
		{
			for(j=0;j<256;++j)
			{
				if(sort[j]==metaSprites[i+1]) {metaSprites[i+1]=j; break;}
			}
		}

        for(i=0;i<256;++i)
		{
				memcpy(chrt+i*16,&chr[bankViewTable[set+i]+i*16],16);
				propt[i]=tileProperties[bankViewTable[set+i]/16+i];

		}
		//memcpy(chrt,&chr[bankActive],sizeof(chrt));
		//memcpy(propt,&tileProperties[bankActive/16],sizeof(propt));

		for(i=0;i<256;++i)
		{
            memcpy(&chr[bankViewTable[set+i] + i*16],&chrt[sort[i]*16],16);
			tileProperties[bankViewTable[set+i]+i]=propt[sort[i]];

			//memcpy(&chr[bankActive+i*16],&chrt[sort[i]*16],16);
			//memcpy(&tileProperties[(bankActive/16)+i],&propt[sort[i]],sizeof(uint32_t));
		}

		UpdateAll();
		MCHRDensitySort->Checked=true;
		MCHRDensitySort->Caption="Undo Sort by d&ifference";
		LabelStats->Caption="Tiles sorted by bitwise difference. \n\nTip: Press again to undo.";
		StatusUpdateWaiter->Enabled=true;
		holdStats=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Swapattributes1Click(TObject *Sender)
{
	//unsigned char paltemp[16*4];

	//we can also use ATTR_MAX_SIZE. In that case, remember to update the loop down below.
	int tmp_AttrSize = (nameTableWidth+3)/4*((nameTableHeight+3)/4);

	memcpy (palBuf, bgPal, 4*16);                      //buffer
	memcpy (tmpAttrTable, attrTable, tmp_AttrSize);
	memcpy (metaSpritesBuf, metaSprites,256*64*4);

	FormSwapAttributes->ShowModal();

	memcpy (bgPal, palBuf, 4*16);                      //restore
	memcpy (attrTable, tmpAttrTable, tmp_AttrSize);
	memcpy (metaSprites, metaSpritesBuf, 256*64*4);

	if(FormSwapAttributes->Swap)
	{
	   SetUndo();

		if(FormSwapColors->RemapPalette)
		{

			bool perform[4];
			for (int i = 0; i < 4; i++) perform[i]=false;

			if ((FormSwapAttributes->RadioPalAll->Checked) | (FormSwapAttributes->RadioPalOne->Checked)) {
				for (int i = 0; i < 4; i++) perform[i]=true;
				if (FormSwapAttributes->RadioPalOne->Checked) perform[palBank]=false;

			}
			if (FormSwapAttributes->RadioPalCurrent->Checked) {
				perform[palBank]=true;
			}



			//memcpy (paltemp,&bgPal,16*4);

			for (int i = 0; i < 4; i++) {   //iterates through the sets
				if (perform[i]){

					int set=i*16;

					for (int j = 0; j < 4; j++) {   //swaps subpals in a set
						memcpy (&bgPal[set+j*4],&palBuf[set+FormSwapAttributes->Map[j]*4],4);
					}
				}
			}

			if (FormSwapAttributes->RadioButton4K->Checked)//name inherited from palswap dialogue, really refers to "perform on entire table".
			{
				for (int i = 0; i < tmp_AttrSize; i++) {

					unsigned char result = 0;
					for (int j = 0; j < 8; j += 2) {
						result |= (FormSwapAttributes->Map[(tmpAttrTable[i] >> j)& 0x3 ] & 0x3) << j;
					}
					attrTable[i] = result;


					//another (computationally faster) way of expressing the same
					/*
					attrTable[i] =
					( ( FormSwapAttributes->Map[ (tmpAttrTable[i] & 0x3)]       & 0x3)
					| ((FormSwapAttributes->Map[((tmpAttrTable[i] & 0x3) << 2)] & 0x3) << 2)
					| ((FormSwapAttributes->Map[((tmpAttrTable[i] & 0x3) << 4)] & 0x3) << 4)
					| ((FormSwapAttributes->Map[((tmpAttrTable[i] & 0x3) << 6)] & 0x3) << 6)
					);
					*/

				}
			}
			else if (FormSwapAttributes->RadioButtonSelection->Checked)
			{
				int x,y,w,h;
				GetSelection(nameSelection,x,y,w,h);

				for(int i=0;i<h;i+=2)
				{
					for(int j=0;j<w;j+=2)
					{
						int tmp = FormSwapAttributes->Map[AttrGet(x+j,y+i,false,false)];
						AttrSet(x+j, y+i, tmp, false);

					}
				}
			}
			if (FormSwapAttributes->RadioSpritesAll->Checked)
			{
				for(int i=0;i<256;++i)
				{
					int off=i*64*4;
					for(int j=0;j<64;j+=4)
					{
						if(metaSprites[off+j]<255)
						{
							//reorder the 2 least significant bits of the sprite attribute
							unsigned char attr = metaSprites[off+j+2];
							metaSprites[off + j + 2] =
								(attr & 0xFC) | (FormSwapAttributes->Map[attr & 0x3]& 0x3);
						}
					}
				}
			}


		}

	}
	pal_validate();

	UpdateTiles(true);
	//WorkCHRToBankCHR();
	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);
	DrawPalettes();
	cueUpdateMetasprite=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Clearrogueattributes1Click(TObject *Sender)
{
	int count=0;
	SetUndo();
	for(int x=0; x<nameTableWidth; x+=2){
		for(int y=0; y<nameTableHeight; y+=2){

			for(int i=0; i<2; i++){
				for(int j=0; j<2; j++){
					if (GetTileHasContents(&chr[nameTable[(y+j)*nameTableWidth +(x+i) + bankActive]])) goto skip;

					}
				}


		if( palActive!=AttrGet(x,y,false,false)){
			count++;
			AttrSet(x, y, palActive, false);
		}
		skip:
		}
	}

	UpdateNameTable(-1,-1,true);
	FormNavigator->Draw(false,false,false);

	if(count==0) {
		LabelStats->Caption="No rogue attributes found.";
	}
	else {
		LabelStats->Caption=IntToStr(count)+" attribute cells set to current subpalette.";
	}

	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Normal1Click(TObject *Sender)
{
	AnsiString str;
	AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
	bool b = PageControlEditor->ActivePage==TabSheetTile?true:false;
	int a = ((TMenuItem*)Sender)->Tag;

	tileViewTagBuf = a; //used to recall from the metasprite editor.
	SetViewTable(a);

	if (a==0) {
		ButtonTileView->Down=false;

		if(b) {
			TabSheetTile->Caption=tmp_LinkCHRstr+"[F3] Tiles";
			GroupBoxTiles->Caption="Screen (view only)";
		}
		else {
			GroupBoxTiles->Caption=tmp_LinkCHRstr+"Tileset";
		}
	}
	else{
		//str=((TMenuItem*)Sender)->Caption;
		ButtonTileView->Down=true;
		if(b) {
			TabSheetTile->Caption=tmp_LinkCHRstr+"[F3] Tiles <O>";
			GroupBoxTiles->Caption="Screen (view only)";
		}
		else {
			str=tmp_LinkCHRstr+"Tileset ("+((TMenuItem*)Sender)->Caption+")";
			RemoveCharacter(str.c_str(), '&');
			GroupBoxTiles->Caption=str;
		}
	}
	SetCheckMenuItem(((TMenuItem*)Tilesetmode1),a);         //view menu correction
	SetCheckPopupMenuItem(((TPopupMenu*)TileViewPop),a);    //popup menu correction
}
//---------------------------------------------------------------------------
unsigned char __fastcall TFormMain::Get_8x16pos(unsigned char id)
{
	unsigned char y = id/16;
	unsigned char x = (id%32);

	unsigned char num= (y/2)*32 +(x/2);


	return num;

}

void __fastcall TFormMain::SetViewTable(int tag)
{
	//create 8x16 vs 8x8 indirection table

	int num;
	//reset
	for(int i=0;i<256;i++) {tileViewTable[i]=i;}

	//8x16 mode
	if(tag==1){
		for(int i=0;i<16;i++)
		{
			num=(i/2)*32+(i&1);
			for(int j=0;j<16;j++)
			{
				tileViewTable[i*16+j]=num;

				num+=2;
			}
		}
	}
	else if(tag==10) SetViewTable_SortFreqency();
	else if(tag==11) SetViewTable_SortDensity();
	else if(tag==12) SetViewTable_SortDetail();
	else if(tag==13) SetViewTable_SortEdgeDetail();
	else if(tag==14) SetViewTable_SortActiveColour();



	else if(tag==20) //4x1 -> 2x2 ; left to right blocks
	{
		SetViewTable_Destrip(2,true);
	}
	else if(tag==21) //4x1 -> 2x2 ; top down blocks
	{
		SetViewTable_Destrip(2,false);
	}
	else if(tag==22) //16x1 -> 4x4
	{
		SetViewTable_Destrip(4,true);
	}



	//normal / default
	else for(int i=0;i<256;i++) {tileViewTable[i]=i;}

	if(tag>0) ButtonTileView->Down=false;
	else ButtonTileView->Down=true;

	//UpdateMetaSprite();
	UpdateTiles(true);
	cueUpdateMetasprite=true;
	isLastClickedMetaSprite=true;
	isLastClickedSpriteList=false;


}

void __fastcall TFormMain::SetViewTable_Destrip(int block, bool reorder)
{
	 //block 2 means result is expected 2x2
	 //block 4 means result is expected 4x4
	 //the only other option that would make sense is 8 (64x1-->8x8)
	 int num;
	 int row = 16;
	 char tmp[256];
	 for(int x=0;x<16;x++){
		num=(x/block)*(block*row)+(x&(block-1));
		for(int y=0;y<16;y++){
			tmp[y*16+x]=num;
			num+=block;
		}
	 }


	 //arranges blocks left to right.

	 if(reorder){
		for(int x=0;x<16;x+=block){
			for(int y=0;y<16;y+=block){
				for(int bx=0;bx<block;bx++){
					for(int by=0;by<block;by++){
						tileViewTable[(x+by)*16+y+bx]=tmp[(y+by)*16+x+bx];
					}
				}
			}
		}
	}
	else {
		for (int i = 0; i < 256; i++) {
		   tileViewTable[i]=tmp[i];
		}
	}
}

void __fastcall TFormMain::SetViewTable_SortFreqency(void)
{
	 bool forceActive = ForceActiveTab1->Checked;
		bool sweepNT = IncludeNametables1->Checked;
		bool sweepMS = IncludeMetasprites1->Checked;

		int i,j,temp;
		int freq[256];

		memset(freq,0,sizeof(freq));

		if((forceActive)||!(sweepNT||sweepMS)) //if no sweep checkbox is set, also default to active.
		{
		   if(PageControlEditor->ActivePage==TabSheetSprite) {
                for(j=0;j<256*64*4;j+=4) if(metaSprites[j]!=0xFF) ++freq[metaSprites[j+1]];
			 }
		   else {
				for(i=0;i<nameTableHeight*nameTableWidth;++i) ++freq[nameTable[i]];
			}
		}
		else
		{
		   if(sweepNT) {
				for(i=0;i<nameTableHeight*nameTableWidth;++i) ++freq[nameTable[i]];
			}
		   if(sweepMS) {
				for(j=0;j<256*64*4;j+=4) if(metaSprites[j]!=0xFF) ++freq[metaSprites[j+1]];
			}
		}


		for(i=0;i<256;++i) tileViewTable[i]=i;

		for(i=0;i<256-1;++i)
		{
			for(j=0;j<256-i-1;++j)
			{
				if(freq[j]<freq[j+1])
				{
					temp=freq[j];
					freq[j]=freq[j+1];
					freq[j+1]=temp;

					temp=tileViewTable[j];
					tileViewTable[j]=tileViewTable[j+1];
					tileViewTable[j+1]=temp;
				}
			}
		}
}

void __fastcall TFormMain::SetViewTable_SortDensity(void)
{
	int i,j,k,ptr,mask,temp;
	int freq[256];



	for(i=0;i<256;++i)
	{
		tileViewTable[i]=i;
		freq[i]=0;

		ptr=i*16+bankActive;

		for(j=0;j<8;++j)
		{
			mask=128;

			for(k=0;k<8;++k)
			{
				if((chr[ptr]&mask)||(chr[ptr+8]&mask)) ++freq[i];

				mask>>=1;
			}

				++ptr;
		}
	}

	for(i=0;i<256-1;++i)
		{
		for(j=0;j<256-i-1;++j)
		{
			if(freq[j]<freq[j+1])
			{
				temp=freq[j];
				freq[j]=freq[j+1];
				freq[j+1]=temp;

				temp=tileViewTable[j];
				tileViewTable[j]=tileViewTable[j+1];
				tileViewTable[j+1]=temp;
			}
		}
	}
}

void __fastcall TFormMain::SetViewTable_SortDetail(void)
{
	int i,j,k,ptr,mask,temp;
	int freq[256];

	for(i=0;i<256;++i)
	{
		tileViewTable[i]=i;
		freq[i]=0;

		ptr=i*16+bankActive;

		//tiles that have contents get a base point over tiles that don't.
		//this tidies up solid tiles with a single colour over empty tiles.
		freq[i]+=GetTileHasContents(&chr[ptr]);

		//vertical
		freq[i]+=count_diff_bytes(&chr[ptr],8,0xFF);
		freq[i]+=count_diff_bytes(&chr[ptr+8],8,0xFF);

		//horizontal
		for(j=0;j<16;++j)
		{

			//horizontal difference
			freq[i]+=count_diff_bits(chr[ptr]);
			++ptr;
		}
	}

	for(i=0;i<256-1;++i)
	{
		for(j=0;j<256-i-1;++j)
		{
			if(freq[j]<freq[j+1])
			{
				temp=freq[j];
				freq[j]=freq[j+1];
				freq[j+1]=temp;

				temp=tileViewTable[j];
				tileViewTable[j]=tileViewTable[j+1];
				tileViewTable[j+1]=temp;
			}
		}
	}

}

void __fastcall TFormMain::SetViewTable_SortEdgeDetail(void)
{
   int i,j,k,ptr,mask,temp;
   int	freq[256];

	for(i=0;i<256;++i)
	{
		tileViewTable[i]=i;
		freq[i]=0;

		ptr=i*16+bankActive;

		//tiles that have contents get a base point over tiles that don't.
		//this tidies up solid tiles with a single colour over empty tiles.
		freq[i]+=GetTileHasContents(&chr[ptr]);

		//gives a relatively strong score for difference: max 56
		//vertical
		freq[i]+=count_diff_bytes(&chr[ptr],8,0x81);
		freq[i]+=count_diff_bytes(&chr[ptr+8],8,0x81);

		//horizontal difference
		freq[i]+=count_diff_bits(chr[ptr]);
		freq[i]+=count_diff_bits(chr[ptr+7]);
		freq[i]+=count_diff_bits(chr[ptr+8]);
		freq[i]+=count_diff_bits(chr[ptr+15]);


		//

		//gives a weaker modification for solidity. max 28
		int imask=128;

		for(k=0;k<8;++k)
		{
			if((chr[ptr]&imask)||(chr[ptr+8]&imask)) ++freq[i];
			if((chr[ptr+7]&imask)||(chr[ptr+15]&imask)) ++freq[i];

			if(((chr[ptr+1]&0x81)&imask)||((chr[ptr+9]&0x81)&imask)) ++freq[i];
			if(((chr[ptr+2]&0x81)&imask)||((chr[ptr+10]&0x81)&imask)) ++freq[i];
			if(((chr[ptr+3]&0x81)&imask)||((chr[ptr+11]&0x81)&imask)) ++freq[i];
			if(((chr[ptr+4]&0x81)&imask)||((chr[ptr+12]&0x81)&imask)) ++freq[i];
			if(((chr[ptr+5]&0x81)&imask)||((chr[ptr+13]&0x81)&imask)) ++freq[i];
			if(((chr[ptr+6]&0x81)&imask)||((chr[ptr+14]&0x81)&imask)) ++freq[i];

				imask>>=1;
			}
	}


   for(i=0;i<256-1;++i)
	{
		for(j=0;j<256-i-1;++j)
		{
			if(freq[j]<freq[j+1])
			{
				temp=freq[j];
				freq[j]=freq[j+1];
				freq[j+1]=temp;

				temp=tileViewTable[j];
				tileViewTable[j]=tileViewTable[j+1];
				tileViewTable[j+1]=temp;
			}
		}
	}

}

void __fastcall TFormMain::SetViewTable_SortActiveColour(void)
{
	int i,j,k,ptr,mask,temp;
	int freq[256];



	for(i=0;i<256;++i)
	{
		tileViewTable[i]=i;
		freq[i]=0;

		ptr=i*16+bankActive;

		for(j=0;j<8;++j)
		{
			mask=128;

			for(k=0;k<8;++k)
			{
				switch(bgPalCur){
				case 0:
					if(((~chr[ptr])&mask)&&((~chr[ptr+8])&mask)) ++freq[i];
					break;
				case 1:
					if((chr[ptr]&mask)&&((~chr[ptr+8])&mask)) ++freq[i];
					break;
				case 2:
					if(((~chr[ptr])&mask)&&(chr[ptr+8]&mask)) ++freq[i];
					break;
				case 3:
					if((chr[ptr]&mask)&&(chr[ptr+8]&mask)) ++freq[i];
					break;
				}

				mask>>=1;
			}

				++ptr;
		}
	}

	for(i=0;i<256-1;++i)
		{
		for(j=0;j<256-i-1;++j)
		{
			if(freq[j]<freq[j+1])
			{
				temp=freq[j];
				freq[j]=freq[j+1];
				freq[j+1]=temp;

				temp=tileViewTable[j];
				tileViewTable[j]=tileViewTable[j+1];
				tileViewTable[j+1]=temp;
			}
		}
	}

}




void __fastcall TFormMain::ButtonTileViewMouseEnter(TObject *Sender)
{
	LabelStats->Caption="View mode:\t Lets you view and edit the tileset in arrangements\nfreed from physical memory layout.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;

}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ButtonTileViewMouseLeave(TObject *Sender)
{
	LabelStats->Caption="---";
	holdStats=false;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ButtonTileViewClick(TObject *Sender)
{
	TPoint p = Mouse->CursorPos;
	int x= p.x; //+ ButtonTileView->Left+12;
	int y= p.y; //+ ButtonTileView->Top+8;
	TileViewPop->Popup(x,y);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Mirror1Click(TObject *Sender)
{
	int set=bankActive/16;
	int x,y,w,h;
	int i,j,k,l;
	int pw,ph;
	int ps; //source
	int pd; //dest
	int pp;
	int hMid,wMid;
	//int wOdd;
	//int hOdd;
	int tmp;    //sliver
	char tmpTile[16]; //tile
	char midMask; //midtile bitmask
	char vMask[16];	//used by vertical masks


	int fLen=16;
	int fOff=0;

	//used by colour masks
	//char mask[16]; //colour protect mask
	/*
	bool b0 = FormCHREditor->Protect0->Down;
	bool b1 = FormCHREditor->Protect1->Down;
	bool b2 = FormCHREditor->Protect2->Down;
	bool b3 = FormCHREditor->Protect3->Down;
	*/

	//apply bitplane masks
	if (!(FormCHREditor->ButtonBitmaskLo->Down||FormCHREditor->ButtonBitmaskHi->Down))
		{if(Applytopaste1->Checked) return;}
	if (!FormCHREditor->ButtonBitmaskLo->Down&&Applytopaste1->Checked) {fLen-=8; fOff=8;}
	if (!FormCHREditor->ButtonBitmaskHi->Down&&Applytopaste1->Checked) {fLen-=8;}

	int tag = ((TMenuItem*)Sender)->Tag;



	if(chrSelectRect)
	{
		SetUndo();
		GetSelection(chrSelection,x,y,w,h);
		//if (w%2!=0) wOdd=1; else wOdd=0;
		//if (h%2!=0) hOdd=1; else hOdd=0;

		if((tag==0) || (tag==2)) //Horz mirror
		{
			for(i=0;i<h;i++){
				for(j=0;j<w;j++){
					for(k=0;k<16;k++){
						ps=tileViewTable[set+(x+j) + (y+i)*16] + tileViewTable[(x+j) + (y+i)*16]*16 + k;
						chrCopy[ps]=chr[ps];
					}
				}
			}

			//mirror pixels horz
			for (i = 0; i < h; i++){
				for (j = 0; j < w; j++){
					for(k=0;k<16;k++){
						tmp=0;
						ps=bankViewTable[set+(x+j) + (y+i)*16] + tileViewTable[(x+j) + (y+i)*16]*16 + k;
						for(l=0;l<8;l++){
							tmp|=(chrCopy[ps]&(128>>l))?1<<l:0;
						}
						chrCopy[ps]=tmp;
					}
				}
			}

			//paste in reverse horz order;

			ph=0;
			for(i=0;i<h;i++)   ////2-hOdd    //(i=h-1;i>=h/2+hOdd;i--)
			{
				pw=0;
				for(j=w-1;j>=w/2;j--)  //+wOdd;
				{
					if(x+j<16&&y+i<16)  //safety for making sure selection isnÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´t larger than table.
					{
						if((j==0) || (pw==w/2)) midMask=0x0F; else midMask=0xFF;
						ps=bankViewTable[set+(x+pw) + (y+i)*16] + tileViewTable[(x+pw) + (y+i)*16]*16;
						for(k=0;k<fLen;k++)  //pasting tile
						{
							pd=bankViewTable[set+(x+j) + (y+ph)*16] + tileViewTable[(x+j) + (y+ph)*16]*16 + k;
							chr[pd+fOff]=(chr[pd+fOff]&(~midMask));
							chr[pd+fOff]=chr[pd+fOff]|(chrCopy[ps+k+fOff]&midMask);
						}
					}
				pw++;
				}
			ph++;
			}
		}

		if((tag==1) || (tag==2)) //Vert mirror
		{
			//make buffer
			for(i=0;i<h;i++){
				for(j=0;j<w;j++){
					for(k=0;k<16;k++){
						ps=bankViewTable[set+(x+j) + (y+i)*16] + tileViewTable[(x+j) + (y+i)*16]*16 + k;
						chrCopy[ps]=chr[ps];
					}
				}
			}

			//mirror pixels vert
			for (i = 0; i < h; i++){
				for (j = 0; j < w; j++){
					
					pp=bankViewTable[set+(x+j) + (y+i)*16] + tileViewTable[(x+j) + (y+i)*16]*16;

					for(k=0;k<16;k++){
						tmpTile[k]=chrCopy[pp];
						pp++;
					}
					for(k=0;k<16;k++){
						pp--;
						chrCopy[pp]=tmpTile[k];
					}
				}
			}

			//paste in reverse vert order;

			ph=0;
			for(i=h-1;i>=h/2;i--)
			{
				pw=0;
				for(j=0;j<w;j++)            
				{
					if(x+j<16&&y+i<16)  //safety for making sure selection isnÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â´t larger than table.
					{
						if((i==0) || (ph==h/2)) midMask=0x00; else midMask=0xFF;
						for(k=0;k<4;k++) {vMask[k]=midMask;  vMask[k+8]=midMask;}
						for(k=4;k<8;k++) {vMask[k]=0xFF;     vMask[k+8]=0xFF;}

						ps=bankViewTable[set+(x+j) + (y+ph)*16] + tileViewTable[(x+j) + (y+ph)*16]*16;
						for(k=0;k<fLen;k++)  //pasting tile
						{


							pd=bankViewTable[set+(x+j) + (y+i)*16] + tileViewTable[(x+j) + (y+i)*16]*16 + k;
							chr[pd+fOff]=(chr[pd+fOff]&(~vMask[k+fOff]));
							chr[pd+fOff]=chr[pd+fOff]|(chrCopy[ps+k+fOff]&vMask[k+fOff]);
						}
					}
				}
			ph++;
			}
		}
	}
	else //multiselect behaviour. if we want that.
	{

	}
	UpdateAll();
    /*
	UpdateTiles(true);
	UpdateNameTable(-1,-1,true);
	if (FormNavigator->Visible) FormNavigator->Draw(false,false);
	cueUpdateMetasprite=true; */
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Linetooldetails1Click(TObject *Sender)
{
	FormLineDetails->Visible^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnLineMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	//if(Shift.Contains(ssRight))
	if(!FormLineDetails->Visible) FormLineDetails->Show();

		
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnLineClick(TObject *Sender)
{
	if(Sender==btnLine){FormCHREditor->btnLine->Down = btnLine->Down;
    	lineToolY_toggleperm=0;
		Linemode1->Checked=btnLine->Down;
	}
	if(Sender==btnQuant){FormCHREditor->btnQuant->Down = btnQuant->Down;
		Quantized1->Checked=btnQuant->Down;
	}
	if(Sender==btnThick){FormCHREditor->btnThick->Down = btnThick->Down;
		TogglePenBrush1->Checked=btnThick->Down;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButton1UpClick(TObject *Sender)
{
	if(Sender==SpeedButton1Left)
		FormCHREditor->ScrollHorz(true);
	if(Sender==SpeedButton1Right)
		FormCHREditor->ScrollHorz(false);
	if(Sender==SpeedButton1Up)
		FormCHREditor->ScrollVert(false);
	if(Sender==SpeedButton1Down)
		FormCHREditor->ScrollVert(true);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButton1UpMouseEnter(TObject *Sender)
{
	AnsiString str;
	bool b =(FormBrush->Active|FormCHREditor->Active);
	if (b)	str="[WASD] Nudges pattern of the selected tile(s) ";
	else str="Nudges pattern of the selected tile(s) ";
	if(Sender==SpeedButton1Up) str+="up.";
	if(Sender==SpeedButton1Down) str+="down.";
	if(Sender==SpeedButton1Left) str+="left.";
	if(Sender==SpeedButton1Right) str+="right.";
	str +="\nWraparound occurs at the seams of the selection.";
	FormMain->LabelStats->Caption=str;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Protect0MouseEnter(TObject *Sender)
{
	int n;
	AnsiString str;
	if(Sender==Protect0) n=0;
	if(Sender==Protect1) n=1;
	if(Sender==Protect2) n=2;
	if(Sender==Protect3) n=3;
	str="Protects pixels of colour "+IntToStr(n)+" from being drawn over."
		+"\n\n[Click] to Toggle.\t[Ctrl+click] to enable all but."
		+"\n[Shift + click] to enable all.\t[Alt+click] to 'solo'.";
	FormMain->LabelStats->Caption=str;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Protect0MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(Shift.Contains(ssAlt) && !Shift.Contains(ssCtrl)) //set everyone else
	{
			Protect0->Down=true;    //bc click applies after, this one is inverted
			Protect1->Down=true;
			Protect2->Down=true;
			Protect3->Down=true;
	}
	if(Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt)) //clear everyone else
	{
		if(Protect0->Down && (Protect1->Down||Protect2->Down||Protect3->Down))
		{
			Protect0->Down=false; //force always down, except if others are already up

		}
		Protect1->Down=false;
		Protect2->Down=false;
		Protect3->Down=false;
	}
	if(Shift.Contains(ssShift))
	{

		Protect1->Down=false;
		Protect2->Down=false;
		Protect3->Down=false;
		if(!Protect0->Down)
		{
			Protect0->Down=true; //force always up

		}
	}	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonHFlipClick(TObject *Sender)
{
	if(!lineDrawing)FormCHREditor->MirrorHorizontal();	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonHFlipMouseEnter(TObject *Sender)
{
	AnsiString str;
	str="[";
	//if (!b) str+="Shift+";      //nope.
	if(Sender==SpeedButtonHFlip) str+="H] ";
	if(Sender==SpeedButtonVFlip) str+="V] ";
	if(Sender==SpeedButtonRotateCCW) str+="L] ";
	if(Sender==SpeedButtonRotateCW) str+="R] ";


	str+="Flips the pattern of the selected tile(s) ";

	if(Sender==SpeedButtonHFlip) str+="\nhorizontally.";
	if(Sender==SpeedButtonVFlip) str+="\nvertically.";
	if(Sender==SpeedButtonRotateCCW) str+="\ncounter clockwise.";
	if(Sender==SpeedButtonRotateCW) str+="\nclockwise.";

	if(FormCHREditor->btn2x2mode->Down) str +="\nIn 2x2 mode; tiles also swap places so long as 2x2 tiles are box selected.";
	FormMain->LabelStats->Caption=str;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonVFlipClick(TObject *Sender)
{
	if(!lineDrawing)FormCHREditor->MirrorVertical();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonRotateCCWClick(TObject *Sender)
{
	if(!lineDrawing){
	FormCHREditor->Flip90(true);
	FormCHREditor->Rotate4tiles(true);

	FormCHREditor->Draw(true);

	UpdateNameTable(-1,-1,true);
	UpdateTiles(false);
	//WorkCHRToBankCHR();
	UpdateMetaSprite(true);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonRotateCWClick(TObject *Sender)
{
	if(!lineDrawing){
	FormCHREditor->Flip90(false);
	FormCHREditor->Rotate4tiles(false);
	FormCHREditor->Draw(true);

	UpdateNameTable(-1,-1,true);
	UpdateTiles(false);
	//WorkCHRToBankCHR();
	UpdateMetaSprite(true);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ButtonBitmaskLoMouseEnter(TObject *Sender)
{
	AnsiString str;
	if(Sender==ButtonBitmaskLo && ButtonBitmaskLo->Down)  str="Disable editing the Low bitplane.";
	if(Sender==ButtonBitmaskLo && !ButtonBitmaskLo->Down) str="Enable editing the Low bitplane.";
	if(Sender==ButtonBitmaskHi && ButtonBitmaskHi->Down)  str="Disable editing the High bitplane.";
	if(Sender==ButtonBitmaskHi && !ButtonBitmaskHi->Down) str="Enable editing the High bitplane.";

	str+="\nNES patterns are stored in 2 bitplanes.\nIt can sometimes be useful to edit them separately.\nSee [Menu->Draw->Bitmask options...] for options.";
	FormMain->LabelStats->Caption=str;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonDoWrapMouseEnter(TObject *Sender)
{
	FormMain->LabelStats->Caption="When down, the +/- buttons will wrap pixels\nat max colour around to min,or vise versa.\n\nWhen up, the buttons cap at max / min value.";	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CHRIncMouseEnter(TObject *Sender)
{
	AnsiString str;
	if(Sender==CHRInc) {str="Increments pixel value in the selected tile(s).";

		str+="\n\nWhen a subpalette is ordered dark to bright,\nthis will make tiles brighter.";
		}
	if(Sender==CHRDec) {str="Decrements pixel value in the selected tile(s).";

		str+="\n\nWhen a subpalette is ordered dark to bright,\nthis will make tiles darker.";
		}
	FormMain->LabelStats->Caption=str;		
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::CHRIncClick(TObject *Sender)
{
	 FormCHREditor->CHRIncDec(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnQuantMouseEnter(TObject *Sender)
{
	FormMain->LabelStats->Caption="[U] Makes pen (etc) valid only for even coordinates.\n\nTip:\tWith brush, it's great for creating dither,\n\t'chunky pixels' or striped line patterns.";
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::btnThickMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(Shift.Contains(ssRight))	FormBrush->Show();

	FormMain->TogglePenBrush1->Checked=btnThick->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnThickMouseEnter(TObject *Sender)
{
	FormMain->LabelStats->Caption="[B] Toggles between Pen and Brush tool \non the [left mouse button].\nTip:\tRight-click this button or press [Shift-F4] \n\tto open brush shape editor.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnSmudgeMouseEnter(TObject *Sender)
{
	FormMain->LabelStats->Caption="Uses the clicked pixel's colour when drawing.\nGood for defining borders/clusters.\nHolding [G] enables smudge momentarily.\nLifting [G] always releases this button.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnSmudgeClick(TObject *Sender)
{
	bSmudge=btnSmudge->Down; FormCHREditor->btnSmudge->Down=btnSmudge->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonDoWrapClick(TObject *Sender)
{
	FormCHREditor->SpeedButtonDoWrap->Down=SpeedButtonDoWrap->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Protect0Click(TObject *Sender)
{
	FormCHREditor->Protect0->Down=Protect0->Down;
	FormCHREditor->Protect1->Down=Protect1->Down;
	FormCHREditor->Protect2->Down=Protect2->Down;
	FormCHREditor->Protect3->Down=Protect3->Down;

	Protectcolour01->Checked=Protect0->Down;
	Protectcolour11->Checked=Protect1->Down;
	Protectcolour21->Checked=Protect2->Down;
	Protectcolour31->Checked=Protect3->Down;


}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ButtonBitmaskLoClick(TObject *Sender)
{
	FormCHREditor->ButtonBitmaskHi->Down= ButtonBitmaskHi->Down;
	FormCHREditor->ButtonBitmaskLo->Down= ButtonBitmaskLo->Down;

    ButtonBitmaskLoMouseEnter(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::CHRBankSelector1Click(TObject *Sender)
{
	if (FormBankCHR->WindowState==wsMinimized) FormNavigator->WindowState=wsNormal;
	else FormBankCHR->Visible^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CHRcollision1Click(TObject *Sender)
{
	FormCHRbit->Show();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Savepropertiescurrentset1Click(TObject *Sender)
{
   BlockDrawing(true);
   SaveTprop(tileActive,256);
   BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Savepropertiesbothsets1Click(TObject *Sender)
{
  BlockDrawing(true);
  SaveTprop(tileActive,512);
  BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Loadproperties1Click(TObject *Sender)
{
	if(!OpenDialogTprop->Execute()) return;

	BlockDrawing(true);

	if(OpenTprop(OpenDialogTprop->FileName, false))
	{
		OpenDialogTprop->FileName=RemoveExt(OpenDialogTprop->FileName);

		SaveDialogTprop->FileName=OpenDialogTprop->FileName;
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Swappatterncolours1Click(TObject *Sender)
{

	//int setActive=bankActive/16;
	bool swap[256*4];
	int i,j,k,pp,col,bit;
	int ib, is, bankOff;
	unsigned char paltemp[4];
	//int curSet=bankActive/4096;
	for(int set=0;set<4;set++){
		for(int tile=0;tile<256;tile++){
			memcpy(chrBuf+(set*4096)+(tile*16),chr+bankViewTable[set*256 + tile]+tile*16,16);
		}
	}
	//memcpy (chrBuf, chr, 4096*4);    //prep buffer so we can perform previews

	FormSwapPatternColour->ShowModal();

	// restore.
	//memcpy (chr, chrBuf, 4096*4);
	for(int set=0;set<4;set++){
		for(int tile=0;tile<256;tile++){
			memcpy(chr+bankViewTable[set*256 + tile]+tile*16,chrBuf+set*4096+tile*16,16);
		}
	}


	if(FormSwapPatternColour->Swap)
	{
		SetUndo();

		for(i=0;i<256*4;i++) swap[i]=false;

		if(FormSwapPatternColour->Selection)
		{
			for(int set=0;set<4;set++){
				if(bankActive/4096 == set || FormSwapPatternColour->WholeCHR==true){
					for(i=0;i<256;i++) swap[(set*256)+i]=chrSelected[i];
				}
			}
		}
		else
		{
			if(FormSwapPatternColour->WholeCHR)
			{
				for(i=0;i<256*4;i++) swap[i]=true;
			}
			else
			{

				for(i=0;i<256;i++) swap[(bankActive/16)+i]=true;
			}
		}

		for(int set=0;set<4;set++){
			for(i=0;i<256;i++)
			{
				if(swap[set*256 +i])
				{
					pp=bankViewTable[set*256+i]+i*16;

					for(j=0;j<8;j++)
					{
						for(k=0;k<8;k++)
						{
							bit=1<<k;
							col=((chr[pp]&bit)?1:0)|((chr[pp+8]&bit)?2:0);
							col=FormSwapPatternColour->Map[col];
							chr[pp]=(chr[pp]&~bit)|((col&1)<<k);
							chr[pp+8]=(chr[pp+8]&~bit)|((col>>1)<<k);
						}
						pp++;
					}
				}
			}
		}
	}
	pal_validate();
	UpdateAll();/*
	UpdateTiles(true);
	//WorkCHRToBankCHR();
	UpdateNameTable(-1,-1,true);
	if(FormNavigator->Visible) FormNavigator->Draw(false,false);
	DrawPalettes();
	//UpdateMetaSprite();
	cueUpdateMetasprite=true;
	*/
}
//---------------------------------------------------------------------------






void __fastcall TFormMain::SpeedButtonMarqTileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	bDrawMarqSwitch = SpeedButtonMarqTile->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMarqTileMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if(bDrawMarqSwitch) SpeedButtonDrawTile->Down^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonDrawTileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   bDrawMarqSwitch = SpeedButtonDrawTile->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonDrawTileMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
   if(bDrawMarqSwitch) SpeedButtonMarqTile->Down^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PaintBoxPalMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageBigTilesMouseEnter(TObject *Sender)
{
	if(uiScale>1 && PageControlEditor->ActivePage==TabSheetTile)
	{	GroupBoxTileControl->Visible=true;}
	else
	{	GroupBoxTileControl->Visible=false;	}	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnLineMouseEnter(TObject *Sender)
{
	FormMain->LabelStats->Caption="[N] Toggles between Pen and Line tool \non the [left mouse button].\nTip:\tClicking or pressing[Shift-F5] \n\topens the Line toolbox.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Metatiletool1Click(TObject *Sender)
{
	FormMetatileTool->Visible^=true;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Openmetaspriteframe1Click(TObject *Sender)
{
	FILE *file;
	AnsiString name;

	BlockDrawing(true);

    SaveDialogMetasprite->FileName=metaSpriteNames[metaSpriteActive];
	if(SaveDialogMetasprite->Execute())
	{
		name=RemoveExt(SaveDialogMetasprite->FileName)+".oam";

		if(!OverwritePrompt(name))
		{
			BlockDrawing(false);
			return;
		}

		file=fopen(name.c_str(),"wb");

		if(file)
		{
			fwrite(metaSprites+(metaSpriteActive*64*4),64*4,1,file);
			fclose(file);
		}
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Savesinglesubpal1Click(TObject *Sender)
{

	FILE *file;
	unsigned char pal[4];
	int i;
	AnsiString name;
	AnsiString tmp;
	BlockDrawing(true);

	tmp=SaveDialogPal->FileName;
	if(SaveDialogPal->Execute())
	{
		name=RemoveExt(SaveDialogPal->FileName)+".pal";


		if(!OverwritePrompt(name))
		{
			SaveDialogPal->FileName=tmp;
			BlockDrawing(false);
			return;
		}

		pal_validate();

		for(i=0;i<4;i++)
		{
			pal[i+ 0]=bgPal[palBank*16+palActive*4+i];
		}

		file=fopen(name.c_str(),"wb");

		if(file)
		{
			fwrite(pal,4,1,file);
			fclose(file);
		}
	}
	SaveDialogPal->FileName=tmp;
	BlockDrawing(false);

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Save4xsetsto1file1Click(TObject *Sender)
{
	FILE *file;
	unsigned char pal[64];
	int i;
	AnsiString name;
	AnsiString tmp;
	BlockDrawing(true);

	tmp=SaveDialogPal->FileName;
	if(SaveDialogPal->Execute())
	{
		name=RemoveExt(SaveDialogPal->FileName)+".pal";


		if(!OverwritePrompt(name))
		{
			SaveDialogPal->FileName=tmp;
			BlockDrawing(false);
			return;
		}

		pal_validate();

		for(i=0;i<64;i++)
		{
			pal[i]=bgPal[i];
		}

		file=fopen(name.c_str(),"wb");

		if(file)
		{
			fwrite(pal,64,1,file);
			fclose(file);
		}
	}
	SaveDialogPal->FileName=tmp;
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Save2setsto1file1Click(TObject *Sender)
{
	FILE *file;
	unsigned char pal[32];

	AnsiString name;
	AnsiString tmp;
	BlockDrawing(true);

	tmp=SaveDialogPal->FileName;

	FormSelect2subpalSets->ShowModal();
	if(FormSelect2subpalSets->bProceed==true)
	{

		if(SaveDialogPal->Execute())
		{
			name=RemoveExt(SaveDialogPal->FileName)+".pal";


			if(!OverwritePrompt(name))
			{
				SaveDialogPal->FileName=tmp;
				BlockDrawing(false);
				return;
			}

			pal_validate();
            int a,b;
			if(FormSelect2subpalSets->btn1a->Down) a=0;
			else if(FormSelect2subpalSets->btn1b->Down) a=1;
			else if(FormSelect2subpalSets->btn1c->Down) a=2;
			else if(FormSelect2subpalSets->btn1d->Down) a=3;

			if(FormSelect2subpalSets->btn2a->Down) b=0;
			else if(FormSelect2subpalSets->btn2b->Down) b=1;
			else if(FormSelect2subpalSets->btn2c->Down) b=2;
			else if(FormSelect2subpalSets->btn2d->Down) b=3;

			for(int i=0;i<16;i++)
			{
				pal[i]		=bgPal[a * 16,i];
				pal[i+16]   =bgPal[b * 16,i];
			}

			//done to silence warnings.
				a=b;
				b=a;
			//---

			file=fopen(name.c_str(),"wb");

			if(file)
			{
				fwrite(pal,32,1,file);
				fclose(file);
			}
		}
		SaveDialogPal->FileName=tmp;
	}

	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Deriveandsave512bpalette1Click(TObject *Sender)
{
	FormEmphasisPalette->Show();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SpeedButtonCheckerMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(Shift.Contains(ssRight)||Shift.Contains(ssCtrl))	FormAttrChecker->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MetatilesToBMP(AnsiString tmp_filename)
{
	//assumes BlockDrawing(true) is set and cleared by the caller.
	TPicture *picture;
	Graphics::TBitmap *bmp;

	bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
	bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
	bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;
	int metasOnRow=8; //sqrt(iSetView);
	int tmpi_id,tmpi_pal;
	int x,y,d,id,pal;
	int tmpSets;

	AnsiString str=RemoveExt(tmp_filename);

	if(str=="") str="metatiles";

	str+=".bmp";

		picture=new TPicture();
		bmp=new Graphics::TBitmap();


		if(bTab2x2) {
			d=2;
			tmpSets=metatileSets_2x2;
		}

		if(bTab4x4) {
			d=4;
			tmpSets=metatileSets_4x4;
		}

		if(bTab8x8) {
			d=8;
			tmpSets=metatileSets_8x8;
		}


		bmp->SetSize(metasOnRow*d*8, metasOnRow*d*8*tmpSets);
		bmp->PixelFormat=pf4bit;
		SetBMPPalette(bmp);
		picture->Bitmap=bmp;


		x=0;

		for(int i=0;i<tmpSets;i++)
		{
			y=8*d*metasOnRow*i;
			for(int j=0;j<64;j++) //metas in set
			{
				for(int k=0;k<d;k++) //tiles Ã¯Â¿Â½n meta
				{
					for(int l=0;l<d;l++) //tiles Ã¯Â¿Â½n meta
					{
						tmpi_id=(l*d) + k + (j*d*d) + (i*64*d*d);
						if(bTab2x2) id= (metatileSet_2x2_id[tmpi_id]);
						if(bTab4x4) id= (metatileSet_4x4_id[tmpi_id]);
						if(bTab8x8) id= (metatileSet_8x8_id[tmpi_id]);

						if(FormMetatileTool->btnUseAttr->Down==false) pal=palActive; //change this for paletted metas
						else{
							//Extract 16x16px attributes out of 8x8px attr mode.
							tmpi_pal=((l*d)/(d*2))*(d*2) + (k/2)*2 + (j*d*d) + (i*64*d*d);
							if(bTab2x2) pal= (metatileSet_2x2_pal[tmpi_pal]);
							if(bTab4x4) pal= (metatileSet_4x4_pal[tmpi_pal]);
							if(bTab8x8) pal= (metatileSet_8x8_pal[tmpi_pal]);
						}


						int xOff = x + k*8;
						int yOff = y + l*8;

						DrawExportTile16(picture,xOff,yOff,id,pal,false);
					}
				}
				x+=8*d;
				if(x>=8*d*metasOnRow) {x=0; y+=8*d;}
			}
		}

		picture->SaveToFile(str);

		delete bmp;
		delete picture;




}

//---------------------------------------------------------------------------

void __fastcall TFormMain::Metatiles1Click(TObject *Sender)
{
	TPicture *picture;
	Graphics::TBitmap *bmp;

	bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
	bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
	bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;
	int metasOnRow=8; //sqrt(iSetView);
	int tmpi_id,tmpi_pal;
	int x,y,d,id,pal;
	int tmpSets;
	AnsiString str;

	BlockDrawing(true);

	str=RemoveExt(SaveDialogSession->FileName);

	if(str=="") str="metatiles";

	SaveDialogImage->FileName=str+".bmp";

	if(SaveDialogImage->Execute())
	{
		picture=new TPicture();
		bmp=new Graphics::TBitmap();


		if(bTab2x2) {
			d=2;
			tmpSets=metatileSets_2x2;
		}

		if(bTab4x4) {
			d=4;
			tmpSets=metatileSets_4x4;
		}

		if(bTab8x8) {
			d=8;
			tmpSets=metatileSets_8x8;
		}


		bmp->SetSize(metasOnRow*d*8, metasOnRow*d*8*tmpSets);
		bmp->PixelFormat=pf4bit;
		SetBMPPalette(bmp);
		picture->Bitmap=bmp;


		x=0;

		for(int i=0;i<tmpSets;i++)
		{
			y=8*d*metasOnRow*i;
			for(int j=0;j<64;j++) //metas in set
			{
				for(int k=0;k<d;k++) //tiles in meta
				{
					for(int l=0;l<d;l++) //tiles in meta
					{
						tmpi_id=(l*d) + k + (j*d*d) + (i*64*d*d);
						if(bTab2x2) id= (metatileSet_2x2_id[tmpi_id]);
						if(bTab4x4) id= (metatileSet_4x4_id[tmpi_id]);
						if(bTab8x8) id= (metatileSet_8x8_id[tmpi_id]);

						if(FormMetatileTool->btnUseAttr->Down==false) pal=palActive; //change this for paletted metas
						else{
							//Extract 16x16px attributes out of 8x8px attr mode.
							tmpi_pal=((l*d)/(d*2))*(d*2) + (k/2)*2 + (j*d*d) + (i*64*d*d);
							if(bTab2x2) pal= (metatileSet_2x2_pal[tmpi_pal]);
							if(bTab4x4) pal= (metatileSet_4x4_pal[tmpi_pal]);
							if(bTab8x8) pal= (metatileSet_8x8_pal[tmpi_pal]);
						}


						int xOff = x + k*8;
						int yOff = y + l*8;

						DrawExportTile16(picture,xOff,yOff,id,pal,false);
					}
				}
				x+=8*d;
				if(x>=8*d*metasOnRow) {x=0; y+=8*d;}
			}
		}

		picture->SaveToFile(SaveDialogImage->FileName);

		delete bmp;
		delete picture;
	}

	BlockDrawing(false);
}

//---------------------------------------------------------------------------


void __fastcall TFormMain::Metatilecollision1Click(TObject *Sender)
{
	FormMTprops->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Pipelinehelper1Click(TObject *Sender)
{
	FormPipelineHelper->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CollisionpropertyIDs1Click(TObject *Sender)
{
	FormPropID->Show();
}
//---------------------------------------------------------------------------
bool __fastcall TFormMain::EnterLinkedCHRmode(AnsiString filename)
{
	//check file validity and prep file.
	size_t tmpSize;
	FILE *file;
	char *text;
	int i,j,size;

	file=fopen(filename.c_str(),"rb");

	if(!file) return false;

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	text=(char*)malloc(size+1);
	text[size]=0;

	fread(text,size,1,file);
	fclose(file);



	//reset
	curViewCHRbankCanvas2=0;
	//swap to buffer

	chrBanks_LinkBuf 					= chrBanks;
	undo_chrBanks_LinkBuf 				= undo_chrBanks;
	checkpoint_chrBanks_LinkBuf			= checkpoint_chrBanks;
	undo_checkpoint_chrBanks_LinkBuf	= undo_checkpoint_chrBanks;



	chrBanks					=nss_get_int(text,size,"VarCHRBanks=");

	if(CheckTag(text,size,"VarCHRBanksUndo=")){
		undo_chrBanks               =nss_get_int(text,size,"VarCHRBanksUndo=");
	} else { undo_chrBanks=chrBanks; }

	if(CheckTag(text,size,"VarCheckpoint_CHRBanks=")){
		checkpoint_chrBanks         =nss_get_int(text,size,"VarCheckpoint_CHRBanks=");
	} else { checkpoint_chrBanks = chrBanks; }

	if(CheckTag(text,size,"VarCheckpoint_CHRBanksUndo=")){
	undo_checkpoint_chrBanks    =nss_get_int(text,size,"VarCheckpoint_CHRBanksUndo=");
	} else { checkpoint_chrBanks = chrBanks; }


    bool bGenerateCHRBanks = ((chrBanks==0)
		||(undo_chrBanks==0)
		||(checkpoint_chrBanks==0)
		||(undo_checkpoint_chrBanks==0)
		)? true:false;



	if(chrBanks<4) chrBanks=4;
	if(undo_chrBanks<4) chrBanks=4;
	if(checkpoint_chrBanks<4) chrBanks=4;
	if(undo_checkpoint_chrBanks<4) chrBanks=4;


	//labels

	chrBankLabels_LinkBuf.clear();
	undo_chrBankLabels_LinkBuf.clear();
	checkpoint_chrBankLabels_LinkBuf.clear();
	undo_checkpoint_chrBankLabels_LinkBuf.clear();


	chrBankLabels_LinkBuf 					 = chrBankLabels;
	undo_chrBankLabels_LinkBuf               = undo_chrBankLabels;
	checkpoint_chrBankLabels_LinkBuf         = checkpoint_chrBankLabels;
	undo_checkpoint_chrBankLabels_LinkBuf    = undo_checkpoint_chrBankLabels;

	if(bGenerateCHRBanks){    //make a generic setup
		//memcpy(chrBank,chr,4096*2);
		//memcpy(undo_chrBank,undoChr,4096*2);
		//memcpy(checkpoint_chrBank,checkpointChr,4096*2);
		//memcpy(undo_checkpoint_chrBank,undoCheckpointChr,4096*2);

		chrBankLabels.clear();
		undo_chrBankLabels.clear();
		checkpoint_chrBankLabels.clear();
		undo_checkpoint_chrBankLabels.clear();


		for(unsigned int k=0;k<chrBanks;k++){chrBankLabels.push_back("Unlabeled");}
		for(unsigned int k=0;k<undo_chrBanks;k++){undo_chrBankLabels.push_back("Unlabeled");}
		for(unsigned int k=0;k<checkpoint_chrBanks;k++){checkpoint_chrBankLabels.push_back("Unlabeled");}
		for(unsigned int k=0;k<undo_checkpoint_chrBanks;k++){undo_checkpoint_chrBankLabels.push_back("Unlabeled");}


	}
	else{
	if(CheckTag(text,size, "CHRBankLabels=")) chrBankLabels = deserializeVector(text, "CHRBankLabels=");
		else {
			chrBankLabels.clear();
			for(unsigned int k=0;k<chrBanks;k++){chrBankLabels.push_back("Unlabeled");}
		}
		if(CheckTag(text,size, "CHRBankLabelsUndo=")) undo_chrBankLabels = deserializeVector(text, "CHRBankLabelsUndo=");
		else{
			undo_chrBankLabels.clear();
			for(unsigned int k=0;k<undo_chrBanks;k++){undo_chrBankLabels.push_back("Unlabeled");}
		}

		if(CheckTag(text,size, "Checkpoint_CHRBankLabels=")) checkpoint_chrBankLabels = deserializeVector(text, "Checkpoint_CHRBankLabels=");
		else{
			checkpoint_chrBankLabels.clear();
			for(unsigned int k=0;k<checkpoint_chrBanks;k++){checkpoint_chrBankLabels.push_back("Unlabeled");}
		}

		if(CheckTag(text,size, "Checkpoint_CHRBankLabelsUndo=")) undo_checkpoint_chrBankLabels = deserializeVector(text, "Checkpoint_CHRBankLabelsUndo=");
		else{
			undo_checkpoint_chrBankLabels.clear();
			for(unsigned int k=0;k<undo_checkpoint_chrBanks;k++){undo_checkpoint_chrBankLabels.push_back("Unlabeled");}

		}
	}
	//tmp_chrBankLabels.clear();


	//banks

	tmpSize = max(chrBanks*4096,chrBanks_LinkBuf*4096);

	chr 					= (char*)realloc(chr,tmpSize);
	chr_LinkBuf 			= (char*)realloc(chr_LinkBuf,tmpSize);
	tileProperties		 	= (char*)realloc(tileProperties,tmpSize/16);
	tileProperties_LinkBuf  = (char*)realloc(tileProperties_LinkBuf,tmpSize/16);

	mem_exchange(chr_LinkBuf,chr,tmpSize);
	mem_exchange(tileProperties_LinkBuf,tileProperties,tmpSize/16);

	tmpSize = max(undo_chrBanks*4096,undo_chrBanks_LinkBuf*4096);
	undoChr 						  = (char*)realloc(undoChr,tmpSize);
	undoChr_LinkBuf 				  = (char*)realloc(undoChr_LinkBuf,tmpSize);
	tilePropertiesUndo                = (char*)realloc(tilePropertiesUndo,tmpSize/16);
	tilePropertiesUndo_LinkBuf        = (char*)realloc(tilePropertiesUndo_LinkBuf,tmpSize/16);



	mem_exchange(undoChr_LinkBuf,undoChr,tmpSize);
	mem_exchange(tilePropertiesUndo_LinkBuf,tilePropertiesUndo,tmpSize/16);

	

	tmpSize = max(checkpoint_chrBanks_LinkBuf*4096,checkpoint_chrBanks*4096);
	checkpointChr 						= (char*)realloc(checkpointChr,tmpSize);
	checkpointChr_LinkBuf 				= (char*)realloc(checkpointChr_LinkBuf,tmpSize);
	checkpointTileProperties  			= (char*)realloc(checkpointTileProperties,tmpSize/16);
	checkpointTileProperties_LinkBuf  	= (char*)realloc(checkpointTileProperties_LinkBuf,tmpSize/16);

	mem_exchange(checkpointChr_LinkBuf,checkpointChr,tmpSize);
	mem_exchange(checkpointTileProperties_LinkBuf,checkpointTileProperties,tmpSize/16);


	tmpSize = max(undo_checkpoint_chrBanks_LinkBuf*4096,undo_checkpoint_chrBanks*4096);
	undoCheckpointChr 						=  (char*)realloc(undoCheckpointChr,tmpSize);
	undoCheckpointChr_LinkBuf 				=  (char*)realloc(undoCheckpointChr_LinkBuf,tmpSize);
	checkpointTilePropertiesUndo 			=  (char*)realloc(checkpointTilePropertiesUndo,tmpSize/16);
	checkpointTilePropertiesUndo_LinkBuf 	=  (char*)realloc(checkpointTilePropertiesUndo_LinkBuf,tmpSize/16);

	mem_exchange(undoCheckpointChr_LinkBuf,undoCheckpointChr,tmpSize);
	mem_exchange(checkpointTilePropertiesUndo_LinkBuf,checkpointTilePropertiesUndo,tmpSize/16);


	nss_get_bytes(text,size,"CHRMain="    ,chr          ,chrBanks*4096);
	nss_get_bytes(text,size,"CHRUndo="    ,undoChr      ,undo_chrBanks*4096);
	nss_get_bytes(text,size,"Checkpoint_CHRMain="  ,checkpointChr          ,checkpoint_chrBanks*4096);
	nss_get_bytes(text,size,"Checkpoint_CHRUndo="  ,undoCheckpointChr      ,undo_checkpoint_chrBanks*4096);


	//related collision properties.
	if(CheckTag(text,size, "32_TileCollisionProperties=")){
			nss_get_uint32s(text,size,"32_TileCollisionProperties=",tileProperties32,sizeof(tileProperties32));
			memset(tileProperties,0,chrBanks*256);
			for (size_t i = 0; i < sizeof(tileProperties32) / sizeof(int); i++) {
				tileProperties[i] = static_cast<char>(tileProperties32[i] & 0xFF);
			}
	}
	else {
		memset(tileProperties32,0,sizeof(tileProperties32));
		if(CheckTag(text,size, "8_TileCollisionProperties=")){
			nss_get_bytes(text,size,"8_TileCollisionProperties=",tileProperties,chrBanks*256);
		}
		else memset(tileProperties,0,chrBanks*256);
	}

	if(CheckTag(text,size, "32_TileCollisionPropertiesUndo=")){
			nss_get_uint32s(text,size,"32_TileCollisionPropertiesUndo=",tilePropertiesUndo32,sizeof(tilePropertiesUndo32));
			memset(tilePropertiesUndo,0,undo_chrBanks*256);
			for (size_t i = 0; i < sizeof(tilePropertiesUndo32) / sizeof(int); i++) {
				tilePropertiesUndo[i] = static_cast<char>(tilePropertiesUndo32[i] & 0xFF);
			}
	}
	else {
		memset(tilePropertiesUndo32,0,sizeof(tilePropertiesUndo32));
		if(CheckTag(text,size, "8_TileCollisionPropertiesUndo=")){
			nss_get_bytes(text,size,"8_TileCollisionPropertiesUndo=",tilePropertiesUndo,undo_chrBanks*256);
		}
		else memset(tilePropertiesUndo,0,undo_chrBanks*256);
	}
    if(CheckTag(text,size, "Checkpoint_32_TileCollisionProperties=")){
			nss_get_uint32s(text,size,"Checkpoint_32_TileCollisionProperties=",checkpointTileProperties32,sizeof(checkpointTileProperties32));
			memset(checkpointTileProperties,0,checkpoint_chrBanks*256);
			for (size_t i = 0; i < sizeof(checkpointTileProperties32) / sizeof(int); i++) {
				checkpointTileProperties[i] = static_cast<char>(checkpointTileProperties32[i] & 0xFF);
			}
	}
	else  {
		memset(checkpointTileProperties32,0,sizeof(checkpointTileProperties32));
		if(CheckTag(text,size, "Checkpoint_8_TileCollisionProperties=")){
			nss_get_bytes(text,size,"Checkpoint_8_TileCollisionProperties=",checkpointTileProperties,checkpoint_chrBanks*256);
		}
		else memset(checkpointTileProperties,0,checkpoint_chrBanks*256);
	}


	if(CheckTag(text,size, "Checkpoint_32_TileCollisionPropertiesUndo=")){
			nss_get_uint32s(text,size,"Checkpoint_32_TileCollisionPropertiesUndo=",checkpointTilePropertiesUndo32,sizeof(checkpointTilePropertiesUndo32));
			memset(checkpointTilePropertiesUndo,0,undo_checkpoint_chrBanks*256);
			for (size_t i = 0; i < sizeof(checkpointTilePropertiesUndo32) / sizeof(int); i++) {
				checkpointTilePropertiesUndo[i] = static_cast<char>(checkpointTilePropertiesUndo32[i] & 0xFF);
			}
	}
	else  {
		memset(checkpointTilePropertiesUndo32,0,sizeof(checkpointTilePropertiesUndo32));
		if(CheckTag(text,size, "Checkpoint_8_TileCollisionPropertiesUndo=")){
			nss_get_bytes(text,size,"Checkpoint_8_TileCollisionPropertiesUndo=",checkpointTilePropertiesUndo,undo_checkpoint_chrBanks*256);
		}
		else memset(checkpointTilePropertiesUndo,0,undo_checkpoint_chrBanks*256);
	}
	bLinkedCHRmode=true;
	TimerLinkedModeRefresh->Enabled=true;
	UpdateAll();

	AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
	if(PageControlEditor->ActivePage==TabSheetTile){
		TabSheetTile->Caption=tmp_LinkCHRstr+"[F3] Tiles";
		GroupBoxTiles->Caption="Screen (view only)";
	}

	else
	{

		GroupBoxTiles->Caption=tmp_LinkCHRstr+"Tileset";
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ExitLinkedCHRmode(void)
{

	chrBanks 					= chrBanks_LinkBuf;
	undo_chrBanks 				= undo_chrBanks_LinkBuf;
	checkpoint_chrBanks			= checkpoint_chrBanks_LinkBuf;
	undo_checkpoint_chrBanks	= undo_checkpoint_chrBanks_LinkBuf;

	//labels

	chrBankLabels.clear();
	undo_chrBankLabels.clear();
	checkpoint_chrBankLabels.clear();
	undo_checkpoint_chrBankLabels.clear();

	chrBankLabels 					 = chrBankLabels_LinkBuf;
	undo_chrBankLabels               = undo_chrBankLabels_LinkBuf;
	checkpoint_chrBankLabels         = checkpoint_chrBankLabels_LinkBuf;
	undo_checkpoint_chrBankLabels    = undo_checkpoint_chrBankLabels_LinkBuf;

	//banks
	int tmpSize = max(chrBanks*4096,chrBanks_LinkBuf*4096);
	chr 					= (char*)realloc(chr,tmpSize);
	chr_LinkBuf 			= (char*)realloc(chr_LinkBuf,tmpSize);
	tileProperties 			= (char*)realloc(tileProperties,tmpSize);
	tileProperties_LinkBuf 	= (char*)realloc(tileProperties_LinkBuf,tmpSize);

	mem_exchange(chr_LinkBuf,chr,tmpSize);
	mem_exchange(tileProperties_LinkBuf,tileProperties,tmpSize);

	
	tmpSize = max(undo_chrBanks*4096,undo_chrBanks_LinkBuf*4096);
	undoChr 					= (char*)realloc(undoChr,tmpSize);
	undoChr_LinkBuf 			= (char*)realloc(undoChr_LinkBuf,tmpSize);
	tilePropertiesUndo 			= (char*)realloc(tilePropertiesUndo,tmpSize/16);
	tilePropertiesUndo_LinkBuf 	= (char*)realloc(tilePropertiesUndo_LinkBuf,tmpSize/16);

	mem_exchange(undoChr_LinkBuf,undoChr,tmpSize);
	mem_exchange(tilePropertiesUndo_LinkBuf,tilePropertiesUndo,tmpSize/16);

	tmpSize = max(checkpoint_chrBanks_LinkBuf*4096,checkpoint_chrBanks*4096);
	checkpointChr 						= (char*)realloc(checkpointChr,tmpSize);
	checkpointChr_LinkBuf 				= (char*)realloc(checkpointChr_LinkBuf,tmpSize);
	checkpointTileProperties 			= (char*)realloc(checkpointTileProperties,tmpSize/16);
	checkpointTileProperties_LinkBuf 	= (char*)realloc(checkpointTileProperties_LinkBuf,tmpSize/16);

	mem_exchange(checkpointChr_LinkBuf,checkpointChr,tmpSize);
	mem_exchange(checkpointTileProperties_LinkBuf,checkpointTileProperties,tmpSize/16);


	tmpSize = max(undo_checkpoint_chrBanks_LinkBuf*4096,undo_checkpoint_chrBanks*4096);
	undoCheckpointChr 						= (char*)realloc(undoCheckpointChr,tmpSize);
	undoCheckpointChr_LinkBuf 				= (char*)realloc(undoCheckpointChr_LinkBuf,tmpSize);
	checkpointTilePropertiesUndo 			= (char*)realloc(checkpointTilePropertiesUndo,tmpSize/16);
	checkpointTilePropertiesUndo_LinkBuf 	= (char*)realloc(checkpointTilePropertiesUndo_LinkBuf,tmpSize/16);

	mem_exchange(undoCheckpointChr_LinkBuf,undoCheckpointChr,tmpSize);
	mem_exchange(checkpointTilePropertiesUndo_LinkBuf,checkpointTilePropertiesUndo,tmpSize/16);

	/*
	mem_32exchange(tileProperties_LinkBuf					,tileProperties				,512);
	mem_32exchange(tilePropertiesUndo_LinkBuf				,tilePropertiesUndo			,512);
	mem_32exchange(checkpointTileProperties_LinkBuf 		,checkpointTileProperties	,512);
	mem_32exchange(checkpointTilePropertiesUndo_LinkBuf     ,checkpointTileProperties	,512);
	*/

	bLinkedCHRmode=false;
    TimerLinkedModeRefresh->Enabled=true;
	UpdateAll();

	AnsiString tmp_LinkCHRstr = bLinkedCHRmode? "!":"";
	if(PageControlEditor->ActivePage==TabSheetTile){
		TabSheetTile->Caption=tmp_LinkCHRstr+"[F3] Tiles";
		GroupBoxTiles->Caption="Screen (view only)";
	}

	else
	{

		GroupBoxTiles->Caption=tmp_LinkCHRstr+"Tileset";
	}
}

bool __fastcall TFormMain::OpenMetatiles(AnsiString filename,bool mt2,bool mt4,bool mt8)
{
	FILE *file;
	char *text;
	unsigned char temp[16];
	int i,j,filetype,size;
	bool r;

	file=fopen(filename.c_str(),"rb");

	if(!file){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+filename;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);
			return false;
	}
	fread(temp,sizeof(sessionIDStr),1,file);

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	text=(char*)malloc(size+1);
	text[size]=0;

	fread(text,size,1,file);
	fclose(file);

	filetype= 1;

	if(!memcmp(temp,sessionIDStr,8)) filetype=2;    //session text file
	if(!memcmp(temp,metatiletxtIDstr,8)) filetype=3; //metasprite text file

	if (filetype==1) {
		Application->MessageBox("Not the expected text file.\nPlease check in text editor.","Error",MB_OK);
		return false;
	}

	if(mt2){
		mtClickID_store_2x2=nss_get_int(text,size,"VarMtClickID_store_2x2=");
		metatileSets_2x2     			=nss_get_int(text,size,"VarMetatileSets_2x2=");

		metatileSet_2x2_id 					  = (char*) realloc(metatileSet_2x2_id, metatileSets_2x2*META_2x2);
		metatileSet_2x2_pal				      = (char*) realloc(metatileSet_2x2_pal, metatileSets_2x2*META_2x2);
		metatileSet_2x2_props				  = (char*) realloc(metatileSet_2x2_props, metatileSets_2x2*META_2x2);

		mtContent_2x2                         = (char*) realloc(mtContent_2x2, 64*metatileSets_2x2);
		//mtUsage_2x2                           = (uint32_t*) realloc(mtUsage_2x2, 64*metatileSets_2x2*sizeof(uint32_t));

		nss_get_bytes(text,size,"MetatileSet_2x2_id=",metatileSet_2x2_id, metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"MetatileSet_2x2_pal=", metatileSet_2x2_pal, metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"MetatileSet_2x2_props=", metatileSet_2x2_props, metatileSets_2x2*META_2x2);


		if(CheckTag(text,size, "MetatileSetLabels_2x2=")) metatileSetLabels_2x2 = deserializeVector(text, "MetatileSetLabels_2x2=");
		else {
			metatileSetLabels_2x2.clear();
			for( int k=0;k<metatileSets_2x2;k++){metatileSetLabels_2x2.push_back("Unlabeled");}
		}
		tmp_metatileSetLabels_2x2.clear();
	}

	if(mt4){
		mtClickID_store_4x4=nss_get_int(text,size,"VarMtClickID_store_4x4=");
		metatileSets_4x4				=nss_get_int(text,size,"VarMetatileSets_4x4=");

		metatileSet_4x4_id 					  = (char*) realloc(metatileSet_4x4_id, metatileSets_4x4*META_4x4);
		metatileSet_4x4_pal				      = (char*) realloc(metatileSet_4x4_pal, metatileSets_4x4*META_4x4);
		metatileSet_4x4_props				  = (char*) realloc(metatileSet_4x4_props, metatileSets_4x4*META_4x4);

		mtContent_4x4                         = (char*) realloc(mtContent_4x4, 64*metatileSets_4x4);
		//mtUsage_4x4                           = (uint32_t*) realloc(mtUsage_4x4, 64*metatileSets_4x4*sizeof(uint32_t));

		nss_get_bytes(text,size,"MetatileSet_4x4_id=",metatileSet_4x4_id, metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"MetatileSet_4x4_pal=", metatileSet_4x4_pal, metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"MetatileSet_4x4_props=", metatileSet_4x4_props, metatileSets_4x4*META_4x4);

		if(CheckTag(text,size, "MetatileSetLabels_4x4=")) metatileSetLabels_4x4 = deserializeVector(text, "MetatileSetLabels_4x4=");
		else {
			metatileSetLabels_4x4.clear();
			for( int k=0;k<metatileSets_4x4;k++){metatileSetLabels_4x4.push_back("Unlabeled");}
		}

		tmp_metatileSetLabels_4x4.clear();

	}

	if(mt8){
		mtClickID_store_8x8=nss_get_int(text,size,"VarMtClickID_store_8x8=");
		metatileSets_8x8				=nss_get_int(text,size,"VarMetatileSets_8x8=");

		metatileSet_8x8_id 					  = (char*) realloc(metatileSet_8x8_id, metatileSets_8x8*META_8x8);
		metatileSet_8x8_pal				      = (char*) realloc(metatileSet_8x8_pal, metatileSets_8x8*META_8x8);
		metatileSet_8x8_props				  = (char*) realloc(metatileSet_8x8_props, metatileSets_8x8*META_8x8);

		mtContent_8x8                         = (char*) realloc(mtContent_8x8, 64*metatileSets_8x8);
		//mtUsage_8x8                           = (uint32_t*) realloc(mtUsage_8x8, 64*metatileSets_8x8*sizeof(uint32_t));

		nss_get_bytes(text,size,"MetatileSet_8x8_id=",metatileSet_8x8_id, metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"MetatileSet_8x8_pal=", metatileSet_8x8_pal, metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"MetatileSet_8x8_props=", metatileSet_8x8_props, metatileSets_8x8*META_8x8);


		if(CheckTag(text,size, "MetatileSetLabels_8x8=")) metatileSetLabels_8x8 = deserializeVector(text, "MetatileSetLabels_8x8=");
		else {
			metatileSetLabels_8x8.clear();
			for( int k=0;k<metatileSets_8x8;k++){metatileSetLabels_8x8.push_back("Unlabeled");}
		}

		tmp_metatileSetLabels_8x8.clear();
	 }

	return true;
}

void __fastcall TFormMain::Openmetatilesallsizes1Click(TObject *Sender)
{
  bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
  bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
  bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;
  int tag = ((TMenuItem*)Sender)->Tag;
  bool mt2=false;
  bool mt4=false;
  bool mt8=false;

  if(tag==0) {	mt2= true;	mt4= true; 	mt8= true;}

  if(tag==1){
	mt2=bTab2x2;
	mt4=bTab4x4;
	mt8=bTab8x8;
  }

  if(tag==2) {	mt2= true;	mt4=false; 	mt8=false;}
  if(tag==3) {	mt2=false;	mt4= true; 	mt8=false;}
  if(tag==4) {	mt2=false;	mt4=false; 	mt8= true;}


  if(tag==0) {dialogTitle_OpenMetatiles="Open metatiles (all tabs)";}
  if(tag==1) {dialogTitle_OpenMetatiles="Open metatiles (current tab)";}
  if(tag==2) {dialogTitle_OpenMetatiles="Open metatiles (2x2)";}
  if(tag==3) {dialogTitle_OpenMetatiles="Open metatiles (4x4)";}
  if(tag==4) {dialogTitle_OpenMetatiles="Open metatiles (8x8)";}

  if(tag==0) {OpenDialogMetatiles->Filter="All valid (*.mtt, *.nss, *.mtt2, *.mtt4, *.mtt8)|*.mtt;*.nss;*.mtt2;*.mtt4;*.mtt8;|2x2 Metatiles|Metatiles (*.mtt)|*.mtt|Session (*.nss)|*.nss|*.mtt2|4x4 Metatiles|*.mtt4|8x8 Metatiles|*.mtt8|Any (*.*)|*.*";}
  if(tag==1) {
	if(mt2) {OpenDialogMetatiles->Filter="All valid (*.mtt, *.nss, *.mtt2)|*.mtt;*.nss;*.mtt2;|Metatiles (*.mtt)|*.mtt|Session (*.nss)|*.nss|2x2 Metatiles|*.mtt2|Any (*.*)|*.*";}
	if(mt4) {OpenDialogMetatiles->Filter="All valid (*.mtt, *.nss, *.mtt4)|*.mtt;*.nss;*.mtt4;|Metatiles (*.mtt)|*.mtt|Session (*.nss)|*.nss|4x4 Metatiles|*.mtt4|Any (*.*)|*.*";}
	if(mt8) {OpenDialogMetatiles->Filter="All valid (*.mtt, *.nss, *.mtt8)|*.mtt;*.nss;*.mtt8;|Metatiles (*.mtt)|*.mtt|Session (*.nss)|*.nss|8x8 Metatiles|*.mtt8|Any (*.*)|*.*";}

	}
  if(tag==2) {OpenDialogMetatiles->Filter="All valid (*.mtt, *.nss, *.mtt2)|*.mtt;*.nss;*.mtt2;|Metatiles (*.mtt)|*.mtt|Session (*.nss)|*.nss|2x2 Metatiles|*.mtt2|Any (*.*)|*.*";}
  if(tag==3) {OpenDialogMetatiles->Filter="All valid (*.mtt, *.nss, *.mtt4)|*.mtt;*.nss;*.mtt4;|Metatiles (*.mtt)|*.mtt|Session (*.nss)|*.nss|4x4 Metatiles|*.mtt4|Any (*.*)|*.*";}
  if(tag==4) {OpenDialogMetatiles->Filter="All valid (*.mtt, *.nss, *.mtt8)|*.mtt;*.nss;*.mtt8;|Metatiles (*.mtt)|*.mtt|Session (*.nss)|*.nss|8x8 Metatiles|*.mtt8|Any (*.*)|*.*";}


  OpenDialogMetatiles->Title=dialogTitle_OpenMetatiles;

  if(!OpenDialogMetatiles->Execute()) return;

  BlockDrawing(true);

  if(OpenMetatiles(OpenDialogMetatiles->FileName,mt2,mt4,mt8))
  {
		OpenDialogMetatiles->FileName=RemoveExt(OpenDialogMetatiles->FileName);


  }

	BlockDrawing(false);

   UpdateAll();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SpeedButtonClearMetaSpriteMouseEnter(TObject *Sender)
{
   LabelStats->Caption="Clears current metasprite of all contents.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteGridMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Toggles grid visibility on/off.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonNextMetaSpriteMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Go to next metasprite.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPrevMetaSpriteMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Go to previous metasprite.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::LabelMetaSpriteMouseEnter(TObject *Sender)
{
   LabelStats->Caption="Double click or press [Shift+R] to edit the label of this metasprite.\n";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMetaSpriteHFlipMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Flip whole metasprite; horizontally.\nFlipping sprites is a PPU navite ability.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMetaSpriteRotateMouseEnter(
	  TObject *Sender)
{
	LabelStats->Caption="Rotates the position of sprites.\nNote that content rotation is not a native PPU ability.\nYou'll need to reassign appropriate tiles, and/or use the H/V flip bits for 180 degree rotation.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonMetaSpriteVFlipMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Flip whole metasprite; vertically.\nFlipping sprites is a PPU navite ability.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SBPriorityToggle1MouseEnter(TObject *Sender)
{
	LabelStats->Caption="Toggle the Priority bit. A 'P' in the list will indicate the sprites' priority is set to true.\nfalse: in front of background.\true:behind background.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SBB4MouseEnter(TObject *Sender)
{
   LabelStats->Caption="Toggle attribute bit 4.\nIt is unused by the PPU, but you may use it in your game logic.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SBB3MouseEnter(TObject *Sender)
{
	LabelStats->Caption="Toggle attribute bit 3.\nIt is unused by the PPU, but you may use it in your game logic.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SBB2MouseEnter(TObject *Sender)
{
	LabelStats->Caption="Toggle attribute bit 2.\nIt is unused by the PPU, but you may use it in your game logic.";
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SpeedButtonSpriteVFlipMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Click: toggle the Vertical flip bit of selected sprites, and flip their positions too.\nCtrl-Click: Flip positions without flipping bits.\nShift-Click:Flip bits without flipping positions.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonSpriteHFlipMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Click: toggle the Horizontal flip bit of selected sprites, and flip their positions too.\nCtrl-Click: Flip positions without flipping bits.\nShift-Click:Flip bits without flipping positions.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Scanlinelimit1Click(TObject *Sender)
{
	FormScanlineWarnings->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SaveMetatiles(AnsiString filename, bool mt2, bool mt4, bool mt8)
{
	FILE *file;
	int i,j;
	unsigned char pal[16];
	std::string serializedVector = "";
	file=fopen(filename.c_str(),"wt");

	if(!file)
	{
		Application->MessageBox("Can't save session","Error",MB_OK);
		return;
	}
	
	//signature

	fprintf(file,"%s\n\n",metatiletxtIDstr);

	fprintf(file,"VarMtClickID=%i\n", mtClickID);
	if(mt2) fprintf(file,"VarMtClickID_store_2x2=%i\n", mtClickID_store_2x2);
	if(mt4) fprintf(file,"VarMtClickID_store_4x4=%i\n", mtClickID_store_4x4);
	if(mt8) fprintf(file,"VarMtClickID_store_8x8=%i\n", mtClickID_store_8x8);

	if(mt2) fprintf(file,"VarMetatileSets_2x2=%i\n", metatileSets_2x2);
	if(mt4) fprintf(file,"VarMetatileSets_4x4=%i\n", metatileSets_4x4);
	if(mt8) fprintf(file,"VarMetatileSets_8x8=%i\n", metatileSets_8x8);

	if(mt2){
		nss_put_bytes(file,"\n\nMetatileSet_2x2_id=",metatileSet_2x2_id, metatileSets_2x2*META_2x2);
		nss_put_bytes(file,"\n\nMetatileSet_2x2_pal=", metatileSet_2x2_pal, metatileSets_2x2*META_2x2);
		nss_put_bytes(file,"\n\nMetatileSet_2x2_props=", metatileSet_2x2_props, metatileSets_2x2*META_2x2);
	}

	if(mt4){
		nss_put_bytes(file,"\n\nMetatileSet_4x4_id=",metatileSet_4x4_id, metatileSets_4x4*META_4x4);
		nss_put_bytes(file,"\n\nMetatileSet_4x4_pal=", metatileSet_4x4_pal, metatileSets_4x4*META_4x4);
		nss_put_bytes(file,"\n\nMetatileSet_4x4_props=", metatileSet_4x4_props, metatileSets_4x4*META_4x4);
	}

	if(mt8){
		nss_put_bytes(file,"\n\nMetatileSet_8x8_id=",metatileSet_8x8_id, metatileSets_8x8*META_8x8);
		nss_put_bytes(file,"\n\nMetatileSet_8x8_pal=", metatileSet_8x8_pal, metatileSets_8x8*META_8x8);
		nss_put_bytes(file,"\n\nMetatileSet_8x8_props=", metatileSet_8x8_props, metatileSets_8x8*META_8x8);
	}

	if(mt2){
		serializedVector.clear();
		for (std::vector<std::string>::const_iterator iter = metatileSetLabels_2x2.begin(); iter != metatileSetLabels_2x2.end(); ++iter) {
			serializedVector += *iter;
			if (iter + 1 != metatileSetLabels_2x2.end()) {serializedVector += ",";}
		}
		fprintf(file,"\n\nMetatileSetLabels_2x2=%s",serializedVector.c_str());
	}

	if(mt4){
		serializedVector.clear();
		for (std::vector<std::string>::const_iterator iter = metatileSetLabels_4x4.begin(); iter != metatileSetLabels_4x4.end(); ++iter) {
			serializedVector += *iter;
			if (iter + 1 != metatileSetLabels_4x4.end()) {serializedVector += ",";}
		}
		fprintf(file,"\n\nMetatileSetLabels_4x4=%s",serializedVector.c_str());
	}

	if(mt8){
		serializedVector.clear();

			for (std::vector<std::string>::const_iterator iter = metatileSetLabels_8x8.begin(); iter != metatileSetLabels_8x8.end(); ++iter) {
			serializedVector += *iter;
			if (iter + 1 != metatileSetLabels_8x8.end()) {serializedVector += ",";}
		}
	fprintf(file,"\n\nMetatileSetLabels_8x8=%s",serializedVector.c_str());


	}
	fclose(file);
}

//---------------------------------------------------------------------------

void __fastcall TFormMain::Savemetatiles1Click(TObject *Sender)
{

  bool bTab2x2 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet2x2?true:false;
  bool bTab4x4 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet4x4?true:false;
  bool bTab8x8 = FormMetatileTool->PageControl1->ActivePage==FormMetatileTool->TabSheet8x8?true:false;
  int tag = ((TMenuItem*)Sender)->Tag;
  bool mt2=false;
  bool mt4=false;
  bool mt8=false;
  AnsiString ext;

  if(tag==0) {	mt2= true;	mt4= true; 	mt8= true; ext=".mtt";}

  if(tag==1){
	mt2=bTab2x2;
	mt4=bTab4x4;
	mt8=bTab8x8;

	if(mt2)ext=".mtt2";
	if(mt4)ext=".mtt4";
	if(mt8)ext=".mtt8";
  }

  if(tag==2) {	mt2= true;	mt4=false; 	mt8=false; ext=".mtt2";}
  if(tag==3) {	mt2=false;	mt4= true; 	mt8=false; ext=".mtt4";}
  if(tag==4) {	mt2=false;	mt4=false; 	mt8= true; ext=".mtt8";}


  if(tag==0) {SaveDialogMetatiles->Title="Save metatiles (all tabs)";}
  if(tag==1) {SaveDialogMetatiles->Title="Save metatiles (current tab)";}
  if(tag==2) {SaveDialogMetatiles->Title="Save metatiles (2x2)";}
  if(tag==3) {SaveDialogMetatiles->Title="Save metatiles (4x4)";}
  if(tag==4) {SaveDialogMetatiles->Title="Save metatiles (8x8)";}

  SaveDialogMetatiles->FileName=RemoveExt(SaveDialogMetatiles->FileName)+ext;

  BlockDrawing(true);

  if(SaveDialogMetatiles->Execute())
  {
		AnsiString name = RemoveExt(SaveDialogMetatiles->FileName)+ext;
		SaveMetatiles(name, mt2, mt4, mt8);
  }
  BlockDrawing(false);


}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ExtractMetatileAppendData(char* text,size_t size, bool mt2, bool mt4, bool mt8)
{

	if(mt2){
		if(CheckTag(text,size,"VarMetatileSets_2x2=")){
			buf_metatileSets_2x2     	  =nss_get_int(text,size,"VarMetatileSets_2x2=");
		}else{buf_metatileSets_2x2=1;}

		metatileBuf_2x2_id 					  = (char*) realloc(metatileBuf_2x2_id, buf_metatileSets_2x2*META_2x2);
		metatileBuf_2x2_pal				      = (char*) realloc(metatileBuf_2x2_pal, buf_metatileSets_2x2*META_2x2);
		metatileBuf_2x2_props				  = (char*) realloc(metatileBuf_2x2_props, buf_metatileSets_2x2*META_2x2);

		nss_get_bytes(text,size,"MetatileSet_2x2_id=",metatileBuf_2x2_id, buf_metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"MetatileSet_2x2_pal=", metatileBuf_2x2_pal, buf_metatileSets_2x2*META_2x2);
		nss_get_bytes(text,size,"MetatileSet_2x2_props=", metatileBuf_2x2_props, buf_metatileSets_2x2*META_2x2);
	}
	if(mt4){
		if(CheckTag(text,size,"VarMetatileSets_4x4=")){
			buf_metatileSets_4x4     	  =nss_get_int(text,size,"VarMetatileSets_4x4=");
		} else {buf_metatileSets_4x4=1;}
		metatileBuf_4x4_id 					  = (char*) realloc(metatileBuf_4x4_id, buf_metatileSets_4x4*META_4x4);
		metatileBuf_4x4_pal				      = (char*) realloc(metatileBuf_4x4_pal, buf_metatileSets_4x4*META_4x4);
		metatileBuf_4x4_props				  = (char*) realloc(metatileBuf_4x4_props, buf_metatileSets_4x4*META_4x4);

		nss_get_bytes(text,size,"MetatileSet_4x4_id=",metatileBuf_4x4_id, buf_metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"MetatileSet_4x4_pal=", metatileBuf_4x4_pal, buf_metatileSets_4x4*META_4x4);
		nss_get_bytes(text,size,"MetatileSet_4x4_props=", metatileBuf_4x4_props, buf_metatileSets_4x4*META_4x4);

	}
	if(mt8){
		if(CheckTag(text,size,"VarMetatileSets_8x8=")){
			buf_metatileSets_8x8     			=nss_get_int(text,size,"VarMetatileSets_8x8=");
		} else {buf_metatileSets_8x8=1;}
		metatileBuf_8x8_id 					  = (char*) realloc(metatileBuf_8x8_id, buf_metatileSets_8x8*META_8x8);
		metatileBuf_8x8_pal				      = (char*) realloc(metatileBuf_8x8_pal, buf_metatileSets_8x8*META_8x8);
		metatileBuf_8x8_props				  = (char*) realloc(metatileBuf_8x8_props, buf_metatileSets_8x8*META_8x8);

		nss_get_bytes(text,size,"MetatileSet_8x8_id=",metatileBuf_8x8_id, buf_metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"MetatileSet_8x8_pal=", metatileBuf_8x8_pal, buf_metatileSets_8x8*META_8x8);
		nss_get_bytes(text,size,"MetatileSet_8x8_props=", metatileBuf_8x8_props, buf_metatileSets_8x8*META_8x8);

	}
}
void __fastcall TFormMain::Attributecheckeroptions1Click(TObject *Sender)
{
	FormAttrChecker->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ResetAttributecheckersettings1Click(TObject *Sender)
{
   FormAttrChecker->ResetPresets(true);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::UpdateExternalFormsTimer(TObject *Sender)
{

   if(!openByFileDone) return;
	
   UpdateExternalForms->Enabled=false;

   FormMetatileTool->UpdateUI(true);

   FormBankCHR->MakeList(true,true);
   FormCHRbit->UpdateBitButtons(false);
   FormMTprops->UpdateBitButtons_metasetClick();
   FormPipelineHelper->HandleLoadedLinkedMode();
   FormPipelineHelper->UpdateUI();
   FormPropID->MakeList();
   FormPropID->UpdateUI();


   bool tmp =! cueUpdateMetasprite_noListUpdate;
   cueUpdateMetasprite_noListUpdate=false;

   if(PageControlEditor->ActivePage==TabSheetSprite)UpdateMetaSprite(tmp);
   if(FormManageMetasprites)
		if(FormManageMetasprites->Visible)
		{
			FormManageMetasprites->UpdateActive();
		}
  FormMetatileTool->chkReserve1st->Checked=  bMT_Reserve1st;
  FormMetatileTool->chkAlignScr->Checked=    bMT_AlignScr;

  FormMetatileTool->btnUseAttr->Down=        bMT_UseAttr;
  FormMetatileTool->btnClonestamp->Down=     bMT_Clonestamp;
  FormMetatileTool->btnCloneSnap->Down=      bMT_CloneSnap;
  FormMetatileTool->btnTiles->Down=          bMT_Tiles;
  FormMetatileTool->btnAttr->Down=           bMT_Attr;
  FormMetatileTool->btnProps->Down=          bMT_Props;
  FormMetatileTool->btnMap->Down=            bMT_Map;

  FormSwapBanks->chkInclProps->Checked=      bBS_InclLabel;
  FormSwapBanks->chkInclLabel->Checked=      bBS_InclProps;

  FormPipelineHelper->chkAutoExpMetatilesBMP->Checked	=bAutoExport_metatiles;
  FormPipelineHelper->RadioButton1->Checked				=bAutoExportRadio1;
  FormPipelineHelper->RadioButton2->Checked				=bAutoExportRadio2;
  FormPipelineHelper->RadioButton3->Checked				=bAutoExportRadio3;

  FormScanlineWarnings->UpDown1->Position=iWarnMsprCyan;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnMaskClick(TObject *Sender)
{
	//Attributes1->Checked = SpeedButtonChecker->Down;
	cueUpdateMetasprite=true;
	UpdateNameTable(-1,-1,true);
	UpdateTiles(true);
	if(FormNavigator->Visible) FormNavigator->Draw(false,false,false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnSelectedOnlyInvertedMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="Inverts which tiles are hidden when 'selected only' is on.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnMaskMouseEnter(TObject *Sender)
{
	if(uiScale<3) GroupBoxTileControl->Visible=false;
	LabelStats->Caption="Shows the solid pixels (colours 1-3) mask.\n\nGood for knowing which pixels are on the character layer of the PPU.\nAlso useful for testing the silhouette readability of a metasprite.";
}
//---------------------------------------------------------------------------


bool __fastcall TFormMain::OpenCollisionIDList(AnsiString filename)
{
	FILE *file;
	char *text;
	unsigned char temp[16];
	int cnt,filetype,size;
	bool r;

	file=fopen(filename.c_str(),"rb");

	if(!file){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+filename;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);
			return false;
	}
	fread(temp,sizeof(sessionIDStr),1,file);

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	text=(char*)malloc(size+1);
	text[size]=0;

	fread(text,size,1,file);
	fclose(file);

	filetype= 1;

	if(!memcmp(temp,sessionIDStr,8)) filetype=2;    //session text file
	if(!memcmp(temp,collisionIDtxtIDstr,8)) filetype=3; //collision text file

	if (filetype==1) {
		Application->MessageBox("Not the expected text file.\nPlease check in text editor.","Error",MB_OK);
		return false;
	}

	cnt=0;
	//test data
	for (int i = 0; i <256; i++) {
		char label[28];
		sprintf(label, "collisionIDlabel%i=", i);
		if(CheckTag(text,size,label)) cnt++;

		sprintf(label, "collisionID_R%i=", i);
		if(CheckTag(text,size,label)) cnt++;
		sprintf(label, "collisionID_G%i=", i);
		if(CheckTag(text,size,label)) cnt++;
		sprintf(label, "collisionID_B%i=", i);
		if(CheckTag(text,size,label)) cnt++;
	}
    //were there data?
	if(cnt<1)
	{
		AnsiString AnsiMessage=
			"File opened, but no list data was found.";
		Application->MessageBox(AnsiMessage.c_str(),"Warning",MB_OK);
			return false;
	}
	else{ //go ahead, get data.
		for (int i = 0; i <256; i++) {
			char label[28];
			sprintf(label, "collisionIDlabel%i=", i);
			collisionIDlabel[i] = nss_get_str(text,size,label);

			sprintf(label, "collisionID_R%i=", i);
			if(CheckTag(text,size,label)) collisionID_R[i] = (char)nss_get_int(text,size,label);
			else collisionID_R[i]=0xff;

			sprintf(label, "collisionID_G%i=", i);
			if(CheckTag(text,size,label)) collisionID_G[i] = (char)nss_get_int(text,size,label);
			else collisionID_G[i]=0xff;

			sprintf(label, "collisionID_B%i=", i);
			if(CheckTag(text,size,label)) collisionID_B[i] = (char)nss_get_int(text,size,label);
			else collisionID_B[i]=0xff;

		}

	}
	return true;


}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SaveCollisionIDList(AnsiString filename)
{
	FILE *file;
    filename=RemoveExt(filename)+".idl";

	file=fopen(filename.c_str(),"wt");

	if(!file)
	{
		Application->MessageBox("Can't save collision ID list","Error",MB_OK);
		return;
	}
	//signature

	fprintf(file,"%s\n\n",collisionIDtxtIDstr);
	for(int i=0;i<256;i++)
	{
		if(collisionIDlabel[i]!="")
		{
			fprintf(file,"collisionIDlabel%i=%s\n",i,collisionIDlabel[i].c_str());
		}
		if(collisionID_R[i]!=0xff)
			fprintf(file,"collisionID_R%i=%i\t",i,collisionID_R[i]);
		if(collisionID_G[i]!=0xff)
			fprintf(file,"collisionID_G%i=%i\t",i,collisionID_G[i]);
		if(collisionID_B[i]!=0xff)
			fprintf(file,"collisionID_B%i=%i\n",i,collisionID_B[i]);

	}
	fclose(file);
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::SaveBrushes(AnsiString name, AnsiString ext, bool customExt){
    FILE *file;


	int mode;
	if (customExt) {
		//user has decided to type a custom extension.
		//However, we still need to treat it according to which action
		//was being used to save it.

		if		(ext==".bm1") {mode=1; name=RemoveExt(name)+".bm1";}
		else if (ext==".bm7") {mode=7; name=RemoveExt(name)+".bm7";}
		else if (ext==".bm14"){mode=14;name=RemoveExt(name)+".bm14"; }
	}
	else {
		//user is using one of the recommended extensions.
		//they may have changed which one of them in the save dialog, so we double-check here.
		ext=GetExt(name);
		if		(ext==".bm1") mode=1;
		else if (ext==".bm7") mode=7;
		else if (ext==".bm14") mode=14;
	}
    file=fopen(name.c_str(),"wt");

	if(!file)
	{
		Application->MessageBox("Can't save session","Error",MB_OK);
		return;
	}



	int set = iBrushPresetIndex<7? 0:7;
    AnsiString tmpBrushString;
	AnsiString tmpBrushSet;
	AnsiString tmpBrushID;


	//signature

	fprintf(file,"%s\n\n",brushMasktxtIDstr);



	if(mode==1){
		nss_put_bytes_from_int(file,"\n\niBrushSize=" ,&iBrushSize[iBrushPresetIndex],1);
		nss_put_bytes_from_int(file,"\n\niBrushSnapSize_x=" ,&iBrushSnapSize_x[iBrushPresetIndex],1);
		nss_put_bytes_from_int(file,"\n\niBrushSnapSize_y=" ,&iBrushSnapSize_y[iBrushPresetIndex],1);
		nss_put_bytes_from_int(file,"\n\niBrushCursorAlignment=" ,&iBrushCursorAlignment[iBrushPresetIndex],1);

		tmpBrushSet= "a";
		tmpBrushID = IntToStr((iBrushPresetIndex%7)+2);
		tmpBrushString="\n\nBrushMask_"+tmpBrushID+tmpBrushSet+"=";
		nss_put_bytes_from_int(file,tmpBrushString.c_str(),&(*ptr_tableBrush[iBrushPresetIndex])[0][0],256);

	}
	else if(mode==14){
		nss_put_bytes_from_int(file,"\n\niBrushSize=" ,iBrushSize,sizeof(14));
		nss_put_bytes_from_int(file,"\n\niBrushSnapSize_x=" ,iBrushSnapSize_x,sizeof(14));
		nss_put_bytes_from_int(file,"\n\niBrushSnapSize_y=" ,iBrushSnapSize_y,sizeof(14));
		nss_put_bytes_from_int(file,"\n\niBrushCursorAlignment=" ,iBrushCursorAlignment,sizeof(14));


		for(int i=0;i<14;i++){
			tmpBrushSet= i<7? "a":"b";
			tmpBrushID = IntToStr((i%7)+2);
			tmpBrushString="\n\nBrushMask_"+tmpBrushID+tmpBrushSet+"=";
			nss_put_bytes_from_int(file,tmpBrushString.c_str(),&(*ptr_tableBrush[i])[0][0],256);
		}
	}
	else{ //default to mode 7
		nss_put_bytes_from_int(file,"\n\niBrushSize=" ,&iBrushSize[set],7);
		nss_put_bytes_from_int(file,"\n\niBrushSnapSize_x=" ,&iBrushSnapSize_x[set],7);
		nss_put_bytes_from_int(file,"\n\niBrushSnapSize_y=" ,&iBrushSnapSize_y[set],7);
		nss_put_bytes_from_int(file,"\n\niBrushCursorAlignment=" ,&iBrushCursorAlignment[set],7);


		for(int i=0;i<7;i++){
			tmpBrushSet= "a";
			tmpBrushID = IntToStr(i+2);
			tmpBrushString="\n\nBrushMask_"+tmpBrushID+tmpBrushSet+"=";
			nss_put_bytes_from_int(file,tmpBrushString.c_str(),&(*ptr_tableBrush[i+set])[0][0],256);
		}

	}
	fclose(file);
}
//---------------------------------------------------------------------------
bool __fastcall TFormMain::OpenBrushes(AnsiString filename){
	FILE *file;
	char *text;
	unsigned char temp[16];
	int cnt,filetype,size;
	bool r;
	int set = iBrushPresetIndex<7? 0:7;

	AnsiString tmpBrushString;
	AnsiString tmpBrushSet;
	AnsiString tmpBrushID;

	file=fopen(filename.c_str(),"rb");

	if(!file){
		AnsiString AnsiFileNotFound=
			"Could not open file.\n\nPlease double-check its path/file/extension,\nor verify its existence:\n"+filename;
		Application->MessageBox(AnsiFileNotFound.c_str(),"Warning",MB_OK);
			return false;
	}
	fread(temp,sizeof(sessionIDStr),1,file);

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	text=(char*)malloc(size+1);
	text[size]=0;

	fread(text,size,1,file);
	fclose(file);

	filetype= 1;

	if(!memcmp(temp,sessionIDStr,8)) filetype=2;    //session text file
	if(!memcmp(temp,brushMasktxtIDstr,8)) filetype=3; //collision text file

	if (filetype==1) {
		Application->MessageBox("Not the expected text file.\nPlease check in text editor.","Error",MB_OK);
		return false;
	}

	cnt=0;
	//test data

	for(int i=0;i<14;i++){
		tmpBrushSet= i<7? "a":"b";
		tmpBrushID = IntToStr((i%7)+2);
		tmpBrushString="BrushMask_"+tmpBrushID+tmpBrushSet+"=";
		if(CheckTag(text,size,tmpBrushString.c_str())) cnt++;

	}



	//what data?

	if(cnt==1) // single brush
	{
		if(CheckTag(text,size,"iBrushSize=")){
			nss_get_int_from_bytes(text,size,"iBrushSize=",&iBrushSize[iBrushPresetIndex],1);
		}

		if(CheckTag(text,size,"iBrushSnapSize_x=")){
			nss_get_int_from_bytes(text,size,"iBrushSnapSize_x=",&iBrushSnapSize_x[iBrushPresetIndex],1);
		}

		if(CheckTag(text,size,"iBrushSnapSize_y=")){
			nss_get_int_from_bytes(text,size,"iBrushSnapSize_y=",&iBrushSnapSize_y[iBrushPresetIndex],1);
		}

		if(CheckTag(text,size,"iBrushCursorAlignment=")){
			nss_get_int_from_bytes(text,size,"iBrushCursorAlignment=",&iBrushCursorAlignment[iBrushPresetIndex],1);
		}

		for(int i=0;i<14;i++){
			tmpBrushSet= i<7? "a":"b";
			tmpBrushID = IntToStr((i%7)+2);
			tmpBrushString="BrushMask_"+tmpBrushID+tmpBrushSet+"=";
			if(CheckTag(text,size,tmpBrushString.c_str()))
				nss_get_int_from_bytes(text,size,tmpBrushString.c_str(),&(*ptr_tableBrush[iBrushPresetIndex])[0][0],256);
		}
	}
	else if((cnt<=7) && (cnt>0)) // single set
	{
		if(CheckTag(text,size,"iBrushSize=")){
			nss_get_int_from_bytes(text,size,"iBrushSize=",iBrushSize+set,7);
		}

		if(CheckTag(text,size,"iBrushSnapSize_x=")){
			nss_get_int_from_bytes(text,size,"iBrushSnapSize_x=",iBrushSnapSize_x+set,7);
		}

		if(CheckTag(text,size,"iBrushSnapSize_y=")){
			nss_get_int_from_bytes(text,size,"iBrushSnapSize_y=",iBrushSnapSize_y+set,7);
		}

		if(CheckTag(text,size,"iBrushCursorAlignment=")){
			nss_get_int_from_bytes(text,size,"iBrushCursorAlignment=",iBrushCursorAlignment+set,7);
		}

		for(int i=0;i<14;i++){
			int tmpi=i%7;
			tmpBrushSet= i<7? "a":"b";
			tmpBrushID = IntToStr((i%7)+2);
			tmpBrushString="BrushMask_"+tmpBrushID+tmpBrushSet+"=";
			if(CheckTag(text,size,tmpBrushString.c_str()))
				nss_get_int_from_bytes(text,size,tmpBrushString.c_str(),&(*ptr_tableBrush[tmpi+set])[0][0],256);
		}


	}
	else if((cnt<=14) && (cnt>0)) // double set or partial set.
	{
		if(CheckTag(text,size,"iBrushSize=")){
			nss_get_int_from_bytes(text,size,"iBrushSize=",iBrushSize,14);
		}

		if(CheckTag(text,size,"iBrushSnapSize_x=")){
			nss_get_int_from_bytes(text,size,"iBrushSnapSize_x=",iBrushSnapSize_x,14);
		}

		if(CheckTag(text,size,"iBrushSnapSize_y=")){
			nss_get_int_from_bytes(text,size,"iBrushSnapSize_y=",iBrushSnapSize_y,14);
		}

		if(CheckTag(text,size,"iBrushCursorAlignment=")){
			nss_get_int_from_bytes(text,size,"iBrushCursorAlignment=",iBrushCursorAlignment,14);
		}

		for(int i=0;i<14;i++){
			tmpBrushSet= i<7? "a":"b";
			tmpBrushID = IntToStr((i%7)+2);
			tmpBrushString="BrushMask_"+tmpBrushID+tmpBrushSet+"=";
			if(CheckTag(text,size,tmpBrushString.c_str()))
			nss_get_int_from_bytes(text,size,tmpBrushString.c_str(),&(*ptr_tableBrush[i])[0][0],256);
		}
	}
	else
	{
		AnsiString AnsiMessage=
			"File opened, but data was missing or incorrect.\nFile should contain 1-14 brushes.";
		Application->MessageBox(AnsiMessage.c_str(),"Warning",MB_OK);
			return false;
	}
	return true;
}
size_t __fastcall TFormMain::ReplaceTagInFile(char* &text, size_t size, const char* tag, const unsigned char* newData, int newDataSize) {
	size_t tagSize = strlen(tag);
	for (size_t i = 0; i <= size - tagSize; ++i) {
        if (memcmp(text + i, tag, tagSize) == 0) {
			// The tag was found; i points to the start of the tag.

			//calc the position after the '=' character.
			size_t dataStart = i + tagSize;

			//find EOD (line break or end of file).
			size_t dataEnd = dataStart;
			while (dataEnd < size && text[dataEnd] != '\n') dataEnd++;

			//length of data to be replaced
			size_t dataLength = dataEnd - dataStart;
			//length of the new data.
			size_t newDataLength = newDataSize;

			//new data is shorter, shift the remaining data left.
			if (newDataLength < dataLength) {
				memmove(text + dataStart + newDataLength, text + dataEnd, size - dataEnd);
			}
			else if (newDataLength > dataLength) {
					memmove(text + dataStart + newDataLength, text + dataEnd, size - dataEnd);
			}

			//copy the new data.
			sprintf(text + dataStart, newData);
			//snprintf(text + dataStart, newDataLength , "%s", newData);

			//update the size of the text.
			size = dataStart + newDataLength;
			//account for the remaining data after the replacement.
			size += (dataEnd - (dataStart + dataLength));

			return size; // Tag replaced successfully.
        }
    }
    return 0; // Tag not found.
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Linemode1Click(TObject *Sender)
{
	 if(SpeedButtonTypeIn->Down) return;
    lineToolY_toggleperm=0;
	FormCHREditor->btnLine->Down^=true;
	btnLine->Down=FormCHREditor->btnLine->Down;
	Linemode1->Checked=FormCHREditor->btnLine->Down;
	if(btnLine->Down) LabelStats->Caption="Line mode ON.";
	else			   LabelStats->Caption="Line mode OFF.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;

}
//---------------------------------------------------------------------------



void __fastcall TFormMain::Saveallchrbanks1Click(TObject *Sender)
{
             BlockDrawing(true);
	int size = chrBanks*4096;

	FILE *file;
	int i;
	AnsiString name;

	if(!SaveDialogChr->Execute()) return;

	name=RemoveExt(SaveDialogChr->FileName)+".chr";

	if(!OverwritePrompt(name)) return;

	file=fopen(name.c_str(),"rb");

	if(file)
	{
		fseek(file,0,SEEK_END);
		i=ftell(file);
		fclose(file);

		if(size!=i)
		{
			if(Application->MessageBox(("Previous file has different size ("+IntToStr(i)+" bytes)!\nDo you really want to overwrite?").c_str(),"Confirm",MB_YESNO)!=IDYES) return;
		}
	}

	file=fopen(name.c_str(),"wb");

	if(!file) return;

	fwrite(chr,size,1,file);
	fclose(file);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Buckettoolbox1Click(TObject *Sender)
{
	FormBucketToolbox->Visible^=true;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TimerScrollWheelDisablerTimer(TObject *Sender)
{
	TimerScrollWheelDisabler->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonChrBank1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	int tag;
	TSpeedButton *speedButton = dynamic_cast<TSpeedButton*>(Sender);
	if (speedButton){
		tag = speedButton->Tag;
	}
	else return;

	if(Shift.Contains(ssRight)){
	  if(tag==0) SpeedButtonChrBank1->Flat^=true;
	  if(tag==1) SpeedButtonChrBank2->Flat^=true;
	  if(tag==2) SpeedButtonChrBank3->Flat^=true;
	  if(tag==3) SpeedButtonChrBank4->Flat^=true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::OpenByAssociationListMakerTimer(TObject *Sender)
{
	if(!openByFileDone) return;
	FormBankCHR->MakeList(true,false);
	OpenByAssociationListMaker->Enabled=false;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Savecurrentbankitem1Click(TObject *Sender)
{
	int tmp=curViewCHRbankCanvas2*4096;
	BlockDrawing(true);
	SaveCHR(tmp,4096,false);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::NESGB1Click(TObject *Sender)
{
	char buf[16];

	int set=bankActive/16;
	int i,j,k,pp;
	int x,y,w,h;
	bool skip[256]={false};
	bool doWhole;
	int cnt=0;

	//these allow nonselected tiles to be skipped.
	GetSelection(chrSelection,x,y,w,h);
	if(chrSelectRect)
	{
		if(w>1||h>1)
		{
			for(i=0;i<16;++i)
			{
				for(j=0;j<16;++j)
				{
					skip[i*16+j]=(i>=y&&i<y+h&&j>=x&&j<x+w)?false:true;
				}
			}
		}
	}
	else
	{
		for(i=0;i<256;++i)
		{
			skip[i]= chrSelected[i]?false:true;
			if (chrSelected[i]) cnt++;
		}
	}

	if (cnt>0 || (chrSelectRect && (w>1||h>1))) doWhole=false;
	else doWhole=true;

	SetUndo();

	int tile=0;
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			if(!doWhole && skip[j+i*16]) {tile++; continue;}

			//int y=tileViewTable[j+i*16]/16;
			//int x=tileViewTable[j+i*16]&15;

			int pp=tileViewTable[tile]*16+bankViewTable[set+tile];
			memcpy(buf,chr+pp,16);
			for(k=0;k<8;k++){
			   chr[tileViewTable[tile]*16+bankViewTable[set+tile] + k*2]  	=buf[k];
			   chr[tileViewTable[tile]*16+bankViewTable[set+tile] + k*2 +1]	=buf[k+8];
			}

			tile++;
		}
	}
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::GBNES1Click(TObject *Sender)
{
   char buf[16];

	int set=bankActive/16;
	int i,j,k,pp;
	int x,y,w,h;
	bool skip[256]={false};
	bool doWhole;
	int cnt=0;

	//these allow nonselected tiles to be skipped.
	GetSelection(chrSelection,x,y,w,h);
	if(chrSelectRect)
	{
		if(w>1||h>1)
		{
			for(i=0;i<16;++i)
			{
				for(j=0;j<16;++j)
				{
					skip[i*16+j]=(i>=y&&i<y+h&&j>=x&&j<x+w)?false:true;
				}
			}
		}
	}
	else
	{
		for(i=0;i<256;++i)
		{
			skip[i]= chrSelected[i]?false:true;
			if (chrSelected[i]) cnt++;
		}
	}

	if (cnt>0 || (chrSelectRect && (w>1||h>1))) doWhole=false;
	else doWhole=true;

	SetUndo();

	int tile=0;
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			if(!doWhole && skip[j+i*16]) {tile++; continue;}

			//int y=tileViewTable[j+i*16]/16;
			//int x=tileViewTable[j+i*16]&15;

			int pp=tileViewTable[tile]*16+bankViewTable[set+tile];
			memcpy(buf,chr+pp,16);
			for(k=0;k<8;k++){
			   chr[tileViewTable[tile]*16+bankViewTable[set+tile] + k]  	=buf[k*2];
			   chr[tileViewTable[tile]*16+bankViewTable[set+tile] + k+8]	=buf[k*2 +1];
			}

			tile++;
		}
	}
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Deres1Click(TObject *Sender)
{
 		char buf[16];
	int tag= ((TMenuItem*)Sender)->Tag;
	int set=bankActive/16;
	int i,j,k,pp;
	int x,y,w,h;
	bool skip[256]={false};
	bool doWhole;
	int cnt=0;

	//these allow nonselected tiles to be skipped.
	GetSelection(chrSelection,x,y,w,h);
	if(chrSelectRect)
	{
		if(w>0||h>0)
		{
			for(i=0;i<16;++i)
			{
				for(j=0;j<16;++j)
				{
					skip[i*16+j]=(i>=y&&i<y+h&&j>=x&&j<x+w)?false:true;
				}
			}
		}
	}
	else
	{
		for(i=0;i<256;++i)
		{
			skip[i]= chrSelected[i]?false:true;
			if (chrSelected[i]) cnt++;
		}
	}

	if (cnt>0 || (chrSelectRect && (w>0||h>0))) doWhole=false;
	else doWhole=true;

	SetUndo();

	int tile=0;
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			if(!doWhole && skip[j+i*16]) {tile++; continue;}

			//int y=tileViewTable[j+i*16]/16;
			//int x=tileViewTable[j+i*16]&15;

			int pp=tileViewTable[tile]*16+bankViewTable[set+tile];
			memcpy(buf,chr+pp,16);

			if(tag==0 || tag==2){
			for(k=0;k<16;k++){
				char byte=buf[k];
				for (int l=0; l<8; l += 2) {
					if ((byte & (1 << l )) || (byte & (1 << l +1 ))) {
						byte |= (1 << l );
						byte |= (1 << l +1);

					}
					else {
					byte &= ~(1 << l);
					byte &= ~(1 << l+1);}
				}

				buf[k]=byte;

				}
				for(k=0;k<16;k++){
					chr[tileViewTable[tile]*16+bankViewTable[set+tile] + k]  	=buf[k];
				}
			}
				if(tag==1 || tag==2){
				for(k=0;k<8;k++){
					char byte=buf[k*2]|buf[k*2 +1];
                    chr[tileViewTable[tile]*16+bankViewTable[set+tile] + k*2]  	=byte;
					chr[tileViewTable[tile]*16+bankViewTable[set+tile] + k*2+1]  	=byte;
				}
			}
			tile++;
		}
	}
	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ButtonForceViewMouseEnter(TObject *Sender)
{
	LabelStats->Caption="Automatically forces tileset to be in 8x16 view mode while 8x16 sprite mode is on.";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CHRBankSwapper1Click(TObject *Sender)
{
	FormSwapBanks->Show();	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::StarterA1Click(TObject *Sender)
{
	SetUndo();
	memcpy(&bgPal[palBank*16],&bgPalDefault_NEXXT2[((TMenuItem*)Sender)->Tag*16],16);

	DrawPalettes();
	UpdateAll();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::LineQuickmultiline1Click(TObject *Sender)
{

	if(SpeedButtonTypeIn->Down) return;
	FormLineDetails->btnQuick->Down^=true;
	LineQuickmultiline1->Checked=FormLineDetails->btnQuick->Down;
	int i = iLinePresetIndex;
	bLinePreset_adjustAutoreset[i]=FormLineDetails->btnSmear->Down;
	FormLineDetails->LinePresetSaveTimer->Enabled=true;


	if(FormLineDetails->btnQuick->Down) 	LabelStats->Caption="Quick multiline ON.";
	else			   					LabelStats->Caption="Quick multiline OFF.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::LineCoating1Click(TObject *Sender)
{
	if(SpeedButtonTypeIn->Down) return;
	FormLineDetails->btnSmear->Down^=true;
	LineCoating1->Checked=FormLineDetails->btnSmear->Down;
	int i = iLinePresetIndex;
	bLinePreset_modeCoat[i]=FormLineDetails->btnSmear->Down;
	FormLineDetails->LinePresetSaveTimer->Enabled=true;

	if(FormLineDetails->btnSmear->Down) 	LabelStats->Caption="Coat mode ON.";
	else			   					LabelStats->Caption="Coat mode OFF.";
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::PresetA1Click(TObject *Sender)
{
	TMenuItem *tmp = dynamic_cast<TMenuItem*>(Sender);
	if (tmp){
		iLinePresetIndex = tmp->Tag;
	}
	FormLineDetails->GetPreset();
	switch(iLinePresetIndex)
	{
	case 1:
		LabelStats->Caption="Line preset B selected.";
		FormLineDetails->btnPresetB->Down=true;
		break;

	case 2:
		LabelStats->Caption="Line preset C selected.";
		FormLineDetails->btnPresetC->Down=true;
		break;

	case 3:
		LabelStats->Caption="Line preset D selected.";
		FormLineDetails->btnPresetD->Down=true;
		break;
	default:
		LabelStats->Caption="Line preset A selected.";
    	FormLineDetails->btnPresetA->Down=true;
	}
	StatusUpdateWaiter->Enabled=true;
	holdStats=true;


}
//---------------------------------------------------------------------------

void __fastcall TFormMain::BrushToolbox1Click(TObject *Sender)
{
	FormBrush->Visible^=true;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Buckettoolbox2Click(TObject *Sender)
{
		FormBucketToolbox->Visible^=true;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Showhide1Click(TObject *Sender)
{
	FormLineDetails->Visible^=true;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Protectcolour01Click(TObject *Sender)
{

	if (!lineDrawing) {
	//linedrawing has its own implementation	

	int i;
	TMenuItem *tmp = dynamic_cast<TMenuItem*>(Sender);
	if (tmp){
		i = tmp->Tag;
	}
	if(i==0){
		Protect0->Down^=true; FormCHREditor->Protect0->Down = Protect0->Down;
		if(Protect0->Down) 	LabelStats->Caption="Colour 0 Protected.";
		else			   	LabelStats->Caption="Colour 0 Overwritable.";
		StatusUpdateWaiter->Enabled=true; holdStats=true;
	}

	if(i==1) {
		Protect1->Down^=true; FormCHREditor->Protect1->Down = Protect1->Down;
		if(Protect1->Down) 	LabelStats->Caption="Colour 1 Protected.";
		else			   	LabelStats->Caption="Colour 1 Overwritable.";
		StatusUpdateWaiter->Enabled=true; holdStats=true;
	}

	if(i==2) {
		Protect2->Down^=true; FormCHREditor->Protect2->Down = Protect2->Down;
		if(Protect2->Down) 	LabelStats->Caption="Colour 2 Protected.";
		else			   	LabelStats->Caption="Colour 2 Overwritable.";
		StatusUpdateWaiter->Enabled=true; holdStats=true;
	}

	if(i==3) {
		Protect3->Down^=true; FormCHREditor->Protect3->Down = Protect3->Down;
		if(Protect3->Down) 	LabelStats->Caption="Colour 3 Protected.";
		else			   	LabelStats->Caption="Colour 3 Overwritable.";
		StatusUpdateWaiter->Enabled=true; holdStats=true;
	}}
	Protectcolour01->Checked=Protect0->Down;
	Protectcolour11->Checked=Protect1->Down;
	Protectcolour21->Checked=Protect2->Down;
	Protectcolour31->Checked=Protect3->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TimerLinkedModeRefreshTimer(TObject *Sender)
{
	if(!openByFileDone) return;

	TimerLinkedModeRefresh->Enabled=false;
	FormBankCHR->MakeList(true,true);
}
//---------------------------------------------------------------------------



void __fastcall TFormMain::Reloaddefaults1Click(TObject *Sender)
{
	FILE *file;
	unsigned char buf[64];
	bool bUserSuccess=false;
	AnsiString dir;
	dir=globalDir;

	file=fopen((dir+"default_subpals.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==64)
		 {
			fread(buf,64,1,file);
			fclose(file);
			memcpy(bgPal,buf,64);
			memcpy(bgPalUndo,buf,64);
			memcpy(bgPalCheckpoint,buf,64);
			memcpy(bgPalUndoCheckPoint,buf,64);

			bUserSuccess=true;
		 }
	}


	file=fopen((dir+"default_subpal0.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(bgPal,buf,16);
			memcpy(bgPalUndo,buf,16);
			memcpy(bgPalCheckpoint,buf,16);
			memcpy(bgPalUndoCheckPoint,buf,16);

			bUserSuccess=true;
		 }
	}
	file=fopen((dir+"default_subpal1.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(&bgPal[16],buf,16);
			memcpy(&bgPalUndo[16],buf,16);
			memcpy(&bgPalCheckpoint[16],buf,16);
			memcpy(&bgPalUndoCheckPoint[16],buf,16);

			bUserSuccess=true;
		 }
	}
	file=fopen((dir+"default_subpal2.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(&bgPal[32],buf,16);
			memcpy(&bgPalUndo[32],buf,16);
			memcpy(&bgPalCheckpoint[32],buf,16);
			memcpy(&bgPalUndoCheckPoint[32],buf,16);

			bUserSuccess=true;
		 }
	}
	file=fopen((dir+"default_subpal3.pal").c_str(),"rb");
	if(file)
	{
		 if(get_file_size(file)==16)
		 {
			fread(buf,16,1,file);
			fclose(file);
			memcpy(&bgPal[48],buf,16);
			memcpy(&bgPalUndo[48],buf,16);
			memcpy(&bgPalCheckpoint[48],buf,16);
			memcpy(&bgPalUndoCheckPoint[48],buf,16);

			bUserSuccess=true;
		 }
	}

	if(bUserSuccess) LabelStats->Caption="Subpalettes reloaded from user defaults.";
	else 			 LabelStats->Caption="No user default present. Reloaded from internal default.";

	StatusUpdateWaiter->Enabled=true;
	holdStats=true;

	UpdateAll();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Storeallsetsasnewstartupdefault1Click(
      TObject *Sender)
{
	BlockDrawing(true);

	FILE *file;

	file=fopen((globalDir+"default_subpals.pal").c_str(),"wt");

	if(!file)
	{
		Application->MessageBox("Couldn't save default subpalette sets to file","Warning",MB_OK);
		return;
	}

	//pal_validate();
	fwrite(bgPal,64,1,file);
	fclose(file);


	fclose(file);
	BlockDrawing(false);

	LabelStats->Caption="Subpalette sets A...D stored as user startup defaults.";

	StatusUpdateWaiter->Enabled=true;
	holdStats=true;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Storethissetasoverloadingdefault1Click(
      TObject *Sender)
{
     BlockDrawing(true);

	FILE *file;

	AnsiString name="default_subpal0.pal";

	if(palBank==0)  name="default_subpal0.pal";
	if(palBank==1)  name="default_subpal1.pal";
	if(palBank==2)  name="default_subpal2.pal";
	if(palBank==3)  name="default_subpal3.pal";

	file=fopen((globalDir+name).c_str(),"wt");

	if(!file)
	{
		Application->MessageBox("Couldn't save default subpalette sets to file","Warning",MB_OK);
		return;
	}


	int tmp=palBank*16;

	fwrite(&bgPal[tmp],16,1,file);
	fclose(file);
	BlockDrawing(false);


	if(palBank==0) LabelStats->Caption="Subpalette as A stored to user startup defaults.";
	if(palBank==1) LabelStats->Caption="Subpalette as B stored to user startup defaults.";
	if(palBank==2) LabelStats->Caption="Subpalette as C stored to user startup defaults.";
	if(palBank==3) LabelStats->Caption="Subpalette as D stored to user startup defaults.";

	StatusUpdateWaiter->Enabled=true;
	holdStats=true;


}
//---------------------------------------------------------------------------


void __fastcall TFormMain::Save16KSetsAD1Click(TObject *Sender)
{
	BlockDrawing(true);
	SaveCHR(0,8192*2,true);
	BlockDrawing(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Pastemodetoolbox1Click(TObject *Sender)
{
	FormSpecialPasteCHR->Visible^=true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpecialA1Click(TObject *Sender)
{
	int i=0;
	TMenuItem *tmp = dynamic_cast<TMenuItem*>(Sender);
	if (tmp){
		i = tmp->Tag;
	}
	PasteCHR(i);
}
//---------------------------------------------------------------------------



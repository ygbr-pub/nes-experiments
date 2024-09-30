//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"
#include "UnitMain.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TAboutBox *AboutBox;
//--------------------------------------------------------------------- 
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TAboutBox::FormShow(TObject *Sender)
{
	ProductName->Caption=Application->Title;
	StaticText1->Caption="Build date: April 11th, 2024.\n\nDeveloped by FrankenGraphics.\nBased on NESST; developed by Shiru.\n\nThis software is Public Domain.\n\nThanks to Antoine Gohin (Broke Studio), jroweboy,\nand the NESdev community.\n\nSpecial thanks to Shiru, and to my patreon supporters, for without this work would not have come this far:\n\nMatthew Klundt, W-, Ninja Dynamics, Ted Milker, Matthew Cooper, Michael Thompson, afk, mark666, ALR, Robert Dowden, Marc Moore, Margaret McNulty-Beldyk, Jayson, Ryan Pilbean, zzox, Sean Robinson, Leo, ringring answer, Isabel, Joshua Molina, Eric DeSantis, RetroNES Software, EJ, Jay Carter, Daniel Greenberg (Winterion Game Studios), Brooks Long, Chuck Kowalski, Colin Reed, Jean-Phillipe Grenier, Erik Brunther, Pepe Spicer, JKJK, Colin Kingfisher, Juan Pablo Nunez Rojas, Howie Day, Michael Jung, Amina, NovaSquirrel, Jeff Kiska, ReJ aka Renaldas Zioma, michael_emh, Farmer Dan, Gordea, Jaume Alcazo Castellarnau, Matt Haddow, Santiago Zapata (slashie), Rusty Gerard, Joe's Computer Museum, Nicholas Berthiaume, Barry White, Kacper Wozniak, Jacob Speicher, Ben Smith, Kalle Siukola, Cornel, Kg. Madee II, David Hayes, Justin Orenich, Nealon Ledbetter, Infinite NES Lives LLC, André Luís Baptista da Silva, Synvinkel, Ryan Nighswander, zeta0134, Raul de Avila Jr, yaros, Deadeye, Trek Glowacki, BIG EVIL CORPORATION, an insignificant speck of dust, joe1597, Lamonda Steele, Max Meiners, Jone, Lollie, Matt Roszak, Adrian Gordon, Pez Pengelly, Stephen A. Goss, Richard Thames, Raftronaut, Scralings, Frank Provo, María Amat, Paul Preuss, Belmont, Dan Fries, Thomas Griebel, FireSkies Studios.";
	StaticText1->Width=300;
	StaticText1->Height=470;
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::OKButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::btnItchClick(TObject *Sender)
{
	ShellExecute(NULL, "open", "https://frankengraphics.itch.io/nexxt", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::btnCommunityClick(TObject *Sender)
{
  ShellExecute(NULL, "open", "https://frankengraphics.itch.io/nexxt/community", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::btnShiruClick(TObject *Sender)
{
ShellExecute(NULL, "open", "https://shiru.untergrund.net/index.shtml", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::btnTwitterClick(TObject *Sender)
{
	ShellExecute(NULL, "open", "https://twitter.com/FrankenGraphics", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::btnPatreonClick(TObject *Sender)
{
	ShellExecute(NULL, "open", "https://www.patreon.com/frankengraphics", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------

void __fastcall TAboutBox::Button1Click(TObject *Sender)
{
	ShellExecute(NULL, "open", "https://mastodon.art/@FrankenGraphics", "", NULL, SW_RESTORE);
}
//---------------------------------------------------------------------------


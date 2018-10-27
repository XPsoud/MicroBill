#include "splashscreen.h"

#include "microbill.png.h"

SplashScreen::SplashScreen() : wxFrame(NULL, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE)
{
    wxPanel *pnl = new wxPanel(this, -1);
    wxBoxSizer *szr = new wxBoxSizer(wxVERTICAL);

    wxStaticBitmap *stBmp=new wxStaticBitmap(pnl, wxID_STATIC, wxGet_MicroBill_png_Bitmap());
    szr->Add(stBmp, 0, wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER_HORIZONTAL, 10);

    wxStaticText *label = new wxStaticText(pnl, wxID_STATIC, _("Your micro entreprise billing software"));
    wxFont fnt = label->GetFont().MakeLarger().MakeLarger().MakeLarger();
    label->SetFont(fnt);
    szr->Add(label, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 10);

    pnl->SetSizer(szr);

    szr->SetSizeHints(this);

}

SplashScreen::~SplashScreen()
{
    //dtor
}

#include "dlgabout.h"

#include "main.h"
#include "appversion.h"

#include <wx/bitmap.h>
#include <wx/statbmp.h>
#include <wx/statline.h>

#include "aboutlogo.h"

DlgAbout::DlgAbout(wxWindow *parent) : wxDialog(parent, -1, wxGetStockLabel(wxID_ABOUT, wxSTOCK_NOFLAGS), wxDefaultPosition, wxDefaultSize)
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgAbout\" object\n"));
#endif // __WXDEBUG__
    wxStaticBitmap *BmpCtrl;
    wxStaticText *label;

    wxString sMsg=_("Made with");
    sMsg << _T(" ") << wxGetApp().GetBuildInfos(false);

    wxString sTitle=_T(PRODUCTNAME);
    sTitle << _T(" (v") << VERSION_MAJOR << _T(".") << VERSION_MINOR << _T(".") << VERSION_REV << _T(".") << VERSION_BUILD << _T(")");

    wxBoxSizer *szrMain=new wxBoxSizer(wxVERTICAL);
        wxBoxSizer *szrTop=new wxBoxSizer(wxHORIZONTAL);
            BmpCtrl=new wxStaticBitmap(this, -1, wxGet_AboutLogo_png_Bitmap(), wxPoint(10,15));
            szrTop->Add(BmpCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
            wxBoxSizer *szrRight=new wxBoxSizer(wxVERTICAL);
                label=new wxStaticText(this, -1, sTitle);
                wxFont fntTitle=label->GetFont();
                fntTitle.MakeLarger();
                fntTitle.MakeBold();
                label->SetFont(fntTitle);
                szrRight->Add(label, 0, wxALL|wxALIGN_CENTER, 5);
                label=new wxStaticText(this, -1, _T(COPYRIGHT));
                szrRight->Add(label, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER, 5);
                label=new wxStaticText(this, -1, wDESCRIPTION);
                szrRight->Add(label, 0, wxLEFT|wxRIGHT|wxTOP|wxALIGN_CENTER, 5);
                label=new wxStaticText(this, -1, sMsg);
                szrRight->Add(label, 0, wxALL|wxALIGN_CENTER, 5);
            szrTop->Add(szrRight, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
        szrMain->Add(szrTop, 0, wxALL, 5);

        szrMain->Add(CreateSeparatedButtonSizer(wxCANCEL), 0, wxALL|wxEXPAND, 5);

        wxButton *btnCancel=(wxButton*)FindWindow(wxID_CANCEL);
        if (btnCancel!=NULL)
            btnCancel->SetLabel(wxGetStockLabel(wxID_CLOSE));
    SetSizer(szrMain);
    szrMain->SetSizeHints(this);

    CenterOnParent();
}

DlgAbout::~DlgAbout()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgAbout\" object\n"));
#endif // __WXDEBUG__
}

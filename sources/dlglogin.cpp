#include "dlglogin.h"

#include "key.png.h"

#include <wx/statline.h>

DlgLogin::DlgLogin(wxWindow *parent, const wxString& password, int iMaxAttemps) : wxDialog(parent, -1, _("Restricted access"))
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgLogin\" object\n"));
#endif // __WXDEBUG__

    m_sPassword=password;
    m_iAttemps = 0;
    m_iMaxAttemps = iMaxAttemps;

    CreateControls();

    ConnectControls();

    CenterOnParent();

    m_txtPwd->SetFocus();
}

DlgLogin::~DlgLogin()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgLogin\" object\n"));
#endif // __WXDEBUG__
}

void DlgLogin::CreateControls()
{
    wxStaticText *label;
    wxBoxSizer *szrMain=new wxBoxSizer(wxVERTICAL), *hszr;

        hszr=new wxBoxSizer(wxHORIZONTAL);

            wxStaticBitmap *stBmp=new wxStaticBitmap(this, wxID_STATIC, wxGet_key_png_Bitmap());
            hszr->Add(stBmp, 0, wxALL|wxALIGN_CENTER_VERTICAL, 10);
            label=new wxStaticText(this, wxID_STATIC, _("Access to this application has been\nprotected with a password."));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 10);

        szrMain->Add(hszr, 0, wxALL, 0);

        m_lblWrongPwd=new wxStaticText(this, wxID_STATIC, _("Incorrect password!"));
            wxFont fnt=m_lblWrongPwd->GetFont();
            fnt.MakeBold();
            m_lblWrongPwd->SetFont(fnt);
            m_lblWrongPwd->SetForegroundColour(*wxRED);
        szrMain->Add(m_lblWrongPwd, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 10);

        hszr=new wxBoxSizer(wxHORIZONTAL);

            label=new wxStaticText(this, wxID_STATIC, _("Enter the access password:"));
            hszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);

            m_txtPwd=new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
            hszr->Add(m_txtPwd, 1, wxLEFT, 5);

        szrMain->Add(hszr, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 10);

        m_chkShowPwd=new wxCheckBox(this, -1, _("Display password in plain text"));
        szrMain->Add(m_chkShowPwd, 0, wxLEFT|wxRIGHT|wxBOTTOM, 10);

        hszr=(wxBoxSizer*)CreateSeparatedButtonSizer(wxOK|wxCANCEL);
        szrMain->Add(hszr, 0, wxALL|wxEXPAND, 5);

    SetSizer(szrMain);

    m_lblWrongPwd->Hide();

    szrMain->SetSizeHints(this);
}

void DlgLogin::ConnectControls()
{
    m_chkShowPwd->Bind(wxEVT_CHECKBOX, &DlgLogin::OnChkShowPasswordClicked, this);
    Bind(wxEVT_BUTTON, &DlgLogin::OnBtnOkClicked, this, wxID_OK);
}

void DlgLogin::OnChkShowPasswordClicked(wxCommandEvent& event)
{
    Freeze();
    wxString sPass=m_txtPwd->GetValue();
    wxSizer *szr=m_txtPwd->GetContainingSizer();
    long lFrom, lTo;
    m_txtPwd->GetSelection(&lFrom, &lTo);
    int iStyle=(m_chkShowPwd->IsChecked()?0:wxTE_PASSWORD);
    szr->Detach(m_txtPwd);
    delete m_txtPwd;
    m_txtPwd=new wxTextCtrl(this, -1, sPass, wxDefaultPosition, wxDefaultSize, iStyle);
    m_txtPwd->SetFocus();
    m_txtPwd->SetSelection(lFrom, lTo);
    szr->Add(m_txtPwd, 1, wxLEFT, 5);
    szr->Layout();
    Thaw();
}

void DlgLogin::OnBtnOkClicked(wxCommandEvent& event)
{
    bool bOk=(m_sPassword==m_txtPwd->GetValue());

    if (bOk || (m_iAttemps==(m_iMaxAttemps-1)))
        EndModal(bOk?wxID_OK:wxID_ABORT);

    if (m_iAttemps==0)
    {
        m_lblWrongPwd->Show();
        GetSizer()->SetSizeHints(this);
        CenterOnParent();
    }
    m_txtPwd->SetFocus();
    m_txtPwd->SetSelection(-1, -1);
    m_iAttemps++;
    m_lblWrongPwd->SetLabel(wxString::Format(_("Incorrect password: trial %d on %d"), m_iAttemps+1, m_iMaxAttemps));
    GetSizer()->Layout();
}

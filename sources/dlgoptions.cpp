#include "dlgoptions.h"

#include "settingsmanager.h"

#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/stdpaths.h>
#include <wx/richtooltip.h>

DlgOptions::DlgOptions(wxWindow *parent)
    : wxDialog(parent, -1, wxGetStockLabel(wxID_PREFERENCES, wxSTOCK_NOFLAGS)),
    m_options(SettingsManager::Get())
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DlgOptions\" object\n"));
#endif // __WXDEBUG__

    CreateControls();

    ConnectControls();

    FillControls();

    GetSizer()->SetSizeHints(this);

    CenterOnParent();
}

DlgOptions::~DlgOptions()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DlgOptions\" object\n"));
#endif // __WXDEBUG__
}

void DlgOptions::CreateControls()
{
    wxBoxSizer *szrMain=new wxBoxSizer(wxVERTICAL), *lnszr, *pageszr;

        m_nBook=new wxNotebook(this, -1);
        wxPanel *page;
        wxStaticBoxSizer *box, *box2;
        wxFlexGridSizer *flxszr;
        wxStaticText *label;
        wxString sText;

        // "General" tab
        page=new wxPanel(m_nBook, -1);
            pageszr=new wxBoxSizer(wxVERTICAL);

                box=new wxStaticBoxSizer(wxVERTICAL, page, _("Position of the main window:"));
                    label=new wxStaticText(box->GetStaticBox(), -1, _("Position of the main window at application startup"));
                    box->Add(label, 0, wxALL, 5);
                    lnszr=new wxBoxSizer(wxHORIZONTAL);
                        m_optStartType[0]=new wxRadioButton(box->GetStaticBox(), -1, _("Predefined position"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
                        lnszr->Add(m_optStartType[0], 0, wxALL, 5);
                        m_optStartType[1]=new wxRadioButton(box->GetStaticBox(), -1, _("Last registered position"));
                        lnszr->Add(m_optStartType[1], 0, wxTOP|wxBOTTOM|wxRIGHT, 5);
                    box->Add(lnszr, 0, wxALL, 0);

                    box2=new wxStaticBoxSizer(wxVERTICAL, box->GetStaticBox(), _("Predefined positions:"));
                    const wxChar* szLabels[9] = {   _("Top-Left"), _("Top-Center"), _("Top-Right"),
                                                    _("Middle-Left"), _("Center-Screen"), _("Middle-Right"),
                                                    _("Bottom-Left"), _("Bottom-Center"), _("Bottom-Right")};
                    m_stbPos=box2->GetStaticBox();
                    flxszr=new wxFlexGridSizer(3, 5, 5);
                        for (int i=0; i<9; i++)
                        {
                            m_optDefPos[i]=new wxRadioButton(m_stbPos, -1, szLabels[i], wxDefaultPosition, wxDefaultSize, (i==0?wxRB_GROUP:0));
                            flxszr->Add(m_optDefPos[i]);
                        }
                        flxszr->AddGrowableCol(0, 1);
                        flxszr->AddGrowableCol(1, 1);
                        box2->Add(flxszr, 0, wxALL|wxEXPAND, 5);
                    box->Add(box2, 0, wxALL|wxEXPAND, 5);
                pageszr->Add(box, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);

                box=new wxStaticBoxSizer(wxVERTICAL, page, _("Misc :"));
                    m_chkSingleInstance=new wxCheckBox(box->GetStaticBox(), -1, _("Allow only one instance of the application"));
                    box->Add(m_chkSingleInstance, 0, wxALL, 5);
                    m_chkCompSettings=new wxCheckBox(box->GetStaticBox(), -1, _("Compress settings file (for size and privacy)"));
                    box->Add(m_chkCompSettings, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5);
                pageszr->Add(box, 0, wxALL|wxEXPAND, 5);

            page->SetSizer(pageszr);
        m_nBook->AddPage(page, _("General"));

        // "Language" tab
        page=new wxPanel(m_nBook, -1);
            pageszr=new wxBoxSizer(wxVERTICAL);

                box=new wxStaticBoxSizer(wxVERTICAL, page, _("Interface language:"));
                    m_chkKeepLang=new wxCheckBox(box->GetStaticBox(), -1, _("Keep the interface language in English (restart needed)"));
                    box->Add(m_chkKeepLang, 0, wxALL, 5);
                pageszr->Add(box, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

                box=new wxStaticBoxSizer(wxVERTICAL, page, _("Monetary sign:"));
                    lnszr=new wxBoxSizer(wxHORIZONTAL);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("Monetary sign to use:"));
                        lnszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                        m_cmbSign = new wxChoice(box->GetStaticBox(), -1, wxDefaultPosition, wxSize(50, -1));
                        lnszr->Add(m_cmbSign, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("Placed"));
                        lnszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        m_optSignPos[0] = new wxRadioButton(box->GetStaticBox(), -1, _("before"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
                        lnszr->Add(m_optSignPos[0], 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        m_optSignPos[1] = new wxRadioButton(box->GetStaticBox(), -1, _("after"));
                        lnszr->Add(m_optSignPos[1], 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("the value"));
                        lnszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                    box->Add(lnszr, 0, wxALL, 5);
                pageszr->Add(box, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

            page->SetSizer(pageszr);
        m_nBook->AddPage(page, _("Language"));

        // "Customization" tab
        page=new wxPanel(m_nBook, -1);
            pageszr=new wxBoxSizer(wxVERTICAL);

                box=new wxStaticBoxSizer(wxVERTICAL, page, _("Location of your entreprise:"));
                    sText = _("Enter the town in witch your company is located.");
                    sText << _T("\n") << _("It will be placed near the edition date of each pdf documents.");
                    label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, sText);
                    box->Add(label, 0, wxALL, 5);
                    lnszr=new wxBoxSizer(wxHORIZONTAL);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("Location:"));
                        lnszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                        m_txtLocation = new wxTextCtrl(box->GetStaticBox(), -1, wxEmptyString);
                        lnszr->Add(m_txtLocation, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                    box->Add(lnszr, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, 5);
                pageszr->Add(box, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

                box=new wxStaticBoxSizer(wxVERTICAL, page, _("Header image:"));
                    m_stbHeaderImg = new wxStaticBitmap(box->GetStaticBox(), wxID_STATIC, wxBitmap(m_imgHeader));
                    box->Add(m_stbHeaderImg, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
                    lnszr = new wxBoxSizer(wxHORIZONTAL);
                        m_btnSelectImg = new wxButton(box->GetStaticBox(), -1, _("Select an image"));
                        lnszr->Add(m_btnSelectImg, 0, wxALL, 0);
                        m_btnClearImg = new wxButton(box->GetStaticBox(), -1, _("Remove custom image"));
                        lnszr->Add(m_btnClearImg, 0, wxLEFT, 5);
                        m_btnImgInfos = new wxButton(box->GetStaticBox(), -1, _("Infos"));
                        lnszr->Add(m_btnImgInfos, 0, wxLEFT, 5);
                    box->Add(lnszr, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5);
                pageszr->Add(box, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

                box=new wxStaticBoxSizer(wxVERTICAL, page, _("Footer text:"));
                    lnszr=new wxBoxSizer(wxHORIZONTAL);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("Text:"));
                        lnszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                        m_txtFooter = new wxTextCtrl(box->GetStaticBox(), -1, wxEmptyString);
                        lnszr->Add(m_txtFooter, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                    box->Add(lnszr, 0, wxALL|wxEXPAND, 5);
                    lnszr = new wxBoxSizer(wxHORIZONTAL);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("Style:"));
                        lnszr->Add(label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
                        m_chkFootItalic = new wxCheckBox(box->GetStaticBox(), -1, _("Italic"));
                        lnszr->Add(m_chkFootItalic, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        m_chkFootBold = new wxCheckBox(box->GetStaticBox(), -1, _("Bold"));
                        lnszr->Add(m_chkFootBold, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("Size:"));
                        lnszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        m_cmbFootSize = new wxChoice(box->GetStaticBox(), -1);
                            for (int i=6; i<13; ++i)
                                m_cmbFootSize->Append(wxString::Format(_T("%d"), i));
                        lnszr->Add(m_cmbFootSize, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        label = new wxStaticText(box->GetStaticBox(), wxID_STATIC, _("Color:"));
                        lnszr->Add(label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                        m_cpcFootColor = new wxColourPickerCtrl(box->GetStaticBox(), -1);
                        lnszr->Add(m_cpcFootColor, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
                    box->Add(lnszr, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5);
                pageszr->Add(box, 0, wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5);

            page->SetSizer(pageszr);
        m_nBook->AddPage(page, _("Customization"));


        szrMain->Add(m_nBook, 1, wxALL|wxEXPAND, 0);

        lnszr=(wxBoxSizer*)CreateSeparatedButtonSizer(wxOK|wxCANCEL|wxAPPLY);
            m_btnApply=(wxButton*)FindWindow(wxID_APPLY);
        szrMain->Add(lnszr, 0, wxALL|wxEXPAND, 5);
    SetSizer(szrMain);
}

void DlgOptions::ConnectControls()
{
    Bind(wxEVT_BUTTON, &DlgOptions::OnBtnApplyClicked, this, wxID_APPLY);
    Bind(wxEVT_BUTTON, &DlgOptions::OnBtnOkClicked, this, wxID_OK);
    for (int i=0; i<2; i++)
    {
        m_optStartType[i]->Bind(wxEVT_RADIOBUTTON, &DlgOptions::OnStartupPosTypeChanged, this);
    }
    for (int i=0; i<9; i++)
    {
        m_optDefPos[i]->Bind(wxEVT_RADIOBUTTON, &DlgOptions::OnSomethingHasChanged, this);
    }

    Bind(wxEVT_CHECKBOX, &DlgOptions::OnSomethingHasChanged, this);
    Bind(wxEVT_TEXT, &DlgOptions::OnSomethingHasChanged, this);
    Bind(wxEVT_RADIOBUTTON, &DlgOptions::OnSomethingHasChanged, this);
    Bind(wxEVT_CHOICE, &DlgOptions::OnSomethingHasChanged, this);

    m_btnSelectImg->Bind(wxEVT_BUTTON, &DlgOptions::OnBtnSelectImgClicked, this);
    m_btnClearImg->Bind(wxEVT_BUTTON, &DlgOptions::OnBtnRemoveImgClicked, this);
    m_btnClearImg->Bind(wxEVT_UPDATE_UI, &DlgOptions::OnUpdateUI_BtnClearImg, this);
    m_btnImgInfos->Bind(wxEVT_BUTTON, &DlgOptions::OnBtnImgInfosClicked, this);
}

void DlgOptions::FillControls()
{
    int iStartPos=m_options.GetMainWndStartupPos();
    int iOptButton;
    wxCommandEvent evt(wxEVT_COMMAND_RADIOBUTTON_SELECTED, 0);
    if (iStartPos==wxALIGN_NOT)
    {
        evt.SetId(m_optStartType[1]->GetId());
        m_optStartType[1]->SetValue(true);
        m_optDefPos[4]->SetValue(true);
    }
    else
    {
        evt.SetId(m_optStartType[0]->GetId());
        m_optStartType[0]->SetValue(true);
        if (iStartPos==wxCENTER_ON_SCREEN)
        {
            iOptButton=4;
        }
        else
        {
            int iX=(((iStartPos&wxRIGHT)==wxRIGHT)?2:((iStartPos&wxLEFT)==wxLEFT)?0:1);
            int iY=(((iStartPos&wxBOTTOM)==wxBOTTOM)?6:((iStartPos&wxTOP)==wxTOP)?0:3);
            iOptButton=iX+iY;
        }
        m_optDefPos[iOptButton]->SetValue(true);
    }
    OnStartupPosTypeChanged(evt);

    m_chkSingleInstance->SetValue(m_options.GetMultipleInstancesAllowed()==false);
    m_chkCompSettings->SetValue(m_options.GetCompressSettings());

    m_chkKeepLang->SetValue(m_options.GetProhibitI18N());
    const wxArrayString& arsMoneySigns = m_options.GetMoneySigns();
    for (size_t i=0; i<arsMoneySigns.Count(); ++i)
        m_cmbSign->Append(arsMoneySigns[i]);
    m_cmbSign->SetStringSelection(m_options.GetMonetarySign());
    int iIndex = (m_options.GetMoneySignPos()==wxLEFT)?0:1;
    m_optSignPos[iIndex]->SetValue(true);

    m_txtLocation->ChangeValue(m_options.GetCompanyLocation());
    SetHeaderImage(m_options.GetHeaderImage());
    m_bHeaderImageChanged = false;
    m_bImageHeaderDefault = !m_options.HasCustomHeaderImage();
    m_nBook->GetPage(2)->GetSizer()->Layout();
    m_txtFooter->ChangeValue(m_options.GetFooterText());
    wxString sStyle = m_options.GetFooterStyle();
    unsigned char r, g, b;
    long lValue;
    sStyle.Mid(0, 3).ToLong(&lValue); r=(unsigned char)lValue;
    sStyle.Mid(3, 3).ToLong(&lValue); g=(unsigned char)lValue;;
    sStyle.Mid(6, 3).ToLong(&lValue); b=(unsigned char)lValue;
    sStyle.Mid(9, 2).ToLong(&lValue);
    m_chkFootItalic->SetValue(sStyle.Find(_T('I'))!=wxNOT_FOUND);
    m_chkFootBold->SetValue(sStyle.Find(_T('B'))!=wxNOT_FOUND);
    m_cmbFootSize->SetStringSelection(wxString::Format(_T("%d"), lValue));
    m_cpcFootColor->SetColour(wxColour(r, g, b));

    m_btnApply->Disable();
}

bool DlgOptions::ApplySettings()
{
    int iIndex=wxNOT_FOUND;
    if (m_optStartType[0]->GetValue())
    {
        for (int i=0; i<9; i++)
        {
            if (m_optDefPos[i]->GetValue())
            {
                iIndex=i;
                break;
            }
        }
    }
    int iStartPos=wxALIGN_NOT;
    if (iIndex>wxNOT_FOUND)
    {
        int iH=(((iIndex==0)||(iIndex==3)||(iIndex==6))?wxLEFT:((iIndex==1)||(iIndex==4)||(iIndex==7))?(wxDirection)wxCENTER:wxRIGHT);
        int iV=(((iIndex>-1)&&(iIndex<3))?wxTOP:((iIndex>2)&&(iIndex<6))?(wxDirection)wxCENTER:wxBOTTOM);
        if ((iH==wxCENTER)&&(iV==wxCENTER))
        {
            iStartPos=wxCENTER_ON_SCREEN;
        }
        else
        {
            iStartPos=iH|iV;
        }
    }
    m_options.SetMainWndStartupPos(iStartPos);

    m_options.SetMultipleInstancesAllowed(m_chkSingleInstance->IsChecked()==false);
    m_options.SetCompressSettings(m_chkCompSettings->IsChecked());

    m_options.SetProhibitI18N(m_chkKeepLang->IsChecked());
    m_options.SetMonetarySign(m_cmbSign->GetStringSelection());
    m_options.SetMoneySignPos(m_optSignPos[0]->GetValue()?wxLEFT:wxRIGHT);

    m_options.SetCompanyLocation(m_txtLocation->GetValue());

    if (m_bHeaderImageChanged)
    {
        if (m_bImageHeaderDefault)
        {
            m_options.SetHeaderImage(NULL);
        }
        else
        {
            m_options.SetHeaderImage(&m_imgHeader);
        }
    }

    wxString sStyle=wxEmptyString;
    wxColour fcol = m_cpcFootColor->GetColour();
    sStyle << wxString::Format(_T("%03d%03d%03d"), fcol.Red(), fcol.Green(), fcol.Blue());
    sStyle << wxString::Format(_T("00%s"), m_cmbFootSize->GetStringSelection()).Right(2);
    sStyle << (m_chkFootItalic->GetValue()?_T("I"):_T(""));
    sStyle << (m_chkFootBold->GetValue()?_T("B"):_T(""));
    m_options.SetFooterText(m_txtFooter->GetValue(), sStyle);

    m_btnApply->Disable();
    return true;
}

void DlgOptions::SetHeaderImage(const wxImage& img)
{
    double dHght = 50.;
    double dWdth = szDefaultHeaderImgSize.GetWidth()*dHght/szDefaultHeaderImgSize.GetHeight();
    m_imgHeader = img;
    wxImage tmpImg(img);
    tmpImg.Rescale(dWdth, dHght, wxIMAGE_QUALITY_HIGH);
    m_stbHeaderImg->SetBitmap(wxBitmap(tmpImg));
}

void DlgOptions::OnStartupPosTypeChanged(wxCommandEvent& event)
{
    bool bEnable=m_optStartType[0]->GetValue();
    m_stbPos->Enable(bEnable);
    for (int i=0; i<9; i++)
    {
        m_optDefPos[i]->Enable(bEnable);
    }
    OnSomethingHasChanged(event);
}

void DlgOptions::OnSomethingHasChanged(wxCommandEvent& event)
{
    m_btnApply->Enable();
}

void DlgOptions::OnBtnApplyClicked(wxCommandEvent& event)
{
    ApplySettings();
}

void DlgOptions::OnBtnOkClicked(wxCommandEvent& event)
{
    if (ApplySettings())
        EndModal(wxID_OK);
}

void DlgOptions::OnBtnSelectImgClicked(wxCommandEvent& event)
{
    wxString sDir = wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Pictures);
    wxString sWldcrd = _("All images files|*.png;*.jpg");
    sWldcrd << _T("|") << _("Png images files (*.png)|*.png");
    sWldcrd << _T("|") << _("Jpeg images files (*.jpg)|*.jpg");
    sWldcrd << _T("|") << _("All files (*.*)|*.*");

    wxString sImg = wxFileSelector(_("Select an image file"),
                                    sDir, wxEmptyString, wxEmptyString,
                                    sWldcrd,
                                    wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (sImg.IsEmpty())
        return;
    // Try to load the selected image file
    wxImage img;
    if (!img.LoadFile(sImg))
    {
        wxMessageBox(_("The selected image file can't be loaded !"), _("Error"), wxICON_EXCLAMATION|wxCENTER|wxOK);
        return;
    }

    SetHeaderImage(img);
    m_bImageHeaderDefault = false;
    m_bHeaderImageChanged = true;

    // The image has been changed : enable the "Apply" button
    m_btnApply->Enable();
}

void DlgOptions::OnBtnRemoveImgClicked(wxCommandEvent& event)
{
    SetHeaderImage(m_options.GetDefaultHeaderImage());
    m_bImageHeaderDefault = true;
    m_bHeaderImageChanged = true;

    // The image has been changed : enable the "Apply" button
    m_btnApply->Enable();
}

void DlgOptions::OnBtnImgInfosClicked(wxCommandEvent& event)
{
    wxString sTitle = _("Custom header image");
    wxString sMsg = _("Image that will be placed on top of pdf documents.");
    sMsg << _T("\n\n") << wxString::Format(_("The selected image must respect the ratio of %d x %d pixels."), szDefaultHeaderImgSize.GetWidth(), szDefaultHeaderImgSize.GetHeight());
    sMsg << _T("\n") << _("It will be resized to these values on pdf documents.");

    wxRichToolTip tip(sTitle, sMsg);
    tip.SetIcon(wxICON_INFORMATION);
    tip.ShowFor(m_btnImgInfos);
}

void DlgOptions::OnUpdateUI_BtnClearImg(wxUpdateUIEvent& event)
{
    event.Enable(!m_bImageHeaderDefault);
}

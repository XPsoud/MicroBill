#ifndef __DLGOPTIONS_H_INCLUDED__
#define __DLGOPTIONS_H_INCLUDED__

#include <wx/wx.h>
#include <wx/clrpicker.h>

class SettingsManager;
class wxNotebook;

class DlgOptions : public wxDialog
{
    public:
    DlgOptions(wxWindow *parent);
    virtual ~DlgOptions();
    private:
    // Misc functions
    void CreateControls();
    void FillControls();
    void ConnectControls();
    bool ApplySettings();
    void SetHeaderImage(const wxImage& img);
    // Events handlers
    void OnStartupPosTypeChanged(wxCommandEvent &event);
    void OnSomethingHasChanged(wxCommandEvent &event);
    void OnBtnApplyClicked(wxCommandEvent &event);
    void OnBtnOkClicked(wxCommandEvent &event);
    void OnBtnSelectImgClicked(wxCommandEvent &event);
    void OnBtnRemoveImgClicked(wxCommandEvent &event);
    void OnBtnImgInfosClicked(wxCommandEvent &event);
    void OnUpdateUI_BtnClearImg(wxUpdateUIEvent &event);
    // "General" tab controls vars
    wxRadioButton *m_optStartType[2], *m_optDefPos[9];
    wxStaticBox *m_stbPos;
    wxCheckBox *m_chkSingleInstance, *m_chkCompSettings, *m_chkSplashScreen;
    wxTextCtrl *m_txtPassword;
    // "Language" tab controls vars
    wxCheckBox *m_chkKeepLang;
    wxChoice *m_cmbSign;
    wxRadioButton *m_optSignPos[2];
    // "Customization" tab controls vars
    wxTextCtrl *m_txtLocation, *m_txtFooter;
    wxStaticBitmap *m_stbHeaderImg;
    wxButton *m_btnSelectImg, *m_btnClearImg, *m_btnImgInfos;
    wxCheckBox *m_chkFootItalic, *m_chkFootBold;
    wxChoice *m_cmbFootSize;
    wxColourPickerCtrl *m_cpcFootColor;
    // Main dialog box controls
    wxNotebook *m_nBook;
    wxButton *m_btnApply;
    // Misc vars
    SettingsManager& m_options;
    wxImage m_imgHeader;
    bool m_bHeaderImageChanged, m_bImageHeaderDefault;
};

#endif // __DLGOPTIONS_H_INCLUDED__

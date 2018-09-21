#ifndef __MAINFRAME_H_INCLUDED__
#define __MAINFRAME_H_INCLUDED__

#include <wx/wx.h>

class SettingsManager;
class wxNotebook;

#include "panelnbookpage.h"

class MainFrame: public wxFrame
{
    public:
        MainFrame(const wxString& title);
        ~MainFrame();
    private:
        // Misc functions
        void CreateControls();
        void ConnectControls();
        // Events handlers
        void OnSize(wxSizeEvent &event);
        void OnMove(wxMoveEvent &event);
        void OnPrefsClicked(wxCommandEvent &event);
        void OnExitClicked(wxCommandEvent &event);
        void OnAboutClicked(wxCommandEvent &event);
        void OnConvertEstim2Bill(wxCommandEvent &event);
        // Controls vars
        wxNotebook *m_nBook;
        PanelNBookPage* m_nbPage[PNBP_TYPE_COUNT];
        // Misc vars
        SettingsManager& m_settings;
};

#endif // __MAINFRAME_H_INCLUDED__

#ifndef __MAINFRAME_H_INCLUDED__
#define __MAINFRAME_H_INCLUDED__

#include <wx/wx.h>

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
        // Controls vars
        // Misc vars
};

#endif // __MAINFRAME_H_INCLUDED__

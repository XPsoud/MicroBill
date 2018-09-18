#ifndef __DLGADDEDITCLIENT_H_INCLUDED__
#define __DLGADDEDITCLIENT_H_INCLUDED__

#include <wx/wx.h>

class Client;

class DlgAddEditClient : public wxDialog
{
    public:
        DlgAddEditClient(wxWindow *parent, Client* item=NULL);
        virtual ~DlgAddEditClient();
        void SaveToClientItem(Client* item);
    protected:
    private:
        // Misc functions
        void CreateControls();
        void ConnectControls();
        // Events handlers
        void OnBtnValidClicked(wxCommandEvent &event);
        void OnUpdateUI_BtnValid(wxUpdateUIEvent &event);
        // Controls vars
        wxTextCtrl *m_txtCompany, *m_txtFirstName, *m_txtLastName, *m_txtEmail, *m_txtPhone, *m_txtCell;
        wxTextCtrl *m_txtAddr1, *m_txtAddr2, *m_txtZipCode, *m_txtCity, *m_txtCountry;
        // Misc vars
        Client* m_Client;
};

#endif // __DLGADDEDITCLIENT_H_INCLUDED__

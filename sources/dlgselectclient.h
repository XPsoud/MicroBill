#ifndef __DLGSELECTCLIENT_H_INCLUDED__
#define __DLGSELECTCLIENT_H_INCLUDED__

#include <wx/wx.h>

class Client;

class DlgSelectClient : public wxDialog
{
    public:
        DlgSelectClient(wxWindow *parent, Client* item=NULL);
        virtual ~DlgSelectClient();
        Client* GetClient() { return m_Client; }
    protected:
    private:
        // Misc functions
        void CreateControls();
        void ConnectControls();
        void UpdateClientsList();
        // Events handlers
        void OnTxtFilterChanged(wxCommandEvent &event);
        void OnCmbFilterFieldChanged(wxCommandEvent &event);
        void OnLstClientsDblClick(wxCommandEvent &event);
        void OnUpdateUI_BtnValid(wxUpdateUIEvent &event);
        // Controls vars
        wxTextCtrl *m_txtFilter;
        wxChoice *m_cmbFilterField;
        wxStaticText *m_lblResults;
        wxListBox *m_lstClients;
        // Misc vars
        Client* m_Client;
};

#endif // __DLGSELECTCLIENT_H_INCLUDED__

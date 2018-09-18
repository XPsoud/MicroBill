#ifndef __PANELNBOOKPAGE_H_INCLUDED__
#define __PANELNBOOKPAGE_H_INCLUDED__

#include <wx/wx.h>

enum
{
    PNBP_TYPE_UNKNOWN = -1,
    PNBP_TYPE_HOME = 0, // Homepage
    PNBP_TYPE_CLIENTS,  // Clients management
    PNBP_TYPE_ESTIMATES,// Estimates management

    PNBP_TYPE_COUNT
};

#include "datasmanager.h"
#include "settingsmanager.h"

class PanelNBookPage : public wxPanel
{
    public:
        PanelNBookPage(wxWindow* parent, int type);
        virtual ~PanelNBookPage();
        int GetPageType() { return m_iType; }
        wxSizer* GetMainSizer() { return m_szrMain; }
        virtual void RefreshDatasLists();
    protected:
        SettingsManager& m_SettingsMngr;
        DatasManager& m_DatasMngr;
    private:
        // Misc functions
        virtual void CreateControls();
        virtual void ConnectControls();
        // Base controls vars
        wxSizer *m_szrMain;
        wxStaticBox *m_stbMain;
        // Misc vars
        int m_iType;
};

#endif // __PANELNBOOKPAGE_H_INCLUDED__

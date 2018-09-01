#ifndef __PANELNBOOKPAGE_HOME_H_INCLUDED__
#define __PANELNBOOKPAGE_HOME_H_INCLUDED__

#include "panelnbookpage.h"

class PanelNBookPage_Home : public PanelNBookPage
{
    public:
        PanelNBookPage_Home(wxWindow* parent);
        virtual ~PanelNBookPage_Home();
    protected:
    private:
        // Misc functions
        virtual void CreateControls();
        virtual void ConnectControls();
        // Events handlers
#ifdef __WXDEBUG__
        void OnDoStartTest(wxCommandEvent &event);
#endif // __WXDEBUG__
        // Controls vars
#ifdef __WXDEBUG__
        wxButton *m_btnStart;
#endif // __WXDEBUG__
};

#endif // __PANELNBOOKPAGE_HOME_H_INCLUDED__

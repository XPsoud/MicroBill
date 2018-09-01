#ifndef __MAIN_H_INCLUDED__
#define __MAIN_H_INCLUDED__

#include <wx/app.h>
#include <wx/snglinst.h>

class MicroBillApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        wxString GetBuildInfos(bool all=false);
        wxString GetVersionString(bool full=false, bool build=false);
        wxString GetVersionNumString(bool full=false, bool build=false);
    private:
        wxLocale m_locale;
        wxSingleInstanceChecker *m_pSnglInstChkr;
};

DECLARE_APP(MicroBillApp)

#endif // __MAIN_H_INCLUDED__

#ifndef __MAIN_H_INCLUDED__
#define __MAIN_H_INCLUDED__

#include <wx/app.h>

class MicroBillApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual int OnExit();
        wxString GetBuildInfos(bool all=false);
        wxString GetVersionString(bool full=false, bool build=false);
    private:
        //
};

DECLARE_APP(MicroBillApp)

#endif // __MAIN_H_INCLUDED__

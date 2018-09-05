#include "main.h"

#include "mainframe.h"
#include "appversion.h"
#include "datasmanager.h"
#include "settingsmanager.h"

#include <wx/stdpaths.h>

IMPLEMENT_APP(MicroBillApp);

bool MicroBillApp::OnInit()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("MicroBill Application started\n"));
#endif // __WXDEBUG__

    wxInitAllImageHandlers();

    SetAppName(_T(PRODUCTNAME));

    // Initialize the settings manager
    SettingsManager& settings=SettingsManager::Get();
    // Read settings if any
    settings.ReadSettings();

        // Initialize the locale if possible (and if wanted)
    // Easter egg for debugging purpose
    bool bI18N=(settings.GetProhibitI18N()==false);
    if (wxGetKeyState(WXK_SHIFT))
        bI18N=!bI18N;

    if (bI18N)
    {
#ifdef __WXMAC__
        wxString sDir=wxStandardPaths::Get().GetResourcesDir();
#else
        wxString sDir=settings.GetAppPath();
#endif // __WXMAC__
        sDir.Append(_T("langs"));
        m_locale.AddCatalogLookupPathPrefix(sDir);
        m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT);
        m_locale.AddCatalog(_T(PRODUCTNAME));
    }

    // Check for single instance
    m_pSnglInstChkr=NULL;
    if (!settings.GetMultipleInstancesAllowed())
    {
        m_pSnglInstChkr=new wxSingleInstanceChecker();
        m_pSnglInstChkr->Create(_T(PRODUCTNAME));
        if (m_pSnglInstChkr->IsAnotherRunning())
        {
            wxMessageBox(_("An other instance of this application is already running !"), _("Multiple instances forbidden"), wxICON_EXCLAMATION|wxOK|wxCENTER);
            delete m_pSnglInstChkr;
            return false;
        }
    }

    // Initialize the DatasManager instance
    DatasManager& datas=DatasManager::Get();
    // Read the datas if any
    datas.ReadDatas();

    MainFrame* frame = new MainFrame(GetVersionString(true));

    SetTopWindow(frame);
    frame->Show();
    frame->Raise();

    return true;
}

int MicroBillApp::OnExit()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Exiting from the MicroBill Application\n"));
#endif // __WXDEBUG__

    // Get the instance of the datas manager
    DatasManager& datas=DatasManager::Get();
    // Save datas if needed
    if (datas.IsModified())
        datas.SaveDatas();

    // Get the instance of the settings manager
    SettingsManager& settings=SettingsManager::Get();
    // Save settings if needed
    if (settings.IsModified())
        settings.SaveSettings();

    if (m_pSnglInstChkr!=NULL)
        delete m_pSnglInstChkr;

    return wxApp::OnExit();
}

wxString MicroBillApp::GetBuildInfos(bool all)
{
    wxString wxbuild(wxVERSION_STRING);
    if (sizeof(void *)==4)
        wxbuild << _T("-32 Bits");
    else
        wxbuild << _T("-64 Bits");
    if (all)
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-OSX");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if defined(wxUSE_UNICODE)
        wxbuild << _T("-Unicode");
#else
        wxbuild << _T("-Ansi");
#endif // wxUSE_UNICODE

#if defined(WXUSINGDLL)
        wxbuild << _T("-Dynamic");
#else
        wxbuild << _T("-Static");
#endif

        wxbuild << _T(" build");
    }
    return wxbuild;
}

wxString MicroBillApp::GetVersionString(bool full, bool build)
{
    wxString sRes=wxTheApp->GetAppName();
    sRes << _T(" (v") << GetVersionNumString(full, build) << _T(")");
    return sRes;
}

wxString MicroBillApp::GetVersionNumString(bool full, bool build)
{
    wxString sRes=_T("");
    sRes << VERSION_MAJOR << _T(".") << VERSION_MINOR;
    if (full) sRes << _T(".") << VERSION_REV;
    if (build)
        sRes << _T(" build ") << VERSION_BUILD;
    return sRes;
}

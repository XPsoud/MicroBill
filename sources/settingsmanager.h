#ifndef __SETTINGSMANAGER_H_INCLUDED__
#define __SETTINGSMANAGER_H_INCLUDED__

#include <wx/wx.h>

#define MAINFRAME_MIN_SIZE wxSize(400, 300)
#define iStdXmlHeaderSize 38
extern const wxChar* g_szStdXmlFileHeader1;
extern const wxChar* g_szStdXmlFileHeader2;

#define szDefaultHeaderImgSize wxSize(710, 120)

class SettingsManager
{
    public:
        static SettingsManager& Get();
        bool ReadSettings();
        bool SaveSettings();
        // Accessors
        bool IsModified();
        wxString GetAppPath() { return m_sAppPath; }
        wxString GetSettingsPath() { return m_sSettingsPath; }
        // Settings file compression
        bool GetCompressSettings() { return m_bCompSettings; }
        void SetCompressSettings(bool value=true);
        // Position of the main window at application startup
        int GetMainWndStartupPos() { return m_iStartPos; }
        void SetMainWndStartupPos(int value);
        void GetMainWndStartupPos(wxPoint& pos) { pos=m_ptStartPos; }
        const wxSize& GetMainWndStartupSize() { return m_szStartSize; }
        void SetLastWindowRect(const wxPoint& pos, const wxSize& size);
        // Multiple instances
        bool GetMultipleInstancesAllowed() { return (m_bSingleInstance==false); }
        void SetMultipleInstancesAllowed(bool value);
        // Prohibit translations and transaltions related stuff
        bool GetProhibitI18N() { return m_bProhibI18N; }
        void SetProhibitI18N(bool value);
        // Splash screen
        bool GetShowSplashScreen() { return m_bShowSplashScreen; }
        void SetShowSplashScreen(bool value=true);
        // Access password
        wxString GetPassword() { return m_sPassword; }
        void SetPassword(const wxString& value);
        // Monetary sign
        const wxArrayString& GetMoneySigns() { return m_arsMoneySigns; }
        const wxString GetMonetarySign() { return m_sMoneySign; }
        void SetMonetarySign(const wxString& value);
        int GetMoneySignPos() { return m_iMoneySignPos; }
        void SetMoneySignPos(int pos);
        wxString GetFormatedMoneyValue(double value, const wxString& format);
        // Header image for pdf documents
        wxImage GetHeaderImage();
        bool SetHeaderImage(wxImage* img);
        bool HasCustomHeaderImage() { return (m_imgHeader!=NULL); }
        wxImage GetDefaultHeaderImage();
        // Custom texts for pdf documents
        void SetCompanyLocation(const wxString& value);
        wxString GetCompanyLocation() { return m_sLocation; }
        bool HasCompanyLocation() { return !m_sLocation.IsEmpty(); }
        void SetFooterText(const wxString& text, const wxString& style=wxEmptyString);
        wxString GetFooterText() { return m_sFooterText; }
        wxString GetFooterStyle() { return m_sFooterStyle; }
    protected:
    private:
        SettingsManager();
        virtual ~SettingsManager();
        void Initialize();
        const wxString StartupPos2String(int iValue);
        int StartupString2Pos(const wxString& sValue);
        // Unique instance of the singleton
        static SettingsManager m_instance;
        // Misc vars
        bool m_bInitialized, m_bModified;
        wxString m_sAppPath, m_sSettingsPath;
        static const wxChar *m_szSettingsFName, *m_szCustHeaderImgFName;
        wxArrayString m_arsMoneySigns;
        // Settings vars
        bool m_bCompSettings;
        int m_iStartPos;
        wxPoint m_ptStartPos;
        wxSize m_szStartSize;
        bool m_bSingleInstance, m_bProhibI18N, m_bShowSplashScreen;
        wxString m_sPassword, m_sMoneySign;
        int m_iMoneySignPos;
        wxImage *m_imgHeader;
        wxString m_sLocation, m_sFooterText, m_sFooterStyle;
};

#endif // __SETTINGSMANAGER_H_INCLUDED__

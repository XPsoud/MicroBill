#include "settingsmanager.h"

const wxChar* g_szStdXmlFileHeader1=_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
const wxChar* g_szStdXmlFileHeader2=_T("<?xml version='1.0' encoding='UTF-8'?>");

const wxChar* SettingsManager::m_szSettingsFName=_T("Settings.µBill");
const wxChar* SettingsManager::m_szCustHeaderImgFName = _T("CustHeaderImg.png");

#include "microbill.png.h"

#include <wx/dir.h>
#include <wx/zstream.h>
#include <wx/xml/xml.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

SettingsManager SettingsManager::m_instance;

SettingsManager::SettingsManager()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"SettingsManager\" object\n"));
#endif // __WXDEBUG__
    m_bInitialized=false;
    m_bModified=false;
}

SettingsManager::~SettingsManager()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"SettingsManager\" object\n"));
#endif // __WXDEBUG__
}

SettingsManager& SettingsManager::Get()
{
    if (!m_instance.m_bInitialized)
        m_instance.Initialize();

    return m_instance;
}

void SettingsManager::Initialize()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Initializing the SettingsManager\n"));
#endif // __WXDEBUG__
    // Full path of the application
    wxFileName fname(wxStandardPaths::Get().GetExecutablePath());
    fname.Normalize();
    m_sAppPath=fname.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
    if (!m_sAppPath.EndsWith(wxFileName::GetPathSeparator()))
        m_sAppPath.Append(wxFileName::GetPathSeparator());

    // Path for the settings file (platform dependant)
    m_sSettingsPath=wxStandardPaths::Get().GetUserDataDir();
    if (!m_sSettingsPath.EndsWith(wxFileName::GetPathSeparator()))
        m_sSettingsPath.Append(wxFileName::GetPathSeparator());

    // Default position and size of the main window
    m_iStartPos=wxCENTER_ON_SCREEN;
    m_ptStartPos=wxDefaultPosition;
    m_szStartSize=wxDefaultSize;
    // Other default settings
    m_bSingleInstance=true;
    m_bProhibI18N=false;
    m_bShowSplashScreen=true;
    m_bCompSettings=false;
    m_arsMoneySigns.Clear();
    m_arsMoneySigns.Add(_T("\u20AC"));
    m_arsMoneySigns.Add(_T("$"));
    m_arsMoneySigns.Add(_T("\u00A3"));
    m_arsMoneySigns.Add(_T("F"));
    m_arsMoneySigns.Add(_T("\u00a5"));
    m_sMoneySign = _T("\u20AC");
    m_iMoneySignPos = wxRIGHT;
    m_imgHeader = NULL;
    m_sLocation = wxEmptyString;
    m_sFooterText = wxEmptyString;
    m_sFooterStyle = _T("00000000008I");

    m_bInitialized=true;
}

bool SettingsManager::ReadSettings()
{
    if (!m_bInitialized) Initialize();

    wxString sFName = m_sSettingsPath + m_szSettingsFName;

    if (!wxFileExists(sFName))
        return false;

    wxXmlDocument doc;
    wxXmlNode *root, *node;

    wxFileInputStream f_in(sFName);
    if (!f_in.IsOk()) return false;

    // Try to detect if the file is compressed or not
    char szBuff[iStdXmlHeaderSize+2];
    f_in.Read(szBuff, iStdXmlHeaderSize);
    szBuff[iStdXmlHeaderSize]=0;
    wxString sHeader(szBuff);
    f_in.SeekI(0, wxFromStart);

    if (sHeader.IsSameAs(g_szStdXmlFileHeader1, false) ||
        sHeader.IsSameAs(g_szStdXmlFileHeader2, false))
    {
        // Non-compressed file
        if (!doc.Load(f_in)) return false;
    }
    else
    {
        // If the file is not an xml one (empty file for example)
        // we'll have an error message so disable this temporarly
        wxLogNull noLog;
        // Compressed file
        wxZlibInputStream z_in(f_in);
        if (!doc.Load(z_in)) return false;
    }

    root=doc.GetRoot();
    node=root->GetChildren();
    wxString nodName, subName, sValue;
    long lVal;
    while(node)
    {
        nodName=node->GetName();
        if (nodName==_T("StartupPos")) // Last known position of the main window
        {
            // Window's position at application startup
            m_iStartPos=StartupString2Pos(node->GetAttribute(_T("Value"), _T("CenterScreen")));
            if (m_iStartPos==wxALIGN_NOT)
            {
                node->GetAttribute(_T("X"), _T("-1")).ToLong(&lVal);
                m_ptStartPos.x=lVal;
                node->GetAttribute(_T("Y"), _T("-1")).ToLong(&lVal);
                m_ptStartPos.y=lVal;
                node->GetAttribute(_T("W"), _T("-1")).ToLong(&lVal);
                m_szStartSize.SetWidth(lVal);
                node->GetAttribute(_T("H"), _T("-1")).ToLong(&lVal);
                m_szStartSize.SetHeight(lVal);
            }
        }
        if (nodName==_T("MultiInstances"))
        {
            // Only one instance allowed ?
            m_bSingleInstance=(node->GetNodeContent()!=_T("Allowed"));
        }
        if (nodName==_T("ShowSplashScreen"))
        {
            // Show the splash screen at startup ? ?
            m_bShowSplashScreen=(node->GetNodeContent()!=_T("No"));
        }
        if (nodName==_T("CompressSettingsFile"))
        {
            // Settings file compression ?
            m_bCompSettings=(node->GetNodeContent()==_T("Yes"));
        }
        if (nodName==_T("Translation"))
        {
            // Allowed ?
            m_bProhibI18N=(node->GetAttribute(_T("Allowed"), _T("Yes"))!=_T("Yes"));
        }
        // Monetary sign
        if (nodName==_T("MoneySign"))
        {
            SetMonetarySign(node->GetAttribute(_T("Value")));
            node->GetAttribute(_T("Pos")).ToLong(&lVal);
            SetMoneySignPos(lVal);
        }
        // Company location
        if (nodName==_T("Location"))
        {
            m_sLocation = node->GetNodeContent();
        }
        // Footer text
        if (nodName==_T("FooterText"))
        {
            m_sFooterStyle = node->GetAttribute(_T("Style"), m_sFooterStyle);
            m_sFooterText = node->GetNodeContent();
        }

        node = node->GetNext();
    }

    wxString sImgFName = m_sSettingsPath;
    sImgFName << m_szCustHeaderImgFName;
    if (wxFileExists(sImgFName))
    {
        if (m_imgHeader != NULL) // Who knows....
        {
            delete m_imgHeader;
        }
        m_imgHeader = new wxImage(sImgFName, wxBITMAP_TYPE_PNG);
    }

    m_bModified=false;
    return true;
}

bool SettingsManager::SaveSettings()
{
    if (!m_bInitialized) Initialize();

    wxString sVal;

    wxString sFName=m_sSettingsPath + m_szSettingsFName;

    wxFileName fname(sFName);

    //Check if the folder exists
    if (!fname.DirExists())
    {
        // Try to create the folder
        if (!fname.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            return false;
    }

    if (fname.FileExists())
    {
        if (!wxRemoveFile(fname.GetFullPath()))
            return false;
    }

    wxXmlNode *root = new wxXmlNode(NULL,wxXML_ELEMENT_NODE, _T("Settings-file"), wxEmptyString,
                            new wxXmlAttribute(_T("Version"), _T("1.0")));

    wxXmlNode *node;
    // Last known position and size of the main window
    node=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("StartupPos"));
    root->AddChild(node);
    node->AddAttribute(_T("Value"), StartupPos2String(m_iStartPos));
    if (m_iStartPos==wxALIGN_NOT)
    {
        node->AddAttribute(_T("X"), wxString::Format(_T("%d"), m_ptStartPos.x));
        node->AddAttribute(_T("Y"), wxString::Format(_T("%d"), m_ptStartPos.y));
        node->AddAttribute(_T("W"), wxString::Format(_T("%d"), m_szStartSize.GetWidth()));
        node->AddAttribute(_T("H"), wxString::Format(_T("%d"), m_szStartSize.GetHeight()));
    }
    // Settings file compression
    node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("CompressSettingsFile")));
    node = node->GetNext();
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), (m_bCompSettings?_T("Yes"):_T("No"))));

    // Allowing (or not) multiple instances of the application
    node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("MultiInstances")));
    node = node->GetNext();
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), (m_bSingleInstance?_T("Not-Allowed"):_T("Allowed"))));

    // Allowing (or not) interface translation
    node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Translation")));
    node = node->GetNext();
    node->AddAttribute(_T("Allowed"), (m_bProhibI18N?_T("No"):_T("Yes")));

    // Show the splash screen at startup ?
    node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("ShowSplashScreen")));
    node = node->GetNext();
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), (m_bShowSplashScreen?_T("Yes"):_T("No"))));

    // Monetary sign
    node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("MoneySign")));
    node = node->GetNext();
    node->AddAttribute(_T("Value"), m_sMoneySign);
    node->AddAttribute(_T("Pos"), wxString::Format(_T("%d"), m_iMoneySignPos));

    // Company location
    if (!m_sLocation.IsEmpty())
    {
        node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Location")));
        node = node->GetNext();
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_sLocation));
    }

    // Footer text
    if (!m_sFooterText.IsEmpty())
    {
        node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("FooterText")));
        node = node->GetNext();
        node->AddAttribute(_T("Style"), m_sFooterStyle);
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_sFooterText));
    }

    wxXmlDocument doc;
    doc.SetRoot(root);

    wxFileOutputStream f_out(fname.GetFullPath());
    m_bModified=false;
    if (GetCompressSettings())
    {
        // Write the xml document through a compression stream
        wxZlibOutputStream z_out(f_out, 9);
        return doc.Save(z_out);
    }
    else
    {
        // Write the xml document
        return doc.Save(f_out);
    }
}

const wxString SettingsManager::StartupPos2String(int iValue)
{
    // For optimisation : we first test the default value
    if (iValue==wxCENTER_ON_SCREEN) return _T("CenterScreen");
    // Special value
    if (iValue==wxALIGN_NOT) return _T("LastKnownPos");

    wxString sHPos=wxEmptyString, sVPos=wxEmptyString;

    sHPos=(((iValue&wxLEFT)==wxLEFT)? _T("Left"):((iValue&wxRIGHT)==wxRIGHT)?_T("Right"):_T("Center"));
    sVPos=(((iValue&wxTOP)==wxTOP)? _T("Top"):((iValue&wxBOTTOM)==wxBOTTOM)?_T("Bottom"):_T("Center"));

    if ((sHPos==_T("Center")) && (sVPos==_T("Center"))) return _T("CenterScreen");

    return sVPos + sHPos;
}

int SettingsManager::StartupString2Pos(const wxString& sValue)
{
    // For optimisation : we first test the default value
    if (sValue==_T("CenterScreen")) return wxCENTER_ON_SCREEN;
    // Special value
    if (sValue==_T("LastKnownPos")) return wxALIGN_NOT;
    int iHPos=0, iVPos=0;

    iHPos=((sValue.Find(_T("Left"))!=wxNOT_FOUND)?wxLEFT:((sValue.Find(_T("Right"))!=wxNOT_FOUND)?wxRIGHT:(wxDirection)wxCENTER));
    iVPos=((sValue.Find(_T("Top"))!=wxNOT_FOUND)?wxTOP:((sValue.Find(_T("Bottom"))!=wxNOT_FOUND)?wxBOTTOM:(wxDirection)wxCENTER));
    if ((iHPos==wxCENTER)&&(iVPos==wxCENTER)) return wxCENTER_ON_SCREEN;

    return (iHPos | iVPos);
}

bool SettingsManager::IsModified()
{
    // Do any checks here and update the m_bModified var value

    return m_bModified;
}

void SettingsManager::SetMainWndStartupPos(int value)
{
    // We first check that the given value is correct
    int iVal=0;
    if ((value==wxCENTER_ON_SCREEN)||(value==wxALIGN_NOT))
    {
        iVal=value;
    }
    else
    {
        int iHPos=(((value&wxLEFT)==wxLEFT)? wxLEFT:((value&wxRIGHT)==wxRIGHT)?wxRIGHT:(wxDirection)wxCENTER);
        int iVPos=(((value&wxTOP)==wxTOP)? wxTOP:((value&wxBOTTOM)==wxBOTTOM)?wxBOTTOM:(wxDirection)wxCENTER);
        if ((iHPos==wxCENTER)&&(iVPos==wxCENTER))
            iVal=wxCENTER_ON_SCREEN;
        else
            iVal=(iHPos | iVPos);
    }
    if (iVal!=m_iStartPos)
    {
        m_iStartPos=iVal;
        m_bModified=true;
    }
}

void SettingsManager::SetLastWindowRect(const wxPoint& pos, const wxSize& size)
{
    if ((pos!=m_ptStartPos)||(size!=m_szStartSize))
    {
        m_bModified=true;
        m_ptStartPos=pos;
        m_szStartSize=size;
    }
}

void SettingsManager::SetMultipleInstancesAllowed(bool value)
{
    // Inverted test : it is a normal thing ;)
    if (value==m_bSingleInstance)
    {
        m_bSingleInstance=(value==false);
        m_bModified=true;
    }
}

void SettingsManager::SetCompressSettings(bool value)
{
    if (value!=m_bCompSettings)
    {
        m_bCompSettings=value;
        m_bModified=true;
    }
}

void SettingsManager::SetProhibitI18N(bool value)
{
    if (value!=m_bProhibI18N)
    {
        m_bProhibI18N=value;
        m_bModified=true;
    }
}

void SettingsManager::SetShowSplashScreen(bool value)
{
    if (value != m_bShowSplashScreen)
    {
        m_bShowSplashScreen = value;
        m_bModified = true;
    }
}

void SettingsManager::SetMonetarySign(const wxString& value)
{
    if (value == m_sMoneySign)
        return;
    for (size_t i=0; i<m_arsMoneySigns.Count(); ++i)
    {
        if (value == m_arsMoneySigns[i])
        {
            m_bModified = true;
            m_sMoneySign = value;
            return;
        }
    }
}

void SettingsManager::SetMoneySignPos(int pos)
{
    if ((pos == wxLEFT)||(pos == wxRIGHT))
    {
        if (pos != m_iMoneySignPos)
        {
            m_bModified = true;
            m_iMoneySignPos = pos;
        }
    }
}

wxString SettingsManager::GetFormatedMoneyValue(double value, const wxString& format)
{
    wxString sFormat = wxEmptyString;
    if (m_iMoneySignPos == wxLEFT)
    {
        sFormat << _T("%s") << format;
        return wxString::Format(sFormat, m_sMoneySign, value);
    }
    else
    {
        sFormat << format << _T("%s");
        return wxString::Format(sFormat, value, m_sMoneySign);
    }
}

wxImage SettingsManager::GetHeaderImage()
{
    if (m_imgHeader == NULL)
    {
        return GetDefaultHeaderImage();
    }
    else
    {
        return *m_imgHeader;
    }
}

wxImage SettingsManager::GetDefaultHeaderImage()
{
    wxImage imgHeader = wxGet_MicroBill_png_Bitmap().ConvertToImage();
    double zX = double(szDefaultHeaderImgSize.GetWidth())/imgHeader.GetWidth();
    double zY = double(szDefaultHeaderImgSize.GetHeight())/imgHeader.GetHeight();
    wxSize szImg = imgHeader.GetSize();
    if (zX < zY)
    {
        szImg.SetHeight(imgHeader.GetHeight()/zX);
    }
    else
    {
        szImg.SetWidth(imgHeader.GetWidth()/zY);
    }
    imgHeader.Resize(szImg, wxPoint(0, 0));
    return imgHeader;
}

bool SettingsManager::SetHeaderImage(wxImage* img)
{
    // Delete the current image if any
    if (m_imgHeader != NULL)
    {
        delete m_imgHeader;
        m_imgHeader = NULL;
    }
    wxString sImgFName = m_sSettingsPath;
    sImgFName << m_szCustHeaderImgFName;
    // In any case, we should remove the actual file
    if (wxFileExists(sImgFName))
        wxRemoveFile(sImgFName);
    if (img != NULL)
    {
        m_imgHeader = new wxImage(*img);
        m_imgHeader->SaveFile(sImgFName, wxBITMAP_TYPE_PNG);
    }
    return true;
}

void SettingsManager::SetCompanyLocation(const wxString& value)
{
    // Remove extra spaces at the begining and at the end
    wxString sValue = value;
    sValue.Trim(true);
    sValue.Trim(false);
    if (sValue != m_sLocation)
    {
        m_sLocation = sValue;
        m_bModified = true;
    }
}

void SettingsManager::SetFooterText(const wxString& text, const wxString& style)
{
    // Remove extra spaces at the begining and at the end
    wxString sValue = text;
    sValue.Trim(true);
    sValue.Trim(false);
    if (sValue != m_sFooterText)
    {
        m_sFooterText = sValue;
        m_bModified = true;
    }
    // TODO (Xaviou#1#): Make real checks on the following value
    if (!style.IsEmpty())
    {
        if (style != m_sFooterStyle)
        {
            m_sFooterStyle = style;
            m_bModified = true;
        }
    }
}

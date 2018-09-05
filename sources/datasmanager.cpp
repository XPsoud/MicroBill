#include "datasmanager.h"

const wxChar* DatasManager::m_szDatasFName=_T("Datas.µBill");

#include <wx/dir.h>
#include <wx/zstream.h>
#include <wx/xml/xml.h>
#include <wx/wfstream.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "appversion.h"
#include "settingsmanager.h"

DatasManager DatasManager::m_instance;

DatasManager::DatasManager()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Creating a \"DatasManager\" object\n"));
#endif // __WXDEBUG__
    m_bInitialized=false;
    m_bModified=false;
}

DatasManager::~DatasManager()
{
    ClearDatas();
#ifdef __WXDEBUG__
    wxPrintf(_T("Destroying a \"DatasManager\" object\n"));
#endif // __WXDEBUG__
}

DatasManager& DatasManager::Get()
{
    if (!m_instance.m_bInitialized)
        m_instance.Initialize();

    return m_instance;
}

void DatasManager::Initialize()
{
#ifdef __WXDEBUG__
    wxPrintf(_T("Initializing the DatasManager\n"));
#endif // ENABLE_DEBUG_MSG

    m_bInitialized=true;
    m_bModified=false;
}

bool DatasManager::ReadDatas()
{
    if (!m_bInitialized) Initialize();

    wxFileName fname=SettingsManager::Get().GetSettingsPath() + m_szDatasFName;

    if (!fname.DirExists())
        return false;

    if (!fname.FileExists())
        return false;

    wxXmlDocument doc;
    wxXmlNode *root;

    wxFileInputStream f_in(fname.GetFullPath());
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

    SetDatasXmlTree(root);

    m_bModified=false;
    return true;
}

bool DatasManager::SaveDatas()
{
    if (!m_bInitialized) Initialize();

    wxFileName fname=SettingsManager::Get().GetSettingsPath() + m_szDatasFName;

    if (!fname.DirExists())
    {
        if (!fname.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            return false;
    }

    if (fname.FileExists())
    {
        if (!wxRemoveFile(fname.GetFullPath()))
        {
            return false;
        }
    }

    wxXmlNode *root = GetDatasXmlTree();

    wxXmlDocument doc;
    doc.SetRoot(root);

    wxFileOutputStream f_out(fname.GetFullPath());
    m_bModified=false;
    if (SettingsManager::Get().GetCompressSettings())
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

void DatasManager::ClearDatas()
{
    // Remove all clients from the list
    m_clients.DeleteContents(true);
    m_clients.Clear();
}

bool DatasManager::IsModified()
{
    if (m_bModified) return true;

    if (!m_clients.IsEmpty())
    {
        ListOfClients::iterator iter;
        Client *item;
        for (iter=m_clients.begin(); iter!=m_clients.end(); ++iter)
        {
            item=*iter;
            if (item->IsModified())
            {
                m_bModified = true;
                return true;
            }
        }
    }

    return m_bModified;
}

Client* DatasManager::AddNewClient()
{
    Client* c=new Client();
    m_clients.Append(c);
    m_bModified=true;
    return c;
}

Client* DatasManager::GetClient(size_t index)
{
    if (index>=m_clients.GetCount())
        return NULL;

    wxListOfClientsNode* node=m_clients.Item(index);
    if (node!=NULL)
        return node->GetData();
    else
        return NULL;
}

Client* DatasManager::GetClient(const wxString& key)
{
    if (key.IsEmpty()) return NULL;
    ListOfClients::iterator iter;
    for (iter=m_clients.begin(); iter!=m_clients.end(); ++iter)
    {
        Client* item=*iter;
        if (item->GetKey()==key) return item;
    }
    return NULL;
}

bool DatasManager::RemoveClient(Client* item)
{
    if (item==NULL) return false;
    m_clients.DeleteContents(true);
    if (m_clients.DeleteObject(item))
    {
        m_bModified=true;
        return true;
    }
    return false;
}

bool DatasManager::IsClientUsed(Client* item)
{
    if (item==NULL) return false;

    // TODO (Xaviou#1#): Make a read test to see if the client is used

    return false;
}

wxString DatasManager::GetClientString(Client* item)
{
    if (item==NULL) return wxEmptyString;
    wxString sRes=item->GetAttribute(_T("LastName"));
    wxString sTmp=item->GetAttribute(_T("FirstName"));
    if (!sTmp.IsEmpty())
    {
        sRes << _T(" ") << sTmp;
    }
    sTmp=item->GetAttribute(_T("Company"));
    if (!sTmp.IsEmpty())
    {
        if (sRes.IsEmpty())
        {
            sRes=sTmp;
        }
        else
        {
            sRes << _T(" (") << sTmp << _T(")");
        }
    }

    return sRes;
}

wxString DatasManager::GetClientString(const wxString& key)
{
    return GetClientString(GetClient(key));
}

bool DatasManager::SortClients(int sortKey, bool asc)
{
    if (sortKey==CATTR_COMPANY)
    {
        if (asc)
            m_clients.Sort(Client::SortClientsByCompanyAscCallback);
        else
            m_clients.Sort(Client::SortClientsByCompanyDescCallback);
    }
    else if (sortKey==CATTR_LASTNAME)
    {
        if (asc)
            m_clients.Sort(Client::SortClientsByLastNameAscCallback);
        else
            m_clients.Sort(Client::SortClientsByLastNameDescCallback);
    }
    else if (sortKey==CATTR_FIRSTNAME)
    {
        if (asc)
            m_clients.Sort(Client::SortClientsByFirstNameAscCallback);
        else
            m_clients.Sort(Client::SortClientsByFirstNameDescCallback);
    }
    else return false;

    m_bModified=true;
    return true;
}

wxXmlNode* DatasManager::GetDatasXmlTree()
{
    wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("MicroBill_Datas-file"), wxEmptyString,
                            new wxXmlAttribute(_T("Version"), _T("1.0")));
    wxXmlNode *node=NULL;
    if (!m_clients.IsEmpty())
    {
        ListOfClients::iterator iter;
        for (iter=m_clients.begin(); iter!=m_clients.end(); ++iter)
        {
            if (node==NULL)
            {
                node=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Client"));
                root->AddChild(node);
            }
            else
            {
                node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Client")));
                node=node->GetNext();
            }
            Client* item=*iter;
            item->ToXmlNode(node);
        }
    }

    return root;
}

void DatasManager::SetDatasXmlTree(wxXmlNode* container)
{
    if (container==NULL) return;

    ClearDatas();

    wxXmlNode* node=container->GetChildren();
    wxString nodName;
    while(node)
    {
        nodName=node->GetName();
        if (nodName==_T("Client"))
        {
            Client* c=new Client(node);
            m_clients.Append(c);
        }
        node = node->GetNext();
    }
    m_bModified=true;
}

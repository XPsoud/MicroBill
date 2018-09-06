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
    m_iLastBillNumber=0;
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
    // Remove all estimates from the list
    m_estimates.DeleteContents(true);
    m_estimates.Clear();
    // Remove all bills from the list
    m_bills.DeleteContents(true);
    m_bills.Clear();
    // Reset the last bill number
    m_iLastBillNumber=0;
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
    if (!m_estimates.IsEmpty())
    {
        ListOfEstimates::iterator iter;
        Estimate *item;
        for (iter=m_estimates.begin(); iter!=m_estimates.end(); ++iter)
        {
            item=*iter;
            if (item->IsModified())
            {
                m_bModified = true;
                return true;
            }
        }
    }
    if (!m_bills.IsEmpty())
    {
        ListOfBills::iterator iter;
        Bill *item;
        for (iter=m_bills.begin(); iter!=m_bills.end(); ++iter)
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

    int iCount=m_estimates.size();
    wxString sKey=item->GetKey();
    Estimate* e;
    for (int i=0; i<iCount; ++i)
    {
        e=m_estimates.Item(i)->GetData();
        if (e->GetClient()==sKey) return true;
    }

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

Estimate* DatasManager::AddNewEstimate()
{
    Estimate* e=new Estimate();
    m_estimates.Append(e);
    m_bModified=true;
    return e;
}

Estimate* DatasManager::GetEstimate(size_t index)
{
    if (index>=m_estimates.GetCount())
        return NULL;

    wxListOfEstimatesNode* node=m_estimates.Item(index);
    if (node!=NULL)
        return node->GetData();
    else
        return NULL;
}

int DatasManager::GetLockedEstimatesCount()
{
    int iRes=0;
    ListOfEstimates::iterator iter;
    for (iter=m_estimates.begin(); iter!=m_estimates.end(); ++iter)
    {
        if ((*iter)->IsLocked()) iRes++;
    }
    return iRes;
}

int DatasManager::GetUnlockedEstimatesCount()
{
    int iRes=0;
    ListOfEstimates::iterator iter;
    for (iter=m_estimates.begin(); iter!=m_estimates.end(); ++iter)
    {
        if (!(*iter)->IsLocked()) iRes++;
    }
    return iRes;
}

bool DatasManager::RemoveEstimate(Estimate* item)
{
    if (item==NULL) return false;
    m_estimates.DeleteContents(true);
    if (m_estimates.DeleteObject(item))
    {
        m_bModified=true;
        return true;
    }
    return false;
}

bool DatasManager::SortEstimates(int sortKey, bool asc)
{
    if (sortKey==0) // Sorting by creation date
    {
        if (asc)
            m_estimates.Sort(Estimate::SortEstimateByCreationDateAscCallback);
        else
            m_estimates.Sort(Estimate::SortEstimateByCreationDateDescCallback);
    }
    else if (sortKey==1) // Sorting by Client
    {
        if (asc)
            m_estimates.Sort(Estimate::SortEstimateByClientAscCallback);
        else
            m_estimates.Sort(Estimate::SortEstimateByClientDescCallback);
    }
    else if (sortKey==2) // Sorting by Total price
    {
        if (asc)
            m_estimates.Sort(Estimate::SortEstimateByTotalAscCallback);
        else
            m_estimates.Sort(Estimate::SortEstimateByTotalDescCallback);
    }
    else if (sortKey==3) // Sorting by Status
    {
        if (asc)
            m_estimates.Sort(Estimate::SortEstimateByStatusAscCallback);
        else
            m_estimates.Sort(Estimate::SortEstimateByStatusDescCallback);
    }
    else if (sortKey==4) // Sorting by TermDate
    {
        if (asc)
            m_estimates.Sort(Estimate::SortEstimateByTermDateAscCallback);
        else
            m_estimates.Sort(Estimate::SortEstimateByTermDateDescCallback);
    }
    else return false;

    m_bModified=true;
    return true;
}

Bill* DatasManager::GetBill(size_t index)
{
    if (index>=m_bills.GetCount())
        return NULL;

    wxListOfBillsNode* node=m_bills.Item(index);
    if (node!=NULL)
        return node->GetData();
    else
        return NULL;
}

int DatasManager::GetLockedBillsCount()
{
    int iRes=0;
    ListOfBills::iterator iter;
    for (iter=m_bills.begin(); iter!=m_bills.end(); ++iter)
    {
        if ((*iter)->IsLocked()) iRes++;
    }
    return iRes;
}

int DatasManager::GetUnlockedBillsCount()
{
    int iRes=0;
    ListOfBills::iterator iter;
    for (iter=m_bills.begin(); iter!=m_bills.end(); ++iter)
    {
        if (!(*iter)->IsLocked()) iRes++;
    }
    return iRes;
}

Bill* DatasManager::AddNewBill()
{
    Bill* b=new Bill();
    m_bills.Append(b);
    b->SetBillNumber(++m_iLastBillNumber);
    m_bModified=true;
    return b;
}

bool DatasManager::RemoveBill(Bill* item)
{
    if (item==NULL) return false;
    m_bills.DeleteContents(true);
    if (m_bills.DeleteObject(item))
    {
        m_bModified=true;
        return true;
    }
    return false;
}

bool DatasManager::SortBills(int sortKey, bool asc)
{
    if (sortKey==BSC_NUMBER) // Sorting by bill number
    {
        if (asc)
            m_bills.Sort(Bill::SortBillByNumberAscCallback);
        else
            m_bills.Sort(Bill::SortBillByNumberDescCallback);
    }
    else if (sortKey==BSC_CREAT_DATE) // Sorting by creation date
    {
        if (asc)
            m_bills.Sort(Bill::SortBillByCreationDateAscCallback);
        else
            m_bills.Sort(Bill::SortBillByCreationDateDescCallback);
    }
    else if (sortKey==BSC_CLIENT) // Sorting by Client
    {
        if (asc)
            m_bills.Sort(Bill::SortBillByClientAscCallback);
        else
            m_bills.Sort(Bill::SortBillByClientDescCallback);
    }
    else if (sortKey==BSC_TOTAL) // Sorting by Total price
    {
        if (asc)
            m_bills.Sort(Bill::SortBillByTotalAscCallback);
        else
            m_bills.Sort(Bill::SortBillByTotalDescCallback);
    }
    else if (sortKey==BSC_STATUS) // Sorting by status
    {
        if (asc)
            m_bills.Sort(Bill::SortBillByStatusAscCallback);
        else
            m_bills.Sort(Bill::SortBillByStatusDescCallback);
    }
    else if (sortKey==BSC_TERM_DATE) // Sorting by term date
    {
        if (asc)
            m_bills.Sort(Bill::SortBillByTermDateAscCallback);
        else
            m_bills.Sort(Bill::SortBillByTermDateDescCallback);
    }
    else return false;

    m_bModified=true;
    return true;
}

Bill* DatasManager::AddNewBill(Estimate* estimate)
{
    if (estimate==NULL) return NULL;

    Bill* b=new Bill();
    m_bills.Append(b);
    b->SetBillNumber(++m_iLastBillNumber);
    m_bModified=true;
        b->SetCreationDate(estimate->GetCreationDate());
        b->SetTermDate(estimate->GetTermDate());
        b->SetClientKey(estimate->GetClient());
        b->SetLocked(estimate->IsLocked());
        int iCount=estimate->GetItemsCount(), iQty;
        wxString sDescr, sComm;
        double dPrice;
        for (int i=0; i<iCount; ++i)
        {
            sDescr=estimate->GetItemDescription(i);
            sComm=estimate->GetItemComments(i);
            iQty=estimate->GetItemQuantity(i);
            dPrice=estimate->GetItemUnitPrice(i);
            b->AddItem(sDescr, dPrice, sComm, iQty);
        }
        b->SetDiscount(estimate->GetDiscount());
        b->SetVAT(estimate->GetVAT());
    return b;
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
    if (!m_estimates.IsEmpty())
    {
        ListOfEstimates::iterator iter;
        for (iter=m_estimates.begin(); iter!=m_estimates.end(); ++iter)
        {
            if (node==NULL)
            {
                node=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Estimate"));
                root->AddChild(node);
            }
            else
            {
                node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Estimate")));
                node=node->GetNext();
            }
            Estimate* item=*iter;
            item->ToXmlNode(node);
        }
    }
    if (!m_bills.IsEmpty())
    {
        ListOfBills::iterator iter;
        for (iter=m_bills.begin(); iter!=m_bills.end(); ++iter)
        {
            if (node==NULL)
            {
                node=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Bill"));
                root->AddChild(node);
            }
            else
            {
                node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Bill")));
                node=node->GetNext();
            }
            Bill* item=*iter;
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
        if (nodName==_T("Estimate"))
        {
            Estimate* e=new Estimate(node);
            m_estimates.Append(e);
        }
        if (nodName==_T("Bill"))
        {
            Bill* b=new Bill(node);
            m_bills.Append(b);
            if (b->GetBillNumber()>m_iLastBillNumber)
                m_iLastBillNumber=b->GetBillNumber();
        }
        node = node->GetNext();
    }
    m_bModified=true;
}

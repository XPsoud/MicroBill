#include "bill.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ListOfBills);

#include <wx/xml/xml.h>
static const wxString szEmptyString=_T("");

#include "datasmanager.h"

Bill::Bill(wxXmlNode* container)
{
    Initialize();
    if (container!=NULL)
        FromXmlNode(container);
}

Bill::~Bill()
{
    // Destructor
}

void Bill::ToXmlNode(wxXmlNode* container)
{
    container->AddAttribute(_T("Number"), wxString::Format(_T("%0d"), m_iBillNumber));
    if (m_bLocked)
    {
        container->AddAttribute(_T("Locked"), _T("Yes"));
    }
    wxXmlNode *node=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("CreationDate"));
    container->AddChild(node);
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_dtCreation.Format(_T("%Y-%m-%d %H:%M:%S"))));
    // Client datas
    node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Client")));
    node=node->GetNext();
    node->AddAttribute(_T("SourceKey"), m_sClientKey);
        wxXmlNode* subNode=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Title"));
        node->AddChild(subNode);
        subNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_sClientTitle));
        subNode->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("FullString")));
        subNode=subNode->GetNext();
        subNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_sClientFullString));
    // Other datas
    if (m_dtTerm.IsValid())
    {
        node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("TermDate")));
        node=node->GetNext();
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_dtTerm.Format(_T("%Y-%m-%d %H:%M:%S"))));
    }
    if (m_dDiscount!=0.)
    {
        node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Discount")));
        node=node->GetNext();
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), wxString::Format(_T("%.02f"), m_dDiscount)));
    }
    if (m_dVAT!=0.)
    {
        node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("VAT")));
        node=node->GetNext();
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), wxString::Format(_T("%.02f"), m_dVAT)));
    }
    for (size_t i=0; i<m_arsItemDescr.GetCount(); ++i)
    {
        node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Item")));
        node=node->GetNext();
        if (m_ariQuantity[i]>0)
            node->AddAttribute(_T("Quantity"), wxString::Format(_T("%d"), m_ariQuantity[i]));
        if (m_ardUnitPrice[i]!=0.)
            node->AddAttribute(_T("UnitPrice"), wxString::Format(_T("%.2f"), m_ardUnitPrice[i]));
        subNode=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Description"));
        node->AddChild(subNode);
        subNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_arsItemDescr[i]));
        if (!m_arsItemComments[i].IsEmpty())
        {
            subNode->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Comments")));
            subNode=subNode->GetNext();
            subNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_arsItemComments[i]));
        }
    }
    if (!m_sPayNote.IsEmpty())
    {
        node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("PayNote")));
        node=node->GetNext();
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_sPayNote));
    }
}

void Bill::FromXmlNode(wxXmlNode* container)
{
    long lVal;
    if (container->HasAttribute(_T("Number")))
    {
        container->GetAttribute(_T("Number")).ToLong(&lVal);
        m_iBillNumber=lVal;
    }
    // Needed to automatcally switch from datas xml file v1.0 to v1.1
    if (container->HasAttribute(_T("Client")))
    {
        m_sClientKey=container->GetAttribute(_T("Client"));
    }
    if (container->HasAttribute(_T("Locked")))
    {
        m_bLocked=(container->GetAttribute(_T("Locked"))==_T("Yes"));
    }
    wxXmlNode *node=container->GetChildren(), *subNode;
    wxString sName, sSubName, sValue;
    wxString::const_iterator sit;
    double dVal;
    while(node!=NULL)
    {
        sName=node->GetName();
        if (sName==_T("Client"))
        {
            m_sClientKey=node->GetAttribute(_T("SourceKey"));
            subNode=node->GetChildren();
            while(subNode!=NULL)
            {
                sSubName=subNode->GetName();
                if (sSubName==_T("Title"))
                {
                    m_sClientTitle=subNode->GetNodeContent();
                }
                if (sSubName==_T("FullString"))
                {
                    m_sClientFullString=subNode->GetNodeContent();
                }
                subNode=subNode->GetNext();
            }
        }
        if (sName==_T("Item"))
        {
            sValue=node->GetAttribute(_T("Quantity"), _T("0"));
            sValue.ToLong(&lVal);
            m_ariQuantity.Add(lVal);
            sValue=node->GetAttribute(_T("UnitPrice"), _T("0"));
            sValue.ToDouble(&dVal);
            m_ardUnitPrice.Add(dVal);
            subNode=node->GetChildren();
            while(subNode!=NULL)
            {
                sSubName=subNode->GetName();
                if (sSubName==_T("Description"))
                {
                    m_arsItemDescr.Add(subNode->GetChildren()->GetContent());
                }
                if (sSubName==_T("Comments"))
                {
                    m_arsItemComments.Add(subNode->GetChildren()->GetContent());
                }
                subNode=subNode->GetNext();
            }
            while(m_arsItemComments.GetCount()<m_arsItemDescr.GetCount())
                m_arsItemComments.Add(wxEmptyString);
        }
        else
        {
            subNode=node->GetChildren();
            if (subNode!=NULL)
                sValue=subNode->GetContent();
            else
                sValue=wxEmptyString;
            if (sName==_T("CreationDate"))
            {
                m_dtCreation.ParseDateTime(sValue, &sit);
            }
            if (sName==_T("TermDate"))
            {
                m_dtTerm.ParseDateTime(sValue, &sit);
            }
            if (sName==_T("Discount"))
            {
                sValue.ToDouble(&m_dDiscount);
            }
            if (sName==_T("VAT"))
            {
                sValue.ToDouble(&m_dVAT);
            }
            if (sName==_T("PayNote"))
            {
                m_sPayNote=sValue;
            }
        }

        node=node->GetNext();
    }
    m_bModified=false;
    // If clients strings are empty, fill them with source datas
    Client *c=DatasManager::Get().GetClient(m_sClientKey);
    if (c==NULL) return;
    if (m_sClientTitle.IsEmpty())
        UpdateClientTitle();
    if (m_sClientFullString.IsEmpty())
        UpdateClientFullString();
}

void Bill::Initialize()
{
    m_dtCreation=wxDateTime::Now();
    m_dtTerm=wxInvalidDateTime;
    m_sClientKey=wxEmptyString;
    m_sClientTitle=wxEmptyString;
    m_sClientFullString=wxEmptyString;
    m_bLocked=false;
    m_dDiscount=0.;
    m_dVAT=0.;
    m_arsItemDescr.Clear();
    m_arsItemComments.Clear();
    m_ariQuantity.Clear();
    m_ardUnitPrice.Clear();
    m_iBillNumber=-1;
    m_sPayNote=wxEmptyString;

    m_bModified=false;
}

void Bill::UpdateClientTitle()
{
    m_sClientTitle=DatasManager::Get().GetClientString(m_sClientKey);
}

void Bill::UpdateClientFullString()
{
    Client *c=DatasManager::Get().GetClient(m_sClientKey);
    if (c==NULL) return;
    m_sClientFullString=c->GetAttribute(_T("Company"));
    if (!m_sClientFullString.IsEmpty()) m_sClientFullString << _T(" - ");
    m_sClientFullString << c->GetAttribute(_T("LastName")) << _T(" ") << c->GetAttribute(_T("FirstName"));
    if (!c->GetAttribute(_T("Addr1")).IsEmpty())
        m_sClientFullString << _T("\n") << c->GetAttribute(_T("Addr1"));
    if (!c->GetAttribute(_T("Addr2")).IsEmpty())
        m_sClientFullString << _T("\n") << c->GetAttribute(_T("Addr2"));
    m_sClientFullString << _T("\n") << c->GetAttribute(_T("ZipCode")) << _T(" ") << c->GetAttribute(_T("City"));
    if (c->HasAttribute(_T("Country")))
        m_sClientFullString << _T(" - ") << c->GetAttribute(_T("Country"));
}

bool Bill::SetBillNumber(int number)
{
    if (m_bLocked) return false;
    if (number!=m_iBillNumber)
    {
        m_iBillNumber=number;
        m_bModified=true;
    }
    return true;
}

bool Bill::SetClientKey(const wxString& key)
{
    if (m_bLocked) return false;
    if (key!=m_sClientKey)
    {
        m_sClientKey=key;
        UpdateClientTitle();
        UpdateClientFullString();
        m_bModified=true;
    }
    return true;
}

bool Bill::SetClientTitle(const wxString& title)
{
    if (m_bLocked) return false;
    if (title!=m_sClientTitle)
    {
        m_sClientTitle=title;
        m_bModified=true;
    }
    return true;
}

bool Bill::SetClientFullString(const wxString& value)
{
    if (m_bLocked) return false;
    if (value!=m_sClientFullString)
    {
        m_sClientFullString=value;
        m_bModified=true;
    }
    return true;
}

bool Bill::IsClientUpToDate()
{
    Client *c=DatasManager::Get().GetClient(m_sClientKey);
    if (c==NULL) return false;

    wxString sTmp=m_sClientTitle;
    UpdateClientTitle();
    bool bRes = sTmp.IsSameAs(m_sClientTitle);
    m_sClientTitle=sTmp;
    if (!bRes) return false;

    sTmp=m_sClientFullString;
    UpdateClientFullString();
    bRes = sTmp.IsSameAs(m_sClientFullString);
    m_sClientFullString=sTmp;

    return bRes;
}

void Bill::UpdateClientDatas()
{
    UpdateClientTitle();
    UpdateClientFullString();
    m_bModified=true;
}

void Bill::SetLocked(bool locked)
{
    if (m_bLocked!=locked)
    {
        m_bLocked=locked;
        m_bModified=true;
    }
}

bool Bill::RemoveItem(size_t index)
{
    if ((index>=m_arsItemDescr.GetCount())||
        (index>=m_arsItemComments.GetCount())||
        (index>=m_ariQuantity.GetCount())||
        (index>=m_ardUnitPrice.GetCount()))
        return false;
    m_arsItemDescr.RemoveAt(index);
    m_arsItemComments.RemoveAt(index);
    m_ariQuantity.RemoveAt(index);
    m_ardUnitPrice.RemoveAt(index);
    m_bModified=true;

    return true;
}

bool Bill::RemoveAllItems()
{
    if (m_arsItemDescr.IsEmpty())
        return true;
    m_arsItemDescr.Clear();
    m_arsItemComments.Clear();
    m_ardUnitPrice.Clear();
    m_ariQuantity.Clear();
    m_bModified=true;

    return true;
}

int Bill::AddItem(const wxString& descr, double unitprice, const wxString& comments, int quantity)
{
    m_arsItemDescr.Add(descr);
    m_arsItemComments.Add(comments);
    m_ariQuantity.Add(quantity);
    m_ardUnitPrice.Add(unitprice);
    m_bModified=true;

    return m_arsItemDescr.GetCount();
}

bool Bill::SetItemDescription(size_t index, const wxString& descr)
{
    if (index>=m_arsItemDescr.GetCount())
        return false;
    if (m_arsItemDescr[index]!=descr)
    {
        m_bModified=true;
        m_arsItemDescr[index]=descr;
    }
    return true;
}

const wxString& Bill::GetItemDescription(size_t index)
{
    if (index>=m_arsItemDescr.GetCount())
        return szEmptyString;
    return m_arsItemDescr[index];
}

bool Bill::SetItemComments(size_t index, const wxString& comments)
{
    if (index>=m_arsItemComments.GetCount())
        return false;
    if (m_arsItemComments[index]!=comments)
    {
        m_bModified=true;
        m_arsItemComments[index]=comments;
    }
    return true;
}

const wxString& Bill::GetItemComments(size_t index)
{
    if (index>=m_arsItemComments.GetCount())
        return szEmptyString;
    return m_arsItemComments[index];
}

bool Bill::SetItemUnitPrice(size_t index, double price)
{
    if (index>=m_ardUnitPrice.GetCount())
        return false;
    if (m_ardUnitPrice[index]!=price)
    {
        m_bModified=true;
        m_ardUnitPrice[index]=price;
    }
    return true;
}

double Bill::GetItemUnitPrice(size_t index)
{
    if (index>=m_ardUnitPrice.GetCount())
        return (double)wxNOT_FOUND;
    return m_ardUnitPrice[index];
}

bool Bill::SetItemQuantity(size_t index, int quantity)
{
    if (index>=m_ariQuantity.GetCount())
        return false;
    if (m_ariQuantity[index]!=quantity)
    {
        m_bModified=true;
        m_ariQuantity[index]=quantity;
    }
    return true;
}

int Bill::GetItemQuantity(size_t index)
{
    if (index>=m_ariQuantity.GetCount())
        return wxNOT_FOUND;
    return m_ariQuantity[index];
}

void Bill::SetDiscount(double value)
{
    if (m_dDiscount!=value)
    {
        m_bModified=true;
        m_dDiscount=value;
    }
}

void Bill::SetCreationDate(wxDateTime dt)
{
    if (dt!=m_dtCreation)
    {
        m_bModified=true;
        m_dtCreation=dt;
    }
}

void Bill::SetTermDate(wxDateTime dt)
{
    if (dt!=m_dtTerm)
    {
        m_bModified=true;
        m_dtTerm=dt;
    }
}

void Bill::SetVAT(double vat)
{
    if (vat!=m_dVAT)
    {
        m_dVAT=vat;
        m_bModified=true;
    }
}

void Bill::SetPayNote(const wxString& note)
{
    if (note!=m_sPayNote)
    {
        m_sPayNote=note;
        m_bModified=true;
    }
}

double Bill::GetTotalPrice() const
{
    double dTotal=0;
    for (int i=0; i<GetItemsCount(); ++i)
    {
        dTotal += m_ardUnitPrice[i] * m_ariQuantity[i];
    }
    double dDisc=1.-(m_dDiscount/100.);
    dTotal *=dDisc;
    if (m_dVAT!=0.)
    {
        double dVat=1.+(m_dVAT/100.);
        dTotal *= dVat;
    }
    return dTotal;
}

int Bill::SortBillByNumberAscCallback(const Bill** b1, const Bill** b2)
{
    static DatasManager& datasMngr=DatasManager::Get();
    const Bill *pb1=*b1, *pb2=*b2;
    int i1=pb1->GetBillNumber(), i2=pb2->GetBillNumber();
    if (i1<i2)
        return -1;
    else
    {
        if (i1>i2)
            return 1;
        else
        {
            return datasMngr.GetClientString(pb1->GetClientKey()).Cmp(datasMngr.GetClientString(pb2->GetClientKey()));
        }
    }
    return 0;
}
int Bill::SortBillByCreationDateAscCallback(const Bill** b1, const Bill** b2)
{
    static DatasManager& datasMngr=DatasManager::Get();
    const Bill *pb1=*b1, *pb2=*b2;
    if (pb1->GetCreationDate().IsEarlierThan(pb2->GetCreationDate()))
        return -1;
    else
    {
        if (pb1->GetCreationDate().IsLaterThan(pb2->GetCreationDate()))
            return 1;
        else
        {
            return datasMngr.GetClientString(pb1->GetClientKey()).Cmp(datasMngr.GetClientString(pb2->GetClientKey()));
        }
    }
    return 0;
}

int Bill::SortBillByClientAscCallback(const Bill** b1, const Bill** b2)
{
    static DatasManager& datasMngr=DatasManager::Get();
    const Bill *pb1=*b1, *pb2=*b2;
    return datasMngr.GetClientString(pb1->GetClientKey()).Cmp(datasMngr.GetClientString(pb2->GetClientKey()));
}

int Bill::SortBillByTotalAscCallback(const Bill** b1, const Bill** b2)
{
    const Bill *pb1=*b1, *pb2=*b2;
    double d1=pb1->GetTotalPrice(), d2=pb2->GetTotalPrice();
    if (d1<d2)
        return -1;
    else
    {
        if (d1>d2)
            return 1;
        else
        {
            return SortBillByCreationDateAscCallback(b1, b2);
        }
    }
}

int Bill::SortBillByStatusAscCallback(const Bill** b1, const Bill** b2)
{
    const Bill *pb1=*b1, *pb2=*b2;
    return (pb1->IsLocked()==pb2->IsLocked()?0:1);
}

int Bill::SortBillByTermDateAscCallback(const Bill** b1, const Bill** b2)
{
    const Bill *pb1=*b1, *pb2=*b2;
    wxDateTime d1=pb1->GetTermDate(), d2=pb2->GetTermDate();
    if (d1<d2)
        return -1;
    else
    {
        if (d1>d2)
            return 1;
        else
        {
            return 0;
        }
    }
}

int Bill::SortBillByNumberDescCallback(const Bill** b1, const Bill** b2)
{
    return SortBillByNumberAscCallback(b2, b1);
}

int Bill::SortBillByCreationDateDescCallback(const Bill** b1, const Bill** b2)
{
    return SortBillByCreationDateAscCallback(b2, b1);
}

int Bill::SortBillByClientDescCallback(const Bill** b1, const Bill** b2)
{
    return SortBillByClientAscCallback(b2, b1);
}

int Bill::SortBillByTotalDescCallback(const Bill** b1, const Bill** b2)
{
    return SortBillByTotalAscCallback(b2, b1);
}
int Bill::SortBillByStatusDescCallback(const Bill** b1, const Bill** b2)
{
    return SortBillByStatusAscCallback(b2, b1);
}

int Bill::SortBillByTermDateDescCallback(const Bill** b1, const Bill** b2)
{
    return SortBillByTermDateAscCallback(b2, b1);
}

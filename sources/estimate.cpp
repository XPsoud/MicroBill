#include "estimate.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ListOfEstimates);

#include <wx/xml/xml.h>
static const wxString szEmptyString=_T("");

#include "datasmanager.h"

Estimate::Estimate(wxXmlNode* container)
{
    Initialize();
    if (container!=NULL)
        FromXmlNode(container);
}

Estimate::~Estimate()
{
    // Destructor
}

void Estimate::ToXmlNode(wxXmlNode* container)
{
    container->AddAttribute(_T("Client"), m_sClient);
    if (m_bLocked)
    {
        container->AddAttribute(_T("Locked"), _T("Yes"));
    }
    wxXmlNode *node=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("CreationDate"));
    container->AddChild(node);
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_dtCreation.Format(_T("%Y-%m-%d %H:%M:%S"))));
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
        wxXmlNode *subNode=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Description"));
        node->AddChild(subNode);
        subNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_arsItemDescr[i]));
        if (!m_arsItemComments[i].IsEmpty())
        {
            subNode->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("Comments")));
            subNode=subNode->GetNext();
            subNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), m_arsItemComments[i]));
        }
    }
}

void Estimate::FromXmlNode(wxXmlNode* container)
{
    if (container->HasAttribute(_T("Client")))
    {
        m_sClient=container->GetAttribute(_T("Client"));
    }
    if (container->HasAttribute(_T("Locked")))
    {
        m_bLocked=(container->GetAttribute(_T("Locked"))==_T("Yes"));
    }
    wxXmlNode *node=container->GetChildren(), *subNode;
    wxString sName, sSubName, sValue;
    wxString::const_iterator sit;
    long lVal;
    double dVal;
    while(node!=NULL)
    {
        sName=node->GetName();
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
        }

        node=node->GetNext();
    }
    m_bModified=false;
}

void Estimate::Initialize()
{
    m_dtCreation=wxDateTime::Now();
    m_dtTerm=wxInvalidDateTime;
    m_sClient=wxEmptyString;
    m_bLocked=false;
    m_dDiscount=0.;
    m_dVAT=0.;
    m_arsItemDescr.Clear();
    m_arsItemComments.Clear();
    m_ariQuantity.Clear();
    m_ardUnitPrice.Clear();

    m_bModified=false;
}

void Estimate::SetClient(const wxString& key)
{
    if (key!=m_sClient)
    {
        m_sClient=key;
        m_bModified=true;
    }
}

void Estimate::SetLocked(bool locked)
{
    if (m_bLocked!=locked)
    {
        m_bLocked=locked;
        m_bModified=true;
    }
}

bool Estimate::RemoveItem(size_t index)
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

bool Estimate::RemoveAllItems()
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

int Estimate::AddItem(const wxString& descr, double unitprice, const wxString& comments, int quantity)
{
    m_arsItemDescr.Add(descr);
    m_arsItemComments.Add(comments);
    m_ariQuantity.Add(quantity);
    m_ardUnitPrice.Add(unitprice);
    m_bModified=true;

    return m_arsItemDescr.GetCount();
}

bool Estimate::SetItemDescription(size_t index, const wxString& descr)
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

const wxString& Estimate::GetItemDescription(size_t index)
{
    if (index>=m_arsItemDescr.GetCount())
        return szEmptyString;
    return m_arsItemDescr[index];
}

bool Estimate::SetItemComments(size_t index, const wxString& comments)
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

const wxString& Estimate::GetItemComments(size_t index)
{
    if (index>=m_arsItemComments.GetCount())
        return szEmptyString;
    return m_arsItemComments[index];
}

bool Estimate::SetItemUnitPrice(size_t index, double price)
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

double Estimate::GetItemUnitPrice(size_t index)
{
    if (index>=m_ardUnitPrice.GetCount())
        return (double)wxNOT_FOUND;
    return m_ardUnitPrice[index];
}

bool Estimate::SetItemQuantity(size_t index, int quantity)
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

int Estimate::GetItemQuantity(size_t index)
{
    if (index>=m_ariQuantity.GetCount())
        return wxNOT_FOUND;
    return m_ariQuantity[index];
}

void Estimate::SetDiscount(double value)
{
    if (m_dDiscount!=value)
    {
        m_bModified=true;
        m_dDiscount=value;
    }
}

void Estimate::SetCreationDate(wxDateTime dt)
{
    if (dt!=m_dtCreation)
    {
        m_bModified=true;
        m_dtCreation=dt;
    }
}

void Estimate::SetTermDate(wxDateTime dt)
{
    if (dt!=m_dtTerm)
    {
        m_bModified=true;
        m_dtTerm=dt;
    }
}

void Estimate::SetVAT(double vat)
{
    if (vat!=m_dVAT)
    {
        m_dVAT=vat;
        m_bModified=true;
    }
}

double Estimate::GetTotalPrice() const
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

int Estimate::SortEstimateByCreationDateAscCallback(const Estimate** e1, const Estimate** e2)
{
    static DatasManager& datasMngr=DatasManager::Get();
    const Estimate *pe1=*e1, *pe2=*e2;
    if (pe1->GetCreationDate().IsEarlierThan(pe2->GetCreationDate()))
        return -1;
    else
    {
        if (pe1->GetCreationDate().IsLaterThan(pe2->GetCreationDate()))
            return 1;
        else
        {
            return datasMngr.GetClientString(pe1->GetClient()).Cmp(datasMngr.GetClientString(pe2->GetClient()));
        }
    }
    return 0;
}

int Estimate::SortEstimateByClientAscCallback(const Estimate** e1, const Estimate** e2)
{
    static DatasManager& datasMngr=DatasManager::Get();
    const Estimate *pe1=*e1, *pe2=*e2;
    return datasMngr.GetClientString(pe1->GetClient()).Cmp(datasMngr.GetClientString(pe2->GetClient()));
}

int Estimate::SortEstimateByTotalAscCallback(const Estimate** e1, const Estimate** e2)
{
    const Estimate *pe1=*e1, *pe2=*e2;
    double d1=pe1->GetTotalPrice(), d2=pe2->GetTotalPrice();
    if (d1<d2)
        return -1;
    else
    {
        if (d1>d2)
            return 1;
        else
        {
            return SortEstimateByCreationDateAscCallback(e1, e2);
        }
    }
}

int Estimate::SortEstimateByStatusAscCallback(const Estimate** e1, const Estimate** e2)
{
    const Estimate *pe1=*e1, *pe2=*e2;
    return (pe1->IsLocked()==pe2->IsLocked()?0:1);
}

int Estimate::SortEstimateByTermDateAscCallback(const Estimate** e1, const Estimate** e2)
{
    const Estimate *pe1=*e1, *pe2=*e2;
    wxDateTime d1=pe1->GetTermDate(), d2=pe2->GetTermDate();
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

int Estimate::SortEstimateByCreationDateDescCallback(const Estimate** e1, const Estimate** e2)
{
    return SortEstimateByCreationDateAscCallback(e2, e1);
}

int Estimate::SortEstimateByClientDescCallback(const Estimate** e1, const Estimate** e2)
{
    return SortEstimateByClientAscCallback(e2, e1);
}

int Estimate::SortEstimateByTotalDescCallback(const Estimate** e1, const Estimate** e2)
{
    return SortEstimateByTotalAscCallback(e2, e1);
}

int Estimate::SortEstimateByStatusDescCallback(const Estimate** e1, const Estimate** e2)
{
    return SortEstimateByStatusAscCallback(e2, e1);
}

int Estimate::SortEstimateByTermDateDescCallback(const Estimate** e1, const Estimate** e2)
{
    return SortEstimateByTermDateAscCallback(e2, e1);
}

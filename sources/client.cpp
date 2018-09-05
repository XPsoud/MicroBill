#include "client.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ListOfClients);

#include <wx/xml/xml.h>

const wxChar* Client::szAllowedAttributes[] = {
    _T("Company"), _T("LastName"), _T("FirstName"), _T("Email"), _T("Phone"), _T("Cell"),
    _T("Addr1"), _T("Addr2"), _T("ZipCode"), _T("City"), _T("Country")
};
const int Client::iAllowedAttributesCount = WXSIZEOF(Client::szAllowedAttributes);

Client::Client(wxXmlNode* container)
{
    Initialize();
    if (container!=NULL)
        FromXmlNode(container);
}

Client::~Client()
{
    //dtor
}

void Client::Initialize()
{
    CreateKey();
    m_sDatas.clear();

    m_bModified=false;
}

void Client::CreateKey()
{
    unsigned int i=(unsigned int)wxDateTime::Now().GetTicks();
    i*=1000;
    i+=wxDateTime::UNow().GetMillisecond();
    m_sKey=wxString(_T("")) << i;
}

void Client::ToXmlNode(wxXmlNode* container)
{
    container->AddAttribute(_T("Key"), m_sKey);
    wxXmlNode *node=NULL;
    wxStringToStringHashMap::iterator iter;
    for( iter = m_sDatas.begin(); iter != m_sDatas.end(); ++iter )
    {
        if (node==NULL)
        {
            node=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, iter->first);
            container->AddChild(node);
        }
        else
        {
            node->SetNext(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, iter->first));
            node=node->GetNext();
        }
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, _T(""), iter->second));
    }
}

void Client::FromXmlNode(wxXmlNode* container)
{
    if (container->HasAttribute(_T("Key")))
    {
        m_sKey=container->GetAttribute(_T("Key"));
    }
    wxXmlNode *node=container->GetChildren(), *subNode;
    wxString sName, sValue;
    while(node!=NULL)
    {
        sName=node->GetName();
        subNode=node->GetChildren();
        if (subNode!=NULL)
            sValue=subNode->GetContent();
        else
            sValue=wxEmptyString;
        m_sDatas[sName]=sValue;
        node=node->GetNext();
    }
    m_bModified=false;
}

bool Client::AddAttribute(const wxString& name, const wxString& value)
{
    // Check that the value name is part of the allowed ones
    for (int i=0; i<iAllowedAttributesCount; ++i)
    {
        if (name==szAllowedAttributes[i])
        {
            if (m_sDatas[name]!=value)
                m_bModified=true;
            m_sDatas[name]=value;
            return true;
        }
    }
    // Not an allowed value name
    return false;
}

bool Client::HasAttribute(const wxString& name)
{
    return (m_sDatas.find(name)!=m_sDatas.end());
}

void Client::DeleteAttribute(const wxString& name)
{
    if (m_sDatas.erase(name)!=0)
        m_bModified=true;
}

const wxString Client::GetAttribute(const wxString& name) const
{
    wxStringToStringHashMap::const_iterator iter=m_sDatas.find(name);
    if (iter!=m_sDatas.end())
        return iter->second;
    else
        return wxEmptyString;
}

bool Client::CheckIfMatches(Client* item, const wxString& filter, int field)
{
    if (item==NULL) return false;
    if (filter.IsEmpty()) return true;
    if (field==CATTR_UNKNOWN)
    {
        wxStringToStringHashMap::iterator iter;
        for (iter=item->m_sDatas.begin(); iter!=item->m_sDatas.end(); ++iter)
        {
            if (iter->second.Find(filter)!=wxNOT_FOUND)
                return true;
        }
        return false;
    }
    else
    {
        if (field==CATTR_ADDRESS)
        {
            for (int i=CATTR_ADDR1; i<=CATTR_COUNTRY; ++i)
            {
                if (item->HasAttribute(szAllowedAttributes[i]))
                {
                    return (item->GetAttribute(szAllowedAttributes[i]).Find(filter)!=wxNOT_FOUND);
                }
            }
            return false;
        }
        else
        {
            if (item->HasAttribute(szAllowedAttributes[field]))
            {
                return (item->GetAttribute(szAllowedAttributes[field]).Find(filter)!=wxNOT_FOUND);
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

int Client::SortClientsByCompanyAscCallback(const Client** c1, const Client** c2)
{
    const Client *pc1=*c1, *pc2=*c2;
    int iRes=pc1->GetAttribute(_T("Company")).Cmp(pc2->GetAttribute(_T("Company")));
    if (iRes==0)
    {
        iRes=pc1->GetAttribute(_T("LastName")).Cmp(pc2->GetAttribute(_T("LastName")));
        if (iRes==0)
        {
            return pc1->GetAttribute(_T("FirstName")).Cmp(pc2->GetAttribute(_T("FirstName")));
        }
    }
    return iRes;
}

int Client::SortClientsByLastNameAscCallback(const Client** c1, const Client** c2)
{
    const Client *pc1=*c1, *pc2=*c2;
    int iRes=pc1->GetAttribute(_T("LastName")).Cmp(pc2->GetAttribute(_T("LastName")));
    if (iRes==0)
    {
        iRes=pc1->GetAttribute(_T("FirstName")).Cmp(pc2->GetAttribute(_T("FirstName")));
        if (iRes==0)
        {
            return pc1->GetAttribute(_T("Company")).Cmp(pc2->GetAttribute(_T("Company")));
        }
    }
    return iRes;
}

int Client::SortClientsByFirstNameAscCallback(const Client** c1, const Client** c2)
{
    const Client *pc1=*c1, *pc2=*c2;
    int iRes=pc1->GetAttribute(_T("FirstName")).Cmp(pc2->GetAttribute(_T("FirstName")));
    if (iRes==0)
    {
        iRes=pc1->GetAttribute(_T("LastName")).Cmp(pc2->GetAttribute(_T("LastName")));
        if (iRes==0)
        {
            return pc1->GetAttribute(_T("Company")).Cmp(pc2->GetAttribute(_T("Company")));
        }
    }
    return iRes;
}

int Client::SortClientsByCompanyDescCallback(const Client** c1, const Client** c2)
{
    return SortClientsByCompanyAscCallback(c2, c1);
}

int Client::SortClientsByLastNameDescCallback(const Client** c1, const Client** c2)
{
    return SortClientsByLastNameAscCallback(c2, c1);
}

int Client::SortClientsByFirstNameDescCallback(const Client** c1, const Client** c2)
{
    return SortClientsByFirstNameAscCallback(c2, c1);
}

#ifndef __CLIENT_H_INCLUDED__
#define __CLIENT_H_INCLUDED__

#include <wx/wx.h>
#include <wx/hashmap.h>

class wxXmlNode;

enum
{
    CATTR_UNKNOWN = -1,
    CATTR_COMPANY =0,
    CATTR_LASTNAME,
    CATTR_FIRSTNAME,
    CATTR_EMAIL,
    CATTR_PHONE,
    CATTR_ADDR1,
    CATTR_ADDR2,
    CATTR_ZIPCODE,
    CATTR_CITY,
    CATTR_COUNTRY,

    CATTR_COUNT,
    CATTR_ADDRESS=CATTR_COUNT
};

class Client
{
    public:
        Client(wxXmlNode* container=NULL);
        virtual ~Client();
        void ToXmlNode(wxXmlNode* container);
        void FromXmlNode(wxXmlNode* container);
        // Accessors
        const wxString& GetKey() { return m_sKey; }
        bool IsModified() { return m_bModified; }
        bool AddAttribute(const wxString& name, const wxString& value);
        bool HasAttribute(const wxString& name);
        void DeleteAttribute(const wxString& name);
        const wxString GetAttribute(const wxString& name) const;
        // Misc
        bool Matches(const wxString& filter, int field=CATTR_UNKNOWN) { return CheckIfMatches(this, filter, field); }
        static int SortClientsByCompanyAscCallback(const Client** c1, const Client** c2);
        static int SortClientsByLastNameAscCallback(const Client** c1, const Client** c2);
        static int SortClientsByFirstNameAscCallback(const Client** c1, const Client** c2);
        static int SortClientsByCompanyDescCallback(const Client** c1, const Client** c2);
        static int SortClientsByLastNameDescCallback(const Client** c1, const Client** c2);
        static int SortClientsByFirstNameDescCallback(const Client** c1, const Client** c2);
        // Allowed attributes
        static const wxChar* szAllowedAttributes[];
        static const int iAllowedAttributesCount;
    protected:
    private:
        // Misc functions
        void CreateKey();
        void Initialize();
        static bool CheckIfMatches(Client* item, const wxString& filter, int field);
        // Misc vars
        wxString m_sKey;
        bool m_bModified;
        // Client datas vars
        wxStringToStringHashMap m_sDatas;
};

#include <wx/list.h>
WX_DECLARE_LIST(Client, ListOfClients);

#endif // __CLIENT_H_INCLUDED__

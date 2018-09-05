#ifndef __DATASMANAGER_H_INCLUDED__
#define __DATASMANAGER_H_INCLUDED__

#include <wx/wx.h>

#include "client.h"

class DatasManager
{
    public:
        static DatasManager& Get();
        bool ReadDatas();
        bool SaveDatas();
        void ClearDatas();
        // Accessors
        bool IsModified();
        // Clients management
        int GetClientsCount() { return m_clients.size(); }
        Client* AddNewClient();
        Client* GetClient(size_t index);
        Client* GetClient(const wxString& key);
        bool RemoveClient(Client* item);
        bool IsClientUsed(Client* item);
        wxString GetClientString(Client* item);
        wxString GetClientString(const wxString& key);
        bool SortClients(int sortKey, bool asc=true);
    protected:
    private:
        DatasManager();
        virtual ~DatasManager();
        void Initialize();
        void SetDatasXmlTree(wxXmlNode *container);
        wxXmlNode* GetDatasXmlTree();
        // Unique instance of the singleton
        static DatasManager m_instance;
        // Misc vars
        bool m_bInitialized, m_bModified;
        static const wxChar* m_szDatasFName;
        ListOfClients m_clients;
};

#endif // __DATASMANAGER_H_INCLUDED__

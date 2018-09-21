#ifndef __DATASMANAGER_H_INCLUDED__
#define __DATASMANAGER_H_INCLUDED__

#include <wx/wx.h>

#include "bill.h"
#include "client.h"
#include "estimate.h"

class DatasManager
{
    public:
        static DatasManager& Get();
        bool ReadDatas();
        bool SaveDatas();
        void ClearDatas();
        // Accessors
        bool IsModified();
        // Last used bill number
        int GetLastBillNumber() { return m_iLastBillNumber; }
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
        // Estimates management
        int GetEstimatesCount() { return m_estimates.size(); }
        int GetLockedEstimatesCount();
        int GetUnlockedEstimatesCount();
        Estimate* AddNewEstimate();
        Estimate* GetEstimate(size_t index);
        bool RemoveEstimate(Estimate* item);
        bool SortEstimates(int sortKey, bool asc=true);
        // Bills management
        int GetBillsCount() { return m_bills.size(); }
        int GetLockedBillsCount();
        int GetUnlockedBillsCount();
        Bill* AddNewBill();
        Bill* AddNewBill(Estimate* estimate);
        Bill* GetBill(size_t index);
        bool RemoveBill(Bill* item);
        bool SortBills(int sortKey, bool asc=true);
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
        ListOfEstimates m_estimates;
        ListOfBills m_bills;
        int m_iLastBillNumber;
};

#endif // __DATASMANAGER_H_INCLUDED__

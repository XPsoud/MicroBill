#ifndef __BILL_H_INCLUDED__
#define __BILL_H_INCLUDED__

#include <wx/wx.h>
#include <wx/hashmap.h>

class wxXmlNode;

enum BillSortColumn
{
    BSC_NUMBER=0,
    BSC_CREAT_DATE,
    BSC_CLIENT,
    BSC_TOTAL,
    BSC_STATUS,
    BSC_TERM_DATE,

    BSC_COUNT
};

class Bill
{
    public:
        Bill(wxXmlNode* container=NULL);
        virtual ~Bill();
        void ToXmlNode(wxXmlNode* container);
        void FromXmlNode(wxXmlNode* container);
        // Accessors
        bool IsModified() { return m_bModified; }
        int GetBillNumber() const { return m_iBillNumber; }
        bool SetBillNumber(int number);
        bool SetClientKey(const wxString& key);
        const wxString& GetClientKey() const { return m_sClientKey; }
        bool SetClientTitle(const wxString& title);
        const wxString& GetClientTitle() const { return m_sClientTitle; }
        bool SetClientFullString(const wxString& value);
        bool IsClientUpToDate();
        void UpdateClientDatas();
        const wxString& GetClientFullString() const { return m_sClientFullString; }
        const bool IsLocked() const { return m_bLocked; }
        void SetLocked(bool locked=true);
        void Lock() { SetLocked(true); }
        void UnLock() { SetLocked(false); }
        const wxDateTime& GetCreationDate() const { return m_dtCreation; }
        void SetCreationDate(wxDateTime dt);
        const wxDateTime& GetTermDate() const { return m_dtTerm; }
        void SetTermDate(wxDateTime dt);
        int GetItemsCount()const { return m_arsItemDescr.GetCount(); }
        bool RemoveItem(size_t index);
        bool RemoveAllItems();
        int AddItem(const wxString& descr, double unitprice, const wxString& comments=wxEmptyString, int quantity=0);
        bool SetItemDescription(size_t index, const wxString& descr);
        const wxString& GetItemDescription(size_t index);
        bool SetItemComments(size_t index, const wxString& comments);
        const wxString& GetItemComments(size_t index);
        bool SetItemUnitPrice(size_t index, double price);
        double GetItemUnitPrice(size_t index);
        bool SetItemQuantity(size_t index, int quantity);
        int GetItemQuantity(size_t index);
        void SetDiscount(double value);
        double GetDiscount() { return m_dDiscount; }
        double GetTotalPrice() const;
        void SetVAT(double vat);
        double GetVAT() { return m_dVAT; }
        void SetPayNote(const wxString& note);
        const wxString& GetPayNote() { return m_sPayNote; }
        // Sorting callbacks
        static int SortBillByNumberAscCallback(const Bill** b1, const Bill** b2);
        static int SortBillByCreationDateAscCallback(const Bill** b1, const Bill** b2);
        static int SortBillByClientAscCallback(const Bill** b1, const Bill** b2);
        static int SortBillByTotalAscCallback(const Bill** b1, const Bill** b2);
        static int SortBillByStatusAscCallback(const Bill** b1, const Bill** b2);
        static int SortBillByTermDateAscCallback(const Bill** b1, const Bill** b2);
        static int SortBillByNumberDescCallback(const Bill** b1, const Bill** b2);
        static int SortBillByCreationDateDescCallback(const Bill** b1, const Bill** b2);
        static int SortBillByClientDescCallback(const Bill** b1, const Bill** b2);
        static int SortBillByTotalDescCallback(const Bill** b1, const Bill** b2);
        static int SortBillByStatusDescCallback(const Bill** b1, const Bill** b2);
        static int SortBillByTermDateDescCallback(const Bill** b1, const Bill** b2);
    protected:
    private:
        // Misc functions
        void Initialize();
        void UpdateClientTitle();
        void UpdateClientFullString();
        // Misc vars
        bool m_bModified;
        // Bill datas
        wxDateTime m_dtCreation, m_dtTerm;
        int m_iBillNumber;
        bool m_bLocked;
        double m_dDiscount, m_dVAT;
        wxArrayString m_arsItemDescr, m_arsItemComments;
        wxArrayInt m_ariQuantity;
        wxArrayDouble m_ardUnitPrice;
        wxString m_sPayNote;
        // Client datas
        wxString m_sClientKey, m_sClientTitle, m_sClientFullString;
};

#include <wx/list.h>
WX_DECLARE_LIST(Bill, ListOfBills);

#endif // __BILL_H_INCLUDED__

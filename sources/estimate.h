#ifndef __ESTIMATE_H_INCLUDED__
#define __ESTIMATE_H_INCLUDED__

#include <wx/wx.h>
#include <wx/hashmap.h>

class wxXmlNode;

class Estimate
{
    public:
        Estimate(wxXmlNode* container=NULL);
        virtual ~Estimate();
        void ToXmlNode(wxXmlNode* container);
        void FromXmlNode(wxXmlNode* container);
        // Accessors
        bool IsModified() { return m_bModified; }
        void SetClient(const wxString& key);
        const wxString& GetClient() const { return m_sClient; }
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
        double GetInitialPrice() const;
        double GetFinalPrice() const;
        void SetVAT(double vat);
        double GetVAT() { return m_dVAT; }
        // Sorting callbacks
        static int SortEstimateByCreationDateAscCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByClientAscCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByTotalAscCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByStatusAscCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByTermDateAscCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByCreationDateDescCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByClientDescCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByTotalDescCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByStatusDescCallback(const Estimate** e1, const Estimate** e2);
        static int SortEstimateByTermDateDescCallback(const Estimate** e1, const Estimate** e2);
    protected:
    private:
        // Misc functions
        void Initialize();
        // Misc vars
        bool m_bModified;
        // Estimate datas
        wxDateTime m_dtCreation, m_dtTerm;
        wxString m_sClient;
        bool m_bLocked;
        double m_dDiscount, m_dVAT;
        wxArrayString m_arsItemDescr, m_arsItemComments;
        wxArrayInt m_ariQuantity;
        wxArrayDouble m_ardUnitPrice;
};

#include <wx/list.h>
WX_DECLARE_LIST(Estimate, ListOfEstimates);

#endif // __ESTIMATE_H_INCLUDED__

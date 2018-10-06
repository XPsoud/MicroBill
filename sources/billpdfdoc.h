#ifndef __BILLPDFDOC_H_INCLUDED__
#define __BILLPDFDOC_H_INCLUDED__

#include "microbillpdfdoc.h"

class Bill;

class BillPdfDoc : public MicroBillPdfDoc
{
    public:
        BillPdfDoc(Bill* item);
        virtual ~BillPdfDoc();
    protected:
        void WriteTitle();
        void WriteItems();
        void Finalize();
    private:
        double WriteItem(int itmIdx);
        Bill* m_bill;
};

#endif // __BILLPDFDOC_H_INCLUDED__

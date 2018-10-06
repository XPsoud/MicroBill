#ifndef __ESTIMATEPDFDOC_H_INCLUDED__
#define __ESTIMATEPDFDOC_H_INCLUDED__

#include "microbillpdfdoc.h"

class Estimate;

class EstimatePdfDoc : public MicroBillPdfDoc
{
    public:
        EstimatePdfDoc(Estimate* item);
        virtual ~EstimatePdfDoc();
    protected:
        void WriteTitle();
        void WriteItems();
        void Finalize();
    private:
        void WriteItem(int itmIdx);
        Estimate* m_estimate;
};

#endif // __ESTIMATEPDFDOC_H_INCLUDED__

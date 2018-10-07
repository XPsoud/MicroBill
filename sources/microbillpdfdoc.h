#ifndef __MICROBILLPDFDOC_H_INCLUDED__
#define __MICROBILLPDFDOC_H_INCLUDED__

#include <wx/wx.h>
#include <wx/pdfdoc.h>

#include "settingsmanager.h"

class MicroBillPdfDoc : public wxPdfDocument
{
    public:
        MicroBillPdfDoc();
        virtual ~MicroBillPdfDoc();
        void DoCreateDocument();
    protected:
        virtual void Header();
        virtual void Footer();
        virtual void WriteTitle() = 0;
        virtual void WriteItems() = 0;
        virtual void Finalize() = 0;
        SettingsManager& m_options;
    private:
};

#endif // __MICROBILLPDFDOC_H_INCLUDED__

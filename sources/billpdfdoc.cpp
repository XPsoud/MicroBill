#include "billpdfdoc.h"

#include "bill.h"

#include <wx/tokenzr.h>

BillPdfDoc::BillPdfDoc(Bill* item)
{
    m_bill = item;
}

BillPdfDoc::~BillPdfDoc()
{
    // Destructor
}

void BillPdfDoc::WriteTitle()
{
    SetFont(_T("Helvetica"), _T("B"), 20);
    if (m_bill!=NULL)
    {
        Cell(190, 15, wxString::Format(_("Bill #%s-%04d"), m_bill->GetCreationDate().Format(_T("%Y%m%d%H%M")), m_bill->GetBillNumber()), wxPDF_BORDER_NONE, 2, wxPDF_ALIGN_CENTER);
    }
    else
    {
        Cell(190, 15, _("Bill #_ _ _ _ _ _ _ _ _ _ _ _"), wxPDF_BORDER_NONE, 2, wxPDF_ALIGN_CENTER);
    }
    Line(10, 60, 200, 60);
    SetFont(_T("Helvetica"), _T(""), 12);
    int y=GetY(), iNbLines=0;
    SetY(y+2);
    if (m_bill!=NULL)
    {
        wxString sTmp;
        if (m_options.HasCompanyLocation())
        {
            sTmp=wxString::Format(_("Edited in %s on %s"), m_options.GetCompanyLocation(), m_bill->GetCreationDate().Format(_("%Y/%m/%d")));
        }
        else
        {
            sTmp=wxString::Format(_("Edited on %s"), m_bill->GetCreationDate().Format(_("%Y/%m/%d")));
        }
        if (m_bill->GetTermDate().IsValid())
        {
            sTmp << _T("\n") << _("To be paid before:") << m_bill->GetTermDate().Format(_("%Y/%m/%d"));
        }
        MultiCell(95, 5, sTmp);
        wxString sClient=m_bill->GetClientFullString();
        if (!sClient.IsEmpty())
        {
            wxStringTokenizer stk(sClient, _T("\n"));
            iNbLines += stk.CountTokens();
            SetXY(105, y);
            MultiCell(95, 6, sClient, wxPDF_BORDER_FRAME);
            y+=iNbLines*6;
        }
        else
        {
            SetXY(105, y);
            MultiCell(95, 6, _T(" \n \n \n "), wxPDF_BORDER_FRAME);
            y+=24;
        }
    }
    else
    {
        wxString sTmp = _("Edited on _ _ _ _ / _ _ / _ _");
        sTmp << _T("\n") << _("To be paid before: _ _ _ _ / _ _ / _ _");
        MultiCell(95, 8, sTmp);
        SetXY(105, y);
        MultiCell(95, 6, _T(" \n \n \n "), wxPDF_BORDER_FRAME);
        y+=24;
    }
    Line(10, y, 200, y);
    SetXY(10, y);
}

void BillPdfDoc::WriteItems()
{
    if (m_bill == NULL)
        return;

    int iCount=m_bill->GetItemsCount();
    double dTotal = 0.;

    for (int i=0; i<iCount; ++i)
    {
        dTotal += WriteItem(i);
    }
}

double BillPdfDoc::WriteItem(int itmIdx)
{
    if (m_bill == NULL)
        return 0.;

    if (itmIdx >= m_bill->GetItemsCount())
        return 0.;

    wxString sDescr = m_bill->GetItemDescription(itmIdx);
    wxString sComm = m_bill->GetItemComments(itmIdx);
    int iQty = m_bill->GetItemQuantity(itmIdx);
    double dPrice = m_bill->GetItemUnitPrice(itmIdx);

    // Calculate the item height
    int iHght = 4 + 6; // Top margin + Description line
    wxString sComments=wxEmptyString;
    if (!sComm.IsEmpty())
    {
        wxArrayString arsCommLines=wxStringTokenize(sComm, _T("\n"));
        for (size_t i=0; i<arsCommLines.GetCount(); ++i)
        {
            sComments << (i==0?_T("    "):_T("\n    ")) << arsCommLines[i];
            iHght += 6;
        }
    }
    iHght += 4; // Bottom margin

    // Does it fit in the current page ?
    int iY=GetY();
    if ((iY+iHght)>287)
    {
        AddPage();
        iY = GetY();
    }

    // Write Description
    SetFont(_T("Helvetica"), _T("B"), 12);
    SetXY(10, iY);
    Cell(125, 14, sDescr);
    if (!sComments.IsEmpty())
    {
        SetFont(_T("Helvetica"), _T(""), 12);
        SetXY(10, iY+10);
        MultiCell(125, 6, sComments);
    }
    if (iQty>0)
    {
        SetFont(_T("Courier"), _T("B"), 12);
        SetXY(135, iY);
        Cell(25, iHght, m_options.GetFormatedMoneyValue(dPrice, _T("%.02f")), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_RIGHT);
        Cell(15, iHght, wxString::Format(_T("%0d"), iQty), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_CENTER);
        Cell(25, iHght, m_options.GetFormatedMoneyValue(dPrice*iQty, _T("%.02f")), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_RIGHT);
    }
    SetXY(10, iY+iHght);

    return dPrice*iQty;
}

void BillPdfDoc::Finalize()
{
    if (m_bill == NULL)
        return;
    // Calculate the height needed
    int iHght=4 + 6; // Top margin + Total height
    if (m_bill->GetDiscount() != 0.)
        iHght += 12; // 2 more lines if discount enabled
    if (m_bill->GetVAT() != 0.)
        iHght += 12; // 2 more lines if VAT enabled
    iHght += 4; // Bottom margin

    // Does it fit in the current page ?
    int iY=GetY();
    if ((iY+iHght)>287)
    {
        AddPage();
        iY = GetY();
    }
    else
    {
        Line(10, iY, 200, iY);
    }

    double dTotal = m_bill->GetInitialPrice();
    wxString sTotal, sPrice = m_options.GetFormatedMoneyValue(dTotal, _T("%.02f"));

    if ((m_bill->GetDiscount()==0.) && (m_bill->GetVAT()==0.))
    {
        sTotal = _("Total to pay:");
    }
    else
    {
        if (m_bill->GetDiscount()!=0.)
        {
            sTotal = _("Sub-total:");
            sTotal << _T("\n") << wxString::Format(_("Discount (%0.2f%%)"), m_bill->GetDiscount());
            double dDisc=dTotal*(m_bill->GetDiscount()/100.);
            sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dDisc, _T("%0.2f"));
            dTotal -= dDisc;
            sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dTotal, _T("%0.2f"));
            if (m_bill->GetVAT()!=0.)
            {
                sTotal << _T("\n") << _("Total w/o VAT");
                sTotal << _T("\n") << wxString::Format(_("VAT (%0.1f%%)"), m_bill->GetVAT());
                double dVAT=dTotal*m_bill->GetVAT()/100.;
                sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dVAT, _T("%0.2f"));
                dTotal += dVAT;
                sTotal << _T("\n") << _("Total to pay:");
                sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dTotal, _T("%0.2f"));
            }
            else
            {
                sTotal << _T("\n") << _("Total w/o VAT to pay:");
            }
        }
        else
        {
            sTotal = _("Total w/o VAT:");
            sTotal << _T("\n") << wxString::Format(_("VAT (%0.1f%%)"), m_bill->GetVAT());
            double dVAT=dTotal*m_bill->GetVAT()/100.;
            sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dVAT, _T("%0.2f"));
            dTotal += dVAT;
            sTotal << _T("\n") << _("Total to pay:");
            sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dTotal, _T("%0.2f"));
        }
    }
    SetFont(_T("Helvetica"), _T("B"), 12);
    SetXY(10, iY+4);
    MultiCell(165, 6, sTotal, wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    SetFont(_T("Courier"), _T("B"), 12);
    SetXY(175, iY+4);
    MultiCell(25, 6, sPrice, wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    if (m_bill->GetVAT()==0.)
    {
        iY=GetY();
        SetXY(10, iY);
        SetFont(_T("Helvetica"), _T(""), 10);
        // TODO (Xaviou#1#): Replace the following with a user-customized text
        MultiCell(190, 6, _("V.A.T. non-applicable"), wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    }
    wxString sPayNote=m_bill->GetPayNote();
    if (!sPayNote.IsEmpty())
    {
        iY=GetY();
        SetXY(10, iY);
        MultiCell(190, 6, sPayNote, wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    }
}

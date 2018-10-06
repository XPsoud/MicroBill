#include "estimatepdfdoc.h"

#include "estimate.h"

#include <wx/tokenzr.h>

EstimatePdfDoc::EstimatePdfDoc(Estimate* item)
{
    m_estimate = item;
}

EstimatePdfDoc::~EstimatePdfDoc()
{
    // Destructor
}

void EstimatePdfDoc::WriteTitle()
{
    SetFont(_T("Helvetica"), _T("B"), 20);
    if (m_estimate!=NULL)
    {
        Cell(190, 15, wxString::Format(_("Estimate #%s"), m_estimate->GetCreationDate().Format(_T("%Y%m%d%H%M"))), wxPDF_BORDER_NONE, 2, wxPDF_ALIGN_CENTER);
    }
    else
    {
        Cell(190, 15, _("Estimate #_ _ _ _ _ _ _ _ _ _ _ _"), wxPDF_BORDER_NONE, 2, wxPDF_ALIGN_CENTER);
    }
    Line(10, 60, 200, 60);
}

void EstimatePdfDoc::WriteItems()
{
    if (m_estimate == NULL)
        return;

    int iCount=m_estimate->GetItemsCount();

    for (int i=0; i<iCount; ++i)
    {
        WriteItem(i);
    }
}

void EstimatePdfDoc::WriteItem(int itmIdx)
{
    if (m_estimate == NULL)
        return;

    if (itmIdx >= m_estimate->GetItemsCount())
        return;

    wxString sDescr = m_estimate->GetItemDescription(itmIdx);
    wxString sComm = m_estimate->GetItemComments(itmIdx);
    int iQty = m_estimate->GetItemQuantity(itmIdx);
    double dPrice = m_estimate->GetItemUnitPrice(itmIdx);

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
        Cell(25, iHght, wxString::Format(_("%.02f$"), dPrice), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_RIGHT);
        Cell(15, iHght, wxString::Format(_T("%0d"), iQty), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_CENTER);
        Cell(25, iHght, wxString::Format(_("%.02f$"), dPrice*iQty), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_RIGHT);
    }
    SetXY(10, iY+iHght);
}

void EstimatePdfDoc::Finalize()
{
    if (m_estimate == NULL)
        return;
    // Calculate the height needed
    int iHght=4 + 6; // Top margin + Total height
    if (m_estimate->GetDiscount() != 0.)
        iHght += 12; // 2 more lines if discount enabled
    if (m_estimate->GetVAT() != 0.)
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

    double dTotal = m_estimate->GetInitialPrice();
    wxString sTotal, sPrice = wxString::Format(_("%.02f$"), dTotal);
    if ((m_estimate->GetDiscount()==0.) && (m_estimate->GetVAT()==0.))
    {
        sTotal = _("Total to pay:");
    }
    else
    {
        if (m_estimate->GetDiscount()!=0.)
        {
            sTotal = _("Sub-total:");
            sTotal << _T("\n") << wxString::Format(_("Discount (%0.2f%%)"), m_estimate->GetDiscount());
            double dDisc=dTotal*(m_estimate->GetDiscount()/100.);
            sPrice << _T("\n") << wxString::Format(_("%0.2f$"), dDisc);
            dTotal -= dDisc;
            sPrice << _T("\n") << wxString::Format(_("%0.2f$"), dTotal);
            if (m_estimate->GetVAT()!=0.)
            {
                sTotal << _T("\n") << _("Total w/o VAT");
                sTotal << _T("\n") << wxString::Format(_("VAT (%0.1f%%)"), m_estimate->GetVAT());
                double dVAT=dTotal*m_estimate->GetVAT()/100.;
                sPrice << _T("\n") << wxString::Format(_("%0.2f$"), dVAT);
                dTotal += dVAT;
                sTotal << _T("\n") << _("Total to pay:");
                sPrice << _T("\n") << wxString::Format(_("%0.2f$"), dTotal);
            }
            else
            {
                sTotal << _T("\n") << _("Total w/o VAT to pay:");
            }
        }
        else
        {
            sTotal = _("Total w/o VAT:");
            sTotal << _T("\n") << wxString::Format(_("VAT (%0.1f%%)"), m_estimate->GetVAT());
            double dVAT=dTotal*m_estimate->GetVAT()/100.;
            sPrice << _T("\n") << wxString::Format(_("%0.2f$"), dVAT);
            dTotal += dVAT;
            sTotal << _T("\n") << _("Total to pay:");
            sPrice << _T("\n") << wxString::Format(_("%0.2f$"), dTotal);
        }
    }
    SetFont(_T("Helvetica"), _T("B"), 12);
    SetXY(10, iY+4);
    MultiCell(165, 6, sTotal, wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    SetFont(_T("Courier"), _T("B"), 12);
    SetXY(175, iY+4);
    MultiCell(25, 6, sPrice, wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    if (m_estimate->GetVAT()==0.)
    {
        iY=GetY();
        SetXY(10, iY);
        SetFont(_T("Helvetica"), _T(""), 10);
        // TODO (Xaviou#1#): Replace the following with a user-customized text
        MultiCell(190, 6, _("V.A.T. non-applicable"), wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    }
}

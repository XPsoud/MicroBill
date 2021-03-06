#include "estimatepdfdoc.h"

#include "estimate.h"
#include "datasmanager.h"

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
    SetFont(_T("Helvetica"), _T(""), 12);
    int y=GetY(), iNbLines=0;
    SetY(y+2);
    if (m_estimate!=NULL)
    {
        wxString sTmp;
        if (m_options.HasCompanyLocation())
        {
            sTmp=wxString::Format(_("Edited in %s on %s"), m_options.GetCompanyLocation(), m_estimate->GetCreationDate().Format(_("%Y/%m/%d")));
        }
        else
        {
            sTmp=wxString::Format(_("Edited on %s"), m_estimate->GetCreationDate().Format(_("%Y/%m/%d")));
        }
        if (m_estimate->GetTermDate().IsValid())
        {
            sTmp << _T("\n") << _("Estimate valid until:") << m_estimate->GetTermDate().Format(_("%Y/%m/%d"));
        }
        MultiCell(95, 5, sTmp);
        Client *c=DatasManager::Get().GetClient(m_estimate->GetClient());
        if (c!=NULL)
        {
            wxString sClient=c->GetAttribute(_T("Company"));
            if (!sClient.IsEmpty()) sClient << _T(" - ");
            sClient << c->GetAttribute(_T("LastName")) << _T(" ") << c->GetAttribute(_T("FirstName"));
            iNbLines++;
            if (!c->GetAttribute(_T("Addr1")).IsEmpty())
            {
                sClient << _T("\n") << c->GetAttribute(_T("Addr1"));
                iNbLines++;
            }
            if (!c->GetAttribute(_T("Addr2")).IsEmpty())
            {
                sClient << _T("\n") << c->GetAttribute(_T("Addr2"));
                iNbLines++;
            }
            sClient << _T("\n") << c->GetAttribute(_T("ZipCode")) << _T(" ") << c->GetAttribute(_T("City"));
            iNbLines++;
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
        sTmp << _T("\n") << _("Estimate valid until: _ _ _ _ / _ _ / _ _");
        MultiCell(95, 8, sTmp);
        SetXY(105, y);
        MultiCell(95, 6, _T(" \n \n \n "), wxPDF_BORDER_FRAME);
        y+=24;
    }
    Line(10, y, 200, y);
    SetXY(10, y);
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
        Cell(25, iHght, m_options.GetFormatedMoneyValue(dPrice, _T("%.02f")), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_RIGHT);
        Cell(15, iHght, wxString::Format(_T("%0d"), iQty), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_CENTER);
        Cell(25, iHght, m_options.GetFormatedMoneyValue(dPrice*iQty, _T("%.02f")), wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_RIGHT);
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
    wxString sTotal, sPrice = m_options.GetFormatedMoneyValue(dTotal, _T("%.02f"));
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
            sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dDisc, _T("%0.2f"));
            dTotal -= dDisc;
            sPrice << _T("\n") << m_options.GetFormatedMoneyValue(dTotal, _T("%0.2f"));
            if (m_estimate->GetVAT()!=0.)
            {
                sTotal << _T("\n") << _("Total w/o VAT");
                sTotal << _T("\n") << wxString::Format(_("VAT (%0.1f%%)"), m_estimate->GetVAT());
                double dVAT=dTotal*m_estimate->GetVAT()/100.;
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
            sTotal << _T("\n") << wxString::Format(_("VAT (%0.1f%%)"), m_estimate->GetVAT());
            double dVAT=dTotal*m_estimate->GetVAT()/100.;
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
    if (m_estimate->GetVAT()==0.)
    {
        iY=GetY();
        SetXY(10, iY);
        SetFont(_T("Helvetica"), _T(""), 10);
        // TODO (Xaviou#1#): Replace the following with a user-customized text
        MultiCell(190, 6, _("V.A.T. non-applicable"), wxPDF_BORDER_NONE, wxPDF_ALIGN_RIGHT);
    }
}

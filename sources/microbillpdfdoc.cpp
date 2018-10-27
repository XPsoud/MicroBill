#include "microbillpdfdoc.h"

#include "main.h"

MicroBillPdfDoc::MicroBillPdfDoc() : wxPdfDocument(wxPORTRAIT, _T("mm"), wxPAPER_A4),
    m_options(SettingsManager::Get())
{
    // Constructor
}

MicroBillPdfDoc::~MicroBillPdfDoc()
{
    // Destructor
}

void MicroBillPdfDoc::DoCreateDocument()
{
    AliasNbPages();
    AddPage();

    WriteItems();

    Finalize();
}

void MicroBillPdfDoc::Header()
{
    // Backup current text colour
    wxPdfColour curTxtCol=GetTextColour();

    // Border all around the page
    wxPdfArrayDouble dash;
    wxPdfLineStyle lstyle(0.5, wxPDF_LINECAP_BUTT, wxPDF_LINEJOIN_MITER, dash, 0., wxColour(0, 0, 0));
    SetLineStyle(lstyle);
    Rect(10, 10, 190, 274, wxPDF_STYLE_DRAW);

    // Header image
    wxImage imgHeader = m_options.GetHeaderImage();
    Image(_T("Logo"), imgHeader, 11, 11, 188, 33);

    Line(10, 45, 200, 45);

    SetXY(10, 45);
    if (PageNo()==1) WriteTitle();

    SetFont(_T("Helvetica"), _T("B"), 12);
    Cell(125, 8, _("Description"), wxPDF_BORDER_FRAME, 0, wxPDF_ALIGN_CENTER);
    Cell(25, 8, _("Unit Price"), wxPDF_BORDER_FRAME, 0, wxPDF_ALIGN_CENTER);
    Cell(15, 8, _("Qty"), wxPDF_BORDER_FRAME, 0, wxPDF_ALIGN_CENTER);
    Cell(25, 8, _("Total"), wxPDF_BORDER_FRAME, 2, wxPDF_ALIGN_CENTER);

    // Restore text colour
    SetTextColour(curTxtCol);
}

void MicroBillPdfDoc::Footer()
{
    // Backup current text colour
    wxPdfColour curTxtCol=GetTextColour();

    // Created with MicroBill
    wxString sMBill = _T("\u00B5Bill (v");
    sMBill << wxGetApp().GetVersionNumString(false) << _T(")");
    SetXY(180, -12);
    SetFont(_T("Helvetica"), _T("I"), 8);
    SetTextColour(*wxBLUE);
    Cell(20, 4, wxString::Format(_("Created with %s"), sMBill) , wxPDF_BORDER_NONE, 2, wxPDF_ALIGN_RIGHT);

    // Page number
    SetXY(180, -8);
    SetFont(_T("Helvetica"), _T(""), 8);
    SetTextColour(*wxBLACK);
    wxString sPage=wxString::Format(_("Page %d on {nb}"), PageNo());
    Cell(20, 4, sPage , wxPDF_BORDER_NONE, 2, wxPDF_ALIGN_RIGHT);

    // Custom footer text
    wxString sFooter = m_options.GetFooterText(), sStyle = m_options.GetFooterStyle();
    if (!sFooter.IsEmpty())
    {
        unsigned char r, g, b;
        long lValue;
        sStyle.Mid(0, 3).ToLong(&lValue); r=(unsigned char)lValue;
        sStyle.Mid(3, 3).ToLong(&lValue); g=(unsigned char)lValue;;
        sStyle.Mid(6, 3).ToLong(&lValue); b=(unsigned char)lValue;
        sStyle.Mid(9, 2).ToLong(&lValue);
        if (sStyle.Len()>11) // Extra style (Bold, Italic) ?
            sStyle = sStyle.Mid(10);
        else
            sStyle = _T("");

        SetFont(_T("Helvetica"), sStyle, lValue);
        SetTextColour(wxColour(r, g, b));
        SetXY(10, -12);
        Cell(170, 10, sFooter);
    }

    // Restore text colour
    SetTextColour(curTxtCol);
}

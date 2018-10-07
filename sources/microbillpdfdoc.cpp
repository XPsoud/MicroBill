#include "microbillpdfdoc.h"

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

    // Bordezr all around the page
    wxPdfArrayDouble dash;
    wxPdfLineStyle lstyle(0.5, wxPDF_LINECAP_BUTT, wxPDF_LINEJOIN_MITER, dash, 0., wxColour(0, 0, 0));
    SetLineStyle(lstyle);
    Rect(10, 10, 190, 274, wxPDF_STYLE_DRAW);

    // Header image
// TODO (Xaviou#1#): Get the image using the settings manager or datas manager and write it on the document

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

    // Page number
    SetXY(180, -8);
    SetFont(_T("Helvetica"), _T(""), 8);
    SetTextColour(*wxBLACK);
    wxString sPage=wxString::Format(_("Page %d on {nb}"), PageNo());
    Cell(20, 4, sPage , wxPDF_BORDER_NONE, 2, wxPDF_ALIGN_RIGHT);

    // Restore text colour
    SetTextColour(curTxtCol);
}

#ifndef __ESTIMITEMCTRL_H_INCLUDED__
#define __ESTIMITEMCTRL_H_INCLUDED__

#include <wx/wx.h>

class EstimItemCtrl : public wxPanel
{
    public:
        EstimItemCtrl(wxWindow *parent);
        virtual ~EstimItemCtrl();
        void SetDatas(const wxString& descr, const wxString& comm, double unitPrice, int quantity);
        void SetItemIndex(int index) { m_iIndex=index; };
        int GetItemIndex() { return m_iIndex; }
        void Update();
        static const wxFont& GetFixedFont() { return m_fntFixed; }
        static const wxFont& GetBoldFont() { return m_fntDescr; }
        const wxString& GetDescription() { return m_sDescr; }
        const wxString& GetComments() { return m_sComments; }
        const double& GetUnitPrice() { return m_dUnitPrice; }
        const int& GetQuantity() { return m_iQuantity; }
    protected:
    private:
        // Misc functions
        void CreateControls();
        void ConnectControls();
        // Events handlers
        void OnMouseEnter(wxMouseEvent &event);
        void OnMouseLeave(wxMouseEvent &event);
        // Controls vars
        wxButton *m_btnEdit, *m_btnDelete, *m_btnMoveUp, *m_btnMoveDown;
        wxStaticText *m_lblDescr, *m_lblComments, *m_lblPrice;
        // Item and Misc vars
        static wxFont m_fntDescr, m_fntFixed;
        static wxColour m_colHiglight, m_colNormal;
        wxString m_sDescr, m_sComments;
        double m_dUnitPrice;
        int m_iQuantity, m_iIndex;
        bool m_bBackgroundChanged;
};

#endif // __ESTIMITEMCTRL_H_INCLUDED__

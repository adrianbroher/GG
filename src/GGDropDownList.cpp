/* GG is a GUI for SDL and OpenGL.
   Copyright (C) 2003 T. Zachary Laine

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1
   of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
    
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA

   If you do not wish to comply with the terms of the LGPL please
   contact the author as other terms are available for a fee.
    
   Zach Laine
   whatwasthataddress@hotmail.com */

/* $Header$ */

#include "GGDropDownList.h"

#include <GGScroll.h>
#include <GGApp.h>
#include <GGDrawUtil.h>
#include <XMLValidators.h>

using namespace GG;

namespace {
    const int BORDER_THICK = 2; // should be the same as the BORDER_THICK value in GGListBox.h

    class ModalListPicker : public Wnd
    {
    public:
        ModalListPicker(DropDownList* drop_wnd, ListBox* lb_wnd) :
            Wnd(0, 0, GG::App::GetApp()->AppWidth() - 1, GG::App::GetApp()->AppHeight() - 1, CLICKABLE | MODAL),
            m_drop_wnd(drop_wnd),
            m_lb_wnd(lb_wnd),
            m_old_lb_ul(m_lb_wnd->UpperLeft())
        {
            Connect(m_lb_wnd->SelChangedSignal(), &ModalListPicker::LBSelChangedSlot, this);
            Connect(m_lb_wnd->LeftClickedSignal(), &ModalListPicker::LBLeftClickSlot, this);
            m_lb_wnd->OffsetMove(m_drop_wnd->Parent() ? m_drop_wnd->Parent()->UpperLeft() : Pt());
            AttachChild(m_lb_wnd);
        }
        ~ModalListPicker() {m_lb_wnd->MoveTo(m_old_lb_ul); DetachChild(m_lb_wnd);}

    protected:
        virtual void LClick(const Pt& pt, Uint32 keys) {m_done = true;}

    private:
        void LBSelChangedSlot(const set<int>& rows)
        {
            if (!rows.empty()) {
                m_drop_wnd->Select(*rows.begin());
                m_done = true;
            }
        }
        void LBLeftClickSlot(int, const ListBox::Row*, const Pt&)
        {
            m_done = true;
        }

        DropDownList*  m_drop_wnd;
        ListBox*       m_lb_wnd;
        Pt             m_old_lb_ul;
    };
}

DropDownList::DropDownList(int x, int y, int w, int row_ht, int drop_ht, Clr color, ListBox* lb/* = 0*/,
                           Uint32 flags/* = CLICKABLE*/) :
    Control(x, y, w, row_ht, flags),
    m_current_item_idx(-1),
    m_LB(lb)
{
    if (!m_LB)
        m_LB = new ListBox(x, y, w, drop_ht, color, color, flags);
    m_LB->SetRowHeight(row_ht);
    SetStyle(LB_SINGLESEL);
    // adjust size to keep correct height based on row height, etc.
    Resize(Size().x, row_ht + 2 * m_LB->CellMargin() + 2 * BORDER_THICK);
    m_LB->MoveTo(x, y + Height());
}

DropDownList::DropDownList(int x, int y, int w, int row_ht, int drop_ht, Clr color, Clr interior,
                           ListBox* lb/* = 0*/, Uint32 flags/* = CLICKABLE*/) :
    Control(x, y, w, row_ht, flags),
    m_current_item_idx(-1),
    m_LB(lb)
{
    if (!m_LB)
        m_LB = new ListBox(x, y, w, drop_ht, color, interior, flags);
    m_LB->SetRowHeight(row_ht);
    SetStyle(LB_SINGLESEL);
    // adjust size to keep correct height based on row height, etc.
    Resize(Size().x, row_ht + 2 * m_LB->CellMargin() + 2 * BORDER_THICK);
    m_LB->MoveTo(x, y + Height());
}

DropDownList::DropDownList(const XMLElement& elem) :
    Control(elem.Child("GG::Control"))
{
    if (elem.Tag() != "GG::DropDownList")
        throw std::invalid_argument("Attempted to construct a GG::DropDownList from an XMLElement that had a tag other than \"GG::DropDownList\"");

    m_current_item_idx = lexical_cast<int>(elem.Child("m_current_item_idx").Text());

    if (ListBox* lb = dynamic_cast<ListBox*>(App::GetApp()->GenerateWnd(elem.Child("m_LB").Child(0)))) {
        m_LB = lb;
    } else {
        throw std::runtime_error("DropDownList::DropDownList : Attempted to use a non-ListBox object as the drop-down list.");
    }
}

DropDownList::~DropDownList()
{
    delete m_LB;
}

const DropDownList::Row* DropDownList::CurrentItem() const
{
    return m_current_item_idx == -1 ? 0 : &m_LB->GetRow(m_current_item_idx);
}

bool DropDownList::Render()
{
    // draw beveled rectangle around client area
    Pt ul = UpperLeft(), lr = LowerRight();
    Clr color_to_use = Disabled() ? DisabledColor(m_LB->Color()) : m_LB->Color();
    Clr int_color_to_use = Disabled() ? DisabledColor(m_LB->m_int_color) : m_LB->m_int_color;

    BeveledRectangle(ul.x, ul.y, lr.x, lr.y, int_color_to_use, color_to_use, false, BORDER_THICK);

    // draw ListBox::Row of currently displayed item, if any
    const Row* current_item = CurrentItem();
    if (current_item) {
        // clip row to viewable area, and save old scissor state
        glPushAttrib(GL_SCISSOR_BIT);
        glEnable(GL_SCISSOR_TEST);
        glScissor(ul.x + BORDER_THICK, App::GetApp()->AppHeight() - lr.y + BORDER_THICK,
                  lr.x - ul.x - 2 * BORDER_THICK,
                  lr.y - ul.y - 2 * BORDER_THICK);

        m_LB->RenderRow(current_item, ul.x + BORDER_THICK, ul.y + BORDER_THICK, m_LB->LastVisibleCol());

        // restore previous scissor-clipping state
        glPopAttrib();
    }

    return true;
}

void DropDownList::LClick(const Pt& pt, Uint32 keys)
{
    ModalListPicker picker(this, m_LB);
    const set<int>& LB_sels = m_LB->Selections();
    if (!LB_sels.empty()) {
        if (m_LB->m_vscroll)
            m_LB->m_vscroll->ScrollTo(*LB_sels.begin() * m_LB->RowHeight());
    }
    m_LB->m_first_col_shown = 0;
    picker.Run();
}

void DropDownList::Keypress(Key key, Uint32 key_mods)
{
    switch (key) {
    case GGK_UP: // arrow-up (not numpad arrow)
        if (1 <= m_current_item_idx)
            Select(std::max(0, m_current_item_idx - 1));
        break;
    case GGK_DOWN: // arrow-down (not numpad arrow)
        if (m_current_item_idx < m_LB->NumRows())
            Select(std::min(m_current_item_idx + 1, m_LB->NumRows() - 1));
        break;
    case GGK_PAGEUP: // page up key (not numpad key)
        if (m_LB->NumRows())
            Select(std::max(0, m_current_item_idx - 10));
        break;
    case GGK_PAGEDOWN: // page down key (not numpad key)
        if (m_LB->NumRows())
            Select(std::min(m_current_item_idx + 10, m_LB->NumRows() - 1));
        break;
    case GGK_HOME: // home key (not numpad)
        if (m_LB->NumRows())
            Select(0);
        break;
    case GGK_END: // end key (not numpad)
        if (m_LB->NumRows())
            Select(m_LB->NumRows() - 1);
        break;
    default: // any other key gets passed along to the parent
        if (Parent())
            Parent()->Keypress(key, key_mods);
    }
}

void DropDownList::SizeMove(int x1, int y1, int x2, int y2)
{
    // adjust size to keep correct height based on row height, etc.
    Wnd::SizeMove(x1, y1, x2, y1 + m_LB->RowHeight() + 2 * m_LB->CellMargin() + 2 * BORDER_THICK);
    m_LB->Resize(Width(), m_LB->Height());
    m_LB->MoveTo(x1, y1 + Height());
}

int DropDownList::Insert(Row* row, int at/* = -1*/)
{
    row->height = m_LB->RowHeight();
    row->sub_rows.clear();
    return m_LB->Insert(row, at);
}

void DropDownList::Delete(int idx)
{
    if (idx == m_current_item_idx)
        m_current_item_idx = -1;
    else if (idx < m_current_item_idx)
        --m_current_item_idx;

    m_LB->Delete(idx);
}

void DropDownList::Clear()
{
    m_current_item_idx = -1;
    m_LB->Clear();
}

void DropDownList::Select(int row)
{
    int old_m_current_item_idx = m_current_item_idx;
    if (row < -1 || m_LB->NumRows() <= row)
        m_current_item_idx = -1;
    else
        m_current_item_idx = row;

    if (m_current_item_idx != old_m_current_item_idx)
        m_sel_changed_sig(m_current_item_idx);
}

void DropDownList::SetStyle(Uint32 s)
{
    s &= ~(LB_NOSEL | LB_QUICKSEL | LB_DRAGDROP | LB_USERDELETE | LB_BROWSEUPDATES);
    s |= LB_SINGLESEL;
    m_LB->SetStyle(s);
    m_current_item_idx = -1;
}

void DropDownList::SetSortCol(int n)
{
    m_LB->SetSortCol(n);
    m_current_item_idx = -1;
}

XMLElement DropDownList::XMLEncode() const
{
    XMLElement retval("GG::DropDownList");
    retval.AppendChild(Control::XMLEncode());
    retval.AppendChild(XMLElement("m_current_item_idx", lexical_cast<string>(m_current_item_idx)));
    retval.AppendChild(XMLElement("m_LB", m_LB->XMLEncode()));
    return retval;
}

XMLElementValidator DropDownList::XMLValidator() const
{
    XMLElementValidator retval("GG::DropDownList");
    retval.AppendChild(Control::XMLValidator());
    retval.AppendChild(XMLElementValidator("m_current_item_idx", new Validator<int>()));
    retval.AppendChild(XMLElementValidator("m_LB", m_LB->XMLValidator()));
    return retval;
}

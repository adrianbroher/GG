/* GG is a GUI for SDL and OpenGL.
   Copyright (C) 2003-2008 T. Zachary Laine

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
   whatwasthataddress@gmail.com */

#include <GG/GroupBox.h>

#include <GG/GUI.h>
#include <GG/DrawUtil.h>
#include <GG/StyleFactory.h>
#include <GG/TextControl.h>


using namespace GG;

namespace {
    Y TopOfFrame(bool label, const boost::shared_ptr<Font>& font)
    { return label ? font->Lineskip() : font->Lineskip() / 2 - 1; }
}

////////////////////////////////////////////////
// GG::GroupBox
////////////////////////////////////////////////
// static(s)
const int GroupBox::FRAME_THICK = 2;
const int GroupBox::PIXEL_MARGIN = 4;

GroupBox::GroupBox() :
    m_label(0)
{}

GroupBox::GroupBox(X x, Y y, X w, Y h, const std::string& label, const boost::shared_ptr<Font>& font,
                   Clr color, Clr text_color/* = CLR_BLACK*/, Clr interior/* = CLR_ZERO*/,
                   Flags<WndFlag> flags/* = Flags<WndFlag>()*/) :
    m_color(color),
    m_text_color(text_color),
    m_int_color(interior),
    m_font(font),
    m_label(label.empty() ? 0 : GUI::GetGUI()->GetStyleFactory()->NewTextControl(X0, -m_font->Lineskip(),
                                                                                 X1, m_font->Lineskip(),
                                                                                 label, m_font, m_text_color, FORMAT_LEFT | FORMAT_TOP))
{ AttachChild(m_label); }

Pt GroupBox::ClientUpperLeft() const
{ return UpperLeft() + Pt(X(FRAME_THICK + PIXEL_MARGIN), Y(FRAME_THICK + PIXEL_MARGIN) + TopOfFrame(m_label, m_font)); }

Pt GroupBox::ClientLowerRight() const
{ return LowerRight() - Pt(X(FRAME_THICK + PIXEL_MARGIN), Y(FRAME_THICK + PIXEL_MARGIN)); }

void GroupBox::Render()
{
    Pt ul = UpperLeft(), lr = LowerRight() - Pt(X1, Y1);
    ul.y += TopOfFrame(m_label, m_font);
    Clr light = LightColor(m_color);
    Clr dark = DarkColor(m_color);
    const int GAP_FROM_TEXT = 2;
    int vertices[24] = {
        Value(ul.x) + FRAME_THICK + PIXEL_MARGIN - GAP_FROM_TEXT, Value(ul.y),
        Value(ul.x), Value(ul.y),
        Value(ul.x), Value(lr.y),
        Value(lr.x), Value(lr.y),
        Value(lr.x), Value(ul.y),
        vertices[0], Value(ul.y)
    };
    if (m_label) {
        vertices[0] = Value(m_label->TextUpperLeft().x - GAP_FROM_TEXT);
        vertices[10] = Value(m_label->TextLowerRight().x + GAP_FROM_TEXT);
    }

    for (std::size_t i = 0; i < 12; i += 2) {
        vertices[12 + i + 0] = vertices[i + 0] + 1;
        vertices[12 + i + 1] = vertices[i + 1] + 1;
    }
    --vertices[12];
    --vertices[22];

    glDisable(GL_TEXTURE_2D);

    glColor(light);
    glBegin(GL_LINE_STRIP);
    for (std::size_t i = 12; i < 24; i += 2) {
        glVertex2i(vertices[i + 0], vertices[i + 1]);
    }
    glEnd();

    glColor(dark);
    glBegin(GL_LINE_STRIP);
    for (std::size_t i = 0; i < 12; i += 2) {
        glVertex2i(vertices[i + 0], vertices[i + 1]);
    }
    glEnd();

    glColor(m_int_color);
    glBegin(GL_QUADS);
    glVertex2i(vertices[14] + 1, vertices[5] - 1);
    glVertex2i(vertices[14] + 1, vertices[13] + 1);
    glVertex2i(vertices[6] - 1,  vertices[13] + 1);
    glVertex2i(vertices[6] - 1,  vertices[5] - 1);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

void GroupBox::SetColor(Clr c)
{ m_color = c; }

void GroupBox::SetTextColor(Clr c)
{ m_text_color = c; }

void GroupBox::SetInteriorColor(Clr c)
{ m_int_color = c; }

void GroupBox::SetText(const std::string& str)
{
    delete m_label;

    if (!str.empty()) {
        m_label = GUI::GetGUI()->GetStyleFactory()->NewTextControl(X(FRAME_THICK + PIXEL_MARGIN), Y0,
                                                                   X1, m_font->Lineskip(),
                                                                   str, m_font, m_text_color);
    }
}
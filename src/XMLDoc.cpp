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

/** This notice came from the original file from which all the XML parsing code 
    was taken, part of the spirit distribution.  The code was modified slightly 
    by me, and doesn't contain all the original code.  Thanks to Daniel Nuffer 
    for his great work. */
/*=============================================================================
    simplexml.cpp

    Spirit V1.3
    URL: http://spirit.sourceforge.net/

    Copyright (c) 2001, Daniel C. Nuffer

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the copyright holder be held liable for
    any damages arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute
    it freely, subject to the following restrictions:

    1.  The origin of this software must not be misrepresented; you must
        not claim that you wrote the original software. If you use this
        software in a product, an acknowledgment in the product documentation
        would be appreciated but is not required.

    2.  Altered source versions must be plainly marked as such, and must
        not be misrepresented as being the original software.

    3.  This notice may not be removed or altered from any source
        distribution.
=============================================================================*/


#include "XMLDoc.h"

#include <XMLValidators.h>

#include <boost/spirit.hpp>

#include <stdexcept>


#define DEBUG_OUTPUT 0

namespace GG {

namespace {
const string INDENT_STR = "  "; // indents are 2 spaces
string element_name;
string attribute_name;

using namespace boost::spirit;

typedef chset<unsigned char> chset_t;
typedef chlit<unsigned char> chlit_t;

// XML grammar rules
rule<> document, prolog, element, Misc, Reference,CData, doctypedecl, 
    XMLDecl, SDDecl, VersionInfo, EncodingDecl, VersionNum, Eq, 
    EmptyElemTag, STag, content, ETag, Attribute, AttValue, CharData, 
    Comment, CDSect, CharRef, EntityRef, EncName, Name, Comment1, S;

// XML Character classes
chset_t Char("\x9\xA\xD\x20-\xFF");
chset_t Letter("\x41-\x5A\x61-\x7A\xC0-\xD6\xD8-\xF6\xF8-\xFF");
chset_t Digit("0-9");
chset_t Extender('\xB7');
chset_t NameChar = Letter | Digit | chset_t("._:-") | Extender;
chset_t Sch("\x20\x9\xD\xA");
}

////////////////////////////////////////////////
// GG::XMLElement
////////////////////////////////////////////////
XMLElement::XMLElement(const string& t, bool r) :
    m_tag(t),
    m_root(r)
{
}

const string& XMLElement::Tag() const
{
    return m_tag;
}

const string& XMLElement::Text() const
{
    return m_text;
}

int XMLElement::NumChildren() const
{
    return m_children.size();
}

int XMLElement::NumAttributes() const
{
    return m_attributes.size();
}

bool XMLElement::ContainsChild(const string& child) const
{
    return ChildIndex(child) != -1;
}

bool XMLElement::ContainsAttribute(const string& attrib) const
{
    return m_attributes.find(attrib) != m_attributes.end();
}

int XMLElement::ChildIndex(const string& child) const
{
    int retval = -1;
    for (unsigned int i = 0; i < m_children.size(); ++i) {
        if (m_children[i].m_tag == child) {
            retval = i;
            break;
        }
    }
    return retval;
}

const XMLElement& XMLElement::Child(unsigned int idx) const
{
    return m_children.at(idx);
}

const XMLElement& XMLElement::Child(const string& child) const
{
    unsigned int i = 0;
    for (; i < m_children.size(); ++i) {
        if (m_children[i].m_tag == child)
            break;
    }

    if (i == m_children.size())
        throw std::range_error("XMLElement::Child(): This XMLElement contains no child \"" + child + "\".");

    return m_children[i];
}

const XMLElement& XMLElement::LastChild() const
{
    if (m_children.empty())
        throw std::range_error("XMLElement::LastChild(): LastChild() was called on an empty XMLElement.");

    return m_children.back();
}

const string& XMLElement::Attribute(const string& attrib) const
{
    static const string empty_str("");
    map<string, string>::const_iterator it = m_attributes.find(attrib);
    if (it != m_attributes.end())
        return it->second;
    else
        return empty_str;
}

ostream& XMLElement::WriteElement(ostream& os, int indent/* = 0*/, bool whitespace/* = true*/) const
{
    if (whitespace)
        for (int i = 0; i < indent; ++i)
            os << INDENT_STR;
    os << '<' << m_tag;
    for (std::map<string, string>::const_iterator it = m_attributes.begin(); it != m_attributes.end(); ++it)
        os << ' ' << it->first << "=\"" << it->second << "\"";
    if (m_children.empty() && m_text.empty() && !m_root) {
        os << "/>";
        if (whitespace)
            os << "\n";
    } else {
        os << ">";
        if (!m_text.empty() && m_text.find_first_of("<&") != std::string::npos) {
            os << "<![CDATA[" << m_text << "]]>";
        } else {
            os << m_text;
        }
        if (whitespace && !m_children.empty())
            os << "\n";
        for (unsigned int i = 0; i < m_children.size(); ++i)
            m_children[i].WriteElement(os, indent + 1, whitespace);
        if (whitespace && !m_children.empty()) {
            for (int i = 0; i < indent; ++i) {
                os << INDENT_STR;
            }
        }
        os << "</" << m_tag << ">";
        if (whitespace) os << "\n";
    }
    return os;
}

XMLElement::const_child_iterator XMLElement::child_begin() const
{
    return m_children.begin();
}

XMLElement::const_child_iterator XMLElement::child_end() const
{
    return m_children.end();
}

XMLElement::const_attr_iterator XMLElement::attr_begin() const
{
    return m_attributes.begin();
}

XMLElement::const_attr_iterator XMLElement::attr_end() const
{
    return m_attributes.end();
}

XMLElement& XMLElement::Child(unsigned int idx)
{
    return m_children.at(idx);
}

XMLElement& XMLElement::Child(const string& child)
{
    unsigned int i = 0;
    for (; i < m_children.size(); ++i) {
        if (m_children[i].m_tag == child)
            break;
    }

    if (i == m_children.size())
        throw std::range_error("XMLElement::Child(): This XMLElement contains no child \"" + child + "\".");

    return m_children[i];
}

XMLElement& XMLElement::LastChild()
{
    if (m_children.empty())
        throw std::range_error("XMLElement::LastChild(): LastChild() was called on an empty XMLElement.");

    return m_children.back();
}

void XMLElement::SetAttribute(const string& attrib, const string& val)
{
    m_attributes[attrib] = val;
}

void XMLElement::SetTag(const string& tag)
{
    m_tag = tag;
}

void XMLElement::SetText(const string& text)
{
    m_text = text;
}

void XMLElement::RemoveAttribute(const string& attrib)
{
    m_attributes.erase(attrib);
}

void XMLElement::RemoveAttributes()
{
    m_attributes.clear();
}

void XMLElement::AppendChild(const XMLElement& e)
{
    m_children.push_back(e);
}

void XMLElement::AppendChild(const string& child)
{
    m_children.push_back(XMLElement(child));
}

void XMLElement::AddChildBefore(const XMLElement& e, unsigned int idx)
{
    if (m_children.size() <= idx)
        throw std::range_error("XMLElement::AddChildBefore(): Index " + lexical_cast<string>(idx) + " is out of range.");

    m_children.insert(m_children.begin() + idx, e);
}

void XMLElement::RemoveChild(unsigned int idx)
{
    if (m_children.size() <= idx)
        throw std::range_error("XMLElement::RemoveChild(): Index " + lexical_cast<string>(idx) + " is out of range.");

    m_children.erase(m_children.begin() + idx);
}

void XMLElement::RemoveChild(const string& child)
{
    int idx = ChildIndex(child);

    if (idx == -1)
        throw std::range_error("XMLElement::RemoveChild(): This XMLElement contains no child \"" + child + "\".");

    m_children.erase(m_children.begin() + idx);
}

void XMLElement::RemoveChildren()
{
    m_children.clear();
}

XMLElement::child_iterator XMLElement::child_begin()
{
    return m_children.begin();
}

XMLElement::child_iterator XMLElement::child_end()
{
    return m_children.end();
}

XMLElement::attr_iterator XMLElement::attr_begin()
{
    return m_attributes.begin();
}

XMLElement::attr_iterator XMLElement::attr_end()
{
    return m_attributes.end();
}


////////////////////////////////////////////////
// GG::XMLDoc
////////////////////////////////////////////////
// static(s)
XMLDoc*              XMLDoc::s_curr_parsing_doc = 0;
vector<XMLElement*>  XMLDoc::s_element_stack;
XMLDoc::RuleDefiner  XMLDoc::s_rule_definer;
XMLElement           XMLDoc::s_temp_elem;
string               XMLDoc::s_temp_attr_name;

XMLDoc::XMLDoc(const string& root_tag/*= "GG::XMLDoc"*/) :
    root_node(XMLElement(root_tag, true))
{
}

XMLDoc::XMLDoc(const istream& is) :
    root_node(XMLElement())
{
}

ostream& XMLDoc::WriteDoc(ostream& os, bool whitespace/* = true*/) const
{
    os << "<?xml version=\"1.0\"?>";
    if (whitespace) os << "\n";
    return root_node.WriteElement(os, 0, whitespace);
}

istream& XMLDoc::ReadDoc(istream& is)
{
    root_node = XMLElement(); // clear doc contents
    s_element_stack.clear();  // clear this to start a fresh read
    s_curr_parsing_doc = this;  // indicate where to add elements
    string parse_str;
    string temp_str;
    while (is) {
        getline(is, temp_str);
        parse_str += temp_str + '\n';
    }
    parse(parse_str.c_str(), document);
    s_curr_parsing_doc = 0;
    return is;
}

void XMLDoc::SetElemName(const char* first, const char* last)
{
    s_temp_elem = XMLElement(string(first, last));
}

void XMLDoc::SetAttributeName(const char* first, const char* last)
{
    s_temp_attr_name = string(first, last);
}

void XMLDoc::AddAttribute(const char* first, const char* last)
{
    s_temp_elem.SetAttribute(s_temp_attr_name, string(first, last));
}

void XMLDoc::PushElem1(const char* first, const char* last)
{
    if (XMLDoc* this_ = XMLDoc::s_curr_parsing_doc) {
        if (s_element_stack.empty()) {
            this_->root_node = s_temp_elem;
            s_element_stack.push_back(&this_->root_node);
        } else {
            s_element_stack.back()->AppendChild(s_temp_elem);
            s_element_stack.push_back(&s_element_stack.back()->LastChild());
        }
    }
}

void XMLDoc::PushElem2(const char* first, const char* last)
{
    if (XMLDoc* this_ = XMLDoc::s_curr_parsing_doc) {
        if (s_element_stack.empty()) {
            this_->root_node = s_temp_elem;
        } else {
            s_element_stack.back()->AppendChild(s_temp_elem);
        }
    }
}

void XMLDoc::PopElem(const char*, const char*)
{
    if (!s_element_stack.empty())
        s_element_stack.pop_back();
}

void XMLDoc::AppendToText(const char* first, const char* last)
{
    if (!s_element_stack.empty()) {
        string text(first, last);
        unsigned int first_good_posn = (text[0] != '\"') ? 0 : 1;
        unsigned int last_good_posn = text.find_last_not_of(" \t\n\"\r\f");
        // strip of leading quote and/or trailing quote, and/or trailing whitespace
        if (last_good_posn != string::npos)
            s_element_stack.back()->m_text += text.substr(first_good_posn, (last_good_posn + 1) - first_good_posn);
    }
}

XMLDoc::RuleDefiner::RuleDefiner()
{
    // This is the start rule for XML parsing
    document =
        prolog >> element >> *Misc
        ;
        
    S =
        +(Sch)
        ;
        
    Name =
        (Letter | '_' | ':')
            >> *(NameChar)
        ;
        
    AttValue =
        '"'
            >> (
                (*(anychar_p - (chset_t('<') | '&' | '"')))[&XMLDoc::AddAttribute]
                | *(Reference)
               )
            >> '"'
        |   '\''
            >> (
                (*(anychar_p - (chset_t('<') | '&' | '\'')))[&XMLDoc::AddAttribute]
                | *(Reference)
               )
            >> '\''
        ;

    chset_t CharDataChar(anychar_p - (chset_t('<') | chset_t('&')));

    CharData =
        (*(CharDataChar - str_p("]]>")))[&XMLDoc::AppendToText]
        ;

    Comment1 =
        *(
          (Char - ch_p('-'))
          | (ch_p('-') >> (Char - ch_p('-')))
          )
        ;

    Comment =
        str_p("<!--") >> Comment1 >> str_p("-->")
        ;

    CDSect =
        str_p("<![CDATA[") >> CData >> str_p("]]>")
        ;

    CData =
        (*(Char - str_p("]]>")))[&XMLDoc::AppendToText]
        ;

    prolog =
        !XMLDecl >> *Misc >> !(doctypedecl >> *Misc)
        ;

    XMLDecl =
        str_p("<?xml")
            >> VersionInfo
            >> !EncodingDecl
            >> !SDDecl
            >> !S
            >> str_p("?>")
        ;

    VersionInfo =
        S
            >> str_p("version")
            >> Eq
            >> (
                ch_p('\'') >> VersionNum >> '\''
                | ch_p('"')  >> VersionNum >> '"'
                )
        ;

    Eq =
        !S >> '=' >> !S
        ;

    chset_t VersionNumCh("A-Za-z0-9_.:-");

    VersionNum =
        +(VersionNumCh)
        ;

    Misc =
        Comment | S
        ;

    doctypedecl =
        str_p("<!DOCTYPE")
            >> *(Char - (chset_t('[') | '>'))
            >> !('[' >> *(Char - ']') >> ']')
            >> '>'
        ;

    SDDecl =
        S
            >> str_p("standalone")
            >> Eq
            >> (
                (ch_p('\'') >> (str_p("yes") | str_p("no")) >> '\'')
                | (ch_p('"')  >> (str_p("yes") | str_p("no")) >> '"')
                )
        ;

    element =
        STag[&XMLDoc::PushElem1] >> content >> ETag
        | EmptyElemTag[&XMLDoc::PushElem2]
        ;

    STag =
        '<'
            >> Name[&XMLDoc::SetElemName]
            >> *(S >> Attribute)
            >> !S
            >> '>'
        ;

    Attribute =
        Name[&XMLDoc::SetAttributeName] >> Eq >> AttValue
        ;

    ETag =
        str_p("</") >> Name[&XMLDoc::PopElem] >> !S >> '>'
        ;

    content =
        !CharData
            >> *(
                 (
                  element
                  | Reference
                  | CDSect
                  | Comment
                  )
                 >> !CharData
                 )
        ;

    EmptyElemTag =
        '<'
            >> Name[&XMLDoc::SetElemName]
            >> *(S >> Attribute)
            >> !S
            >> str_p("/>")
        ;

    CharRef =
        str_p("&#") >> +digit_p >> ';'
        | str_p("&#x") >> +xdigit_p >> ';'
        ;

    Reference =
        EntityRef
        | CharRef
        ;

    EntityRef =
        '&' >> Name >> ';'
        ;

    EncodingDecl =
        S
            >> str_p("encoding")
            >> Eq
            >> (
                ch_p('"')  >> EncName >> '"'
                | ch_p('\'') >> EncName >> '\''
                )
        ;

    chset_t EncNameCh = VersionNumCh - chset_t(':');

    EncName =
        alpha_p >> *(EncNameCh)
        ;
}

////////////////////////////////////////////////
// GG::XMLElementValidator
////////////////////////////////////////////////
XMLElementValidator::XMLElementValidator(const string& tag, ValidatorBase* text_validator/* = 0*/) : 
    m_tag(tag), 
    m_text_validator(text_validator)
{
}

XMLElementValidator::XMLElementValidator(const string& tag, const XMLElementValidator& body) : 
    m_tag(tag), 
    m_text_validator(0), 
    m_children(vector<XMLElementValidator>(1, body))
{
}

XMLElementValidator::XMLElementValidator(const XMLElementValidator& rhs)
{
    m_tag = rhs.m_tag;
    m_text_validator = rhs.m_text_validator ? rhs.m_text_validator->Clone() : 0;
    for (map<string, ValidatorBase*>::const_iterator it = rhs.m_attribute_validators.begin(); it != rhs.m_attribute_validators.end(); ++it) {
	m_attribute_validators[it->first] = it->second ? it->second->Clone() : 0;
    }
    for (unsigned int i = 0; i < rhs.m_children.size(); ++i) {
	m_children.push_back(rhs.m_children[i]);
    }
}

const XMLElementValidator& XMLElementValidator::operator=(const XMLElementValidator& rhs)
{
    if (this != &rhs) {
	Clear();
    
	m_tag = rhs.m_tag;
	m_text_validator = rhs.m_text_validator ? rhs.m_text_validator->Clone() : 0;
	for (map<string, ValidatorBase*>::const_iterator it = rhs.m_attribute_validators.begin(); it != rhs.m_attribute_validators.end(); ++it) {
	    m_attribute_validators[it->first] = it->second ? it->second->Clone() : 0;
	}
	for (unsigned int i = 0; i < rhs.m_children.size(); ++i) {
	    m_children.push_back(rhs.m_children[i]);
	}
    }
    return *this;
}

XMLElementValidator::~XMLElementValidator()
{
    Clear();
}

void XMLElementValidator::Validate(const XMLElement& elem) const
{
#if DEBUG_OUTPUT    
    std::cout << "Validating " << m_tag << " with element with tag \"" << elem.Tag() << "\"" << std::endl;
#endif
    if (m_text_validator) {
#if DEBUG_OUTPUT    
	std::cout << "  checking text \"" << elem.Text() << "\"" << std::endl;
#endif
	m_text_validator->Validate(elem.Text());
    }

    if (static_cast<unsigned int>(elem.NumAttributes()) != m_attribute_validators.size()) {
	throw std::runtime_error("XMLElementValidator::Validate() : Encountered an XMLElement with a different number "
				 "of children than this XMLElementValidator.");
    }

    XMLElement::const_attr_iterator attrib_it = elem.attr_begin();
    for (map<string, ValidatorBase*>::const_iterator it = m_attribute_validators.begin(); 
	 it != m_attribute_validators.end(); 
	 ++it, ++attrib_it) {
	if (it->second) {
#if DEBUG_OUTPUT    
	    std::cout << "  checking attribute \"" << it->first << "\" value \"" << attrib_it->second << "\"" << std::endl;
#endif
	    it->second->Validate(attrib_it->second);
	}
    }

    for (unsigned int i = 0; i < m_children.size(); ++i) {
	m_children[i].Validate(elem.Child(i));
    }
#if DEBUG_OUTPUT    
    std::cout << std::endl;
#endif
}

XMLElementValidator& XMLElementValidator::Child(const string& child)
{
    unsigned int i = 0;
    for (; i < m_children.size(); ++i) {
        if (m_children[i].m_tag == child)
            break;
    }

    if (i == m_children.size())
        throw std::range_error("XMLElementValidator::Child(): This XMLElementValidator contains no child \"" + child + "\".");

    return m_children[i];
}

XMLElementValidator& XMLElementValidator::LastChild()
{
    if (m_children.empty())
        throw std::range_error("XMLElementValidator::LastChild(): LastChild() was called on an empty XMLElementValidator.");

    return m_children.back();
}

void XMLElementValidator::SetAttribute(const string& attrib, ValidatorBase* value_validator)
{
    delete m_attribute_validators[attrib];
    m_attribute_validators[attrib] = value_validator;
}

void XMLElementValidator::AppendChild(const XMLElementValidator& ev) 
{
    m_children.push_back(ev);
}

void XMLElementValidator::Clear()
{
    delete m_text_validator;
    m_text_validator = 0;
    for (map<string, ValidatorBase*>::iterator it = m_attribute_validators.begin(); it != m_attribute_validators.end(); ++it) {
	delete it->second;
    }
    m_attribute_validators.clear();
    m_children.clear();
}


////////////////////////////////////////////////
// Free Functions
////////////////////////////////////////////////
vector<string> Tokenize(const string& str)
{
    vector<string> retval;
    parse(str.c_str(), *space_p >> *((+(anychar_p - space_p))[append(retval)] >> *space_p));
    return retval;
}

pair<vector<string>, vector<string> > TokenizeMapString(const string& str)
{
    pair<vector<string>, vector<string> > retval;
    if (!parse(str.c_str(), 
	       *space_p >> *(
			     ch_p('(') >> *space_p >> 
			     (+(anychar_p - space_p - ch_p(',')))[append(retval.first)] >> *space_p >> 
			     ch_p(',') >> *space_p >> 
			     (+(anychar_p - space_p - ch_p(')')))[append(retval.second)] >> *space_p >> 
			     ch_p(')') >> *space_p
		            )
	       ).full) {
	throw std::runtime_error("Tokenize() : The string \"" + str + "\" is not a well-formed map string.");
    }
    return retval;
}


} // namespace GG


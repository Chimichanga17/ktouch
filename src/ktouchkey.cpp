/***************************************************************************
 *   ktouchkey.cpp                                                         *
 *   -------------                                                         *
 *   Copyright (C) 2000 by Håvard Frøiland, 2004 by Andreas Nicolai        *
 *   ghorwin@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktouchkey.h"
#include "ktouchkey.moc"

#include <QtXml>
#include <QtCore>

#include <kdebug.h>

#include "ktouchcolorscheme.h"
#include "ktouchkeyboard.h"

#include "prefs.h"

const double PEN_WIDTH = 1.0;

KTouchKey::KTouchKey(QObject * parent) 
	: QObject(parent), m_state(NormalState), m_type(Normal), m_x(0), m_y(0), m_w(0), m_h(0) 
{
	m_keyChar[0] = QChar();
	m_keyChar[1] = QChar();
	m_keyChar[2] = QChar();
	m_keyChar[3] = QChar();
	m_colorIndex = 0;
	setPos(m_x, m_y);
}

KTouchKey::KTouchKey(QObject * parent, keytype_t type, int x, int y, int w, int h, QChar ch)
	: QObject(parent), m_state(NormalState), m_type(type), m_x(x), m_y(y), m_w(w), m_h(h)
{
	m_keyChar[0] = ch;
	m_keyChar[1] = QChar();
	m_keyChar[2] = QChar();
	m_keyChar[3] = QChar();
	m_colorIndex = 0;
	setPos(m_x, m_y);
}
// ----------------------------------------------------------------------------

KTouchKey::KTouchKey(QObject * parent, int x, int y, int w, int h, const QString &text) 
	: QObject(parent), m_state(NormalState), m_x(x), m_y(y), m_w(w), m_h(h)
{
	m_type = Other;
	m_keyChar[0] = QChar();
	m_keyText = text;
	m_colorIndex = 0;
	setPos(m_x, m_y);
}
// ----------------------------------------------------------------------------

// Reads the key data from the DomElement
bool KTouchKey::read(QDomElement e) {
	if (e.hasAttribute("Type")) {
		QString typetext = e.attribute("Type");
		if (typetext=="NORMAL")  		m_type = Normal;
		else if (typetext=="FINGER")  	m_type = Finger;
		else if (typetext=="ENTER")  	m_type = Enter;
		else if (typetext=="BACKSPACE")	m_type = Backspace;
		else if (typetext=="SHIFT")  	m_type = Shift;
		else if (typetext=="SPACE")  	m_type = Space;
		else if (typetext=="OTHER") {
			m_type = Other;
			if (e.hasAttribute("KeyText"))
				m_keyText = e.attribute("KeyText");
		}
		else
			return false; // TODO : Error message
	}
	QDomElement charElement = e.firstChildElement("Char");
	while (!charElement.isNull()) {
		position_t pos = TopLeft;
		if (charElement.hasAttribute("Position")) {
			QString position = charElement.attribute("Position");
			if (position == "TopLeft")  			pos = TopLeft;
			else if  (position == "TopRight")		pos = TopRight;
			else if  (position == "BottomLeft")		pos = BottomLeft;
			else if  (position == "BottomRight")	pos = BottomRight;
		}
		if (charElement.hasAttribute("Unicode"))
			m_keyChar[pos] = QChar(charElement.attribute("Unicode").toInt());
		charElement = charElement.nextSiblingElement("Char");
	}

	if (e.hasAttribute("Width")) 
		m_w = e.attribute("Width").toInt();
	if (e.hasAttribute("Height")) 
		m_h = e.attribute("Height").toInt();
	if (e.hasAttribute("X")) 
		m_x = e.attribute("X").toInt();
	if (e.hasAttribute("Y")) 
		m_y = e.attribute("Y").toInt();
	
	setPos(m_x, m_y);
	//kDebug() << "Key = " << m_keyChar[TopLeft] << " " << m_x << " " << m_y << " " << m_w << " " << m_h << endl;
	return true;
}
// ----------------------------------------------------------------------------

// Writes the key data into the DomElement
void KTouchKey::write(QDomDocument& doc, QDomElement& root) const {
	QDomElement element = doc.createElement("Key");
	switch (m_type) {
		case Normal     : element.setAttribute("Type", "NORMAL"); break;
		case Finger     : element.setAttribute("Type", "FINGER"); break;
		case Enter		: element.setAttribute("Type", "ENTER"); break;
		case Backspace 	: element.setAttribute("Type", "BACKSPACE"); break;
		case Shift		: element.setAttribute("Type", "SHIFT"); break;
		case Space 		: element.setAttribute("Type", "SPACE"); break;
		case Other 		: 
			element.setAttribute("Type", "OTHER"); 
			element.setAttribute("KeyText", m_keyText);
			break;
	}
	// write the characters
	for (int i=0; i<4; ++i) {
		if (m_keyChar[i] != QChar()) {
			QDomElement char_element = doc.createElement("Char");
			char_element.setAttribute("Unicode", m_keyChar[i].unicode());
			QString position;
			switch (i) {
				case 0 : position = "TopLeft"; break;
				case 1 : position = "TopRight"; break;
				case 2 : position = "BottomLeft"; break;
				case 3 : position = "BottomRight"; break;
			}
			char_element.setAttribute("Unicode", m_keyChar[i].unicode());
			char_element.setAttribute("Position", position);
			QDomText t = doc.createTextNode(QString(m_keyChar[i]));
			char_element.appendChild(t);
			element.appendChild(char_element);
		}
	}
	
	element.setAttribute("X", m_x);
	element.setAttribute("Y", m_y);
	element.setAttribute("Width", m_w);
	element.setAttribute("Height", m_h);
	root.appendChild(element);
}
// ----------------------------------------------------------------------------

void KTouchKey::setState(state_t state) {
	m_state = state;
	update();	
}
// ----------------------------------------------------------------------------

QRectF KTouchKey::boundingRect() const {
	return QRectF(- PEN_WIDTH/2.0, - PEN_WIDTH/2.0, 
		m_w + PEN_WIDTH, m_h + PEN_WIDTH);
}
// ----------------------------------------------------------------------------

void KTouchKey::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

	// get the colorscheme from the configuration
    const KTouchColorScheme& colorScheme = KTouchColorScheme::m_colorSchemes[Prefs::currentColorScheme()];

	// first draw the background
	switch (m_state) {
	  case NormalState : 
        painter->setBrush( colorScheme.m_background[m_colorIndex] );
        painter->setPen( colorScheme.m_frame );
        painter->drawRoundRect(0, 0, m_w, m_h);
	  break;

	  default:
        painter->setBrush( colorScheme.m_backgroundH );
        painter->setPen( colorScheme.m_frame );
        painter->drawRoundRect(0, 0, m_w, m_h);
        painter->setBrush(colorScheme.m_cBackgroundH );
        painter->drawEllipse(0, 0, m_w, m_h);
	  break;
	};

    painter->setPen( colorScheme.m_text );
    KTouchKeyboard * kb = dynamic_cast<KTouchKeyboard *>(parent());
    QFont f = kb->font();
    f.setPointSizeF( qMax(0.01, m_h*0.5) );
    painter->setFont( f );
	switch (m_type) {
	  case Normal :
	  case Finger :
		if (m_keyChar[0] == QChar()) return;
		painter->drawText(QRectF(m_h*0.15, m_h*0.15, m_w - m_h*0.3, m_h*0.7), Qt::AlignLeft | Qt::AlignTop, m_keyChar[0]);
		break;

	  case Other :
		painter->drawText(QRectF(m_h*0.15, m_h*0.15, m_w - m_h*0.3, m_h*0.7), Qt::AlignCenter, m_keyText);
		break;
	}
}

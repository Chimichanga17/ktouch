/***************************************************************************
 *   ktouchstatus.cpp                                                      *
 *   ----------------                                                      *
 *   Copyright (C) 2000 by H�vard Fr�iland, 2003 by Andreas Nicolai        *
 *   haavard@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktouchstatus.h"
#include "ktouchstatus.moc"

#include <qlcdnumber.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include "ktouchsettings.h"

KTouchStatus::KTouchStatus(QWidget *parent)
 : KTouchStatusLayout(parent)
{
}

void KTouchStatus::applyPreferences() {
    // we just need to turn on/off the level change buttons
    levelUpBtn->setEnabled( !KTouchConfig().m_autoLevelChange );
    levelDownBtn->setEnabled( !KTouchConfig().m_autoLevelChange );
}

void KTouchStatus::setNewChars(const QString& newChars) {
    newCharsLabel->setText(newChars);
}

void KTouchStatus::updateStatus(unsigned int level, double correctness) {
    levelLCD->display(static_cast<int>(level)+1); // +1 because our level variable ranges from 0...n-1
    correctnessBar->setProgress(static_cast<int>(correctness*100) );
}

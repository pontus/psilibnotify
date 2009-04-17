/*
 * psilibnotify.h: Psi's interface to libnotify
 * Copyright (C) 2005  Remko Troncon, 2009 Pontus Freyhult
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * You can also redistribute and/or modify this program under the
 * terms of the Psi License, specified in the accompanied COPYING
 * file, as published by the Psi Project; either dated January 1st,
 * 2005, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef PSILIBNOTIFY_H
#define PSILIBNOTIFY_H

#include <QObject>

#include "psipopup.h"

class PsiAccount;

/**
 * An interface for Psi to libnotify.
 * This class is used to interface with libnotify.
 * There is at most 1 PsiLibnotify per Psi session (Singleton). This 
 * notifier can be retrieved using instance().
 *
 */
class PsiLibnotify : public QObject
{
	Q_OBJECT

public:
	static PsiLibnotify* instance();
	void popup(PsiAccount* account, PsiPopup::PopupType type, const Jid& j, const Resource& r, const UserListItem* = 0, PsiEvent* = 0);

public slots:
	void notificationClicked(void*);
	void notificationTimedOut(void*);

private:
	PsiLibnotify();

	static PsiLibnotify* instance_;
};

#endif

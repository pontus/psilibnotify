/*
 * psilibnotify.cpp: Psi's interface to libnotify
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


#include <libnotifymm.h>

#include <QPixmap>
#include <QBuffer>
#include <QStringList>
#include <QCoreApplication>
#include "common.h"
#include "psiaccount.h"
#include "avatars.h"

#include "psilibnotify.h"
#include "psievent.h"
#include "userlist.h"

/**
 * A class representing the notification context, which will be passed to
 * libnotify, and then passed back when a notification is clicked.
 */
class NotificationContext
{
public:
	NotificationContext(PsiAccount* a, Jid j) : account_(a), jid_(j) { }
	PsiAccount* account() { return account_; }
	Jid jid() { return jid_; }


private:
	PsiAccount* account_;
	Jid jid_;
};


/**
 * (Private) constructor of the PsiLibnotify.
 * Initializes notifications and registers with libnotify.
 */
PsiLibnotify::PsiLibnotify() : QObject(QCoreApplication::instance())
{
	// Initialize all notifications
	QStringList nots;
	nots << QObject::tr("Contact becomes Available");
	nots << QObject::tr("Contact becomes Unavailable");
	nots << QObject::tr("Contact changes Status");
	nots << QObject::tr("Incoming Message");
	nots << QObject::tr("Incoming Headline");
	nots << QObject::tr("Incoming File");

	// Initialize default notifications
	QStringList defaults;
	defaults << QObject::tr("Contact becomes Available");
	defaults << QObject::tr("Incoming Message");
	defaults << QObject::tr("Incoming Headline");
	defaults << QObject::tr("Incoming File");


	Notify::init(QObject::tr("Psi").toUtf8().data());

}


/**
 * Requests the global PsiLibnotify instance.
 * If PsiLibnotify wasn't initialized yet, it is initialized.
 *
 * \see Libnotify()
 * \return A pointer to the PsiLibnotify instance
 */
PsiLibnotify* PsiLibnotify::instance() 
{
	if (!instance_) 
		instance_ = new PsiLibnotify();
	
	return instance_;
}


/**
 * Requests a popup to be sent to libnotify.
 *
 * \param account The requesting account.
 * \param type The type of popup to be sent.
 * \param jid The originating jid
 * \param uli The originating userlist item. Can be NULL.
 * \param event The originating event. Can be NULL.
 */
void PsiLibnotify::popup(PsiAccount* account, PsiPopup::PopupType type, const Jid& jid, const Resource& r, const UserListItem* uli, PsiEvent* event)
{
	QString name;
	QString title, desc, contact;
	QString statusTxt = status2txt(makeSTATUS(r.status()));
	QString statusMsg = r.status().status();
	QPixmap icon = account->avatarFactory()->getAvatar(jid.bare());
	if (uli) {
		contact = uli->name();
	}
	else if (event->type() == PsiEvent::Auth) {
		contact = ((AuthEvent*) event)->nick();
	}
	else if (event->type() == PsiEvent::Message) {
		contact = ((MessageEvent*) event)->nick();
	}

	if (contact.isEmpty())
		contact = jid.bare();

	// Default value for the title
	title = contact;

	switch(type) {
		case PsiPopup::AlertOnline:
			name = QObject::tr("Contact becomes Available");
			title = QString("%1 (%2)").arg(contact).arg(statusTxt);
			desc = statusMsg;
			//icon = PsiIconset::instance()->statusPQString(jid, r.status());
			break;
		case PsiPopup::AlertOffline:
			name = QObject::tr("Contact becomes Unavailable");
			title = QString("%1 (%2)").arg(contact).arg(statusTxt);
			desc = statusMsg;
			//icon = PsiIconset::instance()->statusPQString(jid, r.status());
			break;
		case PsiPopup::AlertStatusChange:
			name = QObject::tr("Contact changes Status");
			title = QString("%1 (%2)").arg(contact).arg(statusTxt);
			desc = statusMsg;
			//icon = PsiIconset::instance()->statusPQString(jid, r.status());
			break;
		case PsiPopup::AlertMessage: {
			name = QObject::tr("Incoming Message");
			title = QObject::tr("%1 says:").arg(contact);
			const Message* jmessage = &((MessageEvent *)event)->message();
			desc = jmessage->body();
			//icon = IconsetFactory::iconPQString("psi/message");
			break;
		}
		case PsiPopup::AlertChat: {
			name = QObject::tr("Incoming Message");
			const Message* jmessage = &((MessageEvent *)event)->message();
			desc = jmessage->body();
			//icon = IconsetFactory::iconPQString("psi/start-chat");
			break;
		}
		case PsiPopup::AlertHeadline: {
			name = QObject::tr("Incoming Headline");
			const Message* jmessage = &((MessageEvent *)event)->message();
			if ( !jmessage->subject().isEmpty())
				title = jmessage->subject();
			desc = jmessage->body();
			//icon = IconsetFactory::iconPQString("psi/headline");
			break;
		}
		case PsiPopup::AlertFile:
			name = QObject::tr("Incoming File");
			desc = QObject::tr("[Incoming File]");
			//icon = IconsetFactory::iconPQString("psi/file");
			break;
		default:
			break;
	}

	// Notify
	// NotificationContext* context = new NotificationContext(account, jid);
	//gn_->notify(name, title, desc, icon, false, this, SLOT(notificationClicked(void*)), SLOT(notificationTimedOut(void*)), context);

	Notify::Notification ln((name+title).toUtf8().data(), desc.toUtf8().data());


	QBuffer buffer;
	buffer.open( IO_WriteOnly );
	icon.save( &buffer, "xpm" ); // writes pixmap into ba in xpm format
	
	const char* xpm_data = buffer.data().data();

	qWarning("XPM-data: ");
	qWarning(xpm_data);

	// Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create_from_xpm_data(&xpm_data);
	// ln.set_icon_from_pixbuf(pb);
	ln.show();
}

void PsiLibnotify::notificationClicked(void* c)
{
	NotificationContext* context = (NotificationContext*) c;
	context->account()->actionDefault(context->jid());
	delete context;
}

void PsiLibnotify::notificationTimedOut(void* c)
{
	NotificationContext* context = (NotificationContext*) c;
	delete context;
}

PsiLibnotify* PsiLibnotify::instance_ = 0;

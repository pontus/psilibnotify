#ifndef CALLDLG_H
#define CALLDLG_H

#include <QDialog>

namespace XMPP
{
	class Jid;
}

class PsiAccount;
class JingleRtpSession;

class CallDlg : public QDialog
{
	Q_OBJECT

public:
	CallDlg(PsiAccount *pa, QWidget *parent = 0);
	~CallDlg();

	void setOutgoing(const XMPP::Jid &jid);
	void setIncoming(JingleRtpSession *sess);

private:
	class Private;
	Private *d;
};

#endif

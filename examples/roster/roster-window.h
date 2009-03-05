/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _TelepathyQt4_examples_roster_roster_window_h_HEADER_GUARD_
#define _TelepathyQt4_examples_roster_roster_window_h_HEADER_GUARD_

#include <QMainWindow>
#include <QSharedPointer>

namespace Telepathy {
namespace Client {
class Connection;
class ConnectionManager;
class Contact;
class PendingOperation;
}
}

class QAction;
class QDialog;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class RosterWindow : public QMainWindow
{
    Q_OBJECT

public:
    RosterWindow(const QString &username, const QString &password,
            QWidget *parent = 0);
    virtual ~RosterWindow();

private Q_SLOTS:
    void onCMReady(Telepathy::Client::PendingOperation *);
    void onConnectionCreated(Telepathy::Client::PendingOperation *);
    void onConnectionReady(Telepathy::Client::PendingOperation *);
    void onPresencePublicationRequested(const QSet<QSharedPointer<Telepathy::Client::Contact> > &);
    void onItemSelectionChanged();
    void onAddButtonClicked();
    void onAuthActionTriggered(bool);
    void onDenyActionTriggered(bool);
    void onRemoveActionTriggered(bool);
    void onBlockActionTriggered(bool);
    void onContactRetrieved(Telepathy::Client::PendingOperation *op);
    void updateActions();

private:
    void createActions();
    void setupGui();
    void createItemForContact(const QSharedPointer<Telepathy::Client::Contact> &contact,
            bool checkExists = false);

    Telepathy::Client::ConnectionManager *mCM;
    QSharedPointer<Telepathy::Client::Connection> mConn;
    QString mUsername;
    QString mPassword;
    QAction *mAuthAction;
    QAction *mRemoveAction;
    QAction *mDenyAction;
    QAction *mBlockAction;
    QListWidget *mList;
    QPushButton *mAddBtn;
    QDialog *mAddDlg;
    QLineEdit *mAddDlgEdt;
};

#endif
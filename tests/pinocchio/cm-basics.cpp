#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <QtDBus/QtDBus>

#include <TelepathyQt4/ConnectionManager>
#include <TelepathyQt4/PendingConnection>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/PendingStringList>

#include <tests/pinocchio/lib.h>

using namespace Telepathy::Client;

class TestCmBasics : public PinocchioTest
{
    Q_OBJECT

private:
    Telepathy::Client::ConnectionManagerPtr mCM;

protected Q_SLOTS:
    void onListNames(Telepathy::Client::PendingOperation*);

private Q_SLOTS:
    void initTestCase();
    void init();

    void testBasics();

    void cleanup();
    void cleanupTestCase();
};


void TestCmBasics::initTestCase()
{
    initTestCaseImpl();

    // Wait for the CM to start up
    QVERIFY(waitForPinocchio(5000));
}


void TestCmBasics::init()
{
    initImpl();
}

void TestCmBasics::onListNames(Telepathy::Client::PendingOperation *operation)
{
    Telepathy::Client::PendingStringList *p = static_cast<Telepathy::Client::PendingStringList*>(operation);
    QCOMPARE(p->result().contains("pinocchio"), QBool(true));
    mLoop->exit(0);
}


void TestCmBasics::testBasics()
{
    connect(ConnectionManager::listNames(),
            SIGNAL(finished(Telepathy::Client::PendingOperation *)),
            this,
            SLOT(onListNames(Telepathy::Client::PendingOperation *)));
    QCOMPARE(mLoop->exec(), 0);

    mCM = ConnectionManager::create("pinocchio");
    QCOMPARE(mCM->isReady(), false);

    connect(mCM->becomeReady(),
            SIGNAL(finished(Telepathy::Client::PendingOperation *)),
            this,
            SLOT(expectSuccessfulCall(Telepathy::Client::PendingOperation *)));
    qDebug() << "enter main loop";
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mCM->isReady(), true);

    // calling becomeReady() twice is a no-op
    connect(mCM->becomeReady(),
            SIGNAL(finished(Telepathy::Client::PendingOperation *)),
            this,
            SLOT(expectSuccessfulCall(Telepathy::Client::PendingOperation *)));
    QCOMPARE(mLoop->exec(), 0);
    QCOMPARE(mCM->isReady(), true);

    QCOMPARE(mCM->interfaces(), QStringList());
    QCOMPARE(mCM->supportedProtocols(), QStringList() << "dummy");

    Q_FOREACH (ProtocolInfo *info, mCM->protocols()) {
        QVERIFY(info != 0);
        QVERIFY(info->cmName() == "pinocchio");
        QVERIFY(info->name() == "dummy");

        QCOMPARE(info->hasParameter("account"), true);
        QCOMPARE(info->hasParameter("not-there"), false);

        Q_FOREACH (ProtocolParameter *param, info->parameters()) {
            if (param->name() == "account") {
                QCOMPARE(param->dbusSignature().signature(),
                         QLatin1String("s"));
                QCOMPARE(param->isRequired(), true);
                QCOMPARE(param->isSecret(), false);
            }
            else if (param->name() == "password") {
                QCOMPARE(param->isRequired(), false);
                QCOMPARE(param->isSecret(), true);
            }
        }
        QCOMPARE(info->canRegister(), false);
    }

    QCOMPARE(mCM->supportedProtocols(), QStringList() << "dummy");

    QVariantMap parameters;
    parameters.insert(QLatin1String("account"),
        QVariant::fromValue(QString::fromAscii("empty")));
    parameters.insert(QLatin1String("password"),
        QVariant::fromValue(QString::fromAscii("s3kr1t")));

    PendingConnection *pconn = mCM->requestConnection("dummy", parameters);
    connect(pconn,
            SIGNAL(finished(Telepathy::Client::PendingOperation *)),
            this,
            SLOT(expectSuccessfulCall(Telepathy::Client::PendingOperation *)));
    QCOMPARE(mLoop->exec(), 0);

    QVERIFY(pconn->connection());
}


void TestCmBasics::cleanup()
{
    cleanupImpl();
}


void TestCmBasics::cleanupTestCase()
{
    cleanupTestCaseImpl();
}


QTEST_MAIN(TestCmBasics)
#include "_gen/cm-basics.cpp.moc.hpp"

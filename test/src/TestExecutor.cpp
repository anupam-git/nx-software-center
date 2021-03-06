//
// Created by alexis on 8/01/18.
//

#include <QSignalSpy>
#include <QDebug>
#include <gtest/gtest.h>

#include "interactors/Interactor.h"
#include "entities/Executor.h"
#include <QXmlQuery>

namespace NX_SOFTWARE_CENTER_TESTS {
    class DoNothingInteractor : public  Interactor {
        Q_OBJECT

    public:
        explicit DoNothingInteractor(QObject *parent = nullptr) : Interactor(parent) {}

        Qt::HANDLE threadHandle;

        void execute() override {
            setMetadata(getTestMetadata());

            QXmlQuery query;
            query.setQuery("doc('https://en.wikipedia.org/wiki/Linux')/html/body/string()");

            QString result;
            query.evaluateTo(&result);

            threadHandle = QThread::currentThreadId();
            emit completed();
        }

        static QVariantMap getTestMetadata() {
            QVariantMap d;
            d.insert("field", "value");
            return d;
        }
    };

    class TestExecutor : public testing::Test {
    protected:
        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
    };

    TEST_F(TestExecutor, run) {
        Executor e;
        DoNothingInteractor *i = new DoNothingInteractor();

        QSignalSpy spyStarted(&e, &Executor::taskStarted);
        QSignalSpy spyCompleted(&e, &Executor::taskCompleted);
        QSignalSpy spyDataChanged(&e, &Executor::taskDataChanged);

        QObject::connect(&e, &Executor::taskStarted, [=] (const QString &id) {
            ASSERT_EQ(i->getId(), id);
        });

        QObject::connect(&e, &Executor::taskCompleted, [=] (const QString &id) {
            ASSERT_EQ(i->getId(), id);
        });

        QObject::connect(&e, &Executor::taskDataChanged, [=] (const QString &id, const QVariantMap &data) {
            ASSERT_EQ(i->getId(), id);
            ASSERT_EQ(DoNothingInteractor::getTestMetadata(), data);
        });

        e.execute(i);

        spyCompleted.wait();
        ASSERT_NE(QThread::currentThreadId(), i->threadHandle);
        ASSERT_EQ(1, spyStarted.count());
        ASSERT_EQ(1, spyCompleted.count());
        ASSERT_EQ(1, spyDataChanged.count());

        i->deleteLater();
    }
}

#include "TestExecutor.moc"

/*
    Author: Marco Costalba (C) 2005-2007

    Copyright: See COPYING file that comes with this distribution

*/
#ifndef QGIT_DOMAIN_H_INCLUDED
#define QGIT_DOMAIN_H_INCLUDED

#include <QtCore/QObject>
#include <QtCore/QEvent>

#include "exceptionmanager.h"
#include "common.h"

#define UPDATE_DOMAIN(x)       QApplication::postEvent(x, new UpdateDomainEvent(false))
#define UPDATE()               QApplication::postEvent(this, new UpdateDomainEvent(false))
#define UPDATE_DM_MASTER(x, f) QApplication::postEvent(x, new UpdateDomainEvent(true, f))

class Domain;
class FileHistory;
class Git;
class MainImpl;

class UpdateDomainEvent : public QEvent {
public:
    explicit UpdateDomainEvent(bool fromMaster, bool force = false);
    virtual ~UpdateDomainEvent();

    bool isForced() const;

private:
    bool f;
};

class StateInfo {
public:
    StateInfo();
    StateInfo& operator=(const StateInfo& newState);
    bool operator==(const StateInfo& newState) const;
    bool operator!=(const StateInfo& newState) const;

    void clear();
    const QString sha(bool n = true) const;
    const QString fileName(bool n = true) const;
    const QString diffToSha(bool n = true) const;
    bool selectItem(bool n = true) const;
    bool isMerge(bool n = true) const;
    bool allMergeFiles(bool n = true) const;
    void setSha(const QString& s);
    void setFileName(const QString& s);
    void setDiffToSha(const QString& s);
    void setSelectItem(bool b);
    void setIsMerge(bool b);
    void setAllMergeFiles(bool b);
    bool isChanged(uint what = ANY) const;

    enum Field {
        SHA             = 1,
        FILE_NAME       = 2,
        DIFF_TO_SHA     = 4,
        ALL_MERGE_FILES = 8,
        ANY             = 15
    };

private:
    friend class Domain;

    bool requestPending() const;
    void setLock(bool b);
    void commit();
    void rollBack();
    bool flushQueue();

    class S {
    public:
        S() { clear(); }
        void clear();
        bool operator==(const S& newState) const;
        bool operator!=(const S& newState) const;

        QString sha;
        QString fn;
        QString dtSha;
        bool sel;
        bool isM;
        bool allM;
    };
    S curS;  // current state, what returns from StateInfo::sha()
    S prevS; // previous good state, used to rollBack in case state update fails
    S nextS; // next queued state, waiting for current update to finish
    bool isLocked;
};

class Domain: public QObject {
    Q_OBJECT

public:
    Domain();
    Domain(MainImpl* m, Git* git, bool isMain);
    virtual ~Domain();

    void deleteWhenDone(); // will delete when no more run() are pending
    void showStatusBarMessage(const QString& msg, int timeout = 0);
    void setThrowOnDelete(bool b);
    bool isThrowOnDeleteRaised(int excpId, SCRef curContext);
    MainImpl* m() const;
    FileHistory* model() const;
    bool isLinked() const;
    QWidget* tabPage() const;
    virtual bool isMatch(SCRef);

    StateInfo st;

signals:
    void updateRequested(StateInfo newSt);
    void cancelDomainProcesses();

public slots:
    void on_closeAllTabs();

protected slots:
    virtual void on_contextMenu(const QString&, int);
    void on_updateRequested(StateInfo newSt);
    void on_deleteWhenDone();

protected:
    virtual void clear(bool complete = true);
    virtual bool event(QEvent* e);
    virtual bool doUpdate(bool force) = 0;
    void linkDomain(Domain* d);
    void unlinkDomain(Domain* d);
    void setTabCaption(const QString& caption);

    Git* git;
    QWidget* container;
    bool busy;

private:
    void populateState();
    void update(bool fromMaster, bool force);
    bool flushQueue();
    void sendPopupEvent();

    EM_DECLARE(exDeleteRequest);
    EM_DECLARE(exCancelRequest);

    FileHistory* fileHistory;
    bool linked;
    int popupType;
    QString popupData;
    QString statusBarRequest;
};

#endif // QGIT_DOMAIN_H_INCLUDED

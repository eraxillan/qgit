#ifndef QGIT_REVISION_H
#define QGIT_REVISION_H

//#include <QtCore>

#include "common.h"

class Revision {
    // prevent implicit C++ compiler defaults
    Revision();
    Revision(const Revision&);
    Revision& operator=(const Revision&);

    ShaString m_sha;
    QStringList m_parentSha;
    QString m_author;
    QString m_date;
    QString m_patch;
    QString m_shortLog;
    QString m_longLog;

    // FIXME: remove
private:
    const QByteArray& m_data; // reference here!
    const int m_fixme_start;
    mutable int m_fixme_parentsCnt, m_fixme_shaStart, m_fixme_committerStart, m_fixme_authorStart, m_fixme_authorDateStart;
    mutable int m_fixme_sLogStart, m_fixme_sLogLen, m_fixme_lLogStart, m_fixme_lLogLen, m_fixme_diffStart, m_fixme_diffLen;
    mutable bool m_fixme_indexed;

private:
    void setup() const;
    int indexData(bool quick, bool withDiff) const;
    const QString mid(int start, int len) const;
    const QString midSha(int start, int len) const;

public:
    // FIXME: test begin
    void init(ShaString sha, QStringList parentSha, QString author, QString date, QString patch, QString shortLog, QString longLog);
    // FIXME: test end

    Revision(const QByteArray& b, int s, int idx, int* next, bool withDiff);

    bool isBoundary() const;

    int parentsCount() const;
    const ShaString parent(int idx) const;
    const QStringList parents() const;

    const ShaString sha() const;
    const QString committer() const;
    const QString author() const;
    const QString authorDate() const;
    const QString shortLog() const;
    const QString longLog() const;
    const QString diff() const;

    QVector<int> lanes, children;
    QVector<int> descRefs;     // list of descendant refs index, normally tags
    QVector<int> ancRefs;      // list of ancestor refs index, normally tags
    QVector<int> descBranches; // list of descendant branches index
    int descRefsMaster; // in case of many Revision have the same descRefs, ancRefs or
    int ancRefsMaster;  // descBranches these are stored only once in a Revision pointed
    int descBrnMaster;  // by corresponding index xxxMaster
    int orderIdx;

public:
    bool isDiffCache, isApplied, isUnApplied; // put here to optimize padding
};

using RevisionMap = QHash<ShaString, const Revision*>;  // faster then a map

#endif // QGIT_REVISION_H

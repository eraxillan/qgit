/*
    Author: Marco Costalba (C) 2005-2007

    Copyright: See COPYING file that comes with this distribution

*/
#ifndef QGIT_PATCHCONTENT_H_INCLUDED
#define QGIT_PATCHCONTENT_H_INCLUDED

#include <QtCore/QPointer>
#include <QtGui/QSyntaxHighlighter>
#include <QtWidgets/QTextEdit>

#include "common.h"

class Domain;
class Git;
class MyProcess;
class StateInfo;

class DiffHighlighter : public QSyntaxHighlighter {
public:
    DiffHighlighter(QTextEdit* p);

    void setCombinedLength(int c);
    virtual void highlightBlock(const QString& text);

private:
    int cl;
};

class PatchContent: public QTextEdit {
    Q_OBJECT

public:
    PatchContent(QWidget* parent);

    void setup(Domain* parent, Git* git);
    void clear();
    void centerOnFileHeader(StateInfo& st);
    void refresh();
    void update(StateInfo& st);

    enum PatchFilter {
        VIEW_ALL,
        VIEW_ADDED,
        VIEW_REMOVED
    };
    PatchFilter curFilter, prevFilter;

public slots:
    void on_highlightPatch(const QString&, bool);
    void typeWriterFontChanged();
    void procReadyRead(const QByteArray& data);
    void procFinished();

private:
    friend class DiffHighlighter;

    void scrollCursorToTop();
    void scrollLineToTop(int lineNum);
    int positionToLineNum(int pos);
    int topToLineNum();
    void saveRestoreSizes(bool startup = false);
    int doSearch(const QString& txt, int pos);
    bool computeMatches();
    bool getMatch(int para, int* indexFrom, int* indexTo);
    void centerMatch(int id = 0);
    bool centerTarget(SCRef target);
    void processData(const QByteArray& data, int* prevLineNum = nullptr);

    Git* git;
    DiffHighlighter* diffHighlighter;
    QPointer<MyProcess> proc;
    bool diffLoaded;
    QByteArray patchRowData;
    QString halfLine;
    bool isRegExp;
    QRegExp pickAxeRE;
    QString target;
    bool seekTarget;

    struct MatchSelection {
        int paraFrom;
        int indexFrom;
        int paraTo;
        int indexTo;
    };
    typedef QVector<MatchSelection> Matches;
    Matches matches;
};

#endif // QGIT_PATCHCONTENT_H_INCLUDED

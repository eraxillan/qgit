/*
    Author: Marco Costalba (C) 2005-2007

    Copyright: See COPYING file that comes with this distribution
*/
#ifndef QGIT_REVDESC_H_INCLUDED
#define QGIT_REVDESC_H_INCLUDED

#include <QtWidgets/QTextBrowser>

class Domain;

class RevDesc: public QTextBrowser {
    Q_OBJECT

public:
    RevDesc(QWidget* parent);
    void setup(Domain* dm);

protected:
    virtual void contextMenuEvent(QContextMenuEvent* e);

private slots:
    void on_anchorClicked(const QUrl& link);
    void on_highlighted(const QUrl& link);
    void on_linkCopy();

private:
    Domain* d;
    QString highlightedLink;
};

#endif // QGIT_REVDESC_H_INCLUDED

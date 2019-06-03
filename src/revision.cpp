#include "revision.h"

Revision::Revision(const QByteArray &b, int s, int idx, int *next, bool withDiff)
    : m_data(b), m_fixme_start(s),  orderIdx(idx) {

    m_fixme_indexed = isDiffCache = isApplied = isUnApplied = false;
    descRefsMaster = ancRefsMaster = descBrnMaster = -1;
    *next = indexData(true, withDiff);
}

void Revision::init(ShaString sha, QStringList parentSha, QString author, QString date, QString patch, QString shortLog, QString longLog) {

    m_sha = sha;
    m_parentSha = parentSha;
    m_author = author;
    m_date = date;
    m_patch = patch;
    m_shortLog = shortLog;
    m_longLog = longLog;
}

const QString Revision::mid(int start, int len) const {

    if ((start < 0) || (start >= m_data.length() - 1)) {
        dbp("ASSERT in Revision::mid, start is %1", start);
        return QString::null;
    }
    if (start + len >= m_data.length()) {
        dbp("ASSERT in Revision::mid, len is %1", len);
        return QString::null;
    }

    const char* data = m_data.constData();
    return QString::fromLocal8Bit(data + start, len);
}

const QString Revision::midSha(int start, int len) const {

    // warning no sanity check is done on arguments
    const char* data = m_data.constData();
    return QString::fromLatin1(data + start, len); // faster then formAscii
}


bool Revision::isBoundary() const {

    return (m_data.at(m_fixme_shaStart - 1) == '-');
}

int Revision::parentsCount() const {

    return m_fixme_parentsCnt;
}

const ShaString Revision::parent(int idx) const {

    return ShaString(m_data.constData() + m_fixme_shaStart + 41 + 41 * idx);
}

const QStringList Revision::parents() const {

    QStringList p;
    int idx = m_fixme_shaStart + 41;

    for (int i = 0; i < m_fixme_parentsCnt; i++) {
        p.append(midSha(idx, 40));
        idx += 41;
    }
    return p;
}

const ShaString Revision::sha() const {

    return /*(m_sha.size() > 0) ? m_sha :*/ ShaString(m_data.constData() + m_fixme_shaStart);
}

const QString Revision::committer() const {

    setup();
    return mid(m_fixme_committerStart, m_fixme_authorStart - m_fixme_committerStart - 1);
}

const QString Revision::author() const {

    setup();
    return mid(m_fixme_authorStart, m_fixme_authorDateStart - m_fixme_authorStart - 1);
}

const QString Revision::authorDate() const {

    setup();
    return mid(m_fixme_authorDateStart, 10);
}

const QString Revision::shortLog() const {

    setup();
    return mid(m_fixme_sLogStart, m_fixme_sLogLen);
}

const QString Revision::longLog() const {

    setup();
    return !m_longLog.isEmpty() ? m_longLog : mid(m_fixme_lLogStart, m_fixme_lLogLen);
}

const QString Revision::diff() const {

    setup();
    return mid(m_fixme_diffStart, m_fixme_diffLen);
}

void Revision::setup() const {

    if (!m_fixme_indexed)
        indexData(false, false);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

int Revision::indexData(bool quick, bool withDiff) const {

    /*
  This is what 'git log' produces:

        - a possible one line with "Final output:\n" in case of --early-output option
        - one line with "log size" + len of this record
        - one line with boundary info + sha + an arbitrary amount of parent's sha
        - one line with committer name + e-mail
        - one line with author name + e-mail
        - one line with author date as unix timestamp
        - zero or more non blank lines with other info, as the encoding FIXME
        - one blank line
        - zero or one line with log title
        - zero or more lines with log message
        - zero or more lines with diff content (only for file history)
        - a terminating '\0'
*/
    static int error = -1;
    static int shaLength = 40; // from git ref. spec.
    static int shaEndlLength = shaLength + 1; // an sha key + \n
    static int shaXEndlLength = shaLength + 2; // an sha key + X marker + \n
    static char finalOutputMarker = 'F'; // marks the beginning of "Final output" string
    static char logSizeMarker = 'l'; // marks the beginning of "log size" string
    static int logSizeStrLength = 9; // "log size"
    static int asciiPosOfZeroChar = 48; // char "0" has value 48 in ascii table

    const int last = m_data.size() - 1;
    int logSize = 0, idx = m_fixme_start;
    int logEnd, revEnd;

    // direct access is faster then QByteArray.at()
    if (m_data.isEmpty())
        return -1;
    const char* data = m_data.constData();
    char* fixup = const_cast<char*>(data); // to build '\0' terminating strings

    if (m_fixme_start + shaXEndlLength > last) // at least sha header must be present
        return -1;

    if (data[m_fixme_start] == finalOutputMarker) // "Final output", let caller handle this
        return (m_data.indexOf('\n', m_fixme_start) != -1 ? -2 : -1);

    // parse   'log size xxx\n'   if present -- from git ref. spec.
    if (data[idx] == logSizeMarker) {
        idx += logSizeStrLength; // move idx to beginning of log size value

        // parse log size value
        int digit;
        while ((digit = data[idx++]) != '\n')
            logSize = logSize * 10 + digit - asciiPosOfZeroChar;
    }
    // idx points to the boundary information, which has the same length as an sha header.
    if (++idx + shaXEndlLength > last)
        return error;

    m_fixme_shaStart = idx;

    // ok, now shaStart is valid but msgSize could be still 0 if not available
    logEnd = m_fixme_shaStart - 1 + logSize;
    if (logEnd > last)
        return error;

    idx += shaLength; // now points to 'X' place holder

    fixup[idx] = '\0'; // we want sha to be a '\0' terminated ascii string

    m_fixme_parentsCnt = 0;

    if (data[idx + 2] == '\n') // initial revision
        ++idx;
    else do {
        m_fixme_parentsCnt++;
        idx += shaEndlLength;

        if (idx + 1 >= last)
            break;

        fixup[idx] = '\0'; // we want parents '\0' terminated

    } while (data[idx + 1] != '\n');

    ++idx; // now points to the trailing '\n' of sha line

    // check for !msgSize
    if (withDiff || !logSize) {

        revEnd = (logEnd > idx) ? logEnd - 1: idx;
        revEnd = m_data.indexOf('\0', revEnd + 1);
        if (revEnd == -1)
            return -1;

    } else
        revEnd = logEnd;

    if (revEnd > last) // after this point we know to have the whole record
        return error;

    // ok, now revEnd is valid but logEnd could be not if !logSize
    // in case of diff we are sure content will be consumed so
    // we go all the way
    if (quick && !withDiff)
        return ++revEnd;

    // commiter
    m_fixme_committerStart = ++idx;
    idx = m_data.indexOf('\n', idx); // committer line end
    if (idx == -1) {
        dbs("ASSERT in indexData: unexpected end of data");
        return -1;
    }

    // author
    m_fixme_authorStart = ++idx;
    idx = m_data.indexOf('\n', idx); // author line end
    if (idx == -1) {
        dbs("ASSERT in indexData: unexpected end of data");
        return -1;
    }

    // author date in Unix format (seconds since epoch)
    m_fixme_authorDateStart = ++idx;
    idx = m_data.indexOf('\n', idx); // author date end without '\n'
    if (idx == -1) {
        dbs("ASSERT in indexData: unexpected end of data");
        return -1;
    }
    // if no error, point to trailing \n
    ++idx;

    m_fixme_diffStart = m_fixme_diffLen = 0;
    if (withDiff) {
        m_fixme_diffStart = logSize ? logEnd : m_data.indexOf("\ndiff ", idx);

        if (m_fixme_diffStart != -1 && m_fixme_diffStart < revEnd)
            m_fixme_diffLen = revEnd - ++m_fixme_diffStart;
        else
            m_fixme_diffStart = 0;
    }
    if (!logSize)
        logEnd = m_fixme_diffStart ? m_fixme_diffStart : revEnd;

    // ok, now logEnd is valid and we can handle the log
    m_fixme_sLogStart = idx;

    if (logEnd < m_fixme_sLogStart) { // no shortlog no longLog

        m_fixme_sLogStart = m_fixme_sLogLen = 0;
        m_fixme_lLogStart = m_fixme_lLogLen = 0;
    } else {
        m_fixme_lLogStart = m_data.indexOf('\n', m_fixme_sLogStart);
        if (m_fixme_lLogStart != -1 && m_fixme_lLogStart < logEnd - 1) {

            m_fixme_sLogLen = m_fixme_lLogStart - m_fixme_sLogStart; // skip sLog trailing '\n'
            m_fixme_lLogLen = logEnd - m_fixme_lLogStart; // include heading '\n' in long log

        } else { // no longLog
            m_fixme_sLogLen = logEnd - m_fixme_sLogStart;
            if (data[m_fixme_sLogStart + m_fixme_sLogLen - 1] == '\n')
                m_fixme_sLogLen--; // skip trailing '\n' if any

            m_fixme_lLogStart = m_fixme_lLogLen = 0;
        }
    }
    m_fixme_indexed = true;
    return ++revEnd;
}

#ifndef QROSINTERFACE_BASE_H
#define QROSINTERFACE_BASE_H

#include "Comm.h"

class QROSVersion
{
	union
	{
		struct
		{
			quint8 rvL;	// The "lowest" version part.
			quint8 rvM;	// the "medium" version part.
			quint8 rvG;	// The "major" version part
		};
		quint32 compactedVersion;
	}m_rv;

public:
	QROSVersion() { m_rv.compactedVersion = 0; }
	QROSVersion(const char *s) { fromString(s); }
	QROSVersion(const QString &s) { fromString(s); }
	QROSVersion(const QROSVersion &rv) {m_rv = rv.m_rv; }

	void clear() { m_rv.compactedVersion = 0; }
	bool fromString( const QString &s );
	QString toString() const { return QString("%1.%2.%3").arg(m_rv.rvG).arg(m_rv.rvM).arg(m_rv.rvL); }
	bool isValid() const { return m_rv.compactedVersion != 0; }

	bool operator== (const QROSVersion &rv) const	{ return m_rv.compactedVersion == rv.m_rv.compactedVersion; }
	bool operator>= (const QROSVersion &rv) const	{ return m_rv.compactedVersion >= rv.m_rv.compactedVersion; }
	bool operator<= (const QROSVersion &rv) const	{ return m_rv.compactedVersion <= rv.m_rv.compactedVersion; }
	bool operator> (const QROSVersion &rv) const	{ return m_rv.compactedVersion > rv.m_rv.compactedVersion; }
	bool operator< (const QROSVersion &rv) const	{ return m_rv.compactedVersion < rv.m_rv.compactedVersion; }
};

class QROSVersionChecker : public QList<QROSVersion>
{
public:
	void addValidVersions(const QROSVersion &min, const QROSVersion &max) { append(min); append(max); }
	int getValidVersionPOS(const QROSVersion &rv);
	QString toString();
};

/**
 * @brief The QROSDataBase class
 * Error class returned by several funcions.
 * Use it to comunicate through singal-slot on interface errors.
 */
class QROSInterfaceError
{
public:
	enum ErrorCode
	{
		NoEror = 0,
		ROSVersionUnkown,
		ROSVersionMiss,
		ROSVersionLow,
		ROSVersionHight,
		ROSVersionIncompatible,
		ROSBadConfig
	};

private:
	ErrorCode m_errorCode;
	QString m_errorText;

public:
	QROSInterfaceError() : m_errorCode(NoEror) { }
	QROSInterfaceError(ErrorCode ec, const QString &es) : m_errorCode(ec), m_errorText(es) { }
	QROSInterfaceError(const QROSInterfaceError &ce) : m_errorCode(ce.m_errorCode), m_errorText(ce.m_errorText) { }

	static QString errorString(ErrorCode e, const QString &errorText);
	QString errorString() { return errorString(errorCode(), errorText()); }

	void setError(ErrorCode e, const QString &text = QString()) { m_errorCode = e; m_errorText = text; }
	ErrorCode errorCode() const { return m_errorCode;		}
	QString errorText() const { return m_errorText;	}
	void clearError() { m_errorCode = NoEror; m_errorText.clear(); }
};


class QROSInterface_Base : public QObject
{
	Q_OBJECT

protected:

	ROS::Comm *m_mktComm;
	QROSVersion m_rosVersion;
	QROSVersionChecker m_rosVersionChecker;
	int m_validVersionIndex;
	QROSInterfaceError m_lastError;
	QString m_id;
	qint64 m_requestTime;
	qint64 m_responseTime;

	void setID(const QString &id) { m_id = id; }

	void setError(QROSInterfaceError::ErrorCode e, const QString &text = QString());
	void clearError();

	/**
	 * @brief doRequest
	 * Overload this function to make the request to the ROS
	 * @param rv The router QROSVersion to know if it is possible to perform the request.
	 * @return true/false if the request is possible.
	 */
	virtual bool doRequest() = 0;
	virtual bool parseResponse(const ROS::QSentence &s) = 0;
	virtual bool checkROSVersion();
	void addValidVersions(const QROSVersion &min, const QROSVersion &max) { m_rosVersionChecker.addValidVersions(min, max); }
	int versionIndex() const;

public:
	QROSInterface_Base(ROS::Comm *mktComm, QObject *papi);

	bool setROSVersion(const QROSVersion &rv) { m_rosVersion = rv; return checkROSVersion(); }
	const QROSVersion rosVersion() const { return m_rosVersion; }

	ROS::Comm *mktComm() const { return m_mktComm; }

	const QString &id() const { return m_id; }

	qint64 requestTime() const { return m_requestTime; }
	qint64 responseTime() const { return m_responseTime; }

	const QROSInterfaceError &lastError() const { return m_lastError; }

	void requestData();

private slots:
	void onResponce(const ROS::QSentence &s);

signals:
	void rosInterfaceError(const QROSInterfaceError &);
};

#endif // QROSINTERFACE_BASE_H

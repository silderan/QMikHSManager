#include "Utils.h"

#include <QRegExp>
#include <QStringList>

quint32 Utils::rosTimeToInt(const QString &rosTime)
{
	int weks = 0;
	int days = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	for( int l = 0, i = 0; i < rosTime.length(); i++ )
	{
		switch( rosTime[i].toLatin1() )
		{
		case 'w':
			weks = rosTime.mid(l, i-l).toInt();
			l = i+1;
			break;
		case 'd':
			days = rosTime.mid(l, i-l).toInt();
			l = i+1;
			break;
		case 'h':
			hours = rosTime.mid(l, i-l).toInt();
			l = i+1;
			break;
		case 'm':
			minutes = rosTime.mid(l, i-l).toInt();
			l = i+1;
		case 's':
			seconds = rosTime.mid(l, i-l).toInt();
			break;
		}
	}
	return (weks * 7 * 24 * 60 * 60) + (days * 24 * 60 * 60) + (hours * 60 * 60) + (minutes * 60) + seconds;
}

QString Utils::readableBits(quint64 ll, bool isSpeed)
{
	QString rtn;
	int tmp;

	if( (tmp = (ll >> 40)) )
		rtn = QString(isSpeed ? "%1.%2 Tb/s" : "%1.%2 Tbits").arg(tmp).arg(QString("%1").arg(ll & 0xFFC0000000LL, 2).left(2));
	else
	if( (tmp = (ll >> 30)) )
		rtn = QString(isSpeed ? "%1.%2 Gb/s" : "%1.%2 Gbits").arg(tmp).arg(QString("%1").arg(ll & 0x3FF00000, 2).left(2));
	else
	if( (tmp = (ll >> 20)) )
		rtn = QString(isSpeed ? "%1.%2 Mb/s" : "%1.%2 Mbits").arg(tmp).arg(QString("%1").arg(ll & 0xFFC00, 2).left(2));
	else
	if( (tmp = (ll >> 10)) )
		rtn = QString(isSpeed ? "%1.%2 Kb/s" : "%1.%2 Kbits").arg(tmp).arg(QString("%1").arg(ll & 0x3FF, 2).left(2));
	else
		rtn = QString(isSpeed ? "%1 bits/s" : "%1 bits").arg(ll);

	return rtn;
}


QString Utils::readableBytes(quint64 ll, bool isSpeed)
{
	QString rtn;
	int tmp;

	if( (tmp = (ll >> 40)) )
		rtn = QString(isSpeed ? "%1.%2 TB/s" : "%1.%2 TBytes").arg(tmp).arg(QString("%1").arg(ll & 0xFFC0000000LL, 2).left(2));
	else
	if( (tmp = (ll >> 30)) )
		rtn = QString(isSpeed ? "%1.%2 GB/s" : "%1.%2 GBytes").arg(tmp).arg(QString("%1").arg(ll & 0x3FF00000, 2).left(2));
	else
	if( (tmp = (ll >> 20)) )
		rtn = QString(isSpeed ? "%1.%2 MB/s" : "%1.%2 MBytes").arg(tmp).arg(QString("%1").arg(ll & 0xFFC00, 2).left(2));
	else
	if( (tmp = (ll >> 10)) )
		rtn = QString(isSpeed ? "%1.%2 KB/s" : "%1.%2 KBytes").arg(tmp).arg(QString("%1").arg(ll & 0x3FF, 2).left(2));
	else
		rtn = QString(isSpeed ? "%1 bytes/s" : "%1 bytes").arg(ll);

	return rtn;
}

QString Utils::readableTime(quint64 ll)
{
	return QString("%1").arg(ll);
}

int Utils::ipToInt(QString ip)
{
	int rtn = 0;
	int maskPrefix = ip.indexOf('/');
	if( maskPrefix != -1 )
		ip = ip.left(maskPrefix);
	foreach( QString n, ip.split(".") )
	{
		rtn <<= 8;
		rtn += n.toInt();
	}
	return rtn;
}

QString Utils::intToIPv4(quint32 i, qint8 bits)
{
	if( (bits >= 0) && (bits <= 32) )
		return QString("%1.%2.%3.%4/%5").arg((i>>24) & 0xFF).arg((i>>16) & 0xFF).arg((i>>8) & 0xFF).arg(i & 0xFF).arg(bits);
	else
		return QString("%1.%2.%3.%4").arg((i>>24) & 0xFF).arg((i>>16) & 0xFF).arg((i>>8) & 0xFF).arg(i & 0xFF);
}


Utils::IPv4 Utils::IPv4Range::ip(int i) const
{
	Q_ASSERT( (i>=0) && (i < count()) );

	return IPv4(m_lo + i);
}

void Utils::IPv4Range::fromString(const QString &s)
{
	QStringList l = s.split("-");
	if( l.count() > 1 )
		setRange(l[0], l[1]);
}

void Utils::IPv4Ranges::fromString(const QString &s)
{
	clear();
	foreach(QString s, s.split(","))
		append(IPv4Range(s));
}

bool Utils::IPv4Ranges::inRange(const Utils::IPv4 &ip) const
{
	for( int i = 0; i < count(); i++ )
		if( at(i).inRange(ip) )
			return true;
	return false;
}

QString Utils::IPv4Net::toString(bool networkAddress, bool includeNetbits) const
{
	return networkAddress
			? Utils::intToIPv4((uint)*this & netmask(), includeNetbits ? netbits() : -1)
			: Utils::intToIPv4((uint)*this, includeNetbits ? netbits() : -1);
}

const Utils::IPv4Net &Utils::IPv4Net::fromString(QString ip)
{
	int maskSeparator = ip.indexOf('/');
	int rtn = 0;
	if( maskSeparator != -1 )
	{
		setBits( (quint8)ip.right(ip.length()-maskSeparator-1).toInt() );
		ip = ip.left(maskSeparator);
	}
	else
		setBits(32);
	foreach( QString n, ip.split(".") )
	{
		rtn <<= 8;
		rtn += n.toInt();
	}
	IPv4::operator =(rtn);
	return *this;
}

bool Utils::IPv4Net::inRange(const Utils::IPv4 &ip)
{
	return (((uint)ip) & netmask()) == (((uint)*this) & netmask());
}

quint32 Utils::v4bitsToNetmask(quint8 bits)
{
	static const quint32 bitsTable[] =
	{
		0,
		0x80000000, 0xC0000000, 0xE0000000, 0xF0000000,
		0xF8000000, 0xFC000000, 0xFE000000, 0xFF000000,
		0xFF800000, 0xFFC00000, 0xFFE00000, 0xFFF00000,
		0xFFF80000, 0xFFFC0000, 0xFFFE0000, 0xFFFF0000,
		0xFFFF8000, 0xFFFFC000, 0xFFFFE000, 0xFFFFF000,
		0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 0xFFFFFF00,
		0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 0xFFFFFFF0,
		0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE, 0xFFFFFFFF
	};
	return bitsTable[ (bits > 32) ? 32 : bits ];
}

quint8 Utils::v4NetmaskToBits(quint32 netmask)
{
	quint8 c = 0;
	while( netmask )
	{
		c++;
		netmask >>= 1;
	}
	return c;
}

#ifndef QT_NO_DEBUG
void Utils::UnitTests()
{
	IPv4Net ip("1.1.1.1/24");
	Q_ASSERT( ip.toString(false, true) == "1.1.1.1/24" );
	Q_ASSERT( ip.toString(false, false) == "1.1.1.1" );
	Q_ASSERT( ip.toString(true, false) == "1.1.1.0" );
	Q_ASSERT( ip.toString(true, true) == "1.1.1.0/24" );
	Q_ASSERT( ip.inRange(IPv4("1.1.1.1")) );
	Q_ASSERT( ip.inRange(IPv4("1.1.1.100")) );
	Q_ASSERT( ip.inRange(IPv4("1.1.1.255")) );
	Q_ASSERT( !ip.inRange(IPv4("1.1.2.1")) );
}
#endif

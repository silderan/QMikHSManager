#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QList>

#define BITS_TO_BYTES(b)	(b>>3)
#define BYTES_TO_BITS(B)	(B<<3)

namespace Utils
{
	quint32 rosTimeToInt(const QString &rosTime);
	QString readableBits(quint64 ll, bool isSpeed);
	QString readableBytes(quint64 ll, bool isSpeed);
	QString readableTime(quint64 ll);
	qint32 ipToInt(QString ip);
	QString intToIPv4(quint32 i, qint8 bits = -1);
	quint32 v4bitsToNetmask(quint8 bits);
	quint8 v4NetmaskToBits(quint32 netmask);

	class IPv4
	{
		uint m_ip;

	public:
		IPv4() : m_ip(0) { }
		IPv4(uint ip) : m_ip(ip) { }
		IPv4(const IPv4 &ip) : m_ip(ip.m_ip) { }
		IPv4(const QString &ip) : m_ip(ipToInt(ip)) { }
		IPv4(const char *ip) : m_ip(ipToInt(ip)) { }
		operator int() const { return (int)m_ip; }
		operator uint() const { return m_ip; }
		IPv4 operator+(const IPv4 &other) const		{ return IPv4(m_ip+other.m_ip);	}
		IPv4 operator-(const IPv4 &other) const		{ return IPv4(m_ip-other.m_ip);	}
		IPv4 operator+(int i) const					{ return IPv4(m_ip+i);			}
		IPv4 operator-(int i) const					{ return IPv4(m_ip-i);			}
		IPv4 operator++()							{ m_ip++; return *this;			}
		IPv4 operator--()							{ m_ip--; return *this;			}
		IPv4 operator+=(const IPv4 &o)				{ m_ip+=o.m_ip; return *this;	}
		IPv4 operator-=(const IPv4 &o)				{ m_ip-=o.m_ip; return *this;	}
		IPv4 operator+=(int i)						{ m_ip+=i; return *this;		}
		IPv4 operator-=(int i)						{ m_ip-=i; return *this;		}
		bool operator<(const IPv4 &other) const		{ return m_ip < other.m_ip;		}
		bool operator<=(const IPv4 &other) const	{ return m_ip <= other.m_ip;	}
		bool operator>(const IPv4 &other) const		{ return m_ip > other.m_ip;		}
		bool operator>=(const IPv4 &other) const	{ return m_ip >= other.m_ip;	}
		bool operator==(const IPv4 &other) const	{ return m_ip == other.m_ip;	}
		IPv4 &operator=(int other) { m_ip = (uint)other; return *this; }
		IPv4 &operator=(uint other) { m_ip = other; return *this; }
		QString toString() const { return intToIPv4(m_ip); }
		void fromString(const QString &s) { m_ip = ipToInt(s); }
	};

	class IPv4Mask
	{
		quint8 m_bits;	// Ex. 24
		quint32 m_mask;	// Ex. 0xFFFF00 (255.255.255.0)

	public:
		IPv4Mask() : m_bits(0), m_mask(0) { }
		IPv4Mask(quint8 bits) : m_bits(bits), m_mask(v4bitsToNetmask(bits)) { }
		IPv4Mask(quint32 netmask) : m_bits(v4NetmaskToBits(netmask)), m_mask(netmask) { }
		quint32 netmask() const { return m_mask; }
		quint8 netbits() const { return m_bits; }
		QString toString(bool bitcount = false) const { return bitcount ? QString::number(m_bits) : intToIPv4(m_mask, false); }

		void setBits(quint8 b) { m_bits = b; m_mask = v4bitsToNetmask(b); }
		void setMask(quint32 m) { m_mask = m; m_bits = v4NetmaskToBits(m); }
	};

	class IPv4Net : public IPv4, public IPv4Mask
	{
	public:
		IPv4Net() : IPv4(), IPv4Mask() { }
		IPv4Net(const IPv4Net &ipn) : IPv4((const IPv4 &)ipn), IPv4Mask((const IPv4Mask &)ipn) { }
		IPv4Net(const IPv4 &ip) : IPv4((const IPv4 &)ip), IPv4Mask() { }
		IPv4Net(const char *s)		{ fromString(s); }
		IPv4Net(const QString &s)	{ fromString(s); }

		QString toString(bool networkAddress, bool includeNetbits) const;
		const IPv4Net &fromString(QString ip);
		bool inRange(const IPv4 &ip);
	};

	class IPv4Range
	{
	private:
		IPv4 m_lo;
		IPv4 m_hi;

	public:
		IPv4Range() : m_lo((uint)0), m_hi((uint)0) { }
		IPv4Range(const IPv4Range &ips) { m_lo = ips.m_lo; m_hi = ips.m_hi; }
		IPv4Range(const IPv4 &lo, const IPv4 &hi) { setRange(lo, hi); }
		IPv4Range(const char *s)	{ fromString(s); }
		IPv4Range(const QString &s)	{ fromString(s); }

		const IPv4 &lowIP() const	{ return m_lo; }
		const IPv4 &highIP() const	{ return m_hi; }

		void setLoIP(const IPv4 &ip) { m_lo = ip; }
		void setHiIP(const IPv4 &ip) { m_hi = ip; }
		void setRange(const IPv4 &ipLo, const IPv4 &ipHi)
		{
			if( ipLo > ipHi )
			{
				setLoIP(ipHi);
				setHiIP(ipLo);
			}
			else
			{
				setLoIP(ipLo);
				setHiIP(ipHi);
			}
		}

		bool isValid() const { return m_lo <= m_hi; }
		int count() const { return (int)m_hi - (int)m_lo ; }
		IPv4 first() const { return m_lo; }
		IPv4 last() const { return m_hi; }
		IPv4 ip(int i) const;
		IPv4 next(const IPv4 &ipv4) const	{ return ipv4+1;	}
		void fromString(const QString &s);
		bool inRange(const IPv4 &ip) const { return (ip >= m_lo) && (ip <= m_hi); }
	};

	class IPv4Ranges : public QList<IPv4Range>
	{
	public:
		IPv4Ranges() {}
		IPv4Ranges(const IPv4Range &ir) { append(ir); }
		IPv4Ranges(const char *s)		{ fromString(s); }
		IPv4Ranges(const QString &s)	{ fromString(s); }

		void fromString(const QString &s);
		void append(const IPv4Range &ir) { QList::append(ir); }
		void append(const IPv4 &ipLo, const IPv4 &ipHi) { append(IPv4Range(ipLo, ipHi)); }
		bool inRange(const IPv4 &ip) const;
	};

#ifndef QT_NO_DEBUG
	void UnitTests();
#endif
}

#endif // UTILS_H

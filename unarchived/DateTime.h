#ifndef __FDK_DATETIME_H_INCLUDE__
#define __FDK_DATETIME_H_INCLUDE__
#include <fdk/Base.h>
#include <string>
#include <time.h>

namespace fdk
{	
	typedef time_t UnixTimestamp;  // ��UTC��׼ʱ���(1970��1��1��0��0��0��)��ʼ�����������ֻ�ܱ�ʾ��2038��
	const int UNIXTIMESTAMP_BASE = 0; // UTC��׼ʱ���
	const int MONTH_MIN = 1;
	const int MONTH_MAX = 12;
	const int DAY_MIN = 1;
	const int DAY_MAX = 31;
	const int HOUR_MIN = 0;
	const int HOUR_MAX = 23;
	const int MINUTE_MIN = 0;
	const int MINUTE_MAX = 59;
	const int SECOND_MIN = 0;
	const int SECOND_MAX = 59;
	const int YEAR_MONTHS = 12;
	const int WEEK_DAYS = 7;
	const int DAY_HOURS = 24;
	const int HOUR_MINUTES = 60;
	const int MINUTE_SECONDS = 60;
	const int HOUR_SECONDS = HOUR_MINUTES * MINUTE_SECONDS;
	const int DAY_SECONDS = DAY_HOURS * HOUR_SECONDS;
	const int WEEK_SECONDS = WEEK_DAYS * DAY_SECONDS;

	FDK_API bool isLeapYear(int year);
	FDK_API int getDaysOfYearMonth(int year, int month);
	FDK_API bool isValidDate(int year, int month, int day);
	FDK_API bool isValidTime(int hour, int minute, int second);
	
	// ��ʾ������UTC��׼���ڵı�������
	class FDK_API Date
	{
	public:
		static const Date& BASE(); // UTC��׼ʱ����Ӧ�ı�������
		static Date today(); // ���ص�ǰ�ı�������
		Date(); // as base()
		Date(int year, int month, int day); // ����������ڷǷ���Ч��ͬĬ�Ϲ���
		bool reset(int year, int month, int day);
		void addDays(int days);
		bool fromUnixTimestamp(UnixTimestamp timestamp);
		// UnixTimestamp toUnixTimestamp() const; ���ṹ��֧��ת��ΪUnixʱ�������Ϊʱ���Ѿ���ʧ
		std::string toString(const char* fmt="%Y-%m-%d") const;
		int year() const;
		int month() const;
		int day() const;
		bool operator==(const Date& o) const;
		bool operator<(const Date& o) const;
	private:
		int m_year;
		int m_month;
		int m_day;
	};

	// ��ʾ��00:00:00-23:59:59��ʱ��
	class FDK_API Time
	{
	public:
		static const Time& BASE(); // 00:00:00
		static Time now(); // ���ص�ǰ�ı���ʱ��
		Time(); // as base()
		Time(int hour, int minute, int second); // �������ʱ��Ƿ���Ч��ͬĬ�Ϲ���
		bool reset(int hour, int minute, int second); // �������ʱ��Ƿ��򷵻�false		
		void addSeconds(int seconds); // ��������������0-3599��Χ�����ڻ���
		void fromSeconds(int seconds); // ����0-3599��Χ�����ڻ���		
		int toSeconds() const; // ת��Ϊ��0��0��0�뿪ʼ������		
		std::string toString(const char* fmt="%H:%M:%S") const;
		int hour() const;
		int minute() const;
		int second() const;
		bool operator==(const Time& o) const;
		bool operator<(const Time& o) const;
	private:
		int m_hour;
		int m_minute;
		int m_second;
	};

	// ��ʾ������UTC��׼ʱ��ı���ʱ��
	class FDK_API DateTime
	{
	public:
		static const DateTime& BASE(); // UTC��׼ʱ����Ӧ�ı���ʱ��
		static DateTime now(); // ���ص�ǰ�ı���ʱ��
		DateTime(); // as base()
		DateTime(int year, int month, int day, int hour=0, int minute=0, int second=0); // �������ʱ��Ƿ���Ч��ͬĬ�Ϲ���
		bool reset(int year, int month, int day, int hour=0, int minute=0, int second=0); // �������ʱ��Ƿ��򷵻�false
		bool addSeconds(int seconds); // ע�ⲻ���ǵ���Խ�������
		bool fromUnixTimestamp(UnixTimestamp timestamp);
		UnixTimestamp toUnixTimestamp() const;
		std::string toString(const char* fmt="%Y-%m-%d %H:%M:%S") const;		
		int year() const;
		int month() const;
		int day() const;
		int hour() const;
		int minute() const;
		int second() const;
		bool operator==(const DateTime& o) const;
		bool operator<(const DateTime& o) const;
	private:
		int m_year;
		int m_month;
		int m_day;
		int m_hour;
		int m_minute;
		int m_second;
	};

	FDK_API Date getDate(const DateTime& dateTime);
	FDK_API Time getTime(const DateTime& dateTime);
	FDK_API DateTime makeDateTime(const Date& date, const Time& time);

	inline const Date& Date::BASE()
	{
		static Date instance;
		return instance;
	}

	inline int Date::year() const
	{
		return m_year;
	}

	inline int Date::month() const
	{
		return m_month;
	}

	inline int Date::day() const
	{
		return m_day;
	}

	inline const Time& Time::BASE()
	{
		static Time instance;
		return instance;
	}

	inline int Time::hour() const
	{
		return m_hour;
	}

	inline int Time::minute() const
	{
		return m_minute;
	}

	inline int Time::second() const
	{
		return m_second;
	}

	inline const DateTime& DateTime::BASE()
	{
		static DateTime instance;
		return instance;
	}

	inline int DateTime::year() const
	{
		return m_year;
	}

	inline int DateTime::month() const
	{
		return m_month;
	}

	inline int DateTime::day() const
	{
		return m_day;
	}

	inline int DateTime::hour() const
	{
		return m_hour;
	}

	inline int DateTime::minute() const
	{
		return m_minute;
	}

	inline int DateTime::second() const
	{
		return m_second;
	}
}

#endif

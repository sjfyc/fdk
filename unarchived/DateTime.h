#ifndef __FDK_DATETIME_H_INCLUDE__
#define __FDK_DATETIME_H_INCLUDE__
#include <fdk/Base.h>
#include <string>
#include <time.h>

namespace fdk
{	
	typedef time_t UnixTimestamp;  // 从UTC基准时间点(1970年1月1号0点0分0秒)开始的秒数，最多只能表示到2038年
	const int UNIXTIMESTAMP_BASE = 0; // UTC基准时间点
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
	
	// 表示不早于UTC基准日期的本地日期
	class FDK_API Date
	{
	public:
		static const Date& BASE(); // UTC基准时间点对应的本地日期
		static Date today(); // 返回当前的本地日期
		Date(); // as base()
		Date(int year, int month, int day); // 如果给定日期非法则效果同默认构造
		bool reset(int year, int month, int day);
		void addDays(int days);
		bool fromUnixTimestamp(UnixTimestamp timestamp);
		// UnixTimestamp toUnixTimestamp() const; 本结构不支持转换为Unix时间戳，因为时间已经丢失
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

	// 表示从00:00:00-23:59:59的时间
	class FDK_API Time
	{
	public:
		static const Time& BASE(); // 00:00:00
		static Time now(); // 返回当前的本地时间
		Time(); // as base()
		Time(int hour, int minute, int second); // 如果给定时间非法则效果同默认构造
		bool reset(int hour, int minute, int second); // 如果给定时间非法则返回false		
		void addSeconds(int seconds); // 增减秒数，超出0-3599范围按周期换算
		void fromSeconds(int seconds); // 超出0-3599范围按周期换算		
		int toSeconds() const; // 转换为从0点0分0秒开始的秒数		
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

	// 表示不早于UTC基准时间的本地时间
	class FDK_API DateTime
	{
	public:
		static const DateTime& BASE(); // UTC基准时间点对应的本地时间
		static DateTime now(); // 返回当前的本地时间
		DateTime(); // as base()
		DateTime(int year, int month, int day, int hour=0, int minute=0, int second=0); // 如果给定时间非法则效果同默认构造
		bool reset(int year, int month, int day, int hour=0, int minute=0, int second=0); // 如果给定时间非法则返回false
		bool addSeconds(int seconds); // 注意不考虑叠加越界的问题
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

#include <fdk/DateTime.h>
#include <sstream>
#include <iomanip>
#pragma warning(disable: 4996)

namespace fdk
{
	namespace
	{
		void makeTimeStruct(tm& output, int year, int month, int day, int hour, int minute, int second)
		{
			zeroMemory(output);
			output.tm_year = year - 1900;
			output.tm_mon = month - 1;
			output.tm_mday = day;
			output.tm_hour = hour;
			output.tm_min = minute;
			output.tm_sec = second;
		}

		bool fromUnixTimestamp(int& year, int& month, int& day, int& hour, int& minute, int& second, UnixTimestamp timestamp)
		{
			time_t time_t_object = static_cast<time_t>(timestamp);
			tm* timeStruct = localtime(&time_t_object);
			if (!timeStruct)
			{
				return false;
			}
			year = timeStruct->tm_year +1900;
			month = timeStruct->tm_mon + 1;
			day = timeStruct->tm_mday;
			hour = timeStruct->tm_hour;
			minute = timeStruct->tm_min;
			second = timeStruct->tm_sec;
			return true;
		}

		UnixTimestamp toUnixTimestamp(int year, int month, int day, int hour, int minute, int second)
		{
			tm timeStruct;
			makeTimeStruct(timeStruct, year, month, day, hour, minute, second);
			return static_cast<UnixTimestamp>(mktime(&timeStruct));
		}
	}

	FDK_API bool isLeapYear(int year)
	{
		// 四年一闰，百年不闰，四百年闰
		return ( (year%4==0) && (year%100!=0) ) || (year%400==0);
	}

	FDK_API int getDaysOfYearMonth(int year, int month)
	{
		if (month < MONTH_MIN || month > MONTH_MAX)
		{
			return 0;
		}
		if (month == 2) 
		{
			return isLeapYear(year) ? 29 : 28;
		}
		static const int monthDaysTable[] =
		{
			31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
		};		
		return monthDaysTable[month-1];
	}

	FDK_API bool isValidDate(int year, int month, int day)
	{
		if (day < DAY_MIN || day > getDaysOfYearMonth(year, month))
		{
			return false;
		}
		return true;
	}

	FDK_API bool isValidTime(int hour, int minute, int second)
	{
		return hour >= HOUR_MIN && hour <= HOUR_MAX &&
			minute >= MINUTE_MIN && minute <= MINUTE_MAX &&
			second >= SECOND_MIN && second <= SECOND_MAX;
	}

	Date Date::today()
	{
		DateTime dateTime = DateTime::now();
		return Date(dateTime.year(), dateTime.month(), dateTime.day());
	}

	Date::Date()
	{
		m_year = DateTime::BASE().year();
		m_month = DateTime::BASE().month();
		m_day = DateTime::BASE().day();
	}

	Date::Date(int year, int month, int day)
	{
		m_year = DateTime::BASE().year();
		m_month = DateTime::BASE().month();
		m_day = DateTime::BASE().day();
		reset(year, month, day);
	}

	bool Date::reset(int year, int month, int day)
	{
		if (!isValidDate(year, month, day))
		{
			return false;
		}
		UnixTimestamp timestamp = toUnixTimestamp(year, month, day, 
			DateTime::BASE().hour(), DateTime::BASE().minute(), DateTime::BASE().second());
		if (timestamp < UNIXTIMESTAMP_BASE)
		{
			return false;
		}
		m_year = year;
		m_month = month;
		m_day = day;
		return true;
	}

	void Date::addDays(int days)
	{
		UnixTimestamp timestamp = toUnixTimestamp(m_year, m_month, m_day, 23, 59, 59);
		FDK_ASSERT(timestamp >= UNIXTIMESTAMP_BASE);
		fromUnixTimestamp(timestamp + days * DAY_SECONDS);
	}

	bool Date::fromUnixTimestamp(UnixTimestamp timestamp)
	{
		int temp;
		return fdk::fromUnixTimestamp(m_year, m_month, m_day, temp, temp, temp, timestamp);
	}

	std::string Date::toString(const char* fmt) const
	{
		tm timeStruct;
		makeTimeStruct(timeStruct, m_year, m_month, m_day, 23, 59, 59);
		strftime(__buffer__, __bufferSize__, fmt, &timeStruct);
		return __buffer__;
	}

	bool Date::operator==(const Date& o) const
	{
		return m_year == o.m_year &&
			m_month == o.m_month &&
			m_day == o.m_day;
	}

	bool Date::operator<(const Date& o) const
	{
		if (m_year != o.m_year) { return m_year < o.m_year; }
		if (m_month != o.m_month) { return m_month < o.m_month; }
		if (m_day != o.m_day) { return m_day < o.m_day; }
		return false;
	}

	Time Time::now()
	{
		DateTime dateTime = DateTime::now();
		return Time(dateTime.hour(), dateTime.minute(), dateTime.second());
	}

	Time::Time()
		: m_hour(0)
		, m_minute(0)
		, m_second(0)
	{
	}

	Time::Time(int hour, int minute, int second)
		: m_hour(0)
		, m_minute(0)
		, m_second(0)
	{
		reset(hour, minute, second);
	}

	bool Time::reset(int hour, int minute, int second)
	{
		if (!isValidTime(hour, minute, second))
		{
			return false;
		}
		m_hour = hour;
		m_minute = minute;
		m_second = second;
		return true;
	}

	void Time::addSeconds(int seconds)
	{
		fromSeconds(toSeconds() + seconds);
	}

	void Time::fromSeconds(int seconds)
	{
		seconds %= DAY_SECONDS;
		while (seconds < 0)
		{
			seconds += DAY_SECONDS;
		}
		m_hour = seconds / HOUR_SECONDS;
		m_minute = (seconds - (m_hour * HOUR_SECONDS)) / MINUTE_SECONDS;
		m_second = seconds % MINUTE_SECONDS;
	}

	int Time::toSeconds() const
	{
		return m_hour*HOUR_SECONDS + m_minute*MINUTE_SECONDS + m_second;
	}

	std::string Time::toString(const char* fmt) const
	{
		tm timeStruct;
		makeTimeStruct(timeStruct, 2000, 1, 1, m_hour, m_minute, m_second);
		strftime(__buffer__, __bufferSize__, fmt, &timeStruct);
		return __buffer__;
	}

	bool Time::operator==(const Time& o) const
	{
		return m_hour == o.m_hour &&
			m_minute == o.m_minute &&
			m_second == o.m_second;
	}

	bool Time::operator<(const Time& o) const
	{
		if (m_hour != o.m_hour) { return m_hour < o.m_hour; }
		if (m_minute != o.m_minute) { return m_minute < o.m_minute; }
		if (m_second != o.m_second) { return m_second < o.m_second; }
		return false;
	}

	DateTime DateTime::now()
	{
		DateTime ret;
		ret.fromUnixTimestamp(static_cast<UnixTimestamp>(time(0)));
		return ret;
	}

	DateTime::DateTime()
	{
		fromUnixTimestamp(UNIXTIMESTAMP_BASE);
	}

	DateTime::DateTime(int year, int month, int day, int hour, int minute, int second)
	{
		fromUnixTimestamp(UNIXTIMESTAMP_BASE);
		reset(year, month, day, hour, minute, second); 
	}

	bool DateTime::reset(int year, int month, int day, int hour, int minute, int second)
	{
		if (!isValidDate(year, month, day))
		{
			return false;
		}
		if (!isValidTime(hour, minute, second))
		{
			return false;
		}
		if (fdk::toUnixTimestamp(year, month, day, hour, minute, second) < UNIXTIMESTAMP_BASE)
		{
			return false;
		}
		m_year = year;
		m_month = month;
		m_day = day;
		m_hour = hour;
		m_minute = minute;
		m_second = second;
		return true;
	}

	bool DateTime::addSeconds(int seconds)
	{
		return fromUnixTimestamp(toUnixTimestamp() + seconds);
	}

	bool DateTime::fromUnixTimestamp(UnixTimestamp timestamp)
	{
		return fdk::fromUnixTimestamp(m_year, m_month, m_day, m_hour, m_minute, m_second, timestamp);
	}

	UnixTimestamp DateTime::toUnixTimestamp() const
	{
		UnixTimestamp timestamp = fdk::toUnixTimestamp(m_year, m_month, m_day, m_hour, m_minute, m_second);
		FDK_ASSERT(timestamp >= UNIXTIMESTAMP_BASE);
		return timestamp;
	}

	std::string DateTime::toString(const char* fmt) const
	{
		tm timeStruct;
		makeTimeStruct(timeStruct, m_year, m_month, m_day, m_hour, m_minute, m_second);
		strftime(__buffer__, __bufferSize__, fmt, &timeStruct);
		return __buffer__;
	}

	bool DateTime::operator==(const DateTime& o) const
	{
		return m_year == o.m_year &&
			m_month == o.m_month &&
			m_day == o.m_day &&
			m_hour == o.m_hour &&
			m_minute == o.m_minute &&
			m_second == o.m_second;
	}

	bool DateTime::operator<(const DateTime& o) const
	{
		if (m_year != o.m_year) { return m_year < o.m_year; }
		if (m_month != o.m_month) { return m_month < o.m_month; }
		if (m_day != o.m_day) { return m_day < o.m_day; }
		if (m_hour != o.m_hour) { return m_hour < o.m_hour; }
		if (m_minute != o.m_minute) { return m_minute < o.m_minute; }
		if (m_second != o.m_second) { return m_second < o.m_second; }
		return false;
	}

	FDK_API Date getDate(const DateTime& dateTime)
	{
		return Date(dateTime.year(), dateTime.month(), dateTime.day());
	}

	FDK_API Time getTime(const DateTime& dateTime)
	{
		return Time(dateTime.hour(), dateTime.minute(), dateTime.second());
	}

	FDK_API DateTime makeDateTime(const Date& date, const Time& time)
	{
		return DateTime(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second());
	}
}

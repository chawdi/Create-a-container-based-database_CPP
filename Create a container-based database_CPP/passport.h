#ifndef PASSPORT_H
#define PASSPORT_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <array>
#include "vector.h"

namespace cw {

struct Passport
{
	static constexpr std::array fieldTitles {
		"Name", "Surname", "Series", "Number", "Gender"
	};
	std::array<QString, fieldTitles.size()> fields;

	friend QTextStream& operator>>(QTextStream& is, Passport& p);
	friend QTextStream& operator<<(QTextStream& is, Passport const& p);
	static bool hasAllNumbers(QString const& s) noexcept;
	QString& operator[](char const* key) &;
	bool selfCheck();
};

using DataArray = cw::vector<cw::Passport>;
class DataBase {
public:
	enum class result {success, error, invalid_format, doesnt_exist};
	result loadFromFile(QString const& filename);
	result saveToFile(const QString& filename);
	void move(DataArray&& other) { data = std::move(other); }
	void clear() { data.clear(); }
	DataArray const& get() { return data; }

private:
	DataArray data{};
};

template<typename T, typename Alloc>
bool operator==(const cw::vector<T,Alloc>& lhs, const cw::vector<T,Alloc>& rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	for(auto lit = lhs.begin(), rit = rhs.begin();
	    lit != lhs.end();
	    ++lit, ++rit) {
		if(*lit != *rit)
			return false;
	}
	return true;
}

template<typename T, typename Alloc>
bool operator!=(const cw::vector<T,Alloc>& lhs,
		const cw::vector<T,Alloc>& rhs) {return !(lhs == rhs);}

bool operator==(Passport const& lhs, Passport const& rhs) noexcept;
bool operator!=(Passport const& lhs, Passport const& rhs) noexcept;


} // namespace cw

#endif // PASSPORT_H

#include "passport.h"

namespace cw {

QTextStream& operator>>(QTextStream& is, Passport& p)
{
	for(auto& field: p.fields)
		is >> field;
	return is;
}

QTextStream& operator<<(QTextStream& os, const Passport& p)
{
	for(auto& field: p.fields) {
		os << field << ' ';
	}
	os << '\n';
	return os;
}

bool operator==(const Passport& lhs, const Passport& rhs) noexcept
{
	return (lhs.fields == rhs.fields);
}

bool operator!=(const Passport& lhs, const Passport& rhs) noexcept
{
	return !(lhs == rhs);
}

bool Passport::hasAllNumbers(const QString& s) noexcept
{
	for(auto const& ch: s) {
		if (!(ch >= '0' && ch <= '9'))
			return false;
	}
	return true;
}

QString& Passport::operator[](const char* key) &
{
	for (size_t i = 0; i < fieldTitles.size(); i++) {
		if (!strcmp(key, fieldTitles[i]))
			return fields[i];
	}
	throw std::out_of_range("Invalid key.");
}

bool Passport::selfCheck() {
	return (operator []("Series").length() == 4 &&
		hasAllNumbers(operator []("Series")) &&
			      operator []("Number").length() == 6 &&
			      hasAllNumbers(operator []("Number")) &&
					    (operator []("Gender") == "муж" ||
		 operator []("Gender") == "жен"));
}

DataBase::result DataBase::loadFromFile(const QString& filename)
{
	//data.clear();
	QFile file(filename);
	QTextStream is(&file);

	if (file.open(QIODevice::ReadOnly)) {
		cw::Passport temp;
		while (is >> temp, is.status() == QTextStream::Status::Ok) {
			if (!temp.selfCheck()) {
				file.close();
				data.clear();
				return result::invalid_format;
			}
			data.push_back(std::move(temp));
		}
		file.close();
	} else {
		return result::doesnt_exist;
	}
	if(data.empty())
		return result::error;
	return result::success;
}

DataBase::result DataBase::saveToFile(const QString& filename)
{
	QFile file(filename);
	QTextStream is(&file);
	if (file.open(QIODevice::WriteOnly)) {
		for(auto const& item: data) {
			is << item;
		}
	} else {
		return result::error;
	}
	file.close();
	return result::success;
}

} // namespace cw

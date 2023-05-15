#ifndef ENTRYDIALOG_H
#define ENTRYDIALOG_H

#include <QDialog>
#include <array>
#include <QLineEdit>

#include "passport.h"

namespace Ui {
class EntryDialog;
}

class EntryDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EntryDialog(QWidget *parent = 0);
	~EntryDialog();
	auto const& linesData() {
		return lines;
	}
private:
	Ui::EntryDialog *ui;
	// elements point to non-const objects
	std::array<QLineEdit*, cw::Passport::fieldTitles.size()> lines;
	void setUpFields();
};

#endif // ENTRYDIALOG_H

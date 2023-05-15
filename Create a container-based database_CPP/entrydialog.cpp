#include "entrydialog.h"
#include "ui_entrydialog.h"
#include <QLabel>

EntryDialog::EntryDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EntryDialog)
{
	ui->setupUi(this);
	setUpFields();
}

EntryDialog::~EntryDialog()
{
	delete ui;
}

void EntryDialog::setUpFields()
{
	for (size_t i = 0; i < cw::Passport::fieldTitles.size(); i++) {
		ui->formLayout->addRow(new QLabel(cw::Passport::fieldTitles.at(i)),
				       lines[i] = new QLineEdit());
	}
}

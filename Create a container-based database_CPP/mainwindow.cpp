#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Filename menu
	ui->lineEdit->setText("database.txt");
	ui->statusline->setText("Load from file or add names manually.");

	// Set up column amount
	ui->tableWidget->setColumnCount(cw::Passport::fieldTitles.size());

	// Set up column names
	ui->tableWidget->setHorizontalHeaderLabels([]() {
		QStringList initializer;
		for(auto const& fieldTitle: cw::Passport::fieldTitles)
			initializer.push_back(fieldTitle);
		return initializer;
	}());

	// Additional widgets:

	// Add a search box to the top toolbar
	ui->mainToolBar->addWidget(new QLabel("Search: "));
	searchBox = new QLineEdit();
	searchBox->setMaximumWidth(150);
	connect(searchBox,
		&QLineEdit::returnPressed, this,
		&MainWindow::on_searchBox_returnPressed);
	ui->mainToolBar->addWidget(searchBox);

	// Set menu policy for the table
	ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

	// Add 'remove' right click action to the table
	QAction* removeAction = new QAction("Remove row");
	connect(removeAction, &QAction::triggered,
		this,
		&MainWindow::removeSelectedRows);
	ui->tableWidget->addAction(removeAction);

	// Add 'edit' right click action
	QAction* editAction = new QAction("Edit row");
	connect(editAction, &QAction::triggered, this, [this](){
		if (auto const& selected = ui->tableWidget->selectedItems();
		    !selected.empty()) {
			editRow(selected.front()->row());
		}
	});
	ui->tableWidget->addAction(editAction);
}

MainWindow::~MainWindow()
{
	delete ui;
}

// Private methods

void MainWindow::clearTable()
{
	ui->tableWidget->setRowCount(0);
	ui->statusline->setText("View cleared.");
}

void MainWindow::loadFromFile(QString const& filename)
{
	if (!dumpToDB())
		return;
	ui->statusline->setText([this](auto status) {
		switch (status) {
		case cw::DataBase::result::success:
			displayTable();
			return "Database loaded.";
		case cw::DataBase::result::invalid_format:
			return "Invalid file format.";
		case cw::DataBase::result::doesnt_exist:
			return "File doesn't exist.";
		default:
			return "Error reading file.";
		}
	} (db.loadFromFile(filename)));
}

bool MainWindow::dumpToDB()
{
	if (!viewNewer)
		return true;
	cw::DataArray users(ui->tableWidget->rowCount());
	for(int row = 0; row < ui->tableWidget->rowCount(); row++) {
		for (size_t field = 0; field < cw::Passport::fieldTitles.size(); field++) {
			users[row].fields[field] = ui->tableWidget->item(row,field)->text();
		}
		if (!users[row].selfCheck()) {
			ui->statusline->setText("The current table is invalid. "
						"Please fix it and try again.");
			return false; // here the database is not changed
		}
	}

	if (users != db.get()) { // Here an empty table will be written too.
		db.move(std::move(users));
		viewNewer = false;
	}
	return true;
}

void MainWindow::saveToFile(const QString& filename)
{
	if (!dumpToDB() || db.get().empty())
		return;
	if (db.saveToFile(filename) == cw::DataBase::result::success) {
		ui->statusline->setText("File saved.");
	} else {
		ui->statusline->setText("Unable to save file.");
	}
}

void MainWindow::addEmptyRow()
{
	ui->tableWidget->insertRow(ui->tableWidget->rowCount());
	for(size_t i = 0; i < cw::Passport::fieldTitles.size(); i++) {
		ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1,
					 i,
					 new QTableWidgetItem());
	}
	ui->statusline->setText("A row added.");
	viewNewer = true;
}

void MainWindow::removeSelectedRows()
{
	if (auto cells = ui->tableWidget->selectedItems(); !cells.empty()) {
		for(auto const& cell: cells) {
			if (int i = ui->tableWidget->row(cell); i != -1)
				ui->tableWidget->removeRow(i);
		}
		ui->statusline->setText("Row(s) removed.");
		viewNewer = true;
	} else {
		ui->statusline->setText("No row(s) selected.");
	}
}

void MainWindow::displayTable()
{
	clearTable();
	for(auto const& user: db.get()) {
		ui->tableWidget->insertRow(ui->tableWidget->rowCount());

		for(size_t i = 0; i < cw::Passport::fieldTitles.size(); i++) {
			ui->tableWidget->setItem(
				ui->tableWidget->rowCount() - 1, i,
				new QTableWidgetItem(user.fields[i]));
		}
	}
	viewNewer = false;
}

void MainWindow::editRow(int row)
{
	auto const& lines = e.linesData();
	// Load current row data into the view
	for (size_t col = 0; col < cw::Passport::fieldTitles.size(); col++) {
		lines[col]->setText(ui->tableWidget->item(row, col)->text());
	}

	// Show window
	e.exec(); // blocks main window

	// Apply changes for non-empty fields
	if(e.result() == QDialog::Accepted) {
		for(size_t col = 0; col < cw::Passport::fieldTitles.size(); col++) {
			auto* const item = ui->tableWidget->item(row, col);
			auto const& newText = lines[col]->text();

			if(!newText.isEmpty() && newText != item->text()) {
				item->setText(newText);
				ui->statusline->setText("Cell changed.");
				viewNewer = true;
			}
		}
	}
}

// Private slots

void MainWindow::findMatchingCells(const QString& searchQuery)
{
	ui->tableWidget->clearSelection();
	for (QTableWidgetItem* cell:
	     ui->tableWidget->findItems(searchQuery, Qt::MatchContains)) {
		cell->setSelected(true);
	}
}

void MainWindow::on_pushButton_load_clicked()
{
	loadFromFile(ui->lineEdit->text());
}

void MainWindow::on_pushButton_save_clicked()
{
	saveToFile(ui->lineEdit->text());
}

// Toolbar item slots

void MainWindow::on_actionAdd_username_triggered()
{
	addEmptyRow();
}

void MainWindow::on_actionRemove_username_triggered()
{
	removeSelectedRows();
}

void MainWindow::on_actionClear_triggered()
{
	db.clear();
	clearTable();
}

// Menu actions

// Action "exit" calls mainwindow::close();

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::information(this, "Info", "2017 Pavel Poliakov");
}

void MainWindow::on_tableWidget_cellChanged(int, int)
{
	viewNewer = true;
}

void MainWindow::on_searchBox_returnPressed()
{
	findMatchingCells(searchBox->text());
}

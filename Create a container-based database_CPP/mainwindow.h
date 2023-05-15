#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "passport.h"
#include "entrydialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_pushButton_load_clicked();
	void on_pushButton_save_clicked();
	void on_actionAbout_triggered();
	void on_actionAdd_username_triggered();
	void on_actionRemove_username_triggered();
	void on_actionClear_triggered();
	void on_tableWidget_cellChanged(int, int);

// custom:
	void on_searchBox_returnPressed();

private:
	Ui::MainWindow* ui;
	EntryDialog e{};
	cw::DataBase db{};
	QLineEdit* searchBox{};
	bool viewNewer = false;
	void clearTable();
	void loadFromFile(const QString& filename);
	bool dumpToDB();
	void saveToFile(const QString& filename);
	void addEmptyRow();
	void removeSelectedRows();
	void displayTable();
	void editRow(int row);
	void findMatchingCells(QString const& searchQuery);
};

#endif // MAINWINDOW_H

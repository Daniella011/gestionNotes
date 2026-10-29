#include "mainwindow.h"
#include "da.h"
#include <QApplication>
//#include <QDebug>
//#include <QSqlDatabase>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QtSql>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!Database::ouvrir()){
        return 1;
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}
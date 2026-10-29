#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onEditNomCursorPositionChanged(int arg1, int arg2);

    void onBtnAjouterClicked();

    void onBtnSupprimerClicked();

    void onBtnAjouterEtudiantsClicked();

    void chargerClasses();

    void chargerFiltreClasse();

    void on_comboFiltreClasse_currentIndexChanged(int index);


    void onComboFiltreClasseCurrentIndexChanged(int index);

    void onComboClasseCurrentIndexChanged(int index);

    void onBtnAjouterMatiereClicked();

    void onBtnClassementClicked();

    void onComboClasseActivated(int index);

    void onTableEtudiantsActivated(const QModelIndex &index);

    void onTableViewActivated(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QSqlQueryModel *modelEtudiants;
    QSqlQueryModel *modelMatieres;
    QSqlQueryModel *modelNotes;
    QSqlQueryModel *modelClassement;
    void chargerCombos();
    void chargerEtudiants();
    void chargerMatiere();
    void chargerNotes();
};
#endif // MAINWINDOW_H

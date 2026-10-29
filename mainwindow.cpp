#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include "da.h"
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->tableView, &QTableView::activated, this, &MainWindow::onTableViewActivated);
    connect(ui->tableEtudiants, &QTableView::activated, this, &MainWindow::onTableEtudiantsActivated);
    connect(ui->comboClasse, &QComboBox::activated, this, &MainWindow::onComboClasseActivated);
    connect(ui->btnClassement, &QPushButton::clicked, this, &MainWindow::onBtnClassementClicked);
    connect(ui->btnAjouterMatiere, &QPushButton::clicked, this, &MainWindow::onBtnAjouterMatiereClicked);
    connect(ui->comboClasse, &QComboBox::currentIndexChanged, this, &MainWindow::onComboClasseCurrentIndexChanged);
    modelEtudiants  = new QSqlQueryModel(this);
    modelMatieres   = new QSqlQueryModel(this);
    modelNotes      = new QSqlQueryModel(this);
    modelClassement = new QSqlQueryModel(this);

    ui->tableEtudiants->setModel(modelEtudiants);
    ui->tableView->setModel(modelMatieres);
    ui->tableNotes->setModel(modelNotes);
    ui->tableClassement->setModel(modelClassement);

    for (QTableView *t : {ui->tableEtudiants, ui->tableView,
                          ui->tableNotes, ui->tableClassement}) {
        t->setSelectionBehavior(QAbstractItemView::SelectRows);
        t->setSelectionMode(QAbstractItemView::SingleSelection);
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);
        t->horizontalHeader()->setStretchLastSection(true);
    }

    ui->spinNote->setRange(0, 20);
    ui->spinNote->setDecimals(2);
    ui->spinCoeff->setRange(1, 10);

    chargerCombos();
    chargerEtudiants();
    chargerMatiere();
    chargerNotes();


    connect(ui->comboFiltreClasse, &QComboBox::currentIndexChanged, this, &MainWindow::onComboFiltreClasseCurrentIndexChanged);

    Database::ouvrir();
    chargerClasses();
    chargerFiltreClasse();
    connect(ui->btnAjouterEtudiants, &QPushButton::clicked, this, &MainWindow::onBtnAjouterEtudiantsClicked);
    connect(ui->btnSupprimer, &QPushButton::clicked, this, &MainWindow::onBtnSupprimerClicked);
    connect(ui->btnAjouterEtudiants, &QPushButton::clicked, this, &MainWindow::onBtnAjouterClicked);
    connect(ui->editNom, &QLineEdit::cursorPositionChanged, this, &MainWindow::onEditNomCursorPositionChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onEditNomCursorPositionChanged(int arg1, int arg2)
{
    ui->btnAjouterEtudiants->setText("Ajouter");
}

void MainWindow::onBtnAjouterClicked(){

}

void MainWindow::onBtnSupprimerClicked(){
    QModelIndex idx = ui->tableEtudiants->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::information(this, "Info", "Sélectionne un étudiant dans le tableau.");
        return;
    }
    if (QMessageBox::question(this, "Confirmation",
                              "Supprimer cet étudiant et toutes ses notes ?") != QMessageBox::Yes)
        return;

    int id = modelEtudiants->data(modelEtudiants->index(idx.row(), 0)).toInt();

    QSqlQuery q;
    q.prepare("DELETE FROM note WHERE id_etudiant = ?");   // notes d'abord (clé étrangère)
    q.addBindValue(id);
    q.exec();

    q.prepare("DELETE FROM etudiant WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur", q.lastError().text());
        return;
    }

    chargerEtudiants();
    chargerCombos();
    chargerNotes();
}

void MainWindow::onBtnAjouterEtudiantsClicked(){
    QString matricule = ui->matriculeEdit->text().trimmed();
    QString nom       = ui->editNom->text().trimmed();

    if (matricule.isEmpty() || nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Remplis le matricule et le nom.");
        return;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO etudiant(nom, matricule, id_classe) VALUES(?,?,?)");
    q.addBindValue(nom);
    q.addBindValue(matricule);
    q.addBindValue(ui->comboClasse->currentData());

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur",
                              "Ajout impossible (matricule déjà utilisé ?)\n" + q.lastError().text());
        return;
    }

    ui->matriculeEdit->clear();
    ui->editNom->clear();
    ui->matriculeEdit->setFocus();
    chargerEtudiants();
    chargerCombos();     // met à jour la liste de l'onglet Notes
}
void MainWindow::chargerNotes(){

}
void MainWindow::chargerClasses(){
    ui->comboClasse->clear();
    QSqlQuery q("SELECT id, nom_classe FROM classe");
    while(q.next()){
        ui->comboClasse->addItem(q.value(1).toString(), q.value(0).toInt());
    }
}
void MainWindow::chargerFiltreClasse(){
    ui->comboFiltreClasse->clear();
    ui->comboFiltreClasse->addItem("Toutes les classes", -1);
    QSqlQuery q("SELECT id, nom_classe FROM classe");
    while(q.next()){
        ui->comboFiltreClasse->addItem(q.value(1).toString(), q.value(0).toInt());
    }
}
void MainWindow::on_comboFiltreClasse_currentIndexChanged(int index){
    int idClasse = ui->comboFiltreClasse->currentData().toInt();
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    if(idClasse == -1){
        query.exec("SELECT e.matricule, e.nom, c.nom_classe as classe FROM etudiant e JOIN classe c ON e.id_classe = c.id");
    }else{
        query.prepare("SELECT e.matricule, e.nom, c.nom_classe as classe FROM etudiant e JOIN classe c ON e.id_classe = c.id WHERE c.id = ?");
        query.addBindValue(idClasse);
        query.exec();
    }
    model->setQuery(query);
    ui->tableEtudiants->setModel(model); // attention, le nom peut être tableEtudiants ou autre chez toi
}
void MainWindow::onComboFiltreClasseCurrentIndexChanged(int index)
{


}


void MainWindow::onComboClasseCurrentIndexChanged(int index)
{
    Q_UNUSED(index);
    chargerEtudiants();
}


void MainWindow::onBtnAjouterMatiereClicked()
{
    QString nom = ui->nomMatiereEdit->text().trimmed();
    int coeff = ui->spinCoeff->value();
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Saisis le nom de la matière.");
        return;
    }

    QSqlQuery q(QSqlDatabase::database());
    q.prepare("INSERT INTO matiere(nom_matiere, coeff) VALUES(:nom, :coeff)");
    q.addbindValue(":nom", nom);
    q.arddbindValue(":coeff");

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur",
                              "Ajout impossible (matière déjà existante ?)\n" + q.lastError().text());
    }else{
        chargerMatiere();
        chargerCombos();
    }


    ui->nomMatiereEdit->clear();
    ui->spinCoeff->setValue(1);
    chargerMatiere();
    chargerCombos();     // met à jour comboMatiere dans l'onglet Notes

}


void MainWindow::onBtnClassementClicked()
{
    // Moyenne pondérée par les coefficients, triée de la meilleure à la plus faible
    modelClassement->setQuery(
        "SELECT e.nom, c.nom_classe, "
        "       ROUND(SUM(n.valeur * m.coeff) * 1.0 / SUM(m.coeff), 2) AS moyenne "
        "FROM etudiant e "
        "JOIN note n ON n.id_etudiant = e.id "
        "JOIN matiere m ON m.id_matiere = n.id_matiere "
        "LEFT JOIN classe c ON c.id = e.id_classe "
        "GROUP BY e.id "
        "ORDER BY moyenne DESC");

    modelClassement->setHeaderData(0, Qt::Horizontal, "Étudiant");
    modelClassement->setHeaderData(1, Qt::Horizontal, "Classe");
    modelClassement->setHeaderData(2, Qt::Horizontal, "Moyenne");

    if (modelClassement->rowCount() > 0) {
        QString nom = modelClassement->data(modelClassement->index(0, 0)).toString();
        QString moy = modelClassement->data(modelClassement->index(0, 2)).toString();
        ui->labelLaureat->setText("🏆 Lauréat : " + nom + " (" + moy + "/20)");
    } else {
        ui->labelLaureat->setText("Aucune note enregistrée.");
    }
}
void MainWindow::chargerCombos()
{
    // Classes
    ui->comboClasse->clear();
    ui->comboFiltreClasse->blockSignals(true);
    ui->comboFiltreClasse->clear();
    ui->comboFiltreClasse->addItem("Toutes", 0);

    QSqlQuery q("SELECT id, nom_classe FROM classe ORDER BY id");
    while (q.next()) {
        ui->comboClasse->addItem(q.value(1).toString(), q.value(0));
        ui->comboFiltreClasse->addItem(q.value(1).toString(), q.value(0));
    }
    ui->comboFiltreClasse->blockSignals(false);

    // Onglet Notes : étudiants
    ui->comboNotesEtudiant->clear();
    QSqlQuery e("SELECT id, nom || ' (' || matricule || ')' FROM etudiant ORDER BY nom");
    while (e.next())
        ui->comboNotesEtudiant->addItem(e.value(1).toString(), e.value(0));

    // Onglet Notes : matières
    ui->comboNotesMatiere->clear();
    QSqlQuery m("SELECT id_matiere, nom_matiere FROM matiere ORDER BY nom_matiere");
    while (m.next())
        ui->comboNotesMatiere->addItem(m.value(1).toString(), m.value(0));
}

void MainWindow::chargerEtudiants()
{
    int idClasse = ui->comboFiltreClasse->currentData().toInt();   // 0 = Toutes

    QString sql = "SELECT e.id, e.matricule, e.nom, c.nom_classe "
                  "FROM etudiant e LEFT JOIN classe c ON c.id = e.id_classe";
    if (idClasse > 0)
        sql += " WHERE e.id_classe = " + QString::number(idClasse);
    sql += " ORDER BY e.nom";

    modelEtudiants->setQuery(sql);
    modelEtudiants->setHeaderData(1, Qt::Horizontal, "Matricule");
    modelEtudiants->setHeaderData(2, Qt::Horizontal, "Nom");
    modelEtudiants->setHeaderData(3, Qt::Horizontal, "Classe");
    ui->tableEtudiants->hideColumn(0);
}
void MainWindow::chargerMatiere()
{
    modelMatieres->setQuery(
        "SELECT id_matiere, nom_matiere, coeff FROM matiere ORDER BY nom_matiere");
    modelMatieres->setHeaderData(1, Qt::Horizontal, "Matière");
    modelMatieres->setHeaderData(2, Qt::Horizontal, "Coefficient");
    ui->tableView->hideColumn(0);
}


void MainWindow::onComboClasseActivated(int index)
{

}


void MainWindow::onTableEtudiantsActivated(const QModelIndex &index)
{
    //REQUETE SQL
}
void MainWindow::onTableViewActivated(const QModelIndex &index)
{

}



#include "da.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

bool Database::ouvrir()
{
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)){
        QSqlDatabase db = QSqlDatabase::database();
        if (db.isOpen())
            return true;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("gestionNotes.db");

    if (!db.open()){
        qDebug() << "erreur de db:" << db.lastError().text();
        return false;
    }
    //QSqlQuery query;
   // query.exec(" CREATE TABLE IF NOT EXISTS etudiants (matricule INTEGER PRIMARY KEY, nom VARCHAR, moyenne FLOAT)");
    //return true;

    QSqlQuery query;
    query.exec("PRAGMA foreign_key = ON;");

    query.exec("CREATE TABLE IF NOT EXISTS classe ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "nom_classe TEXT NOT NULL UNIQUE)");

    query.exec("INSERT OR IGNORE INTO classe (nom_classe) " "VALUES ('Seconde'), ('Première'), ('Terminal')");

    //TABLE 1(etudiants)
    query.exec("CREATE TABLE IF NOT EXISTS etudiant ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "nom TEXT NOT NULL,"
               "matricule TEXT NOT NULL,"
               "id_classe INTEGER,"
               "FOREIGN KEY(id_classe) REFERENCES classe(id))");

    //table 2(matiere)
    query.exec("CREATE TABLE IF NOT EXISTS matiere ("
               "id_matiere INTEGER PRIMARY KEY AUTOINCREMENT , "
               "nom_matiere TEXT NOT NULL)");

    //table 3(NOTE)
    query.exec("CREATE TABLE IF NOT EXISTS note ("
               "id_etudiant INTEGER NOT NULL, "
               "id_matiere INTEGER NOT NULL, "
               "valeur REAL NOT NULL, "
               "PRIMARY KEY (id_etudiant, id_matiere), "
               "FOREIGN KEY (id_etudiant) REFERENCES etudiant(id), "
               "FOREIGN KEY (id_matiere) REFERENCES matiere(id_matiere))");

    return true;
}

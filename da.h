#ifndef DA_H
#define DA_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
class Database
{
public:
    Database();
    static bool ouvrir();
    static bool fermer();
};

#endif // DA_H

#ifndef MYSQL_OPERA_H
#define MYSQL_OPERA_H

#include <QObject>

class mysql_opera : public QObject
{
    Q_OBJECT
public:
    explicit mysql_opera(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MYSQL_OPERA_H
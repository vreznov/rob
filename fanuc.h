#ifndef FANUC_H
#define FANUC_H

#include <QObject>

class fanuc : public QObject
{
    Q_OBJECT
public:
    explicit fanuc(QObject *parent = nullptr);

signals:

public slots:
};

#endif // FANUC_H
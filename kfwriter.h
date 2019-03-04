#ifndef FWRITER_H
#define FWRITER_H

#include <QObject>

class fwriter : public QObject
{
    Q_OBJECT
public:
    explicit fwriter(QObject *parent = nullptr);

signals:

public slots:
};

#endif // FWRITER_H
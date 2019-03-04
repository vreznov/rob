#ifndef DATA_SHOW_H
#define DATA_SHOW_H

#include <QMainWindow>

namespace Ui {
class data_show;
}

class data_show : public QMainWindow
{
    Q_OBJECT

public:
    explicit data_show(QWidget *parent = 0);
    ~data_show();

private:
    Ui::data_show *ui;
};

#endif // DATA_SHOW_H

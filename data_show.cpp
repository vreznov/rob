#include "data_show.h"
#include "ui_data_show.h"

data_show::data_show(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::data_show)
{
    ui->setupUi(this);
}

data_show::~data_show()
{
    delete ui;
}

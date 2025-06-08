#include "addresourcedialog.h"
#include "ui_addresourcedialog.h"
#include <QMessageBox>

AddResourceDialog::AddResourceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddResourceDialog)
{
    ui->setupUi(this);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

AddResourceDialog::~AddResourceDialog()
{
    delete ui;
}

QString AddResourceDialog::resourceName() const
{
    return ui->nameLineEdit->text();
}

QString AddResourceDialog::secretKey() const
{
    return ui->keyLineEdit->text();
}

void AddResourceDialog::on_okButton_clicked()
{
    if (resourceName().isEmpty() || secretKey().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните оба поля.");
        return;
    }
    accept();
}

#ifndef ADDRESOURCEDIALOG_H
#define ADDRESOURCEDIALOG_H

#include <QDialog>

namespace Ui {
class AddResourceDialog;
}

class AddResourceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddResourceDialog(QWidget *parent = nullptr);
    ~AddResourceDialog();

    QString resourceName() const;
    QString secretKey() const;

private slots:
    void on_okButton_clicked();

private:
    Ui::AddResourceDialog *ui;
};

#endif // ADDRESOURCEDIALOG_H

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include "addresourcedialog.h"
#include "totp.h"
#include <QTimer>
#include <QDateTime>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked,
            this, &MainWindow::onAddResourceClicked);
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateCodes);
    updateTimer->start(1000);  // обновление каждую секунду

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onAddResourceClicked() {
    AddResourceDialog dlg(this);
    bool dataValid = false;

    while (!dataValid) {
        if (dlg.exec() != QDialog::Accepted) {
            return;
        }

        QString name = dlg.resourceName();
        QString key = dlg.secretKey().trimmed().toUpper();

        // Проверка на пустые поля
        if (name.isEmpty() || key.isEmpty()) {
            QMessageBox::warning(&dlg, "Ошибка", "Название и ключ не могут быть пустыми.");
            continue;
        }

        // Проверка Base32
        static const QRegularExpression base32Regex("^[A-Z2-7]+=*$");
        if (!base32Regex.match(key).hasMatch()) {
            QMessageBox::warning(&dlg, "Ошибка",
                                 "Неверный формат ключа. Допустимые символы: A-Z, 2-7, '=' (выравнивание).\n"
                                 "Пример: JBSWY3DPEHPK3PXP");
            continue;
        }

        if (key.length() < 16) {
            QMessageBox::warning(&dlg, "Ошибка", "Ключ слишком короткий. Минимум 16 символов.");
            continue;
        }

        resources.append({name, key});
        updateCodes();
        dataValid = true;
    }
}


void MainWindow::updateCodes()
{
    int currentRow = ui->listWidget->currentRow();
    ui->listWidget->clear();

    qint64 now = QDateTime::currentSecsSinceEpoch();
    int secondsLeft = 60 - (now % 60);

    for (int i = 0; i < resources.size(); ++i) {
        const AuthResource &res = resources[i];

        QString code = generateTOTP(res.secret, 60, 6);   // вот тут получаем код

        QWidget *itemWidget = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout(itemWidget);
        mainLayout->setContentsMargins(10, 5, 10, 5);

        QHBoxLayout *topLayout = new QHBoxLayout;
        QLabel *nameLabel = new QLabel(res.name);
        QPushButton *removeButton = new QPushButton("Удалить");

        nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        removeButton->setStyleSheet(
            "background-color: red;"
            "color: white;"
            "padding: 4px 12px;"
            "border-radius: 6px;"
            );

        topLayout->addWidget(nameLabel);
        topLayout->addStretch();
        topLayout->addWidget(removeButton);

        QLabel *codeLabel = new QLabel(code);
        codeLabel->setStyleSheet("font-size: 24px; color: #007ACC;");
        codeLabel->setAlignment(Qt::AlignLeft);

        mainLayout->addLayout(topLayout);
        mainLayout->addWidget(codeLabel);

        if (i % 2 == 0) {
            itemWidget->setStyleSheet("background-color: #414345;");
        } else {
            itemWidget->setStyleSheet("background-color: #39434d;");
        }

        connect(removeButton, &QPushButton::clicked, this, [this, i]() {
            removeResource(i);
        });

        QListWidgetItem *item = new QListWidgetItem();
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, itemWidget);
        item->setSizeHint(itemWidget->sizeHint());
    }

    if (currentRow >= 0 && currentRow < ui->listWidget->count()) {
        ui->listWidget->setCurrentRow(currentRow);
    }

    ui->countdownLabel->setText("Обновление кодов через: " + QString::number(secondsLeft) + " сек");

}




void MainWindow::removeResource(int index)
{
    if (index < 0 || index >= resources.size()) return;

    const QString name = resources[index].name;
    auto reply = QMessageBox::question(this, "Удаление",
                        "Удалить ресурс \"" + name + "\"?",
                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        resources.removeAt(index);
        updateCodes();
    }
}



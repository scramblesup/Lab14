#include <QPushButton>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QFormLayout>
#include <QLineEdit>
#include <QAction>
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QDir>

#include "user.h"

namespace dv {
double cost = 5.0;
double fee = 500.0;
double traffic = 100.0;
double discountAbsolute = 1;
double discountRelative = 10;
};


TMainWindow::TMainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Интернет-оператор");
    centralWidget = new TCentralWidget(this);
    setCentralWidget(centralWidget);
}

TMainWindow::~TMainWindow() {}

void TCentralWidget::exitSlot() {
    QApplication::quit();
}

void TCentralWidget::tariffSlot() {
    int currentIndex = typeBox->currentIndex();
    if (currentIndex == 0) {
        QMessageBox::critical(this, "Ошибка!", "Необходимо выбрать способ тарификации!");
        return;
    }

    if (clientsList->currentRow() == -1) {
        QMessageBox::critical(this, "Ошибка!", "Выберите клиента, которому хотите установить тариф!");
        return;
    }

    tariffDialog->config(currentIndex);
    tariffDialog->setModal(true);
    tariffDialog->show();
}

void TCentralWidget::loginSlot() {
    newUserDialog->setModal(true);
    newUserDialog->show();
}

void TCentralWidget::trafficSlot() {
    int currentIndex{clientsList->currentRow()};
    if (currentIndex == -1) {
        QMessageBox::critical(this, "Ошибка!", "Выберите клиента, которому хотите добавить трафик!");
        return;
    }

    clients.find(clientsList->currentItem()->text().toStdString())->second.addTraffic(trafficEdit->value());

    currentClient->displayClient(clientInfo);

    trafficEdit->setValue(500.0);

}

void TCentralWidget::sumSlot() {
    infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Всего реализовано трафика на сумму: " + std::to_string(countAllPrice())));
}

void TCentralWidget::payableSlot() {
    if (clients.empty()) { infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Пользователей еще нет!")); }
    else { infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Больше всех заплатил пользователь " + findMostPayable())); }
}

void TCentralWidget::deleteSlot() {
    QListWidgetItem* item = clientsList->currentItem();
    if (item == nullptr) {
        QMessageBox::critical(this, "Ошибка!", "Выберите пользователя для удаления!");
        return;
    }
    std::string name = item->text().toStdString();
    removeClient(name);
//    clientsList->removeItemWidget(item); // КАК УДАЛИТЬ???
    delete clientsList->takeItem(clientsList->currentRow());
    infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Пользователь " + name + " успешно удален!"));
}

void TCentralWidget::saveSlot() {
    try {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Выберите файл для открытия"), QDir::homePath(), tr("Текстовые файлы (*.txt);;Все файлы (*)"));
        save(filePath.toStdString());
        infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Данные пользователей сохранены!"));
    }
    catch (fileNotWork ex) { QMessageBox::critical(this, "Ошибка!", QString::fromStdString(ex.what())); }
}

void TCentralWidget::loadSlot() {
    try {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Выберите файл для открытия"), QDir::homePath(), tr("Текстовые файлы (*.txt);;Все файлы (*)"));
        load(filePath.toStdString());
        clientsList->clear();
        for(auto i = clients.begin(); i != clients.end(); i++) {
            clientsList->addItem(QString::fromStdString(i->first));
        }
        infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Данные пользователей загружены!"));
    }
    catch (fileNotWork ex) { QMessageBox::critical(this, "Ошибка!", QString::fromStdString(ex.what())); }
}

void TCentralWidget::currentChangedSlot(QListWidgetItem *current, QListWidgetItem *previous) {
    if (!current) {
        std::cerr << "Ошибка: Получен нулевой QListWidgetItem." << std::endl;
        return;
    }

    currentClient = &clients.find(current->text().toStdString())->second;

    if (currentClient) {
        currentClient->displayClient(clientInfo);
    } else {
        std::cerr << "Ошибка: Клиент '" << currentClient->getName() << "' не найден в массиве." << std::endl;
    }
}

void TCentralWidget::userAcceptSlot(std::string name, std::string password) {

    client newClient(name, password, nullptr, 0.0);

    try {
        addClient(newClient);
        clientsList->addItem(QString::fromStdString(newClient.getName()));
        newUserDialog->hide();
        newUserDialog->setModal(false);
        newUserDialog->clear();
        infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Пользователь " + name + " успешно добавлен!"));
    }
    catch (clientAlreadyExists ex) {
        QMessageBox::critical(this, "Ошибка!", ex.what());
    }
}

TCentralWidget::TCentralWidget(QWidget *parent) : QWidget(parent), internet(), currentClient(nullptr) {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    setLayout(mainLayout);

    QVBoxLayout* showLayout = new QVBoxLayout(this);
    mainLayout->addLayout(showLayout);

    clientsList = new QListWidget(this);
    clientsList->setSortingEnabled(true);
    connect(clientsList, &QListWidget::currentItemChanged, this, &TCentralWidget::currentChangedSlot);
    showLayout->addWidget(clientsList);
    clientInfo = new QTextEdit(this);
    clientInfo->setReadOnly(true);
    showLayout->addWidget(clientInfo);
    QPushButton* exitButton = new QPushButton("Выход", this);
    connect(exitButton, &QPushButton::released, this, &TCentralWidget::exitSlot);
    showLayout->addWidget(exitButton);

    QVBoxLayout* interactLayout = new QVBoxLayout(this);
    interactLayout->setAlignment(Qt::AlignTop);
    mainLayout->addLayout(interactLayout);


    QHBoxLayout* tariffLayout = new QHBoxLayout(this);
    interactLayout->addLayout(tariffLayout);

    QPushButton* tariffButton = new QPushButton("Ввести тариф", this);
    tariffLayout->addWidget(tariffButton);
    connect(tariffButton, &QPushButton::released, this, &TCentralWidget::tariffSlot);

    typeBox = new QComboBox(this);
    typeBox->addItem("Выберите способ");
    typeBox->addItem("Плата за срок");
    typeBox->addItem("Плата за трафик");
    tariffLayout->addWidget(typeBox);

    QHBoxLayout* trafficLayout = new QHBoxLayout(this);
    interactLayout->addLayout(trafficLayout);

    trafficEdit = new QDoubleSpinBox(this);
    trafficEdit->setRange(0.0, 999999999.0);
    trafficEdit->setSingleStep(1.0);
    trafficEdit->setValue(500.0);

    QPushButton* trafficButton = new QPushButton("Добавить, МБ:", this);

    connect(trafficButton, &QPushButton::released, this, &TCentralWidget::trafficSlot);

    trafficLayout->addWidget(trafficButton);
    trafficLayout->addWidget(trafficEdit);

    QVBoxLayout* functionsLayout = new QVBoxLayout(this);
    interactLayout->addLayout(functionsLayout);

    QPushButton* loginButton = new QPushButton("Добавить клиента", this);
    interactLayout->addWidget(loginButton);
    connect(loginButton, &QPushButton::released, this, &TCentralWidget::loginSlot);
    QFont buttonFont = loginButton->font();
    buttonFont.setBold(true);
    loginButton->setFont(buttonFont);

    newUserDialog = new TUserDialog(this);
    connect(newUserDialog, &TUserDialog::sendUserData, this, &TCentralWidget::userAcceptSlot);

    tariffDialog = new TTariffDialog(this);
    connect(tariffDialog, &TTariffDialog::sendTariffData, this, &TCentralWidget::tariffAcceptSlot);

    QPushButton* sumButton = new QPushButton("Общая стоимость", this);
    interactLayout->addWidget(sumButton);
    connect(sumButton, &QPushButton::released, this, &TCentralWidget::sumSlot);

    QPushButton* payableButton = new QPushButton("Самый платежеспособный", this);
    interactLayout->addWidget(payableButton);
    connect(payableButton, &QPushButton::released, this, &TCentralWidget::payableSlot);

    QPushButton* deleteButton = new QPushButton("Удалить клиента", this);
    interactLayout->addWidget(deleteButton);
    connect(deleteButton, &QPushButton::released, this, &TCentralWidget::deleteSlot);

    QPushButton* saveButton = new QPushButton("Сохранить", this);
    interactLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::released, this, &TCentralWidget::saveSlot);

    QPushButton* loadButton = new QPushButton("Загрузить", this);
    interactLayout->addWidget(loadButton);
    connect(loadButton, &QPushButton::released, this, &TCentralWidget::loadSlot);

    QLabel* sepLabel = new QLabel("-------------------------------------------------------------", this);
    QLabel* infoLabel = new QLabel("Вывод данных:", this);
    infoLabel->setAlignment(Qt::AlignHCenter);
    sepLabel->setAlignment(Qt::AlignHCenter);
    interactLayout->addWidget(sepLabel);
    interactLayout->addWidget(infoLabel);

    infoEdit = new QTextEdit(this);
    infoEdit->setReadOnly(true);
    interactLayout->addWidget(infoEdit);
}

TCentralWidget::~TCentralWidget() {

}

void TUserDialog::acceptSlot() {
    std::string name(loginEdit->text().toStdString());
    std::string password(passwordEdit->text().toStdString());

    if (name == "" || password == "") {
        QMessageBox::critical(this, "Ошибка!", "Заполните оба поля Логин и Пароль!");
        return;
    }

    emit sendUserData(loginEdit->text().toStdString(), passwordEdit->text().toStdString());
}

void TUserDialog::cancelSlot() {
    hide();
    setModal(false);
}

TUserDialog::TUserDialog(QWidget *parent) : QDialog(parent) {

    setWindowTitle("Добавление пользователя");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QFormLayout* formLayout = new QFormLayout(this);
    mainLayout->addLayout(formLayout);

    loginEdit = new QLineEdit(this);
    formLayout->addRow("Логин:", loginEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Пароль:", passwordEdit);

    QVBoxLayout* buttonsLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonsLayout);
    QPushButton* acceptButton = new QPushButton("Принять", this);
    connect(acceptButton, &QPushButton::released, this, &TUserDialog::acceptSlot);
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    connect(cancelButton, &QPushButton::released, this, &TUserDialog::cancelSlot);
    buttonsLayout->addWidget(acceptButton);
    buttonsLayout->addWidget(cancelButton);
}


void TUserDialog::clear() {
    loginEdit->clear();
    passwordEdit->clear();
}

TTariffDialog::TTariffDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Указание тарифа");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QFormLayout* formLayout = new QFormLayout(this);
    mainLayout->addLayout(formLayout);

    typeBox = new QComboBox(this);
    typeBox->addItem("Бюджетный");
    typeBox->addItem("Оптимальный");
    typeBox->addItem("Люкс");
    typeBox->addItem("Безлимитный");
    typeLabel = new QLabel("Тип тарифа:", this);
    formLayout->addRow(typeLabel, typeBox);

    costEdit = new QDoubleSpinBox(this);
    costEdit->setRange(1.0, 999999999.0);
    costEdit->setSingleStep(0.5);
    costEdit->setValue(dv::cost);
    costLabel = new QLabel("Цена за МБ:", this);
    formLayout->addRow(costLabel, costEdit);

    feeEdit = new QDoubleSpinBox(this);
    feeEdit->setRange(1.0, 999999999.0);
    feeEdit->setSingleStep(0.5);
    feeEdit->setValue(dv::fee);
    feeLabel = new QLabel("Месячная плата:", this);
    formLayout->addRow(feeLabel, feeEdit);

    trafficEdit = new QDoubleSpinBox(this);
    trafficEdit->setRange(1.0, 999999999.0);
    trafficEdit->setSingleStep(0.5);
    trafficEdit->setValue(dv::traffic);
    trafficLabel = new QLabel("Доступный трафик:", this);
    formLayout->addRow(trafficLabel, trafficEdit);

    discountEdit = new QDoubleSpinBox(this);
    discountLabel = new QLabel("Скидка", this);
    formLayout->addRow(discountLabel, discountEdit);

    QVBoxLayout* buttonsLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonsLayout);
    QPushButton* acceptButton = new QPushButton("Принять", this);
    connect(acceptButton, &QPushButton::released, this, &TTariffDialog::acceptSlot);
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    connect(cancelButton, &QPushButton::released, this, &TTariffDialog::cancelSlot);
    buttonsLayout->addWidget(acceptButton);
    buttonsLayout->addWidget(cancelButton);
}

void TTariffDialog::acceptSlot() {
    tariffType type = createTariffType(typeBox->currentIndex());
    std::cout << type << std::endl;
    double costPerMB = costEdit->value();
    double discount = discountEdit->value();
    if (trafficEdit->isHidden()) {
        if (discount > costPerMB) {
            QMessageBox::critical(this, "Ошибка!", "Скидка не может быть больше цены!");
            return;
        }
        emit sendTariffData(new tariffForTraffic(type, discount, costPerMB));
    }
    else {
        double monthlyFee = feeEdit->value();
        double trafficIncluded = trafficEdit->value();
        emit sendTariffData(new tariffForMonth(type, discount, costPerMB, monthlyFee, trafficIncluded));
    }
}

void TTariffDialog::cancelSlot() {
    hide();
    setModal(false);
}

void TTariffDialog::config(int method) {

    typeBox->setCurrentIndex(0);
    costEdit->setValue(dv::cost);

    switch (method) {
    case 0: {
        std::cout << "ТАК БЫТЬ НЕ ДОЛЖНО!!!" << std::endl;
        break;
    }
    case 1: {
        feeEdit->setValue(dv::fee);
        trafficEdit->setValue(dv::traffic);
        discountLabel->setText("Скидка, %");
        discountEdit->setRange(0.0, 100.0);
        discountEdit->setValue(dv::discountRelative);
        feeLabel->show();
        feeEdit->show();
        trafficLabel->show();
        trafficEdit->show();
        break;
    }
    case 2: {
        discountLabel->setText("Скидка, $");
        discountEdit->setRange(0.0, 999999999.0);
        discountEdit->setValue(dv::discountAbsolute);
        feeLabel->hide();
        feeEdit->hide();
        trafficLabel->hide();
        trafficEdit->hide();
        break;
    }
    }
}

void TCentralWidget::tariffAcceptSlot(tariff *newTariff) {
    currentClient->setTariff(newTariff);
    currentClient->displayClient(clientInfo);
    tariffDialog->hide();
    tariffDialog->setModal(false);

    infoEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + QString::fromStdString(" Тариф для пользователя " + clientsList->currentItem()->text().toStdString() + " успешно установлен!"));
}

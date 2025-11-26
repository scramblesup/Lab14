#ifndef USER_H
#define USER_H

#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QDoubleSpinBox>

#include "core.h"

class TCentralWidget;
class TUserDialog;
class TTariffDialog;

class TMainWindow : public QMainWindow {

    Q_OBJECT

    private:
    TCentralWidget* centralWidget;

    public:
        TMainWindow(QWidget *parent = nullptr);
        ~TMainWindow();
};

class TCentralWidget : public QWidget, internet {

    Q_OBJECT

    private:
        QListWidget* clientsList;
        QTextEdit* clientInfo;
        QComboBox* typeBox;

        TUserDialog* newUserDialog;
        TTariffDialog* tariffDialog;
        QDoubleSpinBox* trafficEdit;

        QTextEdit* infoEdit;

        client* currentClient;

    private slots:
        void exitSlot();
        void tariffSlot();
        void loginSlot();
        void trafficSlot();
        void sumSlot();
        void payableSlot();
        void deleteSlot();
        void saveSlot();
        void loadSlot();
        void currentChangedSlot(QListWidgetItem* current, QListWidgetItem* previous);

        void userAcceptSlot(std::string name, std::string password);
        void tariffAcceptSlot(tariff* newTariff);

    public:
        TCentralWidget(QWidget* parent);
        ~TCentralWidget();

};

class TUserDialog : public QDialog {

Q_OBJECT

private:
    QLineEdit* loginEdit;
    QLineEdit* passwordEdit;

 private slots:
    void acceptSlot();
    void cancelSlot();
signals:
    void sendUserData(std::string, std::string);
public:
    void clear();
    TUserDialog(QWidget* parent = 0);
};

class TTariffDialog : public QDialog {

 Q_OBJECT

private:
    QComboBox* typeBox;
    QLabel* typeLabel;

    QDoubleSpinBox* discountEdit;
    QLabel* discountLabel;
    QDoubleSpinBox* costEdit;
    QLabel* costLabel;

    QDoubleSpinBox* feeEdit;
    QLabel* feeLabel;
    QDoubleSpinBox* trafficEdit;
    QLabel* trafficLabel;

public slots:
    void acceptSlot();
    void cancelSlot();
signals:
    void sendTariffData(tariff*);
public:
    void config(int method);
    TTariffDialog(QWidget* parent = 0);
};

#endif // USER_H

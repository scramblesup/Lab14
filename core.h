#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <list>
#include <string>
#include <QTextEdit>

class client;
class tariff;

class internet {
protected:
    std::map<std::string, client> clients;

public:
    void addClient(client);
    std::string findMostPayable();
    void setTariff(std::string, tariff*);
    double countAllPrice();
    void removeClient(std::string name);
    client* findClient(std::string name);
    void save(std::string filePath);
    void load(std::string filePath);
    internet();
};

enum tariffType { budget, optimal, luxury, unlimited };

class clientNotExists : public std::runtime_error {
public:
    clientNotExists(std::string);
};

class clientAlreadyExists : public std::runtime_error {
public:
    clientAlreadyExists(std::string);
};

class fileNotWork : public std::runtime_error {
public:
    fileNotWork(std::string);
};

class tariff {
    friend void internet::save(std::string);
protected:
    tariffType type;
    double discount;
    double costPerMb;
public:
    tariff();
    tariff(tariffType, double, double);
    virtual void displayTariff(QTextEdit* editor) = 0;
    virtual double countExpanses(double trafficConsumed) = 0;
};

class tariffForTraffic : public tariff {
public:
    double countExpanses(double trafficConsumed) override;
    void displayTariff(QTextEdit* editor) override;
    tariffForTraffic();
    tariffForTraffic(tariffType type_, double discount_, double costPerMb_);
};

class tariffForMonth : public tariff {
    friend void internet::save(std::string);
private:
    double monthlyFee;
    double trafficIncluded;
public:
    double countExpanses(double trafficConsumed) override;
    void displayTariff(QTextEdit* editor) override;
    tariffForMonth();
    tariffForMonth(tariffType type_, double discount_, double costPerMb_, double monthlyFee_, double trafficIncluded_);
};

class client {
    friend void internet::save(std::string);
private:
    std::string name;
    std::string password;
    tariff* clientTariff;
    double trafficConsumed;
public:
    client();
    client(std::string, std::string, tariff*, double);
    void addTraffic(double);
    double expanses();
    void setTariff(tariff*);
    void displayClient(QTextEdit* editor) const;
    std::string getName();
};


void displayMenu();
bool isPercent(std::string discount);
bool isDiscount(std::string discount, std::string monthlyFee);
bool isInteger(std::string&);
bool isDouble(std::string&);
tariffType createTariffType(int);


#endif // CORE_H

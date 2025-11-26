#include <fstream>

#include "core.h"

tariffType createTariffType(int type) {
    tariffType type_ = budget;
    switch (type) {
    case 0: {
        type_ = budget;
        break;
    }
    case 1: {
        type_ = optimal;
        break;
    }
    case 2: {
        type_ = luxury;
        break;
    }
    case 3: {
        type_ = unlimited;
        break;
    }
    }
    return type_;
}

void displayMenu() {
    std::cout << "\nИнтернет оператор\n";
    std::cout << "1. Ввести тариф для клиента\n";
    std::cout << "2. Зарегистрировать клиента\n";
    std::cout << "3. Ввести данные о потребленном клиентом трафике\n";
    std::cout << "4. Посчитать общую стоимость реализованного трафика\n";
    std::cout << "5. Найти того, кто заплатил больше всех\n";
    std::cout << "6. Удалить клиента\n";
    std::cout << "7. Выйти\n";
    std::cout << "Выберите опцию: ";
}

bool isPercent(std::string discount) {
    if (!isDouble(discount)) { return false; }
    if (std::stod(discount) >= 100.0) { return false; }
    return true;
}

bool isDiscount(std::string discount, std::string costPerMb) {
    if (!isDouble(discount)) { return false; }
    if (std::stod(discount) > std::stod(costPerMb)) {
        std::cout << " Скидка не может быть больше, чем сама плата!" << std::endl;
        return false;
    }
    return true;
}

bool isDouble(std::string& s) {
    if (s.empty()) {
        s.push_back('9');
        return false;
    }

    if (s.length() >= 11) {
        s.clear();
        s.push_back('9');
        return false;
    }


    for (char c : s) {
        if (!isdigit(c) && c != '.') {
            s.clear();
            s.push_back('9');
            return false;
        }
    }
    return true;
}

bool isInteger(std::string& s) {
    if (s.empty()) {
        s.push_back('9');
        return false;
    }

    if (s.length() >= 9) {
        s.clear();
        s.push_back('9');
        return false;
    }

    for (wchar_t c : s) {
        if (!isdigit(c)) {
            s.clear();
            s.push_back('9');
            return false;
        }
    }
    return true;
}

tariff::tariff() : type(budget), discount(0), costPerMb(0) { }

tariff::tariff(tariffType type_, double discount_, double costPerMb_) : type(type_), discount(discount_), costPerMb(costPerMb_) { }

client::client() : name(""), password(""), clientTariff(nullptr), trafficConsumed(0.0) { }

client::client(std::string name_, std::string password_, tariff* tariff, double trafficConsumed_)
    : name(name_), password(password_), clientTariff(tariff), trafficConsumed(trafficConsumed_) { }

void client::addTraffic(double trafficConsumed_) { trafficConsumed += trafficConsumed_; }

double client::expanses() {
    double exp;
    if (clientTariff != nullptr) { exp = clientTariff->countExpanses(trafficConsumed); }
    else { exp = 0; }
    return exp;
}

void client::setTariff(tariff* clientTariff_) {
    delete clientTariff;
    clientTariff = clientTariff_;
}

std::string client::getName() { return name; }

internet::internet() : clients() { }

void internet::addClient(client newClient) { clients.insert(std::make_pair(newClient.getName(), newClient)); }

std::string internet::findMostPayable() {
    auto mostPayableIt = clients.begin();
    double maxExpanses = mostPayableIt->second.expanses();

    for (auto it = ++clients.begin(); it != clients.end(); ++it) {

        double currentExpanses = it->second.expanses();

        if (currentExpanses > maxExpanses) {
            maxExpanses = currentExpanses;
            mostPayableIt = it;
        }
    }

    return mostPayableIt->first;
}

void internet::setTariff(std::string name, tariff* newTariff) {
    std::map<std::string, client>::iterator i  = clients.find(name);
    if (i == clients.end()) { throw clientNotExists("Клиента с именем " + name + " не существует!"); }
    i->second.setTariff(newTariff);
}

double internet::countAllPrice() {
    double price{ 0.0 };
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        price += it->second.expanses();
    }
    return price;
}

void internet::removeClient(std::string name) {
    size_t count = clients.erase(name);
    if (count == 0) {
        throw clientNotExists("Клиента с именем " + name + " не существует!");
    }
}

void client::displayClient(QTextEdit* editor) const {
    editor->clear();

    QString clientName = QString::fromStdString("Имя клиента: " + name);
    editor->append(clientName);

    QString traffic = QString("Потребленный трафик (МБ): %1").arg(trafficConsumed);
    editor->append(traffic);

    if (clientTariff != nullptr) {
        clientTariff->displayTariff(editor);
    } else {
        editor->append(QStringLiteral("Тариф еще не установлен!"));
    }
}

double tariffForTraffic::countExpanses(double trafficConsumed) {
    return trafficConsumed * (costPerMb - discount);
}

void tariffForTraffic::displayTariff(QTextEdit* editor) {
    QString typeStr;
    switch (type) {
    case budget:
        typeStr = QStringLiteral("Бюджетный");
        break;
    case optimal:
        typeStr = QStringLiteral("Оптимальный");
        break;
    case luxury:
        typeStr = QStringLiteral("Люкс");
        break;
    case unlimited:
        typeStr = QStringLiteral("Безлимитный");
        break;
    default:
        typeStr = QStringLiteral("Неизвестный тип");
        break;
    }

    editor->append(QString("Тип тарифа: %1").arg(typeStr));

    QString cost = QString("Стоимость за 1 МБ: %1").arg(costPerMb);
    editor->append(cost);

    QString discountStr = QString("Скидка (абсолютная величина): %1").arg(discount);
    editor->append(discountStr);

}

tariffForTraffic::tariffForTraffic() { }

tariffForTraffic::tariffForTraffic(tariffType type_, double discount_, double costPerMb_) : tariff(type_, discount_, costPerMb_) { }

double tariffForMonth::countExpanses(double trafficConsumed) {
    double payment{monthlyFee};
    payment = payment - payment * discount / 100.0;
    if (trafficConsumed > trafficIncluded) {
        payment = payment + (trafficConsumed - trafficIncluded) * costPerMb;
    }
    return payment;
}

void tariffForMonth::displayTariff(QTextEdit* editor) {
    QString typeStr;
    switch (type) {
    case budget:
        typeStr = QStringLiteral("Бюджетный");
        break;
    case optimal:
        typeStr = QStringLiteral("Оптимальный");
        break;
    case luxury:
        typeStr = QStringLiteral("Люкс");
        break;
    case unlimited:
        typeStr = QStringLiteral("Безлимитный");
        break;
    default:
        typeStr = QStringLiteral("Неизвестный тип");
        break;
    }

    editor->append(QString("Тип тарифа: %1").arg(typeStr));

    QString cost = QString("Стоимость за 1 МБ: %1").arg(costPerMb);
    editor->append(cost);

    QString fee = QString("Месячная плата: %1").arg(monthlyFee);
    editor->append(fee);

    QString discountStr = QString("Скидка: %1%").arg(discount);
    editor->append(discountStr);

    QString traffic = QString("Трафика в пакете (МБ): %1").arg(trafficIncluded);
    editor->append(traffic);
}

tariffForMonth::tariffForMonth() { }

tariffForMonth::tariffForMonth(tariffType type_, double discount_, double costPerMb_, double monthlyFee_, double trafficIncluded_)
    : tariff(type_, discount_, costPerMb_), monthlyFee(monthlyFee_), trafficIncluded(trafficIncluded_) { }

clientNotExists::clientNotExists(std::string error) : std::runtime_error(error) { }

clientAlreadyExists::clientAlreadyExists(std::string error) : runtime_error(error) { }

void internet::save(std::string filePath) {
    std::ofstream file(filePath);

    if (!file.is_open()) {
        throw fileNotWork("Не удалось открыть Файл!");
    }

    for (auto it = clients.begin(); it != clients.end(); ++it) {
        client& currClient = it->second;

        file << currClient.name << " "
             << currClient.password << " "
             << currClient.trafficConsumed << " ";

        if (currClient.clientTariff == nullptr) {
            file << 0 << "\n";
        }
        else {
            tariffForMonth* tMonth = dynamic_cast<tariffForMonth*>(currClient.clientTariff);
            tariffForTraffic* tTraffic = dynamic_cast<tariffForTraffic*>(currClient.clientTariff);

            if (tMonth) {
                file << 2 << " "
                     << (int)tMonth->type << " "
                     << tMonth->discount << " "
                     << tMonth->costPerMb << " "
                     << tMonth->monthlyFee << " "
                     << tMonth->trafficIncluded << "\n";
            }
            else if (tTraffic) {
                file << 1 << " "
                     << (int)tTraffic->type << " "
                     << tTraffic->discount << " "
                     << tTraffic->costPerMb << "\n";
            }
        }
    }

    file.close();
}

void internet::load(std::string filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw fileNotWork("Не удалось открыть файл!");
    }

    std::string tmpName, tmpPass;
    double tmpTraffic;
    int tariffMarker;

    while (file >> tmpName >> tmpPass >> tmpTraffic >> tariffMarker) {

        tariff* newTariff = nullptr;

        if (tariffMarker == 1) {
            int typeInt;
            double disc, cost;
            file >> typeInt >> disc >> cost;

            newTariff = new tariffForTraffic((tariffType)typeInt, disc, cost);
        }
        else if (tariffMarker == 2) {
            int typeInt;
            double disc, cost, mFee, trIncl;
            file >> typeInt >> disc >> cost >> mFee >> trIncl;

            newTariff = new tariffForMonth((tariffType)typeInt, disc, cost, mFee, trIncl);
        }
        client tempClient(tmpName, tmpPass, newTariff, tmpTraffic);

        this->addClient(tempClient);

    }

    file.close();
}

fileNotWork::fileNotWork(std::string error) : std::runtime_error(error) { }

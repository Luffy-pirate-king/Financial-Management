#ifndef LIBRARY_H_INCLUDED
#define LIBRARY_H_INCLUDED
//LINES ABOVE INCLUDE GUARD LINES TO AVOID DUPPLICATE DECLARATIONS

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <cmath>
#include <sstream>
using std::cin, std::vector, std::cout, std::string, std::to_string, std::getline, std::unordered_map, std::stringstream;

//General Functions
string trim(string);
string addCurrencyDots(long);

class Transactions{  //Data storage
public:
    enum class Type{INCOME, EXPENSE};
    Transactions(Type type, string date, string category, string description, long amount): type(type), date(date), category(category), description(description), amount(amount) {};
    string serialize() const;
    string getCategory() const;
    Type getType() const;
    long getAmount() const;
private:
    Type type;
    string date, category, description;
    long amount;
};

class Budget{  //Track limits per category
public:
    enum class Period{MONTHLY, WEEKLY};
    Budget() : category(""), amount(0.0), period(Period::MONTHLY) {};
    Budget(string category, long amount, Period period): category(category), amount(amount), period(period){};
    string serialize() const;
    double getRemaining(double);
    string getCatagory() const;
    long getAmount() const;
private:
    string category;
    Period period;
    long amount;
};

class FinanceManager{  //Core Logic
public:
    void addTransaction();
    void viewTransactions();
    void showBalance();
    void reportCategory();
    void setBudget();
    void saveAndExit();
    FinanceManager();
private:
    vector<Transactions> saveData;
    long calculateSpent(const string &);
    long calculateIncome(const string &);
    unordered_map<string, Budget> budgets;  //Track budgets
};

class UIHandler{  //User Interface
public:
    void run();
};

//LINE BELOW INCLUDES GUARD LINES TO AVOID DUPPLICATE DECLARATIONS
#endif // LIBRARY_H_INCLUDED

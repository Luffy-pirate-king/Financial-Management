#include "library.h"

//General Functions
string trim(string s){  //Eliminate whitespaces, newlines
    auto start = find_if_not(s.begin(), s.end(), [](int c){  //find_if_not: Finds the first element in a range that does not satisfy a given condition
        return isspace(static_cast<unsigned char>(c));
    });
    auto end = find_if_not(s.rbegin(), s.rend(), [](int c){  //rbegin, rend are short for reversed begin and end => traveling from last to first
        return isspace(static_cast<unsigned char>(c));
    }).base();  //Converts a reverse iterator to its corresponding forward iterator
    return (start == end) ? "" : string(start, end);
}

string addCurrencyDots(long amount){
    string amountStr = to_string(amount);
    for(int i = amountStr.length() - 3; i > 0; i -= 3){
        amountStr.insert(i, ".");
    }
    return amountStr;
}
//Transactions Functions
string Transactions::serialize() const{
    string typeStr = (type == Type::INCOME) ? "INCOME" : "EXPENSE";
    return typeStr + ", " + to_string(amount) + " VND, " + category + ", " + date + ", " + description;
}

long Transactions::getAmount() const{
    return amount;
}

string Transactions::getCategory() const{
    return category;
}

Transactions::Type Transactions::getType() const{
    return type;
}

//Budget Functions
string Budget::serialize() const{
    string periodStr = (period == Period::MONTHLY) ? "MONTHLY" : "WEEKLY";
    return category + ", " + to_string(amount) + ", " + periodStr;
}

double Budget::getRemaining(double spent){
    return amount - spent;
}

string Budget::getCatagory() const{
    return category;
}

long Budget::getAmount() const{
    return amount;
}

//FinanceManager Functions
FinanceManager::FinanceManager(){
    std::ifstream budFile("budData.csv");
    string line;
    while(getline(budFile, line)){
        stringstream ss(line);
        string category, amountStr, periodStr;
        getline(ss, category, ',');
        getline(ss, amountStr, ',');
        getline(ss, periodStr);
        try{
            long amount = stol(amountStr);
            Budget::Period period = (periodStr == "MONTHLY") ? Budget::Period::MONTHLY : Budget::Period::WEEKLY;
            budgets[category] = Budget(category, amount, period);
        } catch(std::invalid_argument& e){
            continue;
        } catch(std::out_of_range& e){
            continue;
        }
    }
}

long FinanceManager::calculateSpent(const string &category){
    long spent = 0;
    for(auto &t: saveData){
        if(t.getType() == Transactions::Type::EXPENSE && t.getCategory() == category){
            spent += t.getAmount();
        }
    }
    return spent;
}

long FinanceManager::calculateIncome(const string &category){
    long income = 0;
    for(auto &t: saveData){
        if(t.getType() == Transactions::Type::INCOME && t.getCategory() == category){
            income += t.getAmount();
        }
    }
    return income;
}

void FinanceManager::addTransaction(){
    string type, category, date, description;
    double amount;
    cout << "Adding Your New Transaction...\n";
    cout << "Insert Transaction Type (INCOME / EXPENSE): ";
    getline(cin, type);
    type = trim(type);
    Transactions::Type typeEnum;
    if(type == "INCOME"){
        typeEnum = Transactions::Type::INCOME;
    } else if(type == "EXPENSE"){
        typeEnum = Transactions::Type::EXPENSE;
    } else {
        cout << "Invalid type!" << std::endl;
        return;
    }
    cout << "Insert Transaction Category: ";
    getline(cin, category);
    category = trim(category);
    cout << "Insert Transaction Date: ";
    getline(cin, date);
    date = trim(date);
    cout << "Insert Description: ";
    getline(cin, description);
    cout << "Insert The Amount: ";
    string amountStr;
    getline(cin, amountStr);
    amountStr = trim(amountStr);
    try{
        amount = stod(amountStr);
    } catch(...){
        cout << "Invalid Amount!\n";
        return;
    }
    Transactions t(typeEnum, date, category, description, amount);
    saveData.push_back(t);
    if(type == "EXPENSE"){  //Modify the budget
        cout << "Added " << amount << " VND EXPENSE For " << category << "!";
        auto budgetItr = budgets.find(category);
        if(budgetItr != budgets.end()){
            long spent = calculateSpent(category);
            double budgetAmount = budgetItr->second.getAmount();
            long remaining = budgetAmount - spent;
            double percent = (spent/budgetAmount) * 100.0;
            cout << "\n[Budget Status]: Used " << addCurrencyDots(spent) << " VND (" << addCurrencyDots(remaining) << " remaining, " << std::fixed << std::setprecision(2) << percent << "% used)";
        }  //WILL ADD A SAVING LINE SO THE USED AMOUNT WILL BE REMEMBER, NOT ERASED EACH TIME
    } else {
        cout << "Added " << std::fixed << addCurrencyDots(amount) << " VND INCOME For " << category << "!";
        auto BudgetItr = budgets.find(category);
        if(BudgetItr != budgets.end()){
            long income = calculateIncome(category);
            cout << "\n[Income Status]: Earned A Total Of " << addCurrencyDots(income) << " From " << category << " This Month!";
        }
    }
    cout << "\nOperation Successfully!\n";
}

void FinanceManager::viewTransactions(){
    cout << "---TRANSACTIONS---\n";
    for(auto &t : saveData){
        cout << t.serialize() << std::endl;
    }
}

void FinanceManager::showBalance(){
    cout << "---BALANCE SUMMARY---\n";
    long income = 0, spent = 0;
    for(auto &t : saveData){
        if(t.getType() == Transactions::Type::EXPENSE){
            spent += t.getAmount();
        } else {
            income += t.getAmount();
        }
    }
    cout << "Total INCOME:\t" << addCurrencyDots(income) << "VND\n";
    cout << "Total EXPENSE:\t" << addCurrencyDots(spent) << "VND\n";
    cout << "--------------------------------\n";
    cout << "Your Current Balance Is: " << addCurrencyDots(income - spent) << "VND\n";
    unordered_map<string, long> incomeByCat;
    unordered_map<string, long> expenseByCat;
    for(auto &t : saveData){
        if(t.getType() == Transactions::Type::EXPENSE) expenseByCat[t.getCategory()] += t.getAmount();
        else incomeByCat[t.getCategory()] += t.getAmount();
    }
    cout << std::endl << "INCOME By Category:\n";
    for(auto &pair : incomeByCat){
        cout << "  " << pair.first << ": " << addCurrencyDots(pair.second) << "VND\n";
    }
    cout << std::endl << "EXPENSE By Category:\n";
    for(auto &pair : expenseByCat){
        cout << "  " << pair.first << ": " << addCurrencyDots(pair.second) << "VND\n";
    }
}

void FinanceManager::reportCategory(){

}

void FinanceManager::setBudget(){
    string category, periodStr, amountStr;
    double amount;
    cout << "Setting Budget...\n";
    cout << "Category: ";
    getline(cin, category);
    cout << "Amount: ";
    getline(cin, amountStr);
    amountStr = trim(amountStr);
    try{
        amount = stod(amountStr);
    } catch(...){
        cout << "Invalid Amount!\n";
        return;
    }
    cout << "Period (MONTHLY/WEEKLY): ";
    getline(cin, periodStr);
    Budget::Period periodEnum;
    if(periodStr == "MONTHLY"){
        periodEnum = Budget::Period::MONTHLY;
    } else if(periodStr == "WEEKLY"){
        periodEnum = Budget::Period::WEEKLY;
    } else {
        cout << "Invalid Period!\n";
        return;
    }
    // Set or update budget
    budgets[category] = Budget(category, amount, periodEnum);
    cout << "Budget set for category: " << category << " (" << amount << " VND)\n";

    // Now calculate spent and display usage
    long spent = calculateSpent(category);
    double percent = (amount > 0) ? (spent / amount) * 100.0 : 0.0;
    cout << spent << " VND spent so far (" << std::fixed << std::setprecision(2) << percent << "% used)\n";
}

void FinanceManager::saveAndExit(){
    cout << "Saving Data...\n";
    std::ofstream tranFile("tranData.csv");
    for(auto &t: saveData){
        tranFile << t.serialize() << std::endl;
    }
    std::ofstream budFile("budData.csv");
    for(auto &pair : budgets){
        budFile << pair.second.serialize() << std::endl;
    }
    cout << "\nOperation Completed!";
}

//UIHandler Functions
void UIHandler::run(){
    FinanceManager manager;
    string choice;
    do{  //Run infinitely until Exit
        cout << "----- PERSONAL FINANCE TRACKER -----\n";
        cout << "1. Add Transaction\n";
        cout << "2. View All Transactions\n";
        cout << "3. Show Balance Summary\n";
        cout << "4. View Category Reports\n";
        cout << "5. Set Budget\n";
        cout << "6. Save & Exit\n";
        cout << "\nEnter choice: ";
        getline(cin, choice);
        choice = trim(choice);  //Eliminate whitespaces
        if(choice == "1"){
            manager.addTransaction();
        } else if(choice == "2"){
            manager.viewTransactions();
        } else if(choice == "3"){
            manager.showBalance();
        } else if(choice == "4"){
            manager.reportCategory();
        } else if(choice == "5"){
            manager.setBudget();
        } else if(choice == "6"){
            manager.saveAndExit();
            return;  //To Exit
        } else {
            cout << "Invalid choice!\n";
        }
    } while(true);
}

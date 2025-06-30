#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>

using namespace std;

#define MIN_BALANCE 500

// Exception for insufficient funds
class InsufficientFunds {};

class Account {
private:
    long accountNumber;
    string firstName;
    string lastName;
    float balance;
    static long NextAccountNumber;

public:
    Account() {}
    Account(string fname, string lname, float balance);

    long getAccNo() { return accountNumber; }
    string getFirstName() { return firstName; }
    string getLastName() { return lastName; }
    float getBalance() { return balance; }

    void Deposit(float amount);
    void Withdraw(float amount);

    static void setLastAccountNumber(long accountNumber);
    static long getLastAccountNumber();

    friend ofstream& operator<<(ofstream& ofs, Account& acc);
    friend ifstream& operator>>(ifstream& ifs, Account& acc);
    friend ostream& operator<<(ostream& os, Account& acc);
};

long Account::NextAccountNumber = 0;

class Bank {
private:
    map<long, Account> accounts;

public:
    Bank();
    Account OpenAccount(string fname, string lname, float balance);
    Account BalanceEnquiry(long accountNumber);
    Account Deposit(long accountNumber, float amount);
    Account Withdraw(long accountNumber, float amount);
    void CloseAccount(long accountNumber);
    void ShowAllAccounts();
    ~Bank();
};

// === Account Method Definitions ===

Account::Account(string fname, string lname, float balance) {
    NextAccountNumber++;
    accountNumber = NextAccountNumber;
    firstName = fname;
    lastName = lname;
    this->balance = balance;
}

void Account::Deposit(float amount) {
    balance += amount;
}

void Account::Withdraw(float amount) {
    if (balance - amount < MIN_BALANCE) throw InsufficientFunds();
    balance -= amount;
}

void Account::setLastAccountNumber(long accountNumber) {
    NextAccountNumber = accountNumber;
}

long Account::getLastAccountNumber() {
    return NextAccountNumber;
}

ofstream& operator<<(ofstream& ofs, Account& acc) {
    ofs << acc.accountNumber << endl;
    ofs << acc.firstName << endl;
    ofs << acc.lastName << endl;
    ofs << acc.balance << endl;
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Account& acc) {
    ifs >> acc.accountNumber >> acc.firstName >> acc.lastName >> acc.balance;
    return ifs;
}

ostream& operator<<(ostream& os, Account& acc) {
    os << "-------------------------------\n";
    os << "Account Number: " << acc.getAccNo() << endl;
    os << "First Name:     " << acc.getFirstName() << endl;
    os << "Last Name:      " << acc.getLastName() << endl;
    os << "Balance:        ₹" << acc.getBalance() << endl;
    os << "-------------------------------\n";
    return os;
}

// === Bank Method Definitions ===

Bank::Bank() {
    Account account;
    ifstream infile("Bank.data");
    if (!infile) return;

    while (infile >> account) {
        accounts[account.getAccNo()] = account;
    }

    Account::setLastAccountNumber(account.getAccNo());
    infile.close();
}

Account Bank::OpenAccount(string fname, string lname, float balance) {
    Account account(fname, lname, balance);
    accounts[account.getAccNo()] = account;

    ofstream outfile("Bank.data", ios::trunc);
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        outfile << it->second;
    }
    outfile.close();

    return account;
}

Account Bank::BalanceEnquiry(long accountNumber) {
    if (accounts.find(accountNumber) == accounts.end()) throw runtime_error("Account not found");
    return accounts[accountNumber];
}

Account Bank::Deposit(long accountNumber, float amount) {
    if (accounts.find(accountNumber) == accounts.end()) throw runtime_error("Account not found");
    accounts[accountNumber].Deposit(amount);
    return accounts[accountNumber];
}

Account Bank::Withdraw(long accountNumber, float amount) {
    if (accounts.find(accountNumber) == accounts.end()) throw runtime_error("Account not found");
    accounts[accountNumber].Withdraw(amount);
    return accounts[accountNumber];
}

void Bank::CloseAccount(long accountNumber) {
    if (accounts.find(accountNumber) == accounts.end()) throw runtime_error("Account not found");
    cout << "Account to be closed:\n" << accounts[accountNumber];
    accounts.erase(accountNumber);
}

void Bank::ShowAllAccounts() {
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        cout << it->second;
    }
}

Bank::~Bank() {
    ofstream outfile("Bank.data", ios::trunc);
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        outfile << it->second;
    }
    outfile.close();
}

// === Main Program ===

int main() {
    Bank b;
    Account acc;
    int choice;
    string fname, lname;
    long accountNumber;
    float balance, amount;

    cout << "========== BANKING SYSTEM ==========\n";

    do {
        cout << "\nPlease choose an option:\n"
             << "1. Open Account\n"
             << "2. Balance Enquiry\n"
             << "3. Deposit\n"
             << "4. Withdrawal\n"
             << "5. Close Account\n"
             << "6. Show All Accounts\n"
             << "7. Exit\n"
             << "Enter your choice (1-7): ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter First Name: ";
                cin >> fname;
                cout << "Enter Last Name: ";
                cin >> lname;
                cout << "Enter Initial Balance (Min ₹" << MIN_BALANCE << "): ";
                cin >> balance;
                if (balance < MIN_BALANCE) {
                    cout << "❌ Balance must be at least ₹" << MIN_BALANCE << "!\n";
                    break;
                }
                acc = b.OpenAccount(fname, lname, balance);
                cout << "✅ Account created successfully!\n" << acc;
                break;

            case 2:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                try {
                    acc = b.BalanceEnquiry(accountNumber);
                    cout << "✅ Account Details:\n" << acc;
                } catch (...) {
                    cout << "❌ Account not found!\n";
                }
                break;

            case 3:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount to Deposit: ";
                cin >> amount;
                try {
                    acc = b.Deposit(accountNumber, amount);
                    cout << "✅ Amount Deposited.\n" << acc;
                } catch (...) {
                    cout << "❌ Invalid account!\n";
                }
                break;

            case 4:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                cout << "Enter Amount to Withdraw: ";
                cin >> amount;
                try {
                    acc = b.Withdraw(accountNumber, amount);
                    cout << "✅ Amount Withdrawn.\n" << acc;
                } catch (InsufficientFunds) {
                    cout << "❌ Insufficient Balance! Minimum ₹" << MIN_BALANCE << " required.\n";
                } catch (...) {
                    cout << "❌ Invalid account!\n";
                }
                break;

            case 5:
                cout << "Enter Account Number: ";
                cin >> accountNumber;
                try {
                    b.CloseAccount(accountNumber);
                    cout << "✅ Account Closed Successfully.\n";
                } catch (...) {
                    cout << "❌ Invalid account!\n";
                }
                break;

            case 6:
                b.ShowAllAccounts();
                break;

            case 7:
                cout << "👋 Exiting... Thank you!\n";
                break;

            default:
                cout << "❌ Invalid choice. Try again.\n";
        }
    } while (choice != 7);

    return 0;
}

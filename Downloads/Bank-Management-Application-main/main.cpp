#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

// ─────────────────────────────────────────────
//  BankAccount
// ─────────────────────────────────────────────
class BankAccount {
private:
    int    accNo;
    std::string name;
    double balance;

public:
    // ── Constructors ──────────────────────────
    BankAccount() : accNo(0), balance(0.0) {}
    BankAccount(int no, const std::string& n, double bal)
        : accNo(no), name(n), balance(bal) {}

    // ── Accessors ─────────────────────────────
    int         getAccNo()   const { return accNo;   }
    std::string getName()    const { return name;    }
    double      getBalance() const { return balance; }

    // ── Core operations ───────────────────────
    void createAccount() {
        std::cout << "Enter Account Number: ";
        while (!(std::cin >> accNo) || accNo <= 0) {
            std::cout << "Invalid. Enter a positive account number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter Customer Name: ";
        std::getline(std::cin, name);
        while (name.empty()) {
            std::cout << "Name cannot be empty. Enter Customer Name: ";
            std::getline(std::cin, name);
        }

        std::cout << "Enter Initial Balance: ";
        while (!(std::cin >> balance) || balance < 0) {
            std::cout << "Invalid. Enter a non-negative balance: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    void displayAccount() const {
        std::cout << "\n----------------------------------\n";
        std::cout << "Account Number : " << accNo        << "\n";
        std::cout << "Customer Name  : " << name         << "\n";
        std::cout << "Balance        : $"
                  << std::fixed << std::setprecision(2)
                  << balance << "\n";
        std::cout << "----------------------------------\n";
    }

    bool deposit(double amount) {
        if (amount <= 0) {
            std::cout << "Deposit amount must be positive.\n";
            return false;
        }
        balance += amount;
        std::cout << "Deposited $" << std::fixed << std::setprecision(2)
                  << amount << " successfully.\n";
        return true;
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            std::cout << "Withdrawal amount must be positive.\n";
            return false;
        }
        if (amount > balance) {
            std::cout << "Insufficient balance! Available: $"
                      << std::fixed << std::setprecision(2)
                      << balance << "\n";
            return false;
        }
        balance -= amount;
        std::cout << "Withdrew $" << std::fixed << std::setprecision(2)
                  << amount << " successfully.\n";
        return true;
    }

    // ── File I/O ──────────────────────────────
    bool writeToFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "' for writing.\n";
            return false;
        }
        file << accNo   << "\n"
             << name    << "\n"
             << balance << "\n";
        return true;
    }

    // Populate this object from an open input stream; returns false on EOF/error.
    bool readFromStream(std::ifstream& in) {
        if (!(in >> accNo)) return false;
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (!std::getline(in, name)) return false;
        if (!(in >> balance))        return false;
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return true;
    }
};

// ─────────────────────────────────────────────
//  File helpers
// ─────────────────────────────────────────────
const std::string DATA_FILE = "bank.txt";

// Load all accounts from file into a vector.
std::vector<BankAccount> loadAccounts() {
    std::vector<BankAccount> accounts;
    std::ifstream file(DATA_FILE);
    if (!file.is_open()) return accounts;   // first run — file doesn't exist yet

    BankAccount tmp;
    while (tmp.readFromStream(file))
        accounts.push_back(tmp);

    return accounts;
}

// Overwrite the file with the current in-memory list.
bool saveAccounts(const std::vector<BankAccount>& accounts) {
    std::ofstream file(DATA_FILE, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Could not save accounts to '" << DATA_FILE << "'.\n";
        return false;
    }
    for (const auto& acc : accounts) {
        file << acc.getAccNo()   << "\n"
             << acc.getName()    << "\n"
             << acc.getBalance() << "\n";
    }
    return true;
}

// Find account index by account number; returns -1 if not found.
int findAccount(const std::vector<BankAccount>& accounts, int accNo) {
    for (int i = 0; i < static_cast<int>(accounts.size()); ++i)
        if (accounts[i].getAccNo() == accNo)
            return i;
    return -1;
}

// ─────────────────────────────────────────────
//  Input helpers
// ─────────────────────────────────────────────
int readInt(const std::string& prompt) {
    int val;
    std::cout << prompt;
    while (!(std::cin >> val)) {
        std::cout << "Invalid input. " << prompt;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return val;
}

double readDouble(const std::string& prompt) {
    double val;
    std::cout << prompt;
    while (!(std::cin >> val)) {
        std::cout << "Invalid input. " << prompt;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return val;
}

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────
int main() {
    std::vector<BankAccount> accounts = loadAccounts();
    std::cout << "Loaded " << accounts.size() << " account(s) from file.\n";

    int choice;
    do {
        std::cout << "\n======= BANK MANAGEMENT SYSTEM =======\n";
        std::cout << "1. Create Account\n";
        std::cout << "2. Display Account\n";
        std::cout << "3. Deposit Amount\n";
        std::cout << "4. Withdraw Amount\n";
        std::cout << "5. Check Balance\n";
        std::cout << "6. List All Accounts\n";
        std::cout << "7. Exit\n";
        std::cout << "======================================\n";

        choice = readInt("Enter Your Choice: ");

        switch (choice) {

        case 1: {
            BankAccount newAcc;
            newAcc.createAccount();
            if (findAccount(accounts, newAcc.getAccNo()) != -1) {
                std::cout << "Account number already exists!\n";
            } else {
                accounts.push_back(newAcc);
                saveAccounts(accounts);
                std::cout << "Account created successfully!\n";
            }
            break;
        }

        case 2: {
            int no = readInt("Enter Account Number: ");
            int idx = findAccount(accounts, no);
            if (idx == -1)
                std::cout << "Account not found.\n";
            else
                accounts[idx].displayAccount();
            break;
        }

        case 3: {
            int no = readInt("Enter Account Number: ");
            int idx = findAccount(accounts, no);
            if (idx == -1) {
                std::cout << "Account not found.\n";
            } else {
                double amount = readDouble("Enter Amount to Deposit: $");
                if (accounts[idx].deposit(amount))
                    saveAccounts(accounts);
            }
            break;
        }

        case 4: {
            int no = readInt("Enter Account Number: ");
            int idx = findAccount(accounts, no);
            if (idx == -1) {
                std::cout << "Account not found.\n";
            } else {
                double amount = readDouble("Enter Amount to Withdraw: $");
                if (accounts[idx].withdraw(amount))
                    saveAccounts(accounts);
            }
            break;
        }

        case 5: {
            int no = readInt("Enter Account Number: ");
            int idx = findAccount(accounts, no);
            if (idx == -1)
                std::cout << "Account not found.\n";
            else
                std::cout << "Current Balance: $"
                          << std::fixed << std::setprecision(2)
                          << accounts[idx].getBalance() << "\n";
            break;
        }

        case 6: {
            if (accounts.empty()) {
                std::cout << "No accounts found.\n";
            } else {
                std::cout << "\n--- All Accounts (" << accounts.size() << ") ---\n";
                for (const auto& acc : accounts)
                    acc.displayAccount();
            }
            break;
        }

        case 7:
            std::cout << "Thank you for using Bank Management System. Goodbye!\n";
            break;

        default:
            std::cout << "Invalid choice. Please enter 1-7.\n";
        }

    } while (choice != 7);

    return 0;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

using namespace std;

// ─── Utility ────────────────────────────────────────────────────────────────

// Case-insensitive substring search
bool containsIgnoreCase(const string& haystack, const string& needle) {
    string h = haystack, n = needle;
    transform(h.begin(), h.end(), h.begin(), ::tolower);
    transform(n.begin(), n.end(), n.begin(), ::tolower);
    return h.find(n) != string::npos;
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ─── Book ────────────────────────────────────────────────────────────────────

class Book {
private:
    int    bookId;
    string title;
    string author;
    int    quantity;

public:
    // Default constructor
    Book() : bookId(0), quantity(0) {}

    // Parameterised constructor (used when loading from file)
    Book(int id, const string& t, const string& a, int q)
        : bookId(id), title(t), author(a), quantity(q) {}

    // ── Getters ──────────────────────────────────────────────────────────────
    int    getBookId()   const { return bookId; }
    string getTitle()    const { return title;  }
    string getAuthor()   const { return author; }
    int    getQuantity() const { return quantity; }

    // ── Input ────────────────────────────────────────────────────────────────
    void inputBook() {
        cout << "Enter Book ID    : ";
        cin  >> bookId;
        clearInputBuffer();

        cout << "Enter Book Title : ";
        getline(cin, title);

        cout << "Enter Author Name: ";
        getline(cin, author);

        cout << "Enter Quantity   : ";
        while (!(cin >> quantity) || quantity < 0) {
            cout << "Invalid quantity. Enter a non-negative number: ";
            clearInputBuffer();
        }
        clearInputBuffer();
    }

    // ── Display ──────────────────────────────────────────────────────────────
    void display() const {
        cout << "\n  Book ID  : " << bookId
             << "\n  Title    : " << title
             << "\n  Author   : " << author
             << "\n  Quantity : " << quantity << "\n";
    }

    // ── Transactions ─────────────────────────────────────────────────────────
    bool issue() {
        if (quantity > 0) {
            quantity--;
            return true;
        }
        return false;
    }

    void returnBook() {
        quantity++;
    }

    // ── Serialisation ────────────────────────────────────────────────────────
    // Write a single record as four lines
    void writeTo(ofstream& out) const {
        out << bookId   << "\n"
            << title    << "\n"
            << author   << "\n"
            << quantity << "\n";
    }
};

// ─── Library ─────────────────────────────────────────────────────────────────

class Library {
private:
    vector<Book> books;
    const string dataFile = "library.txt";

    // ── Persistence ──────────────────────────────────────────────────────────

    void loadFromFile() {
        ifstream file(dataFile);
        if (!file.is_open()) return;   // first run — no file yet

        int    id, qty;
        string title, author, line;

        while (file >> id) {
            file.ignore();
            getline(file, title);
            getline(file, author);
            file >> qty;
            file.ignore();
            books.emplace_back(id, title, author, qty);
        }
        file.close();
    }

    void saveAllToFile() const {
        ofstream file(dataFile, ios::trunc);   // overwrite completely
        if (!file.is_open()) {
            cerr << "Error: could not open " << dataFile << " for writing.\n";
            return;
        }
        for (const auto& b : books) b.writeTo(file);
        file.close();
    }

    // ── Helpers ──────────────────────────────────────────────────────────────

    bool idExists(int id) const {
        for (const auto& b : books)
            if (b.getBookId() == id) return true;
        return false;
    }

    Book* findById(int id) {
        for (auto& b : books)
            if (b.getBookId() == id) return &b;
        return nullptr;
    }

    // ── Menu actions ─────────────────────────────────────────────────────────

    void addBook() {
        Book b;
        b.inputBook();

        if (idExists(b.getBookId())) {
            cout << "A book with ID " << b.getBookId() << " already exists.\n";
            return;
        }

        books.push_back(b);
        saveAllToFile();
        cout << "Book added successfully!\n";
    }

    void displayAll() const {
        if (books.empty()) {
            cout << "No books in the library.\n";
            return;
        }
        cout << "\n========== ALL BOOKS (" << books.size() << ") ==========";
        for (const auto& b : books) b.display();
    }

    void issueBook() {
        cout << "Enter Book ID to Issue: ";
        int id; cin >> id; clearInputBuffer();

        Book* b = findById(id);
        if (!b) { cout << "Book ID " << id << " not found.\n"; return; }

        if (b->issue()) {
            saveAllToFile();
            cout << "\"" << b->getTitle() << "\" issued successfully! "
                 << "Remaining copies: " << b->getQuantity() << "\n";
        } else {
            cout << "Sorry, \"" << b->getTitle() << "\" is currently out of stock.\n";
        }
    }

    void returnBook() {
        cout << "Enter Book ID to Return: ";
        int id; cin >> id; clearInputBuffer();

        Book* b = findById(id);
        if (!b) { cout << "Book ID " << id << " not found.\n"; return; }

        b->returnBook();
        saveAllToFile();
        cout << "\"" << b->getTitle() << "\" returned successfully! "
             << "Available copies: " << b->getQuantity() << "\n";
    }

    void searchByTitle() const {
        cout << "Enter title (or part of it): ";
        string query; getline(cin, query);

        bool found = false;
        for (const auto& b : books) {
            if (containsIgnoreCase(b.getTitle(), query)) {
                b.display();
                found = true;
            }
        }
        if (!found) cout << "No books matched \"" << query << "\".\n";
    }

    void searchByAuthor() const {
        cout << "Enter author (or part of name): ";
        string query; getline(cin, query);

        bool found = false;
        for (const auto& b : books) {
            if (containsIgnoreCase(b.getAuthor(), query)) {
                b.display();
                found = true;
            }
        }
        if (!found) cout << "No books matched \"" << query << "\".\n";
    }

    void deleteBook() {
        cout << "Enter Book ID to Delete: ";
        int id; cin >> id; clearInputBuffer();

        auto it = find_if(books.begin(), books.end(),
                          [id](const Book& b){ return b.getBookId() == id; });

        if (it == books.end()) {
            cout << "Book ID " << id << " not found.\n";
            return;
        }

        cout << "Deleting: " << it->getTitle() << " by " << it->getAuthor() << "\n";
        books.erase(it);
        saveAllToFile();
        cout << "Book deleted successfully!\n";
    }

public:
    Library() { loadFromFile(); }

    void run() {
        int choice;
        do {
            cout << "\n====== LIBRARY MANAGEMENT SYSTEM ======\n"
                 << " 1. Add Book\n"
                 << " 2. Display All Books\n"
                 << " 3. Issue Book\n"
                 << " 4. Return Book\n"
                 << " 5. Search by Title\n"
                 << " 6. Search by Author\n"
                 << " 7. Delete Book\n"
                 << " 8. Exit\n"
                 << "========================================\n"
                 << "Enter your choice: ";

            if (!(cin >> choice)) {
                clearInputBuffer();
                cout << "Please enter a number between 1 and 8.\n";
                choice = 0;
                continue;
            }
            clearInputBuffer();

            switch (choice) {
                case 1: addBook();       break;
                case 2: displayAll();    break;
                case 3: issueBook();     break;
                case 4: returnBook();    break;
                case 5: searchByTitle(); break;
                case 6: searchByAuthor();break;
                case 7: deleteBook();    break;
                case 8: cout << "Goodbye!\n"; break;
                default: cout << "Invalid choice. Enter 1-8.\n";
            }

        } while (choice != 8);
    }
};

// ─── Entry point ─────────────────────────────────────────────────────────────

int main() {
    Library lib;
    lib.run();
    return 0;
}

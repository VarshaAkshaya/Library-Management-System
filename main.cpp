#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <limits>
using namespace std;

// this class is for a single book, it stores all details of one book
class Book {
public:
    string title;
    string author;
    string publisher;
    string ISBN;
    int edition;
    bool isIssued;
    string issuedTo; // name of person who took the book, empty if no one has it

    Book(){
    edition = 0;
    isIssued = false;
  }

    Book(string t, string a, string p, string isbn, int e) : title(t), author(a), publisher(p), ISBN(isbn), edition(e), isIssued(false) {}

    // this function converts the book into one line of text
    string toFileLine() const {
        ostringstream oss;
        oss << title << "|" << author << "|" << publisher << "|" << ISBN << "|" << edition << "|" << isIssued << "|" << issuedTo;
        return oss.str();
    }

    // takes one line from file and makes a Book object from it
    static Book fromFileLine(const string &line) {
        Book b;
        stringstream ss(line);
        string issuedStr;

        getline(ss, b.title, '|');
        getline(ss, b.author, '|');
        getline(ss, b.publisher, '|');
        getline(ss, b.ISBN, '|');

        string editionStr;
        getline(ss, editionStr, '|');
        if(editionStr.empty()) b.edition = 0;
        else b.edition = stoi(editionStr);

        getline(ss, issuedStr, '|');
        b.isIssued = (issuedStr == "1");

        getline(ss, b.issuedTo, '|');

        return b;
    }
};

// it keeps a list of books and does all the operations like add, delete etc
class Library {
private:
    vector<Book> books;
    string dataFile;

public:
    Library(const string &filename) {
        dataFile = filename;
        loadFromFile();
    }

    // reads old data from file so we dont lose previous books
    void loadFromFile() {
        ifstream inFile(dataFile);
        if (!inFile.is_open())
            return;

        string line;
        while (getline(inFile, line)) {
            if (!line.empty())
                books.push_back(Book::fromFileLine(line));
        }
        inFile.close();
    }

    // every time something changes we call this to update the file
    void saveToFile() {
        ofstream outFile(dataFile);
        if (!outFile.is_open()) {
            cout << "Error: could not open file for saving.\n";
            return;
        }
        for (int i = 0; i < books.size(); i++) {
            outFile << books[i].toFileLine() << "\n";
        }
        outFile.close();
    }
    // adds a new book, also checking that user doesnt enter blank stuff
    void addBook() {
        Book b;
        while (true) {
            cout << "Enter Title: ";
            getline(cin, b.title);
            if (!b.title.empty())
                break;
            cout << "Title cannot be empty.\n";
        }
        while (true) {
            cout << "Enter Author: ";
            getline(cin, b.author);
            if (!b.author.empty())
                break;
            cout << "Author cannot be empty.\n";
        }
        while (true) {
            cout << "Enter Publisher: ";
            getline(cin, b.publisher);
            if (!b.publisher.empty())
                break;
            cout << "Publisher cannot be empty.\n";
        }
        while (true) {
            cout << "Enter ISBN: ";
            getline(cin, b.ISBN);

            if (b.ISBN.empty()) {
                cout << "ISBN cannot be empty.\n";
                continue;
            }

            if (findBookIndexByISBN(b.ISBN) != -1) {
                cout << "A book with this ISBN already exists.\n";
                continue;
            }

            break;
        }

        // edition, needs to be a number more than 0
        while (true) {
            cout << "Enter Edition (positive integer): ";

            if (!(cin >> b.edition)) {
                cout << "Invalid input! Please enter a number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            if (b.edition <= 0) {
                cout << "Edition must be greater than 0.\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }

        b.isIssued = false;
        books.push_back(b);
        saveToFile();

        cout << "\nBook added successfully.\n";
    }
    // prints all books in a table format
    void displayAllBooks() const {
        if (books.empty()) {
            cout << "No books in the library yet.\n";
            return;
        }

        cout << left << setw(5) << "No." << setw(20) << "Title" << setw(15) << "Author" << setw(15) << "ISBN" << setw(10) << "Status" << "\n";
        cout << string(65, '-') << "\n";

        for (int i = 0; i < books.size(); i++) {
            cout << left << setw(5) << i + 1 << setw(20) << books[i].title << setw(15) << books[i].author << setw(15) << books[i].ISBN << setw(10) << (books[i].isIssued ? "Issued" : "Available") << "\n";
        }
    }

    //to find book with matching isbn and it should returns -1 if not that book is not found
    int findBookIndexByISBN(const string &isbn) const {
        for (int i = 0; i < books.size(); i++) {
            if (books[i].ISBN == isbn)
                return i;
        }
        return -1;
    }

    void searchBook() const {
        cout << "Enter ISBN to search: ";
        string isbn;
        getline(cin, isbn);

        int idx = findBookIndexByISBN(isbn);
        if (idx == -1) {
            cout << "No book found with that ISBN.\n";
            return;
        }

        cout << "Title: " << books[idx].title << "\n";
        cout << "Author: " << books[idx].author << "\n";
        cout << "Publisher: " << books[idx].publisher << "\n";
        cout << "Edition: " << books[idx].edition << "\n";
        if (books[idx].isIssued)
            cout << "Status: Issued to " << books[idx].issuedTo << "\n";
        else
            cout << "Status: Available\n";
    }

    void issueBook() {
        cout << "Enter ISBN of the book to issue: ";
        string isbn;
        getline(cin, isbn);

        int idx = findBookIndexByISBN(isbn);
        if (idx == -1) {
            cout << "No book found with that ISBN.\n";
            return;
        }
        if (books[idx].isIssued) {
            cout << "This book is already issued to " << books[idx].issuedTo << ".\n";
            return;
        }

        cout << "Enter member name to issue to: ";
        string member;
        getline(cin, member);

        books[idx].isIssued = true;
        books[idx].issuedTo = member;
        saveToFile();
        cout << "Book issued successfully.\n";
    }

    void returnBook() {
        cout << "Enter ISBN of the book to return: ";
        string isbn;
        getline(cin, isbn);

        int idx = findBookIndexByISBN(isbn);
        if (idx == -1) {
            cout << "No book found with that ISBN.\n";
            return;
        }
        if (!books[idx].isIssued) {
            cout << "This book was not issued.\n";
            return;
        }

        books[idx].isIssued = false;
        books[idx].issuedTo = "";
        saveToFile();
        cout << "Book returned successfully.\n";
    }

    void deleteBook() {
        cout << "Enter ISBN of the book to delete: ";
        string isbn;
        getline(cin, isbn);

        int idx = findBookIndexByISBN(isbn);
        if (idx == -1) {
            cout << "No book found with that ISBN.\n";
            return;
        }

        books.erase(books.begin() + idx);
        saveToFile();
        cout << "Book deleted successfully.\n";
    }
};

// prints the menu options
void showMenu() {
    cout << "\n--------------------------------\n";
    cout << "     LIBRARY MANAGEMENT SYSTEM\n";
    cout << "---------------------------------\n";
    cout << "1. Add Book\n";
    cout << "2. Display All Books\n";
    cout << "3. Search Book (by ISBN)\n";
    cout << "4. Issue Book\n";
    cout << "5. Return Book\n";
    cout << "6. Delete Book\n";
    cout << "7. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    Library library("book_data.txt");
    int choice;

    do {
        showMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clearing leftover newline so getline works properly next time

        switch (choice) {
            case 1:
                library.addBook();
                break;
            case 2:
                library.displayAllBooks();
                break;
            case 3:
                library.searchBook();
                break;
            case 4:
                library.issueBook();
                break;
            case 5:
                library.returnBook();
                break;
            case 6:
                library.deleteBook();
                break;
            case 7:
                cout << "Exiting... Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 7);

    return 0;
}

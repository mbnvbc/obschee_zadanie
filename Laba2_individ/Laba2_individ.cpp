#include "pch.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text;

public ref class Document : public IComparable<Document^>
{
private:
    String^ title;
    String^ author;
    int year;

public:
    property String^ Title
    {
        String^ get() { return title; }
        void set(String^ value) { title = value; }
    }

    property String^ Author
    {
        String^ get() { return author; }
        void set(String^ value) { author = value; }
    }

    property int Year
    {
        int get() { return year; }
        void set(int value) { year = value; }
    }

    // Конструктор
    Document(String^ title, String^ author, int year)
    {
        this->title = title;
        this->author = author;
        this->year = year;
    }

    // Абстрактные методы
    virtual String^ GetDocumentType() abstract;
    virtual String^ ToCSV() abstract;

    // Сравнение: сначала по году, потом по автору
    virtual int CompareTo(Document^ other) override
    {
        if (other == nullptr) return 1;
        int yearCmp = Year.CompareTo(other->Year);
        if (yearCmp != 0) return yearCmp;
        return String::Compare(Author, other->Author, StringComparison::OrdinalIgnoreCase);
    }
};

public ref class Book : public Document
{
public:
    Book(String^ title, String^ author, int year)
        : Document(title, author, year) {
    }

    virtual String^ GetDocumentType() override { return "Book"; }
    virtual String^ ToCSV() override
    {
        return String::Format("Book,{0},{1},{2}", Title, Author, Year);
    }
};

public ref class Magazine : public Document
{
public:
    Magazine(String^ title, String^ author, int year)
        : Document(title, author, year) {
    }

    virtual String^ GetDocumentType() override { return "Magazine"; }
    virtual String^ ToCSV() override
    {
        return String::Format("Magazine,{0},{1},{2}", Title, Author, Year);
    }
};

public ref class EBook : public Document
{
public:
    EBook(String^ title, String^ author, int year)
        : Document(title, author, year) {
    }

    virtual String^ GetDocumentType() override { return "EBook"; }
    virtual String^ ToCSV() override
    {
        return String::Format("EBook,{0},{1},{2}", Title, Author, Year);
    }
};

public ref class DocumentFactory
{
public:
    static Document^ FromCSV(String^ line)
    {
        if (String::IsNullOrWhiteSpace(line))
            throw gcnew ArgumentException("Пустая строка CSV");

        array<String^>^ parts = line->Split(',');
        if (parts->Length != 4)
            throw gcnew ArgumentException("Неверное количество полей в строке CSV");

        String^ type = parts[0];
        String^ title = parts[1];
        String^ author = parts[2];
        int year = 0;
        if (!Int32::TryParse(parts[3], year))
            throw gcnew ArgumentException("Неверный год: " + parts[3]);

        if (type == "Book")
            return gcnew Book(title, author, year);
        else if (type == "Magazine")
            return gcnew Magazine(title, author, year);
        else if (type == "EBook")
            return gcnew EBook(title, author, year);
        else
            throw gcnew NotSupportedException("Неизвестный тип документа: " + type);
    }
};

public ref class Library
{
private:
    List<Document^>^ documents;

public:
    Library()
    {
        documents = gcnew List<Document^>();
    }

    void AddDocument(Document^ doc)
    {
        if (doc != nullptr)
            documents->Add(doc);
    }

    property List<Document^>^ Documents
    {
        List<Document^>^ get() { return documents; }
    }

    void Sort()
    {
        documents->Sort();
    }

    void ExportToCSV(String^ filename)
    {
        StreamWriter^ writer = gcnew StreamWriter(filename, false, Encoding::UTF8);
        for each (Document ^ doc in documents)
        {
            writer->WriteLine(doc->ToCSV());
        }
        writer->Close();
    }

    void ImportFromCSV(String^ filename)
    {
        documents->Clear();
        StreamReader^ reader = gcnew StreamReader(filename, Encoding::UTF8);
        String^ line;
        while ((line = reader->ReadLine()) != nullptr)
        {
            line = line->Trim();
            if (!String::IsNullOrEmpty(line))
            {
                documents->Add(DocumentFactory::FromCSV(line));
            }
        }
        reader->Close();
    }

    void PrintCatalog()
    {
        for each (Document ^ doc in documents)
        {
            Console::WriteLine("{0}: \"{1}\" by {2} ({3})",
                doc->GetDocumentType(), doc->Title, doc->Author, doc->Year);
        }
    }
};

int main(array<System::String^>^ args)
{
    try
    {
        Library^ lib = gcnew Library();
        Console::WriteLine("Library Management System");
        Console::WriteLine("Add your documents first.\n");

        
        while (true)
        {
            Console::WriteLine("Select type:");
            Console::WriteLine("1 — Book");
            Console::WriteLine("2 — Magazine");
            Console::WriteLine("3 — EBook");
            Console::WriteLine("0 — Finish input");
            Console::Write("Choice (0-3): ");
            String^ choice = Console::ReadLine()->Trim();

            if (choice == "0") break;
            if (choice != "1" && choice != "2" && choice != "3")
            {
                Console::WriteLine("⚠️ Invalid choice.\n");
                continue;
            }

            Console::Write("Title (empty to cancel): ");
            String^ title = Console::ReadLine()->Trim();
            if (String::IsNullOrEmpty(title)) continue;

            Console::Write("Author: ");
            String^ author = Console::ReadLine()->Trim();
            if (String::IsNullOrEmpty(author)) author = "Unknown";

            Console::Write("Year: ");
            String^ yearStr = Console::ReadLine()->Trim();
            int year = 0;
            Int32::TryParse(yearStr, year); 

            if (choice == "1")
                lib->AddDocument(gcnew Book(title, author, year));
            else if (choice == "2")
                lib->AddDocument(gcnew Magazine(title, author, year));
            else if (choice == "3")
                lib->AddDocument(gcnew EBook(title, author, year));

            Console::WriteLine("Added!\n");
        }

        if (lib->Documents->Count == 0)
        {
            Console::WriteLine("No documents. Exiting.");
            return 0;
        }

        while (true)
        {
            Console::WriteLine("\n=== Menu ===");
            Console::WriteLine("1 — Show unsorted catalog");
            Console::WriteLine("2 — Show sorted catalog");
            Console::WriteLine("3 — Save current catalog to CSV (as is)");
            Console::WriteLine("4 — Save sorted catalog to CSV");
            Console::WriteLine("5 — Exit");
            Console::Write("Your choice (1-5): ");
            String^ opt = Console::ReadLine()->Trim();

            if (opt == "1")
            {
                Console::WriteLine("\n--- Unsorted Catalog ---");
                lib->PrintCatalog();
            }
            else if (opt == "2")
            {
                Library^ temp = gcnew Library();
                for each (Document ^ d in lib->Documents)
                {
                    if (d->GetDocumentType() == "Book")
                        temp->AddDocument(gcnew Book(d->Title, d->Author, d->Year));
                    else if (d->GetDocumentType() == "Magazine")
                        temp->AddDocument(gcnew Magazine(d->Title, d->Author, d->Year));
                    else if (d->GetDocumentType() == "EBook")
                        temp->AddDocument(gcnew EBook(d->Title, d->Author, d->Year));
                }
                temp->Sort();
                Console::WriteLine("\n--- Sorted Catalog (by year, then author) ---");
                temp->PrintCatalog();
            }
            else if (opt == "3")
            {
                lib->ExportToCSV("library_unsorted.csv");
                Console::WriteLine("Saved unsorted catalog to 'library_unsorted.csv'");
            }
            else if (opt == "4")
            {
                Library^ sortedLib = gcnew Library();
                for each (Document ^ d in lib->Documents)
                {
                    if (d->GetDocumentType() == "Book")
                        sortedLib->AddDocument(gcnew Book(d->Title, d->Author, d->Year));
                    else if (d->GetDocumentType() == "Magazine")
                        sortedLib->AddDocument(gcnew Magazine(d->Title, d->Author, d->Year));
                    else if (d->GetDocumentType() == "EBook")
                        sortedLib->AddDocument(gcnew EBook(d->Title, d->Author, d->Year));
                }
                sortedLib->Sort();
                sortedLib->ExportToCSV("library_sorted.csv");
                Console::WriteLine("Saved sorted catalog to 'library_sorted.csv'");
            }
            else if (opt == "5")
            {
                break;
            }
            else
            {
                Console::WriteLine("Invalid option.");
            }
        }
    }
    catch (Exception^ ex)
    {
        Console::Error->WriteLine("Error: {0}", ex->Message);
    }

    Console::WriteLine("Goodbye!");
    Console::ReadKey();
    return 0;
}
#include "pch.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
public interface class IPrintable { void Print(); };
public ref class Shape abstract {
public:
    virtual double Area() abstract;
    virtual double Perimeter() abstract;
    virtual void Print() { Console::WriteLine("{0}", ToString()); }
    virtual String^ ToString() override {
        return String::Format("Shape: A={0:F2}, P={1:F2}", Area(), Perimeter());
    }
};
public ref class Circle sealed : public Shape, public IPrintable {
public:
    double r;
public:
    Circle(double r) {
        if (r <= 0) throw gcnew ArgumentOutOfRangeException("r");
        this->r = r;
    }
    virtual double Area() override { return Math::PI * r * r; }
    virtual double Perimeter() override { return 2 * Math::PI * r; }
    virtual String^ ToString() override {
        return String::Format("Circle(r={0:F2}) A={1:F2} P={2:F2}", r, Area(), Perimeter());
    }
    virtual void Print() override { Shape::Print(); }
};
public ref class Rectangle sealed : public Shape, public IPrintable {
public:
    double w, h;
public:
    Rectangle(double w, double h) {
        if (w <= 0 || h <= 0) throw gcnew ArgumentOutOfRangeException();
        this->w = w; this->h = h;
    }
    virtual double Area() override { return w * h; }
    virtual double Perimeter() override { return 2 * (w + h); }
    virtual String^ ToString() override {
        return String::Format("Rectangle({0:F2}x{1:F2}) A={2:F2} P={3:F2}", w, h, Area(), Perimeter());
    }
    virtual void Print() override { Shape::Print(); }
};
public ref class Triangle sealed : public Shape, public IPrintable {
public:
    double a, b, c;
public:
    Triangle(double a, double b, double c) {
        if (a <= 0 || b <= 0 || c <= 0) throw gcnew ArgumentOutOfRangeException();
        if (a + b <= c || a + c <= b || b + c <= a)
            throw gcnew ArgumentException("Triangle inequality");
        this->a = a; this->b = b; this->c = c;
    }
    virtual double Perimeter() override { return a + b + c; }
    virtual double Area() override {
        double p = Perimeter() / 2.0;
        return Math::Sqrt(p * (p - a) * (p - b) * (p - c)); // Герон
    }
    virtual String^ ToString() override {
        return String::Format("Triangle({0},{1},{2}) A={3:F2} P={4:F2}", a, b, c, Area(), Perimeter());
    }
    virtual void Print() override { Shape::Print(); }
};
int main()
{
    List<Shape^>^ shapes = gcnew List<Shape^>();

    try {
        shapes->Add(gcnew Circle(3.0));
        shapes->Add(gcnew Rectangle(4.0, 5.0));
        shapes->Add(gcnew Triangle(3.0, 4.0, 5.0));
        shapes->Add(gcnew Circle(1.5));
        shapes->Add(gcnew Rectangle(2.0, 2.5));
    }
    catch (Exception^ ex) {
        Console::WriteLine("Error creating shapes: {0}", ex->Message);
        return -1;
    }


    Console::WriteLine("=== Original Shapes ===");
    double totalArea = 0.0, totalPerimeter = 0.0;
    for each (Shape ^ s in shapes) {
        s->Print();
        totalArea += s->Area();
        totalPerimeter += s->Perimeter();
    }

    Console::WriteLine("Total: A = {0:F2}, P = {1:F2}", totalArea, totalPerimeter);

    List<double>^ triangleAreas = gcnew List<double>();
    for each (Shape ^ s in shapes) {
        Triangle^ tri = dynamic_cast<Triangle^>(s);
        if (tri != nullptr) {
            triangleAreas->Add(tri->Area());
        }
    }

    if (triangleAreas->Count > 0) {
        double sum = 0.0;
        for each (double area in triangleAreas) sum += area;
        double avg = sum / triangleAreas->Count;
        Console::WriteLine("Average triangle area: {0:F2}", avg);
    }
    else {
        Console::WriteLine("No triangles found.");
    }

    String^ filename = "shapes.txt";

    try {
        StreamWriter^ writer = gcnew StreamWriter(filename);
        for each (Shape ^ s in shapes) {
            if (dynamic_cast<Circle^>(s)) {
                Circle^ c = static_cast<Circle^>(s);
                writer->WriteLine("Circle;{0}", c->r);
            }
            else if (dynamic_cast<Rectangle^>(s)) {
                Rectangle^ r = static_cast<Rectangle^>(s);
                writer->WriteLine("Rectangle;{0};{1}", r->w, r->h);
            }
            else if (dynamic_cast<Triangle^>(s)) {
                Triangle^ t = static_cast<Triangle^>(s);
                writer->WriteLine("Triangle;{0};{1};{2}", t->a, t->b, t->c);
            }
        }
        writer->Close();
        Console::WriteLine("\nCollection saved to {0}", filename);
    }
    catch (Exception^ ex) {
        Console::WriteLine("Error writing file: {0}", ex->Message);
    }

    List<Shape^>^ loadedShapes = gcnew List<Shape^>();
    try {
        if (!File::Exists(filename)) {
            Console::WriteLine("File {0} not found.", filename);
        }
        else {
            StreamReader^ reader = gcnew StreamReader(filename);
            String^ line;
            int lineNumber = 0;
            while ((line = reader->ReadLine()) != nullptr) {
                lineNumber++;
                try {
                    array<String^>^ parts = line->Split(';');
                    if (parts->Length == 0) continue;

                    String^ type = parts[0];
                    if (type == "Circle" && parts->Length == 2) {
                        double r = Double::Parse(parts[1]);
                        loadedShapes->Add(gcnew Circle(r));
                    }
                    else if (type == "Rectangle" && parts->Length == 3) {
                        double w = Double::Parse(parts[1]);
                        double h = Double::Parse(parts[2]);
                        loadedShapes->Add(gcnew Rectangle(w, h));
                    }
                    else if (type == "Triangle" && parts->Length == 4) {
                        double a = Double::Parse(parts[1]);
                        double b = Double::Parse(parts[2]);
                        double c = Double::Parse(parts[3]);
                        loadedShapes->Add(gcnew Triangle(a, b, c));
                    }
                    else {
                        throw gcnew FormatException("Invalid format");
                    }
                }
                catch (Exception^ ex) {
                    Console::WriteLine("Warning: line {0} skipped - {1}", lineNumber, ex->Message);
                }
            }
            reader->Close();

            Console::WriteLine("\n=== Loaded Shapes ===");
            for each (Shape ^ s in loadedShapes) {
                s->Print();
            }
        }
    }
    catch (Exception^ ex) {
        Console::WriteLine("Error reading file: {0}", ex->Message);
    }

    Console::WriteLine("\nPress any key to exit...");
    Console::ReadKey();
    return 0;
}
# Simple CSV libary for modern C++

This is a CSV(Comma-Separated Values) libary for simply use of reading / writing csv files

---

## Description

This is a CSV libary written by `C++`. It can be used to read or write CSV files. By inheriting `std::vector` class of `C++` standard libary, there are two main classes in the libary, included in namespace `SimpleCSV`. I am going show how to use the libary below.
Delimeter and escape character can be customized, and `','` as delimeter , `'\"'` as escape character and `'\n'` as newline, are set as default. Multiple charactors of delimeter is supported in this libary.

---

## Features

1. C++11 standard is required.
2. Using template to support different types of character.
3. Methods of `std::vector` can be used to operator rows and elements, also STL algorithm.

---

## Introduction

### Namespace

The whole libary is included in Namespace `SimpleCSV`. You may need to use the libary by `using namespace SimpleCSV` or `SimpleCSV::` as qualifier before.

### Defined Variable type

The following variable is defined in the libary:

```c++
   typedef size_t IndexT;   // used to define varaible of rows and columns
```

Tow struct are also be defined:

```c++
    struct CsvRange
    {
        IndexT Header_;
        IndexT CountRows_;
        IndexT Index_;
        IndexT CountColumns_;
    };
    template <typename CharT>
    struct CsvFormat
    {
        std::basic_string<CharT> Delimeter_;
        CharT Quote_;
        CharT Endline_;
    };

```

`struct CsvRange` is defined to descript the range of a CSV file to be read.
`struct CsvFormat` is defined to descript the features of a CSV files.

###Gobal Variable
Four Gobal variable in the namespace are defined as below:

```c++
    static const IndexT nIndex = -1;  // Maxiumn of rows and columns
    template <typename CharT>
    const std::basic_string<CharT> DFL_DELI{(CharT)','};
    template <typename CharT>
    const CharT DFL_QUOTE = (CharT)'\"';
    template <typename CharT>
    const CharT DFL_ENDLINE = (CharT)'\n';

```

### Main Class

Two main class is defined in the libary as:

```c++
    template <typename CharT>
    class BasicCsvRow : public vector<std::basic_string<CharT>>
```

A class to record one row of a CSV table.

```c++
    template <typename CharT>
    class BasicCsvTable : public vector<BasicCsvRow<CharT>>
```

A class to record a CSV table, which is combined by CsvRow(s).

By inheriting `vector` class, the member function of vector is also able to be used directly.

Each class is specialized as:

```cpp
    using CsvRow = BasicCsvRow<char>;
    using wCsvRow = BasicCsvRow<wchar_t>;
    using CsvTable = BasicCsvTable<char>;
    using wCsvTable = BasicCsvTable<wchar_t>;
```

### Public Methods

Public methods are as below

#### struct CsvRange

| Method                                                                                                        | Usage                                                  |
| :------------------------------------------------------------------------------------------------------------ | :----------------------------------------------------- |
| `CsvRange(IndexT _header = 0, IndexT _CountRows = nIndex, IndexT _index = 0, IndexT _CountColumns = nIndex);` | Default construtor to construct a variable of CsvRange |

#### struct Format

| Method                                                            | Usage                                                     |
| :---------------------------------------------------------------- | :-------------------------------------------------------- |
| `CsvFormat(StrT _delimeter = DFL_DELI, char _quote = DFL_QUOTE);` | Default constructor to construct a variable of CsvFormmat |

#### class CsvRow

##### 1. Member Methods

| Method                                                                                            | Usage                                                                                            |
| :------------------------------------------------------------------------------------------------ | :----------------------------------------------------------------------------------------------- |
| `CsvRow()`                                                                                        | Default construtor to construct a variable of CsvRow                                             |
| `CsvRow(const CsvRange &_range, const CsvFormat &_format) noexcept`                               | Customized range and format can be set by using this construtor to initialize a CsvRow Variable. |
| `void Range(const CsvRange &_range) noexcept`<p></p>`const CsvRange &Range() const noexcept`      | Use `.Range(range)` to set range property or use `.Range()` to get current range property.       |
| `void Format(const CsvFormat &_format) noexcept`<p></p>`const CsvFormat &Format() const noexcept` | Use `.Format(format)` to set format property or use `.Format()` to get current format property.  |

##### 2. Non member methods

| Method                                                                                                                              | Usage                                     |
| :---------------------------------------------------------------------------------------------------------------------------------- | :---------------------------------------- |
| `template <typename CharT> std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvRow<CharT> &_csvrow)`       | Easily used to read a row of a CSV file.  |
| `template <typename CharT> std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvRow<CharT> &_csvrow)` | Easily used to write a row to a CSV file. |

#### class CsvTable

##### 1. Member Methods

| Method                                                                                            | Usage                                                                                                |
| :------------------------------------------------------------------------------------------------ | :--------------------------------------------------------------------------------------------------- |
| `CsvTable()`                                                                                      | Default construtor to construct a variable of CsvTable                                               |
| `CsvTable(const CsvRange &_range, const CsvFormat &_format) noexcept`                             | Customized range and format can be set by using this construtor to initialize a CsvTable Variable.   |
| `void Range(const CsvRange &_range) noexcept`<p></p>`const CsvRange &Range() const noexcept`      | Use `.Range(range)` to set range property or use `.Range()` to get current range property.           |
| `void Format(const CsvFormat &_format) noexcept`<p></p>`const CsvFormat &Format() const noexcept` | Use `.Format(format)` to set format property or use `.Format()` to get current format property.      |
| ` CsvRow Column(IndexT _Index) const noexcept`                                                    | Get a whole column data from a CSV table in a type of `CsvRow`.                                      |
| `IndexT HeadIndex(const StrT &_fieldname) const noexcept`                                         | Return a `IndexT` type number by inputting a header string. `nIndex` would be returned if not found. |
| `IndexT Rows() const noexcept`                                                                    | Return number of rows of a table.                                                                    |
| `IndexT Columns() const noexcept`                                                                 | Return number of columns of a table.                                                                 |

##### 2. Non member methods

| Method                                                                                                                                  | Usage                            |
| :-------------------------------------------------------------------------------------------------------------------------------------- | :------------------------------- |
| `template <typename CharT> std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvTable<CharT> &_CsvTable)`       | Easily used to read a CSV file.  |
| `template <typename CharT> std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvTable<CharT> &_CsvTable)` | Easily used to write a CSV file. |

---

## Usage

#### 1. Reading and writing a csv file.

Operator `<<` and `>>` are overloaded to read a CSV file or a row of a CSV file, so that you may need to open a file by using `std::ifstream`, `std::ofstream` or `std::fstream`.
Sample of reading a CSV file:

```c++
    SimpleCSV::CsvTable csvtest;
    std::ifstream ifile("d:\\test.csv");
    if(ifile.is_open())
    {
        ifile >> csvtest;
    }
```

You may also use `std::cout` to show a row of a table in the console like below:

```c++
    std::cout << csvtest[0]; //show the first row(header)
```

#### 2. Setting a special format to process a CSV file

Data is seperated usually by a comma(`','`) in a CSV file. And a quote(`'\"'`) as an escape charater. There would be special files sometimes, however, such as TSV(Tab-seperated Value). You can define a variable by using `struct CsvFormat` to change the this kind of properties to process the special files.

```c++
    SimpleCSV::CsvFormat<char> formattest("\t"); //for TSV files
    SimpleCSV::CsvTable csvtest;
    csvtest.format(formattest);
```

```c++
    SimpleCSV::CsvFormat<wchar_t> formattest1; //for special uses
    formattest1.Delimeter_ = L"::";
    formattest1.Quote_ = L'\\';
```

Note:

- `Delimeter_` can be a multi-character string.
- `Newline_` is set as interface, but not avalible.
- `Delimeter_`and `Quote_` should not be the same charater. If set as the same, `Quote_` would be set to default. And if the `Quote_` already equals `DFL_QUOTE`, `Delimeter_` would be set to default.

#### 3. Setting read-range when reading a CSV file.

You can only read specified rows and columns from a CSV file, by using a `struct CsvRange`.

```c++
    SimpleCSV::CsvRange csvrange1(2, 5, 3, 6);
    csvtest.range(csvrange1);
```

There are for elements in `struct CsvRange`.
| elements | meanings
| --- | ---- |
| Header* | the starting row to be read |
|CountRows* | the number of rows to be read from starting row|
| Index* | the starting column to be read |
| CountColumns* | the number of columns to be read from starting columns |

The sample above means read **5** rows from **2nd** row and **6** columns from **3rd** column, from a CSV file.

#### 4. Operate a specified cell of the table

Inherting from `vector` means you can also use the public method of `vector` to operate the table, such as `insert()`, `erase()`, etc.
Overloaded operator`[]` is of course usable.

| column1 | column2 | column3 |
| :-----: | :-----: | :------ |
| value1  | value2  | value3  |
| value4  | value5  | value6  |

To change `value2`, you can do as below:

```c++
    csvtest[1][csvtest.HeadIndex("column2")] = "value7";
```

## License

This project is licensed under the terms of the MIT license.

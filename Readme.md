<font size = 7>**Simple CSV libary for modern C++**</font>

This is a CSV(Comma-Separated Values) libary for simple use of reading / writing csv files

---

- [Features](#features)
- [Introduction](#introduction)
  - [Namespace](#namespace)
  - [Defined Variable type](#defined-variable-type)
  - [Global Variable](#global-variable)
  - [Main Class](#main-class)
  - [Public Type](#public-type)
    - [enum class SortType](#enum-class-sorttype)
    - [CsvSortList](#csvsortlist)
  - [Public Methods](#public-methods)
    - [struct CsvRange](#struct-csvrange)
    - [struct Format](#struct-format)
    - [class CsvRow](#class-csvrow)
      - [1. Public Member Methods](#1-public-member-methods)
      - [2. Non member methods](#2-non-member-methods)
    - [class CsvTable](#class-csvtable)
      - [1. Public Member Methods](#1-public-member-methods-1)
      - [2. Non member methods](#2-non-member-methods-1)
- [Usage](#usage)
    - [1. Initialize an object.](#1-initialize-an-object)
    - [2. Reading and writing a csv file.](#2-reading-and-writing-a-csv-file)
    - [3. Setting a special format to process a CSV file](#3-setting-a-special-format-to-process-a-csv-file)
    - [4. Setting read-range when reading a CSV file](#4-setting-read-range-when-reading-a-csv-file)
    - [5. Operate a specified cell of the table](#5-operate-a-specified-cell-of-the-table)
    - [6. Sort rows in a table](#6-sort-rows-in-a-table)
- [License](#license)
- [Update Log](#update-log)

This is a CSV libary written by `C++`. It can be used to read or write CSV files. By inheriting `std::vector` class of `C++` standard libary, there are two main classes in the libary, included in namespace `SimpleCSV`. I am going show how to use the libary below.
Delimeter and escape character can be customized, and `','` as delimeter , `'\"'` as escape character and `'\n'` as newline, are set as default. Multiple characters of delimeter is supported in this libary.

---

# Features

1. C++11 standard is required.
2. Using template to support different types of characters.
3. Methods of `std::vector` can be used to operate rows and elements, also STL algorithm.

---

# Introduction

## Namespace

The whole libary is included in Namespace `SimpleCSV`. You may need to use the libary by `using namespace SimpleCSV` or `SimpleCSV::` as qualifier before.

## Defined Variable type

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

## Global Variable

Four Global variable in the namespace are defined as below:

```c++
    static const IndexT nIndex = -1;  // Maxiumn of rows and columns
    template <typename CharT>
    const std::basic_string<CharT> DFL_DELI{(CharT)','};
    template <typename CharT>
    const CharT DFL_QUOTE = (CharT)'\"';
    template <typename CharT>
    const CharT DFL_ENDLINE = (CharT)'\n';

```

## Main Class

Three main class is defined in the libary as:

```c++
    template <typename CharT>
    class BasicCsvColumn : public vector<std::basic_string<CharT>>
```

A class to represent a column. There is not any connection to the table, so that you cannot use this class to operate the table.

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

By inheriting `std::vector` class, the member function of vector is also able to be used directly.

Each class is specialized as:

```cpp
    using CsvRow    =   BasicCsvRow<char>;
    using wCsvRow   =   BasicCsvRow<wchar_t>;
    using CsvTable  =   BasicCsvTable<char>;
    using wCsvTable =   BasicCsvTable<wchar_t>;
```

## Public Type

### enum class SortType

`SortType` is defined to describe the way of sorting by one or several columns. Below is the definition:

```cpp
enum class SortType { Ascend, Decned };
```

### CsvSortList

`CsvSortList` is a specialized template class as `std::vector<std::pair<IndexT, SortType>>`, defined to describe how to compare rows in a table.
Rows are compared one by one from the first element to the last as default, when you want to sort the rows in table. Sometimes, you may want to sort by serveral specified columns however. Then you can use this `CsvSortList` to set.

```cpp
using CsvSortList = vector<std::pair<IndexT, SortType>>
```

- IndexT
  The element represent the column index.
- SortType
  To the way of sorting the rows.

### CsvTablePos
`CsvTablePos` is a specialized template class as `vector<std::pair<IndexT, IndexT>>`, defined to restore all the search result of a specified string in a table. The first element refers to row number, the second one refers to column.

```cpp
using CsvTablePos = vector<std::pair<IndexT, IndexT>>;
```

## Public Methods

Public methods are as below

### struct CsvRange

| Method                                                                                                        | Usage                                                   |
| :------------------------------------------------------------------------------------------------------------ | :------------------------------------------------------ |
| `CsvRange(IndexT _header = 0, IndexT _CountRows = nIndex, IndexT _index = 0, IndexT _CountColumns = nIndex);` | Default constructor to construct a variable of CsvRange |

### struct Format

| Method                                                            | Usage                                                     |
| :---------------------------------------------------------------- | :-------------------------------------------------------- |
| `CsvFormat(StrT _delimeter = DFL_DELI, char _quote = DFL_QUOTE);` | Default constructor to construct a variable of CsvFormmat |

### class CsvRow

#### 1. Public Member Methods

| Method                                                       | Usage                                                        |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| `CsvRow()`                                                   | Default constructor to construct a variable of CsvRow        |
| `CsvRow(const CsvRange &_range, const CsvFormat &_format) noexcept` | Customized range and format can be set by using this constructor to initialize a CsvRow Variable. |
| `void Range(const CsvRange &_range) noexcept`<p></p>`const CsvRange &Range() const noexcept` | Use `.Range(range)` to set range property or use `.Range()` to get current range property. |
| `template <class CharT> void Format<CharT>(const CsvFormat &_format) noexcept`<p></p>`template <class CharT> const CsvFormat<CharT> &Format() const noexcept` | Use `.Format(format)` to set format property or use `.Format()` to get current format property. |
| `void Row(IndexT _row) noexcept` <p></p>`IndexT Row() const noexcept` | Use `.Row(row)` to set a row number for a BasicCsvRow object, or use `.Row()` to get current row number. |
| `template <class CharT> std::basic_string<CharT> &operator[](const std::basic_string<CharT> &_FieldName)`<p></p>`template <class CharT> const std::basic_string<CharT> &operator[](const std::basic_string<CharT> &_FieldName) const` | Use a header string to find a specified elements of a row. A `std::invalid_argument` excpetion will be throwed if the string is not found. |
| `bool operator==(const BasicCsvRow<CharT> &_csvrow) noexcept`<p></p>`bool operator!=(const BasicCsvRow<CharT> &_csvrow) noexcept`<p></p>`bool operator<(const BasicCsvRow<CharT> &_csvrow) noexcept`<p></p>`bool operator<=(const BasicCsvRow<CharT> &_csvrow) noexcept`<p></p>`bool operator>(const BasicCsvRow<CharT> &_csvrow) noexcept`<p></p>`bool operator>=(const BasicCsvRow<CharT> &_csvrow) noexcept` | Used to compare rows. `SortList` is not available for these methods. |
| `bool find(const std::basic_string<CharT> &_TargetStr, vector<IndexT> &_FindResult, const CsvRange &_Range)`<p></p>`bool find(const std::basic_string<CharT> &_TargetStr, vector<IndexT> &_FindResult)` | Used to find a specified string in the row. All found position will be recorded in `_FindResult`. Searching area can be setup by inputting a `CsvRange` variable, range setting of specialized object would be used as default. |
| `bool find(const std::basic_string<CharT> &_TargetStr, const CsvRange &_Range)`<p></p>`bool find(const std::basic_string<CharT> &_TargetStr)` | Used to find a specified string in the row. Only return the result that whether the target string is existed or not. Searching area can be setup by inputting a `CsvRange` variable, range setting of specialized object would be used as default. |

#### 2. Non member methods

| Method                                                                                                                              | Usage                                     |
| :---------------------------------------------------------------------------------------------------------------------------------- | :---------------------------------------- |
| `template <typename CharT> std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvRow<CharT> &_csvrow)`       | Easily used to read a row of a CSV file.  |
| `template <typename CharT> std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvRow<CharT> &_csvrow)` | Easily used to write a row to a CSV file. |

### class CsvTable

#### 1. Public Member Methods

| Method                                                       | Usage                                                        |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| `CsvTable()`                                                 | Default constructor to construct a variable of CsvTable      |
| `CsvTable(const CsvRange &_range, const CsvFormat &_format) noexcept` | Customized range and format can be set by using this constructor to initialize a CsvTable Variable. |
| ` explicit BasicCsvTable(IndexT _Row, IndexT _Columns = 0) noexcept` | Make a empty table of specified rows and columns.            |
| `void Range(const CsvRange &_range) noexcept`<p></p>`const CsvRange &Range() const noexcept` | Use `.Range(range)` to set range property or use `.Range()` to get current range property. |
| `template <class CharT> void Format<CharT>(const CsvFormat &_format) noexcept`<p></p>`template <class CharT> const CsvFormat<CharT> &Format<CharT>() const noexcept` | Use `.Format(format)` to set format property or use `.Format()` to get current format property. |
| `template <class ChaT> BasicCsvColumn<CharT> Column(IndexT _Index) const noexcept` | Get a whole column data from a CSV table in a type of `CsvRow`. |
| `IndexT HeadIndex(const StrT &_fieldname) const noexcept`    | Return a `IndexT` type number by inputting a header string. `nIndex` would be returned if not found. |
| `IndexT Rows() const noexcept`                               | Return number of rows of a table.                            |
| `IndexT Columns() const noexcept` <p> </p> `void Columns(IndexT _Columns)` | Use `.Columns(columns)` to pre-set the columns number for a table, or use `.Columns()` to get current columns number of the current table. |
| `void SwapRow(IndexT _RowIndex1, IndexT _RowIndex2)`         | Swap rows of the current table by using row index. The function will do nothing if one of the parameters is bigger than the size of the table. |
| `void SwapRow(iterator _RowIt1, iterator _RowIt2)`           | Swap rows of the current table by using iterator. The function will do nothing if one of the iterators is not pointing to the table. |
| `iterator NewRow()`                                          | Add and empty row at the last of the table. The row should have the same columns of the table. |
| `typename BasicCsvRow<CharT>::iterator EraseColumn(typename BasicCsvRow<CharT>::iterator _ErasePosition)`<p></p>`typename BasicCsvRow<CharT>::iterator EraseColumn(IndexT _ErasePosition)` | Erase a column which iterator `ErasePosition` is pointing at header. |
| `typename BasicCsvRow<CharT>::iterator EraseColumn(typename BasicCsvRow<CharT>::iterator _FirstPosition, typename BasicCsvRow<CharT>::iterator _LastPosition)`<p></p>`typename BasicCsvRow<CharT>::iterator EraseColumn(IndexT _FirstPosition, IndexT _LastPosition)` | Erase the columns in range `[_FromPosition, _LastPosition)`. |
| `bool operator==(const BasicCsvTable<CharT> &_csvtable) noexcept`<p></p>`bool operator!=(const BasicCsvTable<CharT> &_csvtable) noexcept`<p></p>`bool operator<(const BasicCsvTable<CharT> &_csvtable) noexcept`<p></p>`bool operator<=(const BasicCsvTable<CharT> &_csvtable) noexcept`<p></p>`bool operator>(const BasicCsvTable<CharT> &_csvtable) noexcept`<p></p>`bool operator>=(const BasicCsvTable<CharT> &_csvtable) noexcept` | Used to compare tables.                                      |
| `void sort()` <p></p> `void sort(const CsvSortList &_SortList)` | Sort all the rows except header(if header exists).           |
| `bool find(const std::basic_string<CharT> &_TargetStr, CsvTablePos &_TablePos, const CsvRange &_Range)`<p></p>`bool find(const std::basic_string<CharT> &_TargetStr, CsvTablePos &_TablePos)` | Used to find a specified string in the table. All found position will be recorded in `_TablePos`. Searching area can be setup by inputting a `CsvRange` variable, range setting of specialized object would be used as default. |
| `bool find(const std::basic_string<CharT> &_TargetStr, const CsvRange &_Range)`<p></p>`bool find(const std::basic_string<CharT> &_TargetStr, const CsvRange &_Range)` | Used to find a specified string in the table. Only return the result that whether the target string is existed or not. Searching area can be setup by inputting a `CsvRange` variable, range setting of specialized object would be used as default. |


#### 2. Non member methods

| Method                                                                                                                                  | Usage                            |
| :-------------------------------------------------------------------------------------------------------------------------------------- | :------------------------------- |
| `template <typename CharT> std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvTable<CharT> &_CsvTable)`       | Easily used to read a CSV file.  |
| `template <typename CharT> std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvTable<CharT> &_CsvTable)` | Easily used to write a CSV file. |

---

# Usage

### 1. Initialize an object.

Both `SimpleCSV::BasicCsvRow<CharT>` and `SimpleCSV::BasicCsvTable<CharT>` are `std::vector` base objects. Constructors of `std::vector` are usable to initialize an object for each.

```cpp
	SimpleCSV::wCsvTable csvtest; // default ctor, an empty table.
	SimpleCSV::CsvTable csvtest{{"Head1","Head2","Head3"}, {"Value01","Value02","Value03"},{"Value11","Value12","Value13"}}; // Use intialize-list to initialize csvtest object.
	SimpleCSV::CsvTable csvtest1(csvtest.begin() + 1, csvtest.end() - 1); // Use iterator range to initialize an object.
```

### 2. Reading and writing a csv file.

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

### 3. Setting a special format to process a CSV file

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
- `Newline_` is set as interface, but not available.
- `Delimeter_`and `Quote_` should not be the same character. If set as the same, `Quote_` would be set to default. And if the `Quote_` already equals `DFL_QUOTE`, `Delimeter_` would be set to default.

### 4. Setting read-range when reading a CSV file

You can only read specified rows and columns from a CSV file, by using a `struct CsvRange`.

```c++
    SimpleCSV::CsvRange csvrange1(2, 5, 3, 6);
    csvtest.range(csvrange1);
```

There are for elements in `struct CsvRange`.
| elements | meanings
| --- | ---- |
| Header| the starting row to be read |
|CountRows | the number of rows to be read from starting row|
| Index | the starting column to be read |
| CountColumns | the number of columns to be read from starting columns |

The sample above means read **5** rows from **2nd** row and **6** columns from **3rd** column, from a CSV file.

### 5. Operate a specified cell of the table

Inheriting from `std::vector` means you can also use the public method of `vector` to operate the table, such as `insert()`, `erase()`, etc.
Overloaded operator`[]` is of course usable.

| column1 | column2 | column3 |
| :-----: | :-----: | :------ |
| value1  | value2  | value3  |
| value4  | value5  | value6  |

To change `value2`, you can do as below:

```c++
    csvtest[1][csvtest.HeadIndex("column2")] = "value7";
```

You can also:

```cpp
	csvtest[1]["column2"] = "value7"; //std::invalid_argumnet will be throwed if "column2" does not exist.
```

### 6. Sort rows in a table

There are to sort methods defined in the class. `Sort()` is used to sort the rows in default way, by dictionary order. `Sort(const SortList &_sortlist)` is used to sort the rows in a specified way.
For example

```cpp
CsvTable table;
std::ifstream("file.csv") >> table;
CsvTable::CsvSortList sortlist{
std::makepair(4, CsvTable::SortType::Ascend),
std::makepair(6, CsvTable::SortType::Decend)};
table.sort(sortlist);
```

A variable `sortlist` is defined to sort the table first by the 5th column by ascending order, and then by the 7th column by decending order, and then by default sequence.

# License

This project is licensed under the terms of the MIT license.

# Update Log

| Version | Description                                                                                                                                                                                                              |
| :-----: | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| v0.0.1  | Initial vesion.                                                                                                                                                                                                          |
| v0.0.2  | Reading rules of `Format(CsvFomat _format)` modified.                                                                                                                                                                    |
| v0.1.0  | Templatized the classes.                                                                                                                                                                                                 |
| v0.1.1  | Erased explicit call for detors of `std::vector()::~vector()` in BasicCsvRow and BasicCsvTable                                                                                                                           |
| v0.1.2  | Delete empty rows.<p></p>Made value of CsvFormat in all rows change after change of CsvFormat in BasicCsvTable.<p></p>Trim or fill new rows if the size of the new rows are not equal to the header.                     |
| v0.2.0  | Complete the modifiers(insert，emplace，erase，push_back，emplace_back)                                                                                                                                                  |
| v0.2.1  | Rewrote some of constructors.                                                                                                                                                                                            |
| v0.2.2  | Add `swap()` and `RowSwap()` function.                                                                                                                                                                                   |
| v0.2.3  | Rewrote `assign()`, `operator=()`, copy ctor and move ctor.                                                                                                                                                              |
| v0.2.4  | Add `NewRow()` function.                                                                                                                                                                                                 |
| v0.2.5  | Fixed unable to call `BasicCsvRow.operator[](size_t index)` problem.                                                                                                                                                     |
| v0.2.6  | Simplified the reading codes. Fixed problems。                                                                                                                                                                           |
| v0.3.0  | Add BasicCsvColomn class to represent columns。                                                                                                                                                                          |
| v0.3.1  | Bugfix: Fixed the problem that a whole row might be erase after row read. <p></p>Fixed the problem that ` BasicCsvRow<CharT>::operator[]()` might be recursively called.                                                 |
| v0.3.2  | Modified code to pass GCC compilation.                                                                                                                                                                                   |
| v0.3.3  | Bugfix: Fixed the problem that program was haulted if a row ended with `\"`.<p></p>Modified `BasicCsvRow<CharT>::operator[]()`，an exception `std::invalid_argument` would be throw when the header input was not found. |
| v0.4.0  | Comparision operator added. Sorting method `BasicCsvTable::sort()` addded.                                                                                                                                               |
| v0.4.1  | Bugfix: Fixed some problem.                                                                                                                                                                                              |
| v0.4.2  | Minor changes.                                                                                                                                                                                                           |
| v0.4.3 | Bugfix: Fixed problems of `insert` function. |
| v0.4.4 | Bugfix: Fixed problems. |
| v0.5.0 | Function add: `find()`function is added.|
| v0.5.1 | Bugfix: Fixed problems of `find()` function of range settings. |
|  v0.6.0 | Function add: `find()` (only return existed or not result) function is added. <p></p>Bugfix: Fixed problems of `sort()`. |
| v0.6.1 | Bugfix: Fixed problems of `find()` function. |

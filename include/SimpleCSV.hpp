/*
 *          Simple CSV 第1分支
 *          21年7月28日 20：46
 *          v0.0.1  使用单行类csvrow通过运算符重载可读入或输出单行
 *                  csv类通过连续读入完成全表
 *                  可实现从指定起始行读入指定行数
 */

#ifndef _CSV_HPP_
#define _CSV_HPP_
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
// #include <stdexcept>
// #include <ios>

namespace SimpleCSV
{
    using std::vector;
    typedef std::string str_t;
    typedef size_t index_t;

    const str_t DFL_DELI = ",";
    const char DFL_QUOTE_CHAR = '\"';
    const char DFL_NEWLINE = '\n';

    class csv;

    bool equal_delimeter(str_t::const_iterator it, const str_t &deli) noexcept;

    //SECTION: class csvrow definition.
    class csvrow : public vector<str_t>
    {
    private:
        index_t row;
        str_t delimeter; //分隔符，默认 ","，支持多字符
        char quote_char; //转义字符
        char newline;
        friend csv;

    public:
        using vector<str_t>::vector;
        using vector::operator[];

        explicit csvrow(
            index_t r = 0,
            const str_t &deli = DFL_DELI,
            char qc = DFL_QUOTE_CHAR,
            char nl = DFL_NEWLINE) noexcept : vector(),
                                              row(r),
                                              delimeter(deli),
                                              quote_char(qc),
                                              newline(nl) {}
        ~csvrow() noexcept { vector::~vector(); }

        str_t &operator[](const str_t &fieldname);             //如果没有找到表头对应字串值，抛出std::invalid_argumnent异常
        const str_t &operator[](const str_t &fieldname) const; //如果没有找到表头对应字串值，抛出std::invalid_argumnent异常
        index_t get_rownum() const noexcept { return row; }
        void set_rownum(index_t set_rnum) noexcept { row = set_rnum; }

        friend std::istream &operator>>(std::istream &is, csvrow &crow);
        friend std::ostream &operator<<(std::ostream &os, const csvrow &crow);
    };

    //SECTION:  class csv definition
    class csv : public vector<csvrow>
    {
    public:
        static const index_t n_index = -1;
        // class csv_exception;

    private:
        index_t header;    //读入文件的第header行为表头，-1时为无表头，默认0（第一行）,第header行前的数据会被丢弃
        index_t read_rows; //读入文件的行数，即读入文件的行范围[header, read_rows - header - 1)的内容
        index_t column;    //header的列数
        str_t delimeter;   //分隔符，默认 ","，支持多字符
        char quote_char;   //转义字符
        char newline;      //换行符
        friend csvrow;

        // class csv_exception : public std::ios_base::failure
        // {
        // private:
        //     str_t err_message;

        // public:
        //     csv_exception(const str_t &err) noexcept : err_message(err), failure("") {}
        //     const char *what() const noexcept { return err_message.data(); }
        // };

    public:
        using vector<csvrow>::vector;
        explicit csv(index_t hd = 0,
                     index_t rr = n_index,
                     const str_t &deli = DFL_DELI,
                     char qc = DFL_QUOTE_CHAR,
                     char nl = DFL_NEWLINE) noexcept
            : vector(),
              header(hd),
              read_rows(rr),
              delimeter(deli),
              quote_char(qc),
              newline(nl),
              column(0) {}
        ~csv() { vector::~vector(); }

        void set_header(index_t hd) { header = hd; }
        void set_readrows(index_t rr) { read_rows = rr; }
        void set_delimeter(const str_t &deli = DFL_DELI) noexcept { delimeter = deli; }
        void set_quote(char qc) { quote_char = qc; }
        void set_newline(char nl) { newline = nl; }

        const csvrow &get_crow(index_t index) const noexcept { return operator[](index); } //返回指定行的整行只读数据
        csvrow &get_row(index_t index) noexcept { return operator[](index); }              //返回指定行整行数据，并可以使用vector类的全部接口进行对行操作
        csvrow get_column(index_t index) const noexcept;                                   //返回指定列整列数据
        index_t get_head_index(const str_t &fieldname) const noexcept;                     //返回指定字符串在表头中的位置，如果没有表头，或没有找到字符串，返回n_index;
        index_t get_header() const noexcept { return header; }
        index_t get_readrows() const noexcept { return read_rows; }
        const str_t &get_delimeter() const noexcept { return delimeter; }
        char get_quote_char() const noexcept { return quote_char; }
        char get_newline() const noexcept { return newline; }

        index_t rows() const noexcept { return size(); }    //返回行数
        index_t columns() const noexcept { return column; } //返回列数

        friend std::istream &operator>>(std::istream &is, csv &csv_table);
        friend std::ostream &operator<<(std::ostream &os, const csv &c_table);
    };

    //SECTION: class csvrown methods implemetation
    str_t &csvrow::operator[](const str_t &fieldname)
    {
        auto it = this - row;
        index_t index;
        for (index = 0; index < this->size(); ++index)
        {
            if ((*it)[index] == fieldname)
                return (*this)[index];
        }
        throw std::invalid_argument(fieldname + " not found");
    }

    const str_t &csvrow::operator[](const str_t &fieldname) const
    {
        auto it = this - row;
        index_t index;
        for (index = 0; index < this->size(); ++index)
        {
            if ((*it)[index] == fieldname)
                return (*this)[index];
        }
        throw std::invalid_argument(fieldname + " not found");
    }

    //SECTION: class csv method implementation

    csvrow csv::get_column(index_t index) const noexcept
    {
        csvrow col;
        if (index <= this->data()->size())
        {
            for (auto pd0 : *this)
            {
                col.push_back(pd0[index]);
            }
        }
        return col;
    }

    index_t csv::get_head_index(const str_t &fieldname) const noexcept
    {
        if (~header)
        {
            for (index_t index = 0; index < (*this)[0].size(); ++index)
                if (((*this)[0][index] == fieldname))
                    return index;
        }
        return n_index;
    }

    //SECTION: other non member mothods

    std::istream &operator>>(std::istream &is, csvrow &crow)
    {
        bool endline_flag = false;
        str_t str_src, str_tmp;
        str_t::iterator it1, it2;
        do
        {
            getline(is, str_src);
            if (endline_flag)
                str_src = str_tmp + str_src; //如果内容包含换行，则添加上换行前的内容
            it1 = it2 = str_src.begin();
            while (it1 != str_src.end()) //对读入行进行处理
            {
                if (*it1 == crow.quote_char)
                {
                    ++it1;
                    if (endline_flag)
                    {
                        it2 += str_tmp.size(); //直接从换行后的内容开始，
                        endline_flag = false;  //对换行内容处理完毕
                    }
                    else
                        it2 = it1;
                    while (!(*it2 == crow.quote_char && (equal_delimeter(it2 + 1, crow.delimeter) || it2 >= str_src.end() - 1))) // 当前字节为quote_char且下一部分为delimeter或字符结束，说明字符已结束
                    {
                        if (*it2 == crow.quote_char && *(it2 + 1) == crow.quote_char)
                            str_src.erase(it2);
                        if (it2 >= str_src.end() - 1) // 内容中存在换行
                        {
                            str_tmp = crow.quote_char + str_t(it1, it2 + 1) + crow.newline; //已处理的内容临时存放于于str_tmp中
                            endline_flag = true;                                            //设置换行标记
                            break;
                        }
                        ++it2;
                    }
                }
                else
                {
                    it2 = it1;
                    auto en = str_src.find(crow.delimeter, it1 - str_src.begin());
                    if (~en)
                        it2 = str_src.begin() + en;
                    else
                        it2 = str_src.end();
                }
                if (!endline_flag)
                {
                    crow.emplace_back(it1, it2);
                }
                it1 = it2 + (it2 != str_src.end()) + (*it2 == crow.quote_char && equal_delimeter(it2 + 1, crow.delimeter)) * crow.delimeter.size();
            }
        } while (endline_flag);

        // while (row && size() < crow.column) //列数不足时，追加空字符列至与表头相等
        // {
        //     emplace_back("");
        // }

        return is;
    }

    std::ostream &operator<<(std::ostream &os, const csvrow &crow)
    {
        bool has_special_char = false;
        str_t str_tmp;
        for (auto it_crow = crow.cbegin(); it_crow < crow.cend(); ++it_crow)
        {
            str_tmp = *it_crow;
            for (auto it_str = str_tmp.begin(); it_str < str_tmp.end(); ++it_str)
            {
                if (*it_str == crow.quote_char)
                {
                    it_str = str_tmp.insert(it_str, crow.quote_char) + 1;
                    has_special_char = true;
                }
                if (equal_delimeter(it_str, crow.delimeter) || *it_str == crow.newline)
                    has_special_char = true;
            }
            if (has_special_char)
            {
                str_tmp = crow.quote_char + str_tmp + crow.quote_char;
                has_special_char = false;
            }
            os << str_tmp;
            it_crow < crow.cend() - 1 && os << crow.delimeter;
        }

        return os;
    }

    std::istream &operator>>(std::istream &is, csv &csv_table)
    {
        index_t row = 0, loadedline = 0;
        csvrow csvrow_tmp(row, csv_table.delimeter, csv_table.quote_char, csv_table.newline);
        do
        {
            is >> csvrow_tmp;

            if (csv_table.header == csv_table.n_index || loadedline++ >= csv_table.header) //如果header == n_index，为没有header的情况，每一行都被读入；如果loadedline >= header，可以读入余下的行
            {
                csvrow_tmp.set_rownum(row++);
                csv_table.emplace_back(csvrow_tmp);
            }

            if (csv_table.size() == 1 && ~csv_table.header) //如果csv中只有一个元素（只有一行），且header存在，则该行为header，该行的列数为全表列数，其行与其对齐
                csv_table.column = csvrow_tmp.size();

            csvrow_tmp.clear();
        } while (!is.eof() && row < csv_table.read_rows);
        return is;
    }

    std::ostream &operator<<(std::ostream &os, const csv &csv_table)
    {
        for (auto it_table = csv_table.cbegin(); it_table < csv_table.end(); ++it_table)
        {
            os << *it_table;
            it_table < csv_table.end() - 1 && os << csv_table.newline;
        }
        return os;
    }

    bool equal_delimeter(str_t::const_iterator it, const str_t &deli) noexcept
    {
        auto itdeli = deli.begin();
        while (*it == *itdeli)
            it++, itdeli++;
        return (itdeli == deli.end());
    }

}

#endif
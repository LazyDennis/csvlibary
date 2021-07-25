#ifndef _CSV_HPP_
#define _CSV_HPP_
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <ios>

namespace SimpleCSV
{
    using std::vector;
    typedef std::string str_t;
    typedef size_t index_t;

    const str_t DFL_DELI = ",";
    const char DFL_QUOTE_CHAR = '\"';

    class csv;

    bool equal_delimeter(str_t::const_iterator it, const str_t &deli) noexcept;

    //SECTION: class csvrow definition.
    class csvrow : public vector<str_t>
    {
    private:
        index_t row = 0;
        friend csv;

    public:
        using vector<str_t>::vector;
        using vector::operator[];

        str_t &operator[](const str_t &fieldname);             //如果没有找到表头对应字串值，抛出std::invalid_argumnent异常
        const str_t &operator[](const str_t &fieldname) const; //如果没有找到表头对应字串值，抛出std::invalid_argumnent异常
        index_t get_rownum() const noexcept { return row; }

        const csvrow &readrow(index_t row_no, const csv &c, std::ifstream &ifile);
    };

    //SECTION:  class csv definition
    class csv : public vector<csvrow>
    {
    public:
        static const index_t n_index = -1;
        class csv_exception;

    private:
        str_t filename;
        str_t delimeter;     //分隔符，默认 ","，支持多字符
        char quote_char;     //转义字符
        index_t header;      //读入文件的第header行为表头，-1时为无表头，默认0（第一行）,第header行前的数据会被丢弃
        index_t header_size; //header的列数
        friend csvrow;

        class csv_exception : public std::ios_base::failure
        {
        private:
            str_t err_message;

        public:
            csv_exception(const str_t &err) noexcept : err_message(err), failure("") {}
            const char *what() const noexcept { return err_message.data(); }
        };

    public:
        using vector<csvrow>::vector;
        vector::operator=;
        csv() noexcept
            : filename(""),
              vector(),
              delimeter(DFL_DELI),
              quote_char(DFL_QUOTE_CHAR),
              header(0),
              header_size(0) {}
        explicit csv(const str_t &fn, index_t hd = 0, const str_t &deli = DFL_DELI, char qc = DFL_QUOTE_CHAR) noexcept
            : filename(fn),
              vector(),
              delimeter(deli),
              quote_char(qc),
              header(hd),
              header_size(0) {}

        void set_file(const str_t &fn) noexcept { filename = fn; }
        void set_delimeter(const str_t &deli = DFL_DELI) noexcept { delimeter = deli; }
        void set_header(index_t hd) { header = hd; }
        void set_quote(char qc) { quote_char = qc; }
        void read();
        void write() const;

        const csvrow &get_crow(index_t index) const noexcept { return operator[](index); } //返回指定行的整行只读数据
        csvrow &get_row(index_t index) noexcept { return operator[](index); }              //返回指定行整行数据，并可以使用vector类的全部接口进行对行操作
        csvrow get_column(index_t index) const noexcept;                                   //返回指定列整列数据
        index_t get_head_index(const str_t &fieldname) const noexcept;                     //返回指定字符串在表头中的位置，如果没有表头，或没有找到字符串，返回n_index;
        const str_t &get_filename() const noexcept { return filename; }
        index_t get_headerstatus() const noexcept { return header; }
        const str_t &get_delimeter() const noexcept { return delimeter; }
        char get_quote_char() const noexcept { return quote_char; }

        index_t rows() const noexcept { return size(); }         //返回行数
        index_t columns() const noexcept { return header_size; } //返回列数

        friend std::ostream &operator<<(std::ostream &os, const csv &c) noexcept;
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

    const csvrow &csvrow::readrow(index_t row_no, const csv &c, std::ifstream &ifile)
    {
        bool endline_flag = false;
        str_t str_src, str_tmp;
        str_t::iterator it1, it2;
        do
        {
            getline(ifile, str_src);
            if (endline_flag)
                str_src = str_tmp + str_src; //如果内容包含换行，则添加上换行前的内容
            it1 = it2 = str_src.begin();
            while (it1 != str_src.end()) //对读入行进行处理
            {
                if (*it1 == c.quote_char)
                {
                    ++it1;
                    it2 = it1;
                    if (endline_flag)
                    {
                        it2 += (str_tmp.size() - 1); //直接从换行后的内容开始，
                        endline_flag = false;        //对换行内容处理完毕
                    }
                    while (!(*it2 == c.quote_char && (equal_delimeter(it2 + 1, c.delimeter) || it2 >= str_src.end() - 1))) // 当前字节为quote_char且下一部分为delimeter或字符结束，说明字符已结束
                    {
                        if (*it2 == c.quote_char && *(it2 + 1) == c.quote_char)
                            str_src.erase(it2);
                        if (it2 >= str_src.end() - 1) // 内容中存在换行
                        {
                            str_tmp = c.quote_char + str_t(it1, it2 + 1) + '\n'; //已处理的内容临时存放于于str_tmp中
                            endline_flag = true;                                 //设置换行标记
                            break;
                        }
                        ++it2;
                    }
                }
                else
                {
                    it2 = it1;
                    auto en = str_src.find(c.delimeter, it1 - str_src.begin());
                    if (~en)
                        it2 = str_src.begin() + en;
                    else
                        it2 = str_src.end();
                }
                if (!endline_flag)
                {
                    emplace_back(it1, it2);
                }
                it1 = it2 + (it2 != str_src.end()) + (*it2 == c.quote_char && equal_delimeter(it2 + 1, c.delimeter)) * c.delimeter.size();
            }
        } while (endline_flag);

        row = row_no;

        while (row && size() < c.header_size) //列数不足时，追加空字符列至与表头相等
        {
            emplace_back("");
        }

        return *this;
    }

    //SECTION: class csv method implementation
    void csv::read()
    {
        std::ifstream ifile(filename);
        if (ifile.is_open())
        {
            csvrow arr_tmp;
            index_t row = 0, loadedline = 0;
            do
            {
                arr_tmp.readrow(row, *this, ifile);

                if (header == n_index || loadedline++ >= header) //如果header == n_index，为没有header的情况，每一行都被读入；如果loadedline >= header，可以读入余下的行
                    this->push_back(arr_tmp);

                if (size() == 1 && ~header) //如果csv中只有一个元素（只有一行），且header存在，则该行为header，该行的列数为全表列数，其行与其对齐
                    header_size = arr_tmp.size();

                arr_tmp.clear();
            } while (!ifile.eof());
        }
        else
            throw csv_exception("Failed in opening file \"" + filename + "\".");

        ifile.close();
    }

    void csv::write() const
    {
        std::ofstream ofile(filename);
        if (ofile.is_open())
        {
            ofile << *this;
        }
        else
            throw csv_exception("Failed in opening file \"" + filename + "\".");
        ofile.close();
        return;
    }

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
    std::ostream &operator<<(std::ostream &os, const csv &c) noexcept
    {
        bool has_special_char = false;
        str_t str_tmp;
        for (auto pd0 : c)
        {
            for (auto pd1 = pd0.cbegin(); pd1 < pd0.cbegin() + c.header_size; ++pd1)
            {
                if (pd1 < pd0.cend())
                    str_tmp = *pd1;
                else
                    str_tmp = "";
                for (auto it = str_tmp.begin(); it < str_tmp.end(); ++it)
                {
                    if (*it == c.quote_char)
                    {
                        it = str_tmp.insert(it, c.quote_char) + 1;
                        has_special_char = true;
                    }
                    if (equal_delimeter(it, c.delimeter) || *it == '\n')
                        has_special_char = true;
                }
                if (has_special_char)
                {
                    str_tmp = c.quote_char + str_tmp + c.quote_char;
                    has_special_char = false;
                }
                os << str_tmp;
                pd1 < pd0.cend() - 1 && os << c.delimeter;
            }
            os << "\n";
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
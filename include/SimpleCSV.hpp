/*
 *          Simple CSV Master
 *          
 *          v0.0.1  正式版
 *          v0.0.2  修改了Format(CsvFomat _format)读入时的规则
 *          v0.1.0  模板化CsvRow及CsvTable类
 *          v0.1.1  去掉了BasicCsvRow及BasicCsvTable析构函数中显式调用std::vector()::~vector()的部分
 *          v0.1.2  排除空行；修改BasicCsvTable的CsvFormat后修改所有内部CsvRow的Format属性；补齐或修剪
 *                  列数使与表头相等
 * 
 */

#ifndef __SIMPLECSV_HPP
#define __SIMPLECSV_HPP
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace SimpleCSV
{
    using std::vector;
    typedef size_t IndexT;

    static const IndexT nIndex = -1;
    template <typename CharT>
    const std::basic_string<CharT> DFL_DELI{(CharT)','};
    template <typename CharT>
    const CharT DFL_QUOTE = (CharT)'\"';
    template <typename CharT>
    const CharT DFL_ENDLINE = (CharT)'\n';

    template <typename CharT>
    class BasicCsvRow;
    template <typename CharT>
    class BasicCsvTable;

    using CsvRow = BasicCsvRow<char>;
    using wCsvRow = BasicCsvRow<wchar_t>;
    using CsvTable = BasicCsvTable<char>;
    using wCsvTable = BasicCsvTable<wchar_t>;

    template <typename CharT>
    bool IsDeli(const typename std::basic_string<CharT>::iterator _it, const std::basic_string<CharT> &_deli) noexcept;
    template <typename CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvRow<CharT> &_csvrow);
    template <typename CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvRow<CharT> &_csvrow);
    template <typename CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvTable<CharT> &_CsvTable);
    template <typename CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvTable<CharT> &_CsvTable);

    struct CsvRange
    {
        IndexT Header_;       //读入文件的第Header_行为表头，-1时为无表头，默认0（第一行）,第Header_行前的数据会被丢弃
        IndexT CountRows_;    //读入文件的行数，即读入文件的行范围[Header_, Header_ + CountRows_)的内容
        IndexT Index_;        //文件的第Index_列为起始列
        IndexT CountColumns_; //读入文件文件的列数

        CsvRange(IndexT _header = 0,
                 IndexT _CountRows = nIndex,
                 IndexT _index = 0,
                 IndexT _CountColumns = nIndex) : Header_(_header),
                                                  CountRows_(_CountRows),
                                                  Index_(_index),
                                                  CountColumns_(_CountColumns) {}
    };

    template <typename CharT>
    struct CsvFormat
    {
        std::basic_string<CharT> Delimeter_; //分隔符，默认 ","，支持多字符
        CharT Quote_;                        //转义字符
        CharT Endline_;                      //换行符

        CsvFormat(std::basic_string<CharT> _delimeter = DFL_DELI<CharT>,
                  CharT _quote = DFL_QUOTE<CharT>) : Delimeter_(_delimeter),
                                                     Quote_(_quote),
                                                     Endline_(DFL_ENDLINE<CharT>) {}
    };

    //SECTION: class BasicCsvRow definition.
    template <typename CharT>
    class BasicCsvRow : public vector<std::basic_string<CharT>>
    {
    private:
        CsvRange Range_;
        CsvFormat<CharT> Format_;
        friend BasicCsvTable<CharT>;

    public:
        using vector<std::basic_string<CharT>>::vector;

        BasicCsvRow()
            : vector<std::basic_string<CharT>>(),
              Range_(CsvRange()),
              Format_(CsvFormat<CharT>()) {}
        BasicCsvRow(const CsvRange &_range,
                    const CsvFormat<CharT> &_format) noexcept
            : vector<std::basic_string<CharT>>(),
              Range_(_range)
        {
            Format(_format);
        }
        ~BasicCsvRow() noexcept {}

        void Range(const CsvRange &_range) noexcept { Range_ = _range; }
        void Format(const CsvFormat<CharT> &_format) noexcept;
        const CsvRange &Range() const noexcept { return Range_; }
        const CsvFormat<CharT> &Format() const noexcept { return Format_; }
    };

    //SECTION:  class BasicCsvTable definition
    template <typename CharT>
    class BasicCsvTable : public vector<BasicCsvRow<CharT>>
    {
    private:
        CsvRange Range_;
        CsvFormat<CharT> Format_;
        IndexT Columns_; //Header_的列数
        friend BasicCsvRow<CharT>;

    public:
        using vector<BasicCsvRow<CharT>>::vector;
        BasicCsvTable() : vector<BasicCsvRow<CharT>>(), Range_(CsvRange()), Format_(CsvFormat<CharT>()) {}
        BasicCsvTable(const CsvRange &_range, const CsvFormat<CharT> &_format) noexcept : vector<BasicCsvRow<CharT>>(), Range_(_range) { Format(_format); }
        ~BasicCsvTable() noexcept {}

        void Range(const CsvRange &_range) noexcept { Range_ = _range; }
        void Format(const CsvFormat<CharT> &_format) noexcept;
        const CsvRange &Range() const noexcept { return Range_; }
        const CsvFormat<CharT> &Format() const noexcept { return Format_; }

        // const BasicCsvRow &ConstRow(IndexT _Index) const noexcept { return operator[](_Index); } //返回指定行的整行只读数据
        // BasicCsvRow &Row(IndexT _Index) noexcept { return operator[](_Index); }                  //返回指定行整行数据，并可以使用vector类的全部接口进行对行操作
        BasicCsvRow<CharT> Column(IndexT _Index) const noexcept;                     //返回指定列整列数据
        IndexT HeadIndex(const std::basic_string<CharT> &_fieldname) const noexcept; //返回指定字符串在表头中的位置，如果没有表头，或没有找到字符串，返回nIndex;

        IndexT Rows() const noexcept { return vector<BasicCsvRow<CharT>>::size(); } //返回行数
        IndexT Columns() const noexcept { return Columns_; }                        //返回列数
        void Columns(IndexT _Columns) { Columns_ = _Columns; }
    };

    //SECTION: class BasicCsvRow method implementation

    template <typename CharT>
    void BasicCsvRow<CharT>::Format(const CsvFormat<CharT> &_format) noexcept
    {
        Format_ = _format;
        if (Format_.Delimeter_.empty())
            Format_.Delimeter_ = DFL_DELI<CharT>;
        if (Format_.Quote_ == 0 ||
            (Format_.Delimeter_.size() < 2 &&
             Format_.Delimeter_.front() == Format_.Quote_))
        {
            if (Format_.Quote_ == DFL_QUOTE<CharT>)
                Format_.Delimeter_ = DFL_DELI<CharT>;
            else
                Format_.Quote_ = DFL_QUOTE<CharT>;
        }
        Format_.Endline_ = DFL_ENDLINE<CharT>;
    }

    //SECTION: class BasicCsvTable method implementation

    template <typename CharT>
    void BasicCsvTable<CharT>::Format(const CsvFormat<CharT> &_format) noexcept
    {
        Format_ = _format;
        if (Format_.Delimeter_.empty())
            Format_.Delimeter_ = DFL_DELI<CharT>;
        if (Format_.Quote_ == 0 ||
            Format_.Delimeter_.size() < 2 &&
                Format_.Delimeter_.front() == Format_.Quote_)
        {
            if (Format_.Quote_ == DFL_QUOTE<CharT>)
                Format_.Delimeter_ = DFL_DELI<CharT>;
            else
                Format_.Quote_ = DFL_QUOTE<CharT>;
        }
        Format_.Endline_ = DFL_ENDLINE<CharT>;
        for (auto &pd : *this)
            pd.Format(Format_);
    }

    template <typename CharT>
    BasicCsvRow<CharT> BasicCsvTable<CharT>::Column(IndexT _Index) const noexcept
    {
        BasicCsvRow<CharT> col;
        if (_Index <= vector<BasicCsvRow<CharT>>::data()->size())
        {
            for (auto pd0 : *this)
            {
                col.push_back(pd0[_Index]);
            }
        }
        return col;
    }

    template <typename CharT>
    IndexT BasicCsvTable<CharT>::HeadIndex(const std::basic_string<CharT> &_fieldname) const noexcept
    {
        if (~Range_.Header_)
        {
            for (IndexT itIndex = 0; itIndex < Columns_; ++itIndex)
                if (((*this)[0][itIndex] == _fieldname))
                    return itIndex;
        }
        return nIndex;
    }

    //SECTION: other non member mothods

    template <typename CharT>
    inline bool IsDeli(typename std::basic_string<CharT>::iterator _it, const std::basic_string<CharT> &_deli) noexcept
    {
        auto itDeli = _deli.begin();
        while (*_it == *itDeli)
            _it++, itDeli++;
        return (itDeli == _deli.end());
    }

    template <typename CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvRow<CharT> &_csvrow)
    {
        IndexT column = 0, loadedcolumn = 0;
        bool EndlineFlag = false;
        std::basic_string<CharT> StrSource, StrTmp;
        typename std::basic_string<CharT>::iterator it1, it2;
        do
        {
            getline(is, StrSource);
            if (EndlineFlag)
                StrSource = StrTmp + StrSource; //如果内容包含换行，则添加上换行前的内容
            it1 = it2 = StrSource.begin();
            while (it1 != StrSource.end() && (_csvrow.Range().Index_ == nIndex || column < _csvrow.Range().CountColumns_)) //对读入行进行处理
            {
                if (*it1 == _csvrow.Format().Quote_)
                {
                    ++it1;
                    if (EndlineFlag)
                    {
                        it2 += StrTmp.size(); //直接从换行后的内容开始，
                        EndlineFlag = false;  //对换行内容处理完毕
                    }
                    else
                        it2 = it1;
                    while (!(*it2 == _csvrow.Format().Quote_ && (IsDeli(it2 + 1, _csvrow.Format().Delimeter_) || it2 >= StrSource.end() - 1))) // 当前字节为quote_char且下一部分为Delimeter_或字符结束，说明字符已结束
                    {
                        if (*it2 == _csvrow.Format().Quote_ && *(it2 + 1) == _csvrow.Format().Quote_)
                            StrSource.erase(it2);
                        if (it2 >= StrSource.end() - 1) // 内容中存在换行
                        {
                            StrTmp = _csvrow.Format().Quote_ + std::basic_string<CharT>(it1, it2 + 1) + _csvrow.Format().Endline_; //已处理的内容临时存放于于StrTmp中
                            EndlineFlag = true;                                                                                    //设置换行标记
                            break;
                        }
                        ++it2;
                    }
                }
                else
                {
                    it2 = it1;
                    auto en = StrSource.find(_csvrow.Format().Delimeter_, it1 - StrSource.begin());
                    if (~en)
                        it2 = StrSource.begin() + en;
                    else
                        it2 = StrSource.end();
                }
                if (!EndlineFlag && (_csvrow.Range().Index_ == nIndex || loadedcolumn++ >= _csvrow.Range().Index_))
                {
                    _csvrow.emplace_back(it1, it2);
                    ++column;
                }
                it1 = it2 + (it2 != StrSource.end()) + (*it2 == _csvrow.Format().Quote_ && IsDeli(it2 + 1, _csvrow.Format().Delimeter_)) * _csvrow.Format().Delimeter_.size();
            }
        } while (EndlineFlag);
        return is;
    }

    template <typename CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvRow<CharT> &_csvrow)
    {
        bool HasSpecialChar = false;
        std::basic_string<CharT> StrTmp;
        for (auto itCsvRow = _csvrow.cbegin(); itCsvRow < _csvrow.cend(); ++itCsvRow)
        {
            StrTmp = *itCsvRow;
            for (auto itStr = StrTmp.begin(); itStr < StrTmp.end(); ++itStr)
            {
                if (*itStr == _csvrow.Format().Quote_)
                {
                    itStr = StrTmp.insert(itStr, _csvrow.Format().Quote_) + 1;
                    HasSpecialChar = true;
                }
                if (IsDeli(itStr, _csvrow.Format().Delimeter_) || *itStr == _csvrow.Format().Endline_)
                    HasSpecialChar = true;
            }
            if (HasSpecialChar)
            {
                StrTmp = _csvrow.Format().Quote_ + StrTmp + _csvrow.Format().Quote_;
                HasSpecialChar = false;
            }
            os << StrTmp;
            itCsvRow < _csvrow.cend() - 1 && os << _csvrow.Format().Delimeter_;
        }

        return os;
    }

    template <typename CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvTable<CharT> &_CsvTable)
    {
        IndexT row = 0, loadedline = 0;
        BasicCsvRow<CharT> CsvRow_tmp(_CsvTable.Range(), _CsvTable.Format());
        do
        {
            is >> CsvRow_tmp;

            if (!CsvRow_tmp.empty())
            {
                while (CsvRow_tmp.size() < _CsvTable.Columns()) //增加列数与表头对齐
                    CsvRow_tmp.emplace_back(std::basic_string<CharT>());
                while (CsvRow_tmp.size() > _CsvTable.Columns()) //去掉多出的列数
                    CsvRow_tmp.pop_back();
                if (_CsvTable.Range().Header_ == nIndex || loadedline++ >= _CsvTable.Range().Header_) //如果Header_ == nIndex，为没有Header_的情况，每一行都被读入；如果loadedline >= Header_，可以读入余下的行
                {
                    ++row;
                    _CsvTable.emplace_back(CsvRow_tmp);
                }
            }

            if (row == 1) //以第一行列数为全表列数
                _CsvTable.Columns(CsvRow_tmp.size());

            CsvRow_tmp.clear();
        } while (!is.eof() && row < _CsvTable.Range().CountRows_);
        return is;
    }

    template <typename CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvTable<CharT> &_CsvTable)
    {
        for (auto itTable = _CsvTable.cbegin(); itTable < _CsvTable.end(); ++itTable)
        {
            os << *itTable;
            itTable < _CsvTable.end() - 1 && os << _CsvTable.Format().Endline_;
        }
        return os;
    }

}

#endif //__SIMPLECSV_HPP
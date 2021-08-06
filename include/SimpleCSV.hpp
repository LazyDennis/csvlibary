/*
 *          Simple CSV Master
 *          
 *          v0.0.1  正式版
 *          v0.0.2  修改了Format(CsvFomat _format)读入时的规则
 *          v0.1.0  模板化CsvRow及CsvTable类
 *          v0.1.1  去掉了BasicCsvRow及BasicCsvTable析构函数中显式调用std::vector()::~vector()的部分
 *          v0.1.2  排除空行；修改BasicCsvTable的CsvFormat后修改所有内部CsvRow的Format属性；补齐或修剪
 *                  列数使与表头相等
 *          v0.2.0b1增加2个insert函数。
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
    template <class CharT>
    const std::basic_string<CharT> DFL_DELI{(CharT)','};
    template <class CharT>
    const CharT DFL_QUOTE = (CharT)'\"';
    template <class CharT>
    const CharT DFL_ENDLINE = (CharT)'\n';

    template <class CharT>
    class BasicCsvRow;
    template <class CharT>
    class BasicCsvTable;

    using CsvRow = BasicCsvRow<char>;
    using wCsvRow = BasicCsvRow<wchar_t>;
    using CsvTable = BasicCsvTable<char>;
    using wCsvTable = BasicCsvTable<wchar_t>;

    template <class CharT>
    bool IsDeli(const typename std::basic_string<CharT>::iterator _it, const std::basic_string<CharT> &_deli) noexcept;
    template <class CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvRow<CharT> &_CsvRow);
    template <class CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvRow<CharT> &_CsvRow);
    template <class CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvTable<CharT> &_CsvTable);
    template <class CharT>
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

    template <class CharT>
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
    template <class CharT>
    class BasicCsvRow : public vector<std::basic_string<CharT>>
    {
    private:
        IndexT row_;
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

        void Row(IndexT _row) noexcept { row_ = _row; }
        IndexT Row() const noexcept { return row_; }
    };

    //SECTION:  class BasicCsvTable definition
    template <class CharT>
    class BasicCsvTable : public vector<BasicCsvRow<CharT>>
    {
    private:
        CsvRange Range_;
        CsvFormat<CharT> Format_;
        IndexT Columns_; //Header_的列数
        friend BasicCsvRow<CharT>;

        void ModifyRow(
            typename BasicCsvTable<CharT>::iterator _ItPos,
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            IndexT _Count);

    public:
        using vector<BasicCsvRow<CharT>>::vector;
        BasicCsvTable() : vector<BasicCsvRow<CharT>>(), Range_(CsvRange()), Format_(CsvFormat<CharT>()) {}
        BasicCsvTable(const CsvRange &_range, const CsvFormat<CharT> &_format) noexcept : vector<BasicCsvRow<CharT>>(), Range_(_range) { Format(_format); }
        ~BasicCsvTable() noexcept {}

        void Range(const CsvRange &_range) noexcept { Range_ = _range; }
        void Format(const CsvFormat<CharT> &_format) noexcept;
        const CsvRange &Range() const noexcept { return Range_; }
        const CsvFormat<CharT> &Format() const noexcept { return Format_; }

        BasicCsvRow<CharT> Column(IndexT _Index) const noexcept;                     //返回指定列整列数据
        IndexT HeadIndex(const std::basic_string<CharT> &_fieldname) const noexcept; //返回指定字符串在表头中的位置，如果没有表头，或没有找到字符串，返回nIndex;

        IndexT Rows() const noexcept { return vector<BasicCsvRow<CharT>>::size(); } //返回行数
        IndexT Columns() const noexcept { return Columns_; }                        //返回列数
        void Columns(IndexT _Columns) { Columns_ = _Columns; }                      //设置列数

        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            const BasicCsvRow<CharT> &_CsvRow);
        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            BasicCsvRow<CharT> &&_CsvRow); //TODO: 未实现
        void insert(typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
                    IndexT count, const BasicCsvRow<CharT> &csvrow); //TODO: 未实现
        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
            typename BasicCsvTable<CharT>::const_iterator _FromLastPosition); //TODO：未实现

        typename BasicCsvTable<CharT>::iterator erase(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition); //TODO：未实现
        typename BasicCsvTable<CharT>::iterator erase(
            typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
            typename BasicCsvTable<CharT>::const_iterator _FromLastPosition); //TODO：未实现
    };

    //SECTION: class BasicCsvRow method implementation

    template <class CharT>
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

    template <class CharT>
    void BasicCsvTable<CharT>::ModifyRow(
        typename BasicCsvTable<CharT>::iterator _ItPos,
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        IndexT _Count)
    {
        for (auto ItPosSize = _ItPos->size(); _ItPos < _InsertPosition + _Count; ++_ItPos)
        {
            ItPosSize.Format(Format_);
            ItPosSize = _ItPos->size();
            if (ItPosSize > Columns_)
            {
                auto ItFirst = _ItPos->end() - (ItPosSize - Columns_);
                _ItPos->erase(ItFirst, _ItPos->end());
            }
            if (ItPosSize < Columns_)
            {
                _ItPos->insert(_ItPos->end(), Columns_ - ItPosSize, std::basic_string<CharT>());
            }
        }
        for (auto it = _ItPos; it < this->end(); ++it)
        {
            it->row_ = (this->begin() == it) ? 0 : ((it - 1)->row_ + 1);
        }
    }

    template <class CharT>
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

    template <class CharT>
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

    template <class CharT>
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

    template <class CharT>
    typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        const BasicCsvRow<CharT> &_CsvRow)
    {
        auto ItPos = std::vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _CsvRow);
        ModifyRow(ItPos, _InsertPosition, 1);

        return ItPos;
    }

    template <class CharT>
    typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromLastPosition)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _FromFirstPosition, _FromLastPosition);
        ModifyRow(ItPos, _InsertPosition, (_FromLastPosition - _FromFirstPosition));
        return ItPos;
    }

    //SECTION: other non member mothods

    template <class CharT>
    inline bool IsDeli(typename std::basic_string<CharT>::iterator _it, const std::basic_string<CharT> &_deli) noexcept
    {
        auto itDeli = _deli.begin();
        while (*_it == *itDeli)
            _it++, itDeli++;
        return (itDeli == _deli.end());
    }

    template <class CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvRow<CharT> &_CsvRow)
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
            while (it1 != StrSource.end() && (_CsvRow.Range().Index_ == nIndex || column < _CsvRow.Range().CountColumns_)) //对读入行进行处理
            {
                if (*it1 == _CsvRow.Format().Quote_)
                {
                    ++it1;
                    if (EndlineFlag)
                    {
                        it2 += StrTmp.size(); //直接从换行后的内容开始，
                        EndlineFlag = false;  //对换行内容处理完毕
                    }
                    else
                        it2 = it1;
                    while (!(*it2 == _CsvRow.Format().Quote_ && (IsDeli(it2 + 1, _CsvRow.Format().Delimeter_) || it2 >= StrSource.end() - 1))) // 当前字节为quote_char且下一部分为Delimeter_或字符结束，说明字符已结束
                    {
                        if (*it2 == _CsvRow.Format().Quote_ && *(it2 + 1) == _CsvRow.Format().Quote_)
                            StrSource.erase(it2);
                        if (it2 >= StrSource.end() - 1) // 内容中存在换行
                        {
                            StrTmp = _CsvRow.Format().Quote_ + std::basic_string<CharT>(it1, it2 + 1) + _CsvRow.Format().Endline_; //已处理的内容临时存放于于StrTmp中
                            EndlineFlag = true;                                                                                    //设置换行标记
                            break;
                        }
                        ++it2;
                    }
                }
                else
                {
                    it2 = it1;
                    auto en = StrSource.find(_CsvRow.Format().Delimeter_, it1 - StrSource.begin());
                    if (~en)
                        it2 = StrSource.begin() + en;
                    else
                        it2 = StrSource.end();
                }
                if (!EndlineFlag && (_CsvRow.Range().Index_ == nIndex || loadedcolumn++ >= _CsvRow.Range().Index_))
                {
                    _CsvRow.emplace_back(it1, it2);
                    ++column;
                }
                it1 = it2 + (it2 != StrSource.end()) + (*it2 == _CsvRow.Format().Quote_ && IsDeli(it2 + 1, _CsvRow.Format().Delimeter_)) * _CsvRow.Format().Delimeter_.size();
            }
        } while (EndlineFlag);
        return is;
    }

    template <class CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicCsvRow<CharT> &_CsvRow)
    {
        bool HasSpecialChar = false;
        std::basic_string<CharT> StrTmp;
        for (auto itCsvRow = _CsvRow.cbegin(); itCsvRow < _CsvRow.cend(); ++itCsvRow)
        {
            StrTmp = *itCsvRow;
            for (auto itStr = StrTmp.begin(); itStr < StrTmp.end(); ++itStr)
            {
                if (*itStr == _CsvRow.Format().Quote_)
                {
                    itStr = StrTmp.insert(itStr, _CsvRow.Format().Quote_) + 1;
                    HasSpecialChar = true;
                }
                if (IsDeli(itStr, _CsvRow.Format().Delimeter_) || *itStr == _CsvRow.Format().Endline_)
                    HasSpecialChar = true;
            }
            if (HasSpecialChar)
            {
                StrTmp = _CsvRow.Format().Quote_ + StrTmp + _CsvRow.Format().Quote_;
                HasSpecialChar = false;
            }
            os << StrTmp;
            itCsvRow < _CsvRow.cend() - 1 && os << _CsvRow.Format().Delimeter_;
        }

        return os;
    }

    template <class CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is, BasicCsvTable<CharT> &_CsvTable)
    {
        IndexT row = 0, loadedline = 0;
        BasicCsvRow<CharT> CsvRow_tmp(_CsvTable.Range(), _CsvTable.Format());
        do
        {
            is >> CsvRow_tmp;

            if (!CsvRow_tmp.empty())
            {
                while (row > 0 && CsvRow_tmp.size() < _CsvTable.Columns()) //增加列数与表头对齐
                    CsvRow_tmp.emplace_back(std::basic_string<CharT>());
                while (row > 0 && CsvRow_tmp.size() > _CsvTable.Columns()) //去掉多出的列数
                    CsvRow_tmp.pop_back();
                if (_CsvTable.Range().Header_ == nIndex || loadedline++ >= _CsvTable.Range().Header_) //如果Header_ == nIndex，为没有Header_的情况，每一行都被读入；如果loadedline >= Header_，可以读入余下的行
                {
                    CsvRow_tmp.Row(row);
                    _CsvTable.emplace_back(CsvRow_tmp);
                    ++row;
                }
            }

            if (row == 1) //以第一行列数为全表列数
                _CsvTable.Columns(CsvRow_tmp.size());

            CsvRow_tmp.clear();
        } while (!is.eof() && row < _CsvTable.Range().CountRows_);
        return is;
    }

    template <class CharT>
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
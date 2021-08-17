/*
 *          Simple CSV Master
 *          
 *          v0.0.1  正式版
 *          v0.0.2  修改了Format(CsvFomat _format)读入时的规则
 *          v0.1.0  模板化CsvRow及CsvTable类
 *          v0.1.1  去掉了BasicCsvRow及BasicCsvTable析构函数中显式调用std::vector()::~vector()的部分
 *          v0.1.2  排除空行；修改BasicCsvTable的CsvFormat后修改所有内部CsvRow的Format属性；补齐或修剪
 *                  列数使与表头相等
 *          v0.2.0  完成重写所有使元素增减的修改器（insert，emplace，erase，push_back，emplace_back。
 *          v0.2.1  重写部分构造函数。
 *          v0.2.2  增加行swap()函数，增加SwapRow()函数。
 *          v0.2.3  重写了assign()函数。重写了operator=()函数, 重写copy ctor和move ctor。
 *          v0.2.4  增加NewRow()函数。
 *          v0.2.5  解决无法调用BasicCsvRow.operator[](size_t index)的问题。
 *          v0.2.6  简化表格读入的代码，修复指定列范围时，读入残余的问题。
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
    bool IsDeli(const typename std::basic_string<CharT>::iterator _it,
                const std::basic_string<CharT> &_deli) noexcept;
    template <class CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is,
                                          BasicCsvRow<CharT> &_CsvRow);
    template <class CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os,
                                          const BasicCsvRow<CharT> &_CsvRow);
    template <class CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is,
                                          BasicCsvTable<CharT> &_CsvTable);
    template <class CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os,
                                          const BasicCsvTable<CharT> &_CsvTable);

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
        IndexT RowIndex_;
        CsvRange Range_;
        CsvFormat<CharT> Format_;
        friend BasicCsvTable<CharT>;

    public:
        using vector<std::basic_string<CharT>>::vector;
        using vector<std::basic_string<CharT>>::operator[];

        BasicCsvRow() = default;
        BasicCsvRow(const CsvRange &_range,
                    const CsvFormat<CharT> &_format) noexcept
            : vector<std::basic_string<CharT>>(),
              Range_(_range) { Format(_format); }

        ~BasicCsvRow() noexcept {}

        void Range(const CsvRange &_range) noexcept { Range_ = _range; }
        void Format(const CsvFormat<CharT> &_format) noexcept;
        const CsvRange &Range() const noexcept { return Range_; }
        const CsvFormat<CharT> &Format() const noexcept { return Format_; }

        void Row(IndexT _row) noexcept { RowIndex_ = _row; }
        IndexT Row() const noexcept { return RowIndex_; }

        std::basic_string<CharT> &operator[](
            const std::basic_string<CharT> &_FieldName);
        const std::basic_string<CharT> &operator[](
            const std::basic_string<CharT> &_FieldName) const;
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
            typename BasicCsvTable<CharT>::const_iterator _Position,
            IndexT _Count);

    public:
        BasicCsvTable() = default;
        BasicCsvTable(const CsvRange &_range,
                      const CsvFormat<CharT> &_format) noexcept
            : vector<BasicCsvRow<CharT>>(),
              Range_(_range) { Format(_format); }
        BasicCsvTable(IndexT _Row, const BasicCsvRow<CharT> &_CsvRow) noexcept
            : vector<BasicCsvRow<CharT>>(_Row, _CsvRow) noexcept
        {
            ModifyRow(this->begin(), this->cbegin(), 0);
        }
        explicit BasicCsvTable(IndexT _Row, IndexT _Columns = 0) noexcept
            : vector<BasicCsvRow<CharT>>(_Row, BasicCsvRow<CharT>(_Columns)),
              Columns_(_Columns) { ModifyRow(this->begin(), this->cbegin(), 0); }
        BasicCsvTable(typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
                      typename BasicCsvTable<CharT>::const_iterator _FromLastPosition) noexcept
            : vector<BasicCsvRow<CharT>>(_FromLastPosition,
                                         _FromLastPosition) { ModifyRow(this->begin(), this->cbegin(), 0); }
        BasicCsvTable(std::initializer_list<BasicCsvRow<CharT>> _CsvRowList) noexcept
            : vector<BasicCsvRow<CharT>>(_CsvRowList) { ModifyRow(this->begin(), this->cbegin(), 0); };
        BasicCsvTable(const BasicCsvTable<CharT> &_CsvTable) : vector<BasicCsvRow<CharT>>(_CsvTable)
        {
            Format_ = _CsvTable.Format_;
            Columns_ = _CsvTable.Columns;
        }
        BasicCsvTable(BasicCsvTable &&_CsvTable) : vector<BasicCsvRow<CharT>>(std::move(_CsvTable))
        {
            Format_ = std::move(_CsvTable.Format_);
            Columns_ = _CsvTable.Columns;
        }
        ~BasicCsvTable() noexcept {}

        void Range(const CsvRange &_range) noexcept { Range_ = _range; }
        void Format(const CsvFormat<CharT> &_format) noexcept;
        const CsvRange &Range() const noexcept { return Range_; }
        const CsvFormat<CharT> &Format() const noexcept { return Format_; }

        BasicCsvRow<CharT> Column(IndexT _Index) const noexcept;                     //返回指定列整列数据
        IndexT HeadIndex(const std::basic_string<CharT> &_fieldname) const noexcept; //返回指定字符串在表头中的位置，如果没有表头，或没有找到字符串，返回nIndex;

        IndexT Rows() const noexcept { return this->size(); }  //返回行数
        IndexT Columns() const noexcept { return Columns_; }   //返回列数
        void Columns(IndexT _Columns) { Columns_ = _Columns; } //设置列数
        typename BasicCsvTable<CharT>::iterator NewRow();

        const BasicCsvTable<CharT> &operator=(std::initializer_list<BasicCsvRow<CharT>> _CsvRowList);
        const BasicCsvTable<CharT> &operator=(const BasicCsvTable<CharT> &_CsvTable);
        const BasicCsvTable<CharT> &operator=(BasicCsvTable<CharT> &&_CsvTable);

        void assign(IndexT _Count, const BasicCsvRow<CharT> &_CsvRow);
        void assign(typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
                    typename BasicCsvTable<CharT>::const_iterator _FromLastPosition);
        void assign(std::initializer_list<BasicCsvRow<CharT>> _CsvRowList);

        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            const BasicCsvRow<CharT> &_CsvRow);
        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            BasicCsvRow<CharT> &&_CsvRow);
        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            IndexT _Count, const BasicCsvRow<CharT> &_CsvRow);
        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
            typename BasicCsvTable<CharT>::const_iterator _FromLastPosition);
        typename BasicCsvTable<CharT>::iterator insert(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            std::initializer_list<BasicCsvRow<CharT>> _CsvRowList);

        template <class... Args>
        typename BasicCsvTable<CharT>::iterator emplace(
            typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
            Args &&..._Args);

        typename BasicCsvTable<CharT>::iterator erase(
            typename BasicCsvTable<CharT>::const_iterator _ErasePosition);
        typename BasicCsvTable<CharT>::iterator erase(
            typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
            typename BasicCsvTable<CharT>::const_iterator _FromLastPosition);

        void push_back(const BasicCsvRow<CharT> &_CsvRow);
        void push_back(BasicCsvRow<CharT> &&_CsvRow);

        template <class... Args>
        void emplace_back(Args &&..._Args);

        void resize(IndexT _Count);
        void resize(IndexT _Count, const BasicCsvRow<CharT> &_CsvRow);

        void swap(BasicCsvTable<CharT> &_CsvTable);

        void SwapRow(IndexT _RowIndex1, IndexT _RowIndex2);
        void SwapRow(typename BasicCsvTable<CharT>::iterator _RowIt1,
                     typename BasicCsvTable<CharT>::iterator _RowIt2);
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

    template <class CharT>
    inline std::basic_string<CharT> &BasicCsvRow<CharT>::operator[](
        const std::basic_string<CharT> &_FieldName)
    {
        auto CsvRowHead = this - RowIndex_;
        for (auto ItCsvRow = CsvRowHead->begin(); ItCsvRow < CsvRowHead->end(); ++ItCsvRow)
        {
            if (*ItCsvRow == _FieldName)
                return *(this->begin() + (ItCsvRow - CsvRowHead->begin()));
        }
        throw std::invalid_argument(_FieldName + " not found");
    }

    template <class CharT>
    inline const std::basic_string<CharT> &BasicCsvRow<CharT>::operator[](
        const std::basic_string<CharT> &_FieldName) const
    {
        return this->operator[](_FieldName);
    }

    //SECTION: class BasicCsvTable method implementation

    template <class CharT>
    void BasicCsvTable<CharT>::ModifyRow(
        typename BasicCsvTable<CharT>::iterator _ItPos,
        typename BasicCsvTable<CharT>::const_iterator _Position,
        IndexT _Count)
    {
        auto itpos = _ItPos;
        if (front().size())
            Columns_ = front().size();
        for (IndexT ItPosSize = itpos->size(); itpos < _Position + _Count; ++itpos)
        {
            itpos->Format(Format_);
            ItPosSize = itpos->size();
            if (ItPosSize > Columns_)
            {
                auto ItFirst = itpos->end() - (ItPosSize - Columns_);
                itpos->erase(ItFirst, itpos->end());
            }
            if (ItPosSize < Columns_)
            {
                itpos->insert(itpos->end(), Columns_ - ItPosSize, std::basic_string<CharT>());
            }
        }
        for (itpos = _ItPos; itpos < this->end(); ++itpos)
        {
            itpos->RowIndex_ = (this->begin() == itpos) ? 0 : ((itpos - 1)->RowIndex_ + 1);
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
    inline BasicCsvRow<CharT> BasicCsvTable<CharT>::Column(IndexT _Index) const noexcept
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
    inline IndexT BasicCsvTable<CharT>::HeadIndex(const std::basic_string<CharT> &_fieldname) const noexcept
    {
        if (~Range_.Header_)
            for (auto HeadIt = this->cbegin()->cbegin(); HeadIt < this->cbegin()->cend(); ++HeadIt)
                if (_fieldname == *HeadIt)
                    return HeadIt - this->cbegin()->cbegin();
        return nIndex;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::NewRow()
    {
        if (this->empty())
            this->emplace_back(BasicCsvRow<CharT>());
        else
            this->emplace_back(BasicCsvRow<CharT>(Columns_));
        return this->end() - 1;
    }

    template <class CharT>
    inline const BasicCsvTable<CharT> &BasicCsvTable<CharT>::operator=(
        std::initializer_list<BasicCsvRow<CharT>> _CsvRowList)
    {
        vector<BasicCSvRow<CharT>>::operator=(_CsvRowList);
        ModifyRow(this->begin(), this->begin(), 0);
        return *this;
    }

    template <class CharT>
    inline const BasicCsvTable<CharT> &BasicCsvTable<CharT>::operator=(const BasicCsvTable<CharT> &_CsvTable)
    {
        vector<BasicCsvRow<CharT>>::operator=(_CsvTable);
        Format_ = _CsvTable.Format_;
        Columns_ = _CsvTable.Columns_;
        return *this;
    }

    template <class CharT>
    inline const BasicCsvTable<CharT> &BasicCsvTable<CharT>::operator=(BasicCsvTable<CharT> &&_CsvTable)
    {
        vector<BasicCsvRow<CharT>>::operator=(std::move(_CsvTable));
        Format_ = _CsvTable.Format_;
        Columns_ = _CsvTable.Columns_;
        return *this;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::assign(IndexT _Rows, const BasicCsvRow<CharT> &_CsvRow)
    {
        vector<BasicCsvRow<CharT>>::assign(_Rows, _CsvRow);
        ModifyRow(this->begin(), this->cbegin(), 0);
        return;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::assign(
        typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromLastPosition)
    {
        vector<BasicCsvRow<CharT>>::assign(_FromFirstPosition, _FromLastPosition);
        ModifyRow(this->begin(), this->cbegin(), 0);
        return;
    }
    template <class CharT>
    inline void BasicCsvTable<CharT>::assign(
        std::initializer_list<BasicCsvRow<CharT>> _CsvRowList)
    {
        vector<BasicCsvRow<CharT>>::assign(_CsvRowList);
        ModifyRow(this->begin(), this->cbegin(), 0);
        return;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        const BasicCsvRow<CharT> &_CsvRow)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _CsvRow);
        ModifyRow(ItPos, _InsertPosition, 1);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        BasicCsvRow<CharT> &&_CsvRow)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, std::move(_CsvRow));
        ModifyRow(ItPos, _InsertPosition, 1);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromLastPosition)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _FromFirstPosition, _FromLastPosition);
        ModifyRow(ItPos, _InsertPosition, (_FromLastPosition - _FromFirstPosition));
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        IndexT _Count, const BasicCsvRow<CharT> &_CsvRow)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _Count, _CsvRow);
        ModifyRow(ItPos, _InsertPosition, _Count);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        std::initializer_list<BasicCsvRow<CharT>> _CsvRowList)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _CsvRowList);
        ModifyRow(ItPos, _InsertPosition, _CsvRowList.size());
        return ItPos;
    }

    template <class CharT>
    template <class... Args>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::emplace(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        Args &&..._Args)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::emplace_back(std::forward<Args>(_Args)...);
        ModifyRow(ItPos, _InsertPosition, 1);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::erase(
        typename BasicCsvTable<CharT>::const_iterator _ErasePosition)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::erase(_ErasePosition);
        ModifyRow(ItPos, _ErasePosition, 0);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::erase(
        typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromLastPosition)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::erase(_FromFirstPosition, _FromLastPosition);
        ModifyRow(ItPos, _FromLastPosition, 0);
        return ItPos;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::push_back(const BasicCsvRow<CharT> &_CsvRow)
    {
        vector<BasicCsvRow<CharT>>::push_back(_CsvRow);
        ModifyRow(this->end() - 1, this->end() - 1, 1);
        return;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::push_back(BasicCsvRow<CharT> &&_CsvRow)
    {
        vector<BasicCsvRow<CharT>>::push_back(std::move(_CsvRow));
        ModifyRow(this->end() - 1, this->end() - 1, 1);
        return;
    }

    template <class CharT>
    template <class... Args>
    inline void BasicCsvTable<CharT>::emplace_back(Args &&..._Args)
    {
        vector<BasicCsvRow<CharT>>::emplace_back(std::forward<Args>(_Args)...);
        ModifyRow(this->end() - 1, this->end() - 1, 1);
        return;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::resize(IndexT _Count)
    {
        resize(_Count, BasicCsvRow<CharT>(Columns_));
        return;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::resize(IndexT _Count, const BasicCsvRow<CharT> &_CsvRow)
    {
        if (this->size() < _Count)
        {
            vector<BasicCsvRow<CharT>>::resize(_Count, _CsvRow);
            ModifyRow(end() - _Count, end() - _Count, _Count);
        }
        else
            vector<BasicCsvRow<CharT>>::resize(_Count, _CsvRow);
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::swap(BasicCsvTable<CharT> &_CsvTable)
    {
        IndexT columntemp = _CsvTable.Columns_;
        _CsvTable.Columns_ = Columns_;
        Columns_ = columntemp;
        CsvFormat<CharT> formattemp = _CsvTable.Format_;
        _CsvTable.Format_ = Format_;
        Format_ = formattemp;
        vector<BasicCsvRow<CharT>>::swap(_CsvTable);
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::SwapRow(IndexT _RowIndex1, IndexT _RowIndex2)
    {
        if (_RowIndex1 <= this->size() && _RowIndex2 <= this->size())
            (this->begin() + _RowIndex1)->swap(*(this->begin() + _RowIndex2));
        return;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::SwapRow(typename BasicCsvTable<CharT>::iterator _RowIt1,
                                              typename BasicCsvTable<CharT>::iterator _RowIt2)
    {
        if (_RowIt1 >= this->begin() &&
            _RowIt1 <= this->end() &&
            _RowIt2 >= this->begin() &&
            _RowIt2 <= this->end())
        {
            _RowIt1->swap(_RowIt2);
        }
        return;
    }

    //SECTION: other non member mothods

    template <class CharT>
    inline bool IsDeli(typename std::basic_string<CharT>::iterator _it,
                       const std::basic_string<CharT> &_deli) noexcept
    {
        auto itDeli = _deli.begin();
        while (*_it == *itDeli)
            _it++, itDeli++;
        return (itDeli == _deli.end());
    }

    template <class CharT>
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is,
                                          BasicCsvRow<CharT> &_CsvRow)
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
            while (it1 != StrSource.end() /* &&
                   (_CsvRow.Range().Index_ == nIndex ||
                    column < _CsvRow.Range().CountColumns_) */
                   )                      //对读入行进行处理
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
                    while (!(*it2 == _CsvRow.Format().Quote_ &&
                             (IsDeli(it2 + 1, _CsvRow.Format().Delimeter_) ||
                              it2 >= StrSource.end() - 1))) // 当前字节为quote_char且下一部分为Delimeter_或字符结束，说明字符已结束
                    {
                        if (*it2 == _CsvRow.Format().Quote_ && *(it2 + 1) == _CsvRow.Format().Quote_)
                            StrSource.erase(it2);
                        if (it2 >= StrSource.end() - 1) // 内容中存在换行
                        {
                            StrTmp = _CsvRow.Format().Quote_ +
                                     std::basic_string<CharT>(it1, it2 + 1) +
                                     _CsvRow.Format().Endline_; //已处理的内容临时存放于于StrTmp中
                            EndlineFlag = true;                 //设置换行标记
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
                if (!EndlineFlag &&
                    (_CsvRow.Range().Index_ == nIndex ||
                     loadedcolumn++ >= _CsvRow.Range().Index_))
                {
                    _CsvRow.emplace_back(it1, it2);
                    ++column;
                }
                it1 = it2 +
                      (it2 != StrSource.end()) +
                      (*it2 == _CsvRow.Format().Quote_ && IsDeli(it2 + 1, _CsvRow.Format().Delimeter_)) *
                          _CsvRow.Format().Delimeter_.size();
            }
        } while (EndlineFlag);
        _CsvRow.erase(_CsvRow.begin() + _CsvRow.Range().CountColumns_, _CsvRow.end());
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
                if (IsDeli(itStr, _CsvRow.Format().Delimeter_) ||
                    *itStr == _CsvRow.Format().Endline_)
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
        IndexT /* row = _CsvTable.size(), */ loadedline = 0;
        BasicCsvRow<CharT> CsvRow_tmp(_CsvTable.Range(), _CsvTable.Format());
        do
        {
            is >> CsvRow_tmp;

            if (!CsvRow_tmp.empty())
            {
                // while (row > 0 && CsvRow_tmp.size() < _CsvTable.Columns()) //增加列数与表头对齐
                //     CsvRow_tmp.emplace_back(std::basic_string<CharT>());
                // while (row > 0 && CsvRow_tmp.size() > _CsvTable.Columns()) //去掉多出的列数
                //     CsvRow_tmp.pop_back();
                if (_CsvTable.Range().Header_ == nIndex || loadedline++ >= _CsvTable.Range().Header_) //如果Header_ == nIndex，为没有Header_的情况，每一行都被读入；如果loadedline >= Header_，可以读入余下的行
                {
                    // if (row == 0) //以第一行列数为全表列数
                    //     _CsvTable.Columns(CsvRow_tmp.size());
                    // CsvRow_tmp.Row(row);
                    _CsvTable.emplace_back(std::move(CsvRow_tmp));
                    // ++row;
                }
            }
            CsvRow_tmp.clear();
        } while (!is.eof() && _CsvTable.size() < _CsvTable.Range().CountRows_);
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
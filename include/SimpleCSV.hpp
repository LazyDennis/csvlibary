/*
 *          Simple CSV Master
 *
 *
 */

#ifndef __SIMPLECSV_HPP
#define __SIMPLECSV_HPP
#define CSVVERSION "V0.6.2"
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>


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
    class BasicCsvColumn;
    template <class CharT>
    class BasicCsvRow;
    template <class CharT>
    class BasicCsvTable;

    using CsvColumn = BasicCsvColumn<char>;
    using wCsvColumn = BasicCsvColumn<wchar_t>;
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
        IndexT Header_;
        IndexT CountRows_;
        IndexT Index_;
        IndexT CountColumns_;

        CsvRange(IndexT _header = 0,
            IndexT _CountRows = nIndex,
            IndexT _index = 0,
            IndexT _CountColumns = nIndex) : Header_(_header),
            CountRows_(_CountRows),
            Index_(_index),
            CountColumns_(_CountColumns) {}
    }; //struct CsvRange

    template <class CharT>
    struct CsvFormat
    {
        std::basic_string<CharT> Delimeter_;
        CharT Quote_;
        CharT Endline_;

        CsvFormat(std::basic_string<CharT> _delimeter = DFL_DELI<CharT>,
            CharT _quote = DFL_QUOTE<CharT>) : Delimeter_(_delimeter),
            Quote_(_quote),
            Endline_(DFL_ENDLINE<CharT>) {}
    }; // struct CsvFormat

    //SECTION: class BasicCsvColumn definition.
    template <class CharT>
    class BasicCsvColumn : public vector<std::basic_string<CharT>>
    {
    private:
    public:
        using vector<std::basic_string<CharT>>::vector;
        using vector<std::basic_string<CharT>>::operator=;
        ~BasicCsvColumn() = default;
    }; // class BasicCsvColumn

    //SECTION: class BasicCsvRow definition.
    template <class CharT>
    class BasicCsvRow : public vector<std::basic_string<CharT>>
    {
    private:
        IndexT RowIndex_ = 0;
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
            Range_(_range)
        {
            this->Format(_format);
        }

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

        bool operator==(const BasicCsvRow<CharT> &_csvrow) noexcept
        {
            return vector<std::basic_string<CharT>>(*this) ==
                vector<std::basic_string<CharT>>(_csvrow);
        }

        bool operator!=(const BasicCsvRow<CharT> &_csvrow) noexcept
        {
            return vector<std::basic_string<CharT>>(*this) !=
                vector<std::basic_string<CharT>>(_csvrow);
        }
        bool operator<(const BasicCsvRow<CharT> &_csvrow) noexcept
        {
            return vector<std::basic_string<CharT>>(*this) <
                vector<std::basic_string<CharT>>(_csvrow);
        }

        bool operator>(const BasicCsvRow<CharT> &_csvrow) noexcept
        {
            return vector<std::basic_string<CharT>>(*this) >
                vector<std::basic_string<CharT>>(_csvrow);
        }

        bool operator<=(const BasicCsvRow<CharT> &_csvrow) noexcept
        {
            return vector<std::basic_string<CharT>>(*this) <=
                vector<std::basic_string<CharT>>(_csvrow);
        }

        bool operator>=(const BasicCsvRow<CharT> &_csvrow) noexcept
        {
            return vector<std::basic_string<CharT>>(*this) >=
                vector<std::basic_string<CharT>>(_csvrow);
        }
        bool find(const std::basic_string<CharT> &_TargetStr,
            vector<IndexT> &_FindResult,
            const CsvRange &_Range) const
        {
            IndexT FindColumn;
            for (auto itpos = this->begin() + _Range.Index_;
                itpos < ((this->size() < _Range.Index_ + _Range.CountColumns_) ?
                    this->end() : this->begin() + _Range.Index_ + _Range.CountColumns_);
                ++itpos)
            {
                FindColumn = itpos->find(_TargetStr);
                if (~FindColumn)
                    _FindResult.push_back(itpos - this->begin());
            }
            return _FindResult.size();
        }
        bool find(const std::basic_string<CharT> &_TargetStr, vector<IndexT> &_FindResult) const
        {
            return find(_TargetStr, _FindResult, Range_);
        }
        IndexT find(const std::basic_string<CharT> &_TargetStr, const CsvRange &_Range) const
        {
            for (auto itpos = this->begin() + _Range.Index_;
                itpos < ((this->size() < _Range.Index_ + _Range.CountColumns_) ?
                    this->end() : this->begin() + _Range.Index_ + _Range.CountColumns_);
                ++itpos)
            {
                if (~itpos->find(_TargetStr))
                    return itpos - this->begin();
            }
            return nIndex;
        }

        IndexT find(const std::basic_string<CharT> &_TargetStr) const
        {
            return find(_TargetStr, Range_);
        }
    }; //class BasicCsvRow

    //SECTION:  class BasicCsvTable definition
    template <class CharT>
    class BasicCsvTable : public vector<BasicCsvRow<CharT>>
    {
    public:
        enum class SortType
        {
            Ascend,
            Decend
        };
        using CsvSortList = vector<std::pair<IndexT, SortType>>;
        using CsvTablePos = vector<std::pair<IndexT, IndexT>>;

    private:
        CsvRange Range_;
        CsvFormat<CharT> Format_;
        IndexT Columns_ = 0;

        friend BasicCsvRow<CharT>;

        class CsvRowComp
        {
        private:
            CsvSortList SortList_;

        public:
            CsvRowComp() = default;
            CsvRowComp(const CsvSortList &_sortsist)
                : SortList_(_sortsist) {}
            CsvRowComp(const CsvRowComp &_CsvRowComp) = default;
            CsvRowComp(CsvRowComp &&_CsvRowComp) = default;
            ~CsvRowComp() = default;

            CsvRowComp &operator=(const CsvRowComp &_CsvRowComp) = default;
            CsvRowComp &operator=(CsvRowComp &&_CsvRowComp) = default;

            const CsvSortList &SortList() noexcept { return SortList_; }
            void SortList(const CsvSortList &_sortlist) noexcept
            {
                SortList_ = _sortlist;
            }

            bool operator()(const BasicCsvRow<CharT> &_csvrowl,
                const BasicCsvRow<CharT> &_csvrowr);
        }; // class CsvRowComp

        void ModifyRow(
            typename BasicCsvTable<CharT>::iterator _ItPos,
            typename BasicCsvTable<CharT>::const_iterator _Position,
            IndexT _Count);

    public:
        BasicCsvTable() = default;
        BasicCsvTable(const CsvRange &_range,
            const CsvFormat<CharT> &_format) noexcept
            : vector<BasicCsvRow<CharT>>(),
            Range_(_range)
        {
            this->Format(_format);
        }

        BasicCsvTable(IndexT _Row, const BasicCsvRow<CharT> &_CsvRow) noexcept
            : vector<BasicCsvRow<CharT>>(_Row, _CsvRow)
        {
            ModifyRow(this->begin(), this->cbegin(), 0);
        }
        explicit BasicCsvTable(IndexT _Row, IndexT _Columns = 0) noexcept
            : vector<BasicCsvRow<CharT>>(_Row, BasicCsvRow<CharT>(_Columns)),
            Columns_(_Columns)
        {
            ModifyRow(this->begin(), this->cbegin(), 0);
        }
        BasicCsvTable(typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
            typename BasicCsvTable<CharT>::const_iterator _FromLastPosition) noexcept
            : vector<BasicCsvRow<CharT>>(_FromLastPosition,
                _FromLastPosition)
        {
            ModifyRow(this->begin(), this->cbegin(), 0);
        }
        BasicCsvTable(std::initializer_list<BasicCsvRow<CharT>> _CsvRowList) noexcept
            : vector<BasicCsvRow<CharT>>(_CsvRowList)
        {
            ModifyRow(this->begin(), this->cbegin(), 0);
        };
        BasicCsvTable(const BasicCsvTable<CharT> &_CsvTable) :
            vector<BasicCsvRow<CharT>>(_CsvTable)
        {
            Format_ = _CsvTable.Format_;
            Columns_ = _CsvTable.Columns_;
        }
        BasicCsvTable(BasicCsvTable &&_CsvTable) noexcept :
            vector<BasicCsvRow<CharT>>(_CsvTable)
        {
            Format_ = std::move(_CsvTable.Format_);
            Columns_ = _CsvTable.Columns_;
        }
        ~BasicCsvTable() noexcept {}

        void Range(const CsvRange &_range) noexcept
        {
            Range_ = _range;
            for (auto &pd : *this)
                pd.Range_ = _range;
        }
        void Format(const CsvFormat<CharT> &_format) noexcept;
        const CsvRange &Range() const noexcept { return Range_; }
        const CsvFormat<CharT> &Format() const noexcept { return Format_; }

        BasicCsvColumn<CharT> Column(IndexT _Index) const noexcept;
        IndexT HeadIndex(const std::basic_string<CharT> &_fieldname) const noexcept;

        IndexT Rows() const noexcept { return this->size(); }
        IndexT Columns() const noexcept { return Columns_; }
        void Columns(IndexT _Columns) { Columns_ = _Columns; }
        typename BasicCsvTable<CharT>::iterator NewRow();

        const BasicCsvTable<CharT> &operator=(std::initializer_list<BasicCsvRow<CharT>> _CsvRowList);
        const BasicCsvTable<CharT> &operator=(const BasicCsvTable<CharT> &_CsvTable);
        const BasicCsvTable<CharT> &operator=(BasicCsvTable<CharT> &&_CsvTable);

        bool operator==(const BasicCsvTable<CharT> &_csvtable) noexcept
        {
            return vector<BasicCsvRow<CharT>>(*this) == vector<BasicCsvRow<CharT>>(_csvtable);
        }

        bool operator!=(const BasicCsvTable<CharT> &_csvtable) noexcept
        {
            return vector<BasicCsvRow<CharT>>(*this) != vector<BasicCsvRow<CharT>>(_csvtable);
        }

        bool operator<(const BasicCsvTable<CharT> &_csvtable) noexcept
        {
            return vector<BasicCsvRow<CharT>>(*this) < vector<BasicCsvRow<CharT>>(_csvtable);
        }

        bool operator<=(const BasicCsvTable<CharT> &_csvtable) noexcept
        {
            return vector<BasicCsvRow<CharT>>(*this) <= vector<BasicCsvRow<CharT>>(_csvtable);
        }

        bool operator>(const BasicCsvTable<CharT> &_csvtable) noexcept
        {
            return vector<BasicCsvRow<CharT>>(*this) > vector<BasicCsvRow<CharT>>(_csvtable);
        }

        bool operator>=(const BasicCsvTable<CharT> &_csvtable) noexcept
        {
            return vector<BasicCsvRow<CharT>>(*this) >= vector<BasicCsvRow<CharT>>(_csvtable);
        }

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
            typename BasicCsvTable<CharT>::const_iterator _FirstPosition,
            typename BasicCsvTable<CharT>::const_iterator _LastPosition);

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

        // typename BasicCsvRow<CharT>::iterator InsertColumn(
        //     typename BasicCsvRow<CharT>::iterator _FromFirstPosition,
        //     typename BasicCsvRow<CharT>::iterator _FromLastPosition
        // );

        typename BasicCsvRow<CharT>::iterator EraseColumn(
            typename BasicCsvRow<CharT>::iterator _ErasePosition);
        typename BasicCsvRow<CharT>::iterator EraseColumn(
            typename BasicCsvRow<CharT>::iterator _FirstPosition,
            typename BasicCsvRow<CharT>::iterator _LastPosition);
        typename BasicCsvRow<CharT>::iterator EraseColumn(IndexT _ErasePosition);
        typename BasicCsvRow<CharT>::iterator EraseColumn(
            IndexT _FirstPosition, IndexT _LastPosition);

        void sort()
        {
            if (~Range_.Header_)
                std::sort(this->begin() + 1, this->end());
            else
                std::sort(this->begin(), this->end());
            ModifyRow(this->begin(), this->begin(), this->size());
        }
        void sort(const CsvSortList &_SortList)
        {
            if (~Range_.Header_)
                std::sort(this->begin() + 1, this->end(), CsvRowComp(_SortList));
            else
                std::sort(this->begin(), this->end(), CsvRowComp(_SortList));
            ModifyRow(this->begin(), this->begin(), this->size());
        }

        bool find(const std::basic_string<CharT> _TargetStr,
            CsvTablePos &_TablePos, const CsvRange &_Range) const
        {
            vector<IndexT> FindColumn;
            for (auto itpos = this->begin() + _Range.Header_;
                itpos < ((this->size() < _Range.Header_ + _Range.CountRows_) ?
                    this->end() : this->begin() + _Range.Header_ + _Range.CountRows_);
                ++itpos)
            {
                if (itpos->find(_TargetStr, FindColumn, _Range))
                {
                    for (auto pd : FindColumn)
                        _TablePos.emplace_back(itpos - this->begin(), pd);
                }
                FindColumn.clear();
            }
            return _TablePos.size();
        }
        bool find(const std::basic_string<CharT> _TargetStr, CsvTablePos &_TablePos) const
        {
            return find(_TargetStr, _TablePos, Range_);
        }
        IndexT find(const std::basic_string<CharT> _TargetStr,
            const CsvRange &_Range) const
        {
            for (auto itpos = this->begin() + _Range.Header_;
                itpos < ((this->size() < _Range.Header_ + _Range.CountRows_) ?
                    this->end() : this->begin() + _Range.Header_ + _Range.CountRows_);
                ++itpos)
            {
                if (~itpos->find(_TargetStr, _Range))
                    return itpos - this->begin();
            }
            return nIndex;
        }

        IndexT find(const std::basic_string<CharT> _TargetStr) const
        {
            return find(_TargetStr, Range_);
        }
    }; //class BasicCsvTable

    //SECTION: class BasicCsvRow method implementation

    template <class CharT>
    inline void BasicCsvRow<CharT>::Format(const CsvFormat<CharT> &_format) noexcept
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
        throw std::invalid_argument("Invalid header.");
    }

    template <class CharT>
    inline const std::basic_string<CharT> &BasicCsvRow<CharT>::operator[](
        const std::basic_string<CharT> &_FieldName) const
    {
        auto CsvRowHead = this - RowIndex_;
        for (auto ItCsvRow = CsvRowHead->begin(); ItCsvRow < CsvRowHead->end(); ++ItCsvRow)
        {
            if (*ItCsvRow == _FieldName)
                return *(this->begin() + (ItCsvRow - CsvRowHead->begin()));
        }
        throw std::invalid_argument("Invalid header.");
    }

    //SECTION: class BasicCsvTable method implementation

    template <class CharT>
    bool BasicCsvTable<CharT>::CsvRowComp::operator()(const BasicCsvRow<CharT> &_csvrowl,
        const BasicCsvRow<CharT> &_csvrowr)
    {
        if (_csvrowl.size() != _csvrowr.size() || SortList_.empty())
            return _csvrowl < _csvrowr;

        bool RowCompareResult;
        auto RowSize = _csvrowl.size();
        vector<IndexT> RowSequence(RowSize);
        IndexT i = 0;
        for (auto &pd : RowSequence)
            pd = i++;
        for (const auto &pd : SortList_)
        {
            RowSequence[pd.first] = nIndex;
        }

        auto ItSortList = SortList_.begin();
        auto ItRowSequence = RowSequence.begin();
        while (ItSortList != SortList_.end() || ItRowSequence != RowSequence.end())
        {
            if (ItSortList != SortList_.end())
            {
                if (ItSortList->second == SortType::Ascend)
                {
                    if (_csvrowl[ItSortList->first] < _csvrowr[ItSortList->first])
                    {
                        RowCompareResult = true;
                        break;
                    }
                    else if (_csvrowl[ItSortList->first] > _csvrowr[ItSortList->first])
                    {
                        RowCompareResult = false;
                        break;
                    }
                }
                else
                {
                    if (_csvrowl[ItSortList->first] > _csvrowr[ItSortList->first])
                    {
                        RowCompareResult = true;
                        break;
                    }
                    else if (_csvrowl[ItSortList->first] < _csvrowr[ItSortList->first])
                    {
                        RowCompareResult = false;
                        break;
                    }
                }
                ++ItSortList;
            }
            else if (ItRowSequence != RowSequence.end())
            {
                if (*ItRowSequence == nIndex)
                {
                    ++ItRowSequence;
                    continue;
                }
                if (_csvrowl[*ItRowSequence] < _csvrowr[*ItRowSequence])
                {
                    RowCompareResult = true;
                    break;
                }
                else if (_csvrowl[*ItRowSequence] > _csvrowr[*ItRowSequence])
                {
                    RowCompareResult = false;
                    break;
                }
                ++ItRowSequence;
            }
            else
                RowCompareResult = false;
        }
        return RowCompareResult;
    }

    template <class CharT>
    void BasicCsvTable<CharT>::ModifyRow(
        typename BasicCsvTable<CharT>::iterator _ItPos,
        typename BasicCsvTable<CharT>::const_iterator _Position,
        IndexT _Count)
    {
        auto itpos = _ItPos;
        if (this->front().size())
            Columns_ = this->front().size();
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
    inline void BasicCsvTable<CharT>::Format(const CsvFormat<CharT> &_format) noexcept
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
    inline BasicCsvColumn<CharT> BasicCsvTable<CharT>::Column(IndexT _Index) const noexcept
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
    inline IndexT BasicCsvTable<CharT>::HeadIndex(
        const std::basic_string<CharT> &_fieldname) const noexcept
    {
        if (~Range_.Header_)
            for (auto HeadIt = this->cbegin()->cbegin();
                HeadIt < this->cbegin()->cend();
                ++HeadIt)
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
        vector<BasicCsvRow<CharT>>::operator=(_CsvRowList);
        ModifyRow(this->begin(), this->begin(), 0);
        return *this;
    }

    template <class CharT>
    inline const BasicCsvTable<CharT> &BasicCsvTable<CharT>::operator=(
        const BasicCsvTable<CharT> &_CsvTable)
    {
        vector<BasicCsvRow<CharT>>::operator=(_CsvTable);
        Format_ = _CsvTable.Format_;
        Columns_ = _CsvTable.Columns_;
        return *this;
    }

    template <class CharT>
    inline const BasicCsvTable<CharT> &BasicCsvTable<CharT>::operator=(
        BasicCsvTable<CharT> &&_CsvTable)
    {
        vector<BasicCsvRow<CharT>>::operator=(std::move(_CsvTable));
        Format_ = _CsvTable.Format_;
        Columns_ = _CsvTable.Columns_;
        return *this;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::assign(
        IndexT _Rows, const BasicCsvRow<CharT> &_CsvRow)
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
        ModifyRow(ItPos, ItPos, 1);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        BasicCsvRow<CharT> &&_CsvRow)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, std::move(_CsvRow));
        ModifyRow(ItPos, ItPos, 1);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromFirstPosition,
        typename BasicCsvTable<CharT>::const_iterator _FromLastPosition)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(
            _InsertPosition, _FromFirstPosition, _FromLastPosition);
        ModifyRow(ItPos, ItPos, (_FromLastPosition - _FromFirstPosition));
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        IndexT _Count, const BasicCsvRow<CharT> &_CsvRow)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _Count, _CsvRow);
        ModifyRow(ItPos, ItPos, _Count);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::insert(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        std::initializer_list<BasicCsvRow<CharT>> _CsvRowList)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::insert(_InsertPosition, _CsvRowList);
        ModifyRow(ItPos, ItPos, _CsvRowList.size());
        return ItPos;
    }

    template <class CharT>
    template <class... Args>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::emplace(
        typename BasicCsvTable<CharT>::const_iterator _InsertPosition,
        Args &&..._Args)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::emplace(std::forward<Args>(_Args)...);
        ModifyRow(ItPos, ItPos, 1);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::erase(
        typename BasicCsvTable<CharT>::const_iterator _ErasePosition)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::erase(_ErasePosition);
        ModifyRow(ItPos, ItPos, 0);
        return ItPos;
    }

    template <class CharT>
    inline typename BasicCsvTable<CharT>::iterator BasicCsvTable<CharT>::erase(
        typename BasicCsvTable<CharT>::const_iterator _FirstPosition,
        typename BasicCsvTable<CharT>::const_iterator _LastPosition)
    {
        auto ItPos = vector<BasicCsvRow<CharT>>::erase(_FirstPosition, _LastPosition);
        ModifyRow(ItPos, ItPos, 0);
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
        vector<BasicCsvRow<CharT>>::resize(_Count, BasicCsvRow<CharT>(Columns_));
        return;
    }

    template <class CharT>
    inline void BasicCsvTable<CharT>::resize(IndexT _Count,
        const BasicCsvRow<CharT> &_CsvRow)
    {
        if (this->size() < _Count)
        {
            vector<BasicCsvRow<CharT>>::resize(_Count, _CsvRow);
            ModifyRow(this->end() - _Count, this->end() - _Count, _Count);
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
    inline void BasicCsvTable<CharT>::SwapRow(
        typename BasicCsvTable<CharT>::iterator _RowIt1,
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

    template <class CharT>
    inline typename BasicCsvRow<CharT>::iterator BasicCsvTable<CharT>::EraseColumn(
        typename BasicCsvRow<CharT>::iterator _ErasePosition)
    {
        return this->EraseColumn(_ErasePosition, _ErasePosition + 1);
    }

    template <class CharT>
    inline typename BasicCsvRow<CharT>::iterator BasicCsvTable<CharT>::EraseColumn(
        typename BasicCsvRow<CharT>::iterator _FirstPosition,
        typename BasicCsvRow<CharT>::iterator _LastPosition)
    {
        if (_FirstPosition >= this->front().begin() &&
            _FirstPosition <= this->front().end() &&
            _LastPosition >= this->front().begin() &&
            _LastPosition <= this->front().end())
        {
            auto fpos = _FirstPosition, lpos = _LastPosition;
            auto fdiff = _FirstPosition - this->front().begin(),
                ldiff = _LastPosition - this->front().begin();
            for (auto &itPos : *this)
            {
                fpos = itPos.begin() + fdiff;
                lpos = itPos.begin() + ldiff;
                itPos.erase(fpos, lpos);
            }
        }
        return _FirstPosition;
    }

    template <class CharT>
    inline typename BasicCsvRow<CharT>::iterator BasicCsvTable<CharT>::EraseColumn(
        IndexT _ErasePosition)
    {
        return this->EraseColumn(this->front().begin() + _ErasePosition,
            this->front().begin() + _ErasePosition + 1);
    }

    template <class CharT>
    inline typename BasicCsvRow<CharT>::iterator BasicCsvTable<CharT>::EraseColumn(
        IndexT _FirstPosition, IndexT _LastPosition)
    {
        return this->EraseColumn(this->front().begin() + _FirstPosition,
            this->front().begin() + _LastPosition);
    }

    //SECTION: other non member mothods

    template <class CharT>
    inline bool IsDeli(typename std::basic_string<CharT>::iterator _it,
        const std::basic_string<CharT> &_deli) noexcept
    {
        auto itDeli = _deli.begin();
        while (itDeli != _deli.end() && *_it == *itDeli)
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
                StrSource = StrTmp + StrSource;
            it1 = it2 = StrSource.begin();
            while (it1 != StrSource.end())
            {
                if (*it1 == _CsvRow.Format().Quote_)
                {
                    ++it1;
                    if (EndlineFlag)
                    {
                        it2 += StrTmp.size();
                        EndlineFlag = false;
                    }
                    else
                        it2 = it1;
                    while (!(*it2 == _CsvRow.Format().Quote_ &&
                        (it2 >= StrSource.end() - 1 ||
                            IsDeli(it2 + 1, _CsvRow.Format().Delimeter_))))
                    {
                        if (*it2 == _CsvRow.Format().Quote_ &&
                            *(it2 + 1) == _CsvRow.Format().Quote_)
                            StrSource.erase(it2);
                        if (it2 >= StrSource.end() - 1)
                        {
                            StrTmp = _CsvRow.Format().Quote_ +
                                std::basic_string<CharT>(it1, it2 + 1) +
                                _CsvRow.Format().Endline_;
                            EndlineFlag = true;
                            break;
                        }
                        ++it2;
                    }
                }
                else
                {
                    it2 = it1;
                    auto en = StrSource.find(
                        _CsvRow.Format().Delimeter_, it1 - StrSource.begin());
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
                if (it2 != StrSource.end())
                {
                    if (*it2 == _CsvRow.Format().Quote_)
                    {
                        if (it2 == StrSource.end() - 1)
                        {
                            it1 = it2 + 1;
                        }
                        else
                        {
                            it1 = it2 + 1 + _CsvRow.Format().Delimeter_.size();
                        }
                    }
                    else
                        it1 = it2 + 1;
                }
                else
                {
                    it1 = StrSource.end();
                }
            }
        } while (EndlineFlag);
        if (_CsvRow.Range().CountColumns_ < _CsvRow.size())
            _CsvRow.erase(_CsvRow.begin() + _CsvRow.Range().CountColumns_, _CsvRow.end());
        return is;
    }

    template <class CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os,
        const BasicCsvRow<CharT> &_CsvRow)
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
    std::basic_istream<CharT> &operator>>(std::basic_istream<CharT> &is,
        BasicCsvTable<CharT> &_CsvTable)
    {
        IndexT loadedline = 0;
        BasicCsvRow<CharT> CsvRow_tmp(_CsvTable.Range(), _CsvTable.Format());
        do
        {
            is >> CsvRow_tmp;

            if (!CsvRow_tmp.empty())
            {
                if (_CsvTable.Range().Header_ == nIndex ||
                    loadedline++ >= _CsvTable.Range().Header_)
                {
                    _CsvTable.emplace_back(std::move(CsvRow_tmp));
                }
            }
            CsvRow_tmp.clear();
        } while (!is.eof() && _CsvTable.size() < _CsvTable.Range().CountRows_);
        return is;
    }

    template <class CharT>
    std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os,
        const BasicCsvTable<CharT> &_CsvTable)
    {
        for (auto itTable = _CsvTable.cbegin(); itTable < _CsvTable.end(); ++itTable)
        {
            os << *itTable;
            itTable < _CsvTable.end() - 1 && os << _CsvTable.Format().Endline_;
        }
        return os;
    }
} //namespace SimpleCSV

#endif //__SIMPLECSV_HPP

#include <boost/core/lightweight_test.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <QAbstractTableModel>


constexpr auto MyRole = Qt::UserRole + 1;


class Model : public QAbstractTableModel
{
public:
  Model()
    : items_{{0, 1}, {2, 3}, {4, 5}}
  {}

  int rowCount(QModelIndex const& = {}) const override
  {
    return items_.size();
  }

  int columnCount(QModelIndex const& = {}) const override
  {
    return 2;
  }

  QVariant data(QModelIndex const& index, int role = Qt::DisplayRole)
    const override
  {
    if (!hasIndex(index.row(), index.column(), index.parent()))
    {
      return false;
    }

    auto& item = items_[index.row()];
    auto field = index.column() == 0 ? &item.first : &item.second;
    switch (role) {
    case Qt::DisplayRole: // fall through
    case MyRole: return *field;
    default: return {};
    }
  }

  bool setData(
    QModelIndex const& index, QVariant const& value, int role = Qt::EditRole)
    override
  {
    if (!hasIndex(index.row(), index.column(), index.parent()))
    {
      return false;
    }

    auto& item = items_[index.row()];
    auto field = index.column() == 0 ? &item.first : &item.second;
    switch (role) {
    case
      MyRole: *field = value.toInt();
      return true;
    default: return false;
    }
  }

public:
  std::vector<std::pair<int, int>> items_;
};


bool predicate(QModelIndex const&) { return true; }


// tag::row_iterator[]
class RowIterator
  : public boost::stl_interfaces::iterator_interface<
      RowIterator,
      std::random_access_iterator_tag,
      QModelIndex,
      QModelIndex>
{
public:
  RowIterator() = default;
  RowIterator(QModelIndex index);

  RowIterator& operator+=(std::ptrdiff_t n);
  QModelIndex operator*() const;
  std::ptrdiff_t operator-(RowIterator const& other) const;

private:
  QModelIndex index_;
};
// end::row_iterator[]


RowIterator::RowIterator(QModelIndex index)
  : index_(index)
{
  Q_ASSERT(index.model());
}


RowIterator& RowIterator::operator+=(std::ptrdiff_t n)
{
  index_ = index_.siblingAtRow(index_.row() + n);
  return *this;
}


QModelIndex RowIterator::operator*() const
{
  return index_;
}


std::ptrdiff_t RowIterator::operator-(RowIterator const& other) const
{
  Q_ASSERT(index_.column() == other.index_.column());
  Q_ASSERT(index_.model() == other.index_.model());
  return index_.row() - other.index_.row();
}


// tag::row_range[]
using RowRange = boost::iterator_range<RowIterator>;

RowRange rowRange(
    QModelIndex const& parent,
    int column = 0,
    QAbstractItemModel const* model = nullptr)
{
  if (!model) { model = parent.model(); }

  QModelIndex first = model->index(0, column, parent);

  int count = model->rowCount(parent);
  QModelIndex last =
    count > 0 ? first.siblingAtRow(count - 1) : first;

  return {RowIterator(first), RowIterator(last)};
}
// end::row_range[]


int main()
{
  using boost::adaptors::filtered;

  Model model;

  // tag::row-iterate0[]
  for (int row = 0; row < model.rowCount(); ++row)
  {
      QModelIndex index = model.index(row, 0);
      if (!predicate(index)) continue;

      model.setData(index, 1, MyRole);
      model.setData(
        index.siblingAtColumn(1), 2, MyRole);
  }
  // end::row-iterate0[]

  // tag::row-iterate1[]
  auto rows = rowRange({}, 0, &model)
    | filtered(predicate);
  for (QModelIndex index: rows)
  {
      model.setData(index, 1, MyRole);
      model.setData(
        index.siblingAtColumn(1), 2, MyRole);
  }
  // end::row-iterate1[]

  return boost::report_errors();
}

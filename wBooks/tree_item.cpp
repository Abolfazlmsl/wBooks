#include "tree_item.h"

TreeItem::TreeItem()
   : _itemData(),
     _parentItem(nullptr)
{
}

TreeItem::TreeItem(const QVariant& data, const QString& src)
   : _itemData(data), _itemSrc(src),
     _parentItem(nullptr)
{
}

TreeItem::~TreeItem()
{
   qDeleteAll(_childItems);
}

TreeItem* TreeItem::parentItem()
{
   return _parentItem;
}

void TreeItem::setParentItem(TreeItem* parentItem)
{
   _parentItem = parentItem;
}

void TreeItem::appendChild(TreeItem* item)
{
   if(item && !_childItems.contains(item)){
      _childItems.append(item);
   }
}

void TreeItem::removeChild(TreeItem* item)
{
   if(item){
      _childItems.removeAll(item);
   }
}

TreeItem* TreeItem::child(int row)
{
   return _childItems.value(row);
}

int TreeItem::childCount() const
{
   return _childItems.count();
}

const QVariant& TreeItem::data() const
{
    return _itemData;
}

const QString &TreeItem::source() const
{
    return _itemSrc;
}

void TreeItem::setData(const QVariant& data)
{
   _itemData = data;
}

bool TreeItem::isLeaf() const
{
   return _childItems.isEmpty();
}

int TreeItem::depth() const
{
   int depth = 0;
   TreeItem* anchestor = _parentItem;
   while(anchestor){
      ++depth;
      anchestor = anchestor->parentItem();
   }

   return depth;
}

int TreeItem::row() const
{
   if (_parentItem){
      return _parentItem->_childItems.indexOf(const_cast<TreeItem* >(this));
   }

   return 0;
}

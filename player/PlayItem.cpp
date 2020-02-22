#include "PlayItem.h"

PlayItem::PlayItem(const string &data_source)
    : mDataSource(data_source)
{
}

const string &PlayItem::get_data_source() const
{
    return mDataSource;
}
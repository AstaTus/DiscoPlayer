#ifndef __PLAY_ITEM_H__
#define __PLAY_ITEM_H__
#include <string>
using namespace std;

class PlayItem
{
private:
    string mDataSource;
public:

    PlayItem(string& data_source);
    const string& get_data_source() const;
};

#endif
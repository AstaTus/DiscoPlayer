#ifndef __PLAY_ITEM_H__
#define __PLAY_ITEM_H__
#include <string>
using namespace std;

class PlayItem
{
private:
    string mDataSource;
public:
    const string& get_data_source() const;
};

#endif
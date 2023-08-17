/**
 * @file frame.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_SORTED_LIST_H
#define CX_COMMON_SORTED_LIST_H

#include <list>

namespace cx {

template <typename T, typename CompareFunc>
class SortedList final {
public:
    using List = std::list<T>;

    SortedList(const CompareFunc &func)
    : mComparator(func)
    {
    }

    void Add(const T &item) 
    {
        bool found = false;

        for (auto iter = mList.begin(); iter != mList.end(); iter++) {
            if (!mComparator(*iter, item)) {
                mList.insert(iter, item);
                found = true;
                break;
            }
        }

        if (!found) {
            mList.push_back(item);
        }
    }

    bool empty() const
    {
        return mList.empty();
    }

    const T &front() const
    {
        return mList.front();
    }

    const T &back() const
    {
        return mList.back();
    }

    void pop_front()
    {
        mList.pop_front();
    }

    void pop_back()
    {
        mList.pop_back();
    }

    size_t size() const
    {
        return mList.size();
    }

    typename List::iterator         begin()         {return mList.begin();}
    typename List::iterator         end()           {return mList.end();}
    typename List::const_iterator   begin() const   {return mList.cbegin();}
    typename List::const_iterator   end() const     {return mList.cend();}
    typename List::reverse_iterator rbegin()        {return mList.rbegin();}
    typename List::reverse_iterator rend()          {return mList.rend();}
    typename List::const_iterator   rbegin() const  {return mList.crbegin();}
    typename List::const_iterator   rend() const    {return mList.crend();}

    typename List::iterator erase(typename List::iterator position) {return mList.erase(position);}

private:
    List mList;
    const CompareFunc mComparator;
};

}


#endif /* CX_COMMON_SORTED_LIST_H */


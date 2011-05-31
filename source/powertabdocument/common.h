#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <algorithm>
#include <memory>
#include <boost/iterator/indirect_iterator.hpp>

/// Helper function - clones a shared_ptr
template <typename T>
std::shared_ptr<T> clone_shared_ptr(const std::shared_ptr<T>& ptr)
{
    return std::make_shared<T>(*ptr);
}

/// Performs a deep copy of a vector of shared_ptr
template <typename T>
void deepCopy(const std::vector<std::shared_ptr<T> >& src,
              std::vector<std::shared_ptr<T> >& dest)
{
    dest.clear();
    
    std::transform(src.begin(), src.end(),
                   std::back_inserter(dest),
                   clone_shared_ptr<T>);
}

/// Tests equality for a vector of pointers, by comparing the pointed-to elements
template <typename T>
bool isDeepEqual(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
    if (vec1.size() != vec2.size())
    {
        return false;
    }

    using boost::make_indirect_iterator;
    return std::equal(make_indirect_iterator(vec1.begin()),
                      make_indirect_iterator(vec1.end()),
                      make_indirect_iterator(vec2.begin()));
}

#endif // COMMON_H

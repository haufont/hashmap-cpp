#include<list>
#include<vector>
#include<utility>
#include<stdexcept>
#include<algorithm>
    
template<class KeyType, class ValueType, 
    class Hash = std::hash<KeyType> > 
class HashMap {
 private:
    typedef std::pair<const KeyType, ValueType> MapType;
    typedef std::list<MapType> ListType;

    ListType list_for_iterator;
    std::vector<std::list<typename ListType::iterator>> backets;
    Hash main_hash_func = Hash();
    size_t size_map;

    constexpr static size_t start_size_backet = 2;

    size_t backet_number(const MapType& in) const {
        return main_hash_func(in.first) % backets.size();
    }

    size_t backet_number(const KeyType& in) const {
        return main_hash_func(in) % backets.size();
    }

    typename ListType::iterator insert_without_find(const MapType& in) {
        list_for_iterator.push_back(in);
        ++size_map;
        if (size_map >= 4 * backets.size())
            rebuild_map(4 * backets.size());
        backets[backet_number(in)].push_back(--list_for_iterator.end());
        return --list_for_iterator.end();
    }

    void rebuild_map(size_t new_size_backet) {
        auto copy_list = list_for_iterator;
        backets.clear();
        backets.resize(new_size_backet);
        list_for_iterator.clear();
        size_map = 0;
        for (const auto& it : copy_list) {
            insert(it);
        }
    }

 public:

    using iterator = typename ListType::iterator;
    using const_iterator = typename ListType::const_iterator;

    HashMap(const Hash& Hash_func = Hash()) : 
        main_hash_func(Hash_func), size_map(0) {
        rebuild_map(start_size_backet);
    }

    template<class It>
    HashMap(It it_begin, It it_end, const Hash& Hash_func = Hash()) : 
        main_hash_func(Hash_func), size_map(0) {
        rebuild_map(start_size_backet);
        for (;it_begin != it_end; ++it_begin) {
            insert(*it_begin);
        }
    }

    HashMap(const std::initializer_list<MapType>& in, 
            const Hash& Hash_func = Hash()) : 
        main_hash_func(Hash_func), size_map(0) {
        rebuild_map(start_size_backet);
        for (const auto& item : in) {
            insert(item);
        }
    }

    HashMap& operator=(const HashMap& other) {
        if(this == &other) {
            return (*this);
        }
        main_hash_func = other.hash_function();
        list_for_iterator.clear();
        size_map = 0;
        rebuild_map(other.backets.size());
        for (const auto& it : other) {
            insert_without_find(it);
        }
        return (*this);
    }

    size_t size() const {
        return size_map;
    }

    bool empty() const {
        return size_map == 0;
    }

    const Hash& hash_function() const {
        return main_hash_func;
    }

    void insert(const MapType& in) {
        if (find(in.first) == end())
            insert_without_find(in);
    }

    void erase(const KeyType& in) {
        iterator it_element = find(in);
        size_t id = backet_number(in);
        if (it_element != end()) {
            for (auto it_list = backets[id].begin();
                it_list != backets[id].end(); ++it_list) {
                if ((*it_list) == it_element) {
                    backets[id].erase(it_list);
                    break;
                }
            }
            list_for_iterator.erase(it_element);
            --size_map;
        }
    }

    iterator begin() {
        return list_for_iterator.begin();
    }

    const_iterator begin() const {
        return list_for_iterator.cbegin();
    }

    iterator end() {
        return list_for_iterator.end();
    }

    const_iterator end() const {
        return list_for_iterator.cend();
    }

    iterator find(const KeyType& in) {
        size_t id = backet_number(in);
        iterator out_it = list_for_iterator.end();
        for (auto it : backets[id]) {
            if (it->first == in) {
                out_it = it;
            }
        }
        return out_it;
    }

    const_iterator find(const KeyType& in) const {
        size_t id = backet_number(in);
        const_iterator out_it = list_for_iterator.cend();
        for (auto it : backets[id]) {
            if (it->first == in) {
                out_it = it;
            }
        }
        return out_it;
    }

    ValueType& operator[](const KeyType& in) {
        iterator it = find(in);
        if (it == list_for_iterator.end())
            return insert_without_find({in, ValueType()})->second;
        return it->second;
    }

    const ValueType& at(const KeyType& in) const {
        const_iterator it = find(in);
        if (it == end())
            throw std::out_of_range("");
        else
           return it->second; 
    }

    void clear() {
        list_for_iterator.clear();
        size_map = 0;
        rebuild_map(start_size_backet);
    }
};

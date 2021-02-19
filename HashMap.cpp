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
    typedef std::list<typename ListType::iterator> ItList;

    ListType ListForIterator;
    std::vector<ItList> Buckets;
    Hash hasher;
    size_t current_size;

    constexpr static size_t START_SIZE_BUCKET = 2;
    constexpr static size_t MAX_AVERAGE_SIZE_BUCKET = 4;
    constexpr static size_t MULTIPLIER_NEW_NUMBER_BUCKETS = 4;

    size_t bucket_number(const MapType& in) const {
        return hasher(in.first) % Buckets.size();
    }

    size_t bucket_number(const KeyType& in) const {
        return hasher(in) % Buckets.size();
    }

    typename ListType::iterator insert_without_find(const MapType& in) {
        ListForIterator.push_back(in);
        ++current_size;
        if (current_size >= MAX_AVERAGE_SIZE_BUCKET * Buckets.size()) {
            rebuild_map(MULTIPLIER_NEW_NUMBER_BUCKETS * Buckets.size());
        }
        Buckets[bucket_number(in)].push_back(prev(ListForIterator.end()));
        return prev(ListForIterator.end());
    }

    void rebuild_map(size_t new_size_bucket) {
        ListType copy_list = ListForIterator;
        Buckets.clear();
        Buckets.resize(new_size_bucket);
        ListForIterator.clear();
        current_size = 0;
        for (const auto& it : copy_list) {
            insert(it);
        }
    }

 public:

    using iterator = typename ListType::iterator;
    using const_iterator = typename ListType::const_iterator;

    HashMap(const Hash& hash_ctor = Hash()) : 
        hasher(hash_ctor), current_size(0) {
        rebuild_map(START_SIZE_BUCKET);
    }

    template<class It>
    HashMap(It it_begin, It it_end, const Hash& hash_ctor = Hash()) : 
        hasher(hash_ctor), current_size(0) {
        rebuild_map(START_SIZE_BUCKET);
        for (; it_begin != it_end; ++it_begin) {
            insert(*it_begin);
        }
    }

    HashMap(const std::initializer_list<MapType>& in, 
            const Hash& hash_ctor = Hash()) : 
        hasher(hash_ctor), current_size(0) {
        rebuild_map(START_SIZE_BUCKET);
        for (const auto& item : in) {
            insert(item);
        }
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return (*this);
        }
        hasher = other.hash_function();
        ListForIterator.clear();
        current_size = 0;
        rebuild_map(other.Buckets.size());
        for (const auto& it : other) {
            insert_without_find(it);
        }
        return (*this);
    }

    size_t size() const {
        return current_size;
    }

    bool empty() const {
        return current_size == 0;
    }

    const Hash& hash_function() const {
        return hasher;
    }

    void insert(const MapType& in) {
        if (find(in.first) == end())
            insert_without_find(in);
    }

    void erase(const KeyType& in) {
        iterator it_element = find(in);
        size_t id = bucket_number(in);
        if (it_element != end()) {
            for (auto it_list = Buckets[id].begin();
                it_list != Buckets[id].end(); ++it_list) {
                if ((*it_list) == it_element) {
                    Buckets[id].erase(it_list);
                    break;
                }
            }
            ListForIterator.erase(it_element);
            --current_size;
        }
    }

    iterator begin() {
        return ListForIterator.begin();
    }

    const_iterator begin() const {
        return ListForIterator.cbegin();
    }

    iterator end() {
        return ListForIterator.end();
    }

    const_iterator end() const {
        return ListForIterator.cend();
    }

    iterator find(const KeyType& in) {
        size_t id = bucket_number(in);
        iterator out_it = ListForIterator.end();
        for (auto it : Buckets[id]) {
            if (it->first == in) {
                out_it = it;
            }
        }
        return out_it;
    }

    const_iterator find(const KeyType& in) const {
        size_t id = bucket_number(in);
        const_iterator out_it = ListForIterator.cend();
        for (auto it : Buckets[id]) {
            if (it->first == in) {
                out_it = it;
            }
        }
        return out_it;
    }

    ValueType& operator[](const KeyType& in) {
        iterator it = find(in);
        if (it == ListForIterator.end())
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
        ListForIterator.clear();
        current_size = 0;
        rebuild_map(START_SIZE_BUCKET);
    }
};

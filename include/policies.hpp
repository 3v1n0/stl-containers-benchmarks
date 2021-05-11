//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <type_traits>

// create policies

//Create empty container

template<class Container>
struct Empty {
    inline static Container make(std::size_t) {
        return Container();
    }
    inline static void clean(){}
};

template<class Container>
constexpr bool is_list() {
    return std::is_same_v<Container, std::list<typename Container::value_type>>;
}

template<class Container>
constexpr bool is_forward_list() {
    return std::is_same_v<Container, std::forward_list<typename Container::value_type>>;
}

template<class Container>
constexpr bool has_random_insert() {
    return !is_forward_list<Container>();
}

template<class Container>
constexpr bool has_native_size() {
    return !is_forward_list<Container>();
}

template<class Container>
constexpr bool can_push_back() {
    return !is_forward_list<Container>();
}

template<class Container, typename T>
constexpr void container_push_value(Container& container, T const& value) {
    if constexpr (!has_random_insert<Container>())
        container.push_front({value});
    else
        container.push_back({value});
}

template<class Container, typename T>
constexpr void container_push_value_fastest(Container& container, T const& value) {
    if constexpr (is_forward_list<Container>() || is_list<Container>())
        container.push_front({value});
    else
        container.push_back({value});
}

template<class Container, template <class T> class BaseTest>
struct Shrink : BaseTest<Container> {
    inline static void run(Container &c, std::size_t size){
        BaseTest<Container>::run(c, size);
        c.shrink_to_fit();
    }
};

//Prepare data for fill back
template<class Container>
struct EmptyPrepareBackup {
    static std::vector<typename Container::value_type> v;
    inline static Container make(std::size_t size) {
        bool size_changed = true;
        if constexpr (has_native_size<Container>())
            size_changed = v.size() != size;

        if(size_changed){
            v.clear();
            v.reserve(size);
            for(std::size_t i = 0; i < size; ++i){
                container_push_value(v, i);
            }
        }

        return Container();
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> EmptyPrepareBackup<Container>::v;

template<class Container>
struct Filled {
    inline static Container make(std::size_t size) {
        return Container(size);
    }
    inline static void clean(){}
};

template<class Container>
struct FilledRandom {
    static std::vector<typename Container::value_type> v;
    inline static Container make(std::size_t size){
        // prepare randomized data that will have all the integers from the range
        bool size_changed = true;
        if constexpr (has_native_size<Container>())
            size_changed = v.size() != size;

        if(size_changed){
            v.clear();
            v.reserve(size);
            for(std::size_t i = 0; i < size; ++i){
                container_push_value(v, i);
            }
            std::shuffle(begin(v), end(v), std::mt19937());
        }

        // fill with randomized data
        Container container;
        for(std::size_t i = 0; i < size; ++i){
            container_push_value(container, v[i]);
        }

        return container;
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> FilledRandom<Container>::v;

template<class Container>
struct FilledSequential {
    static std::vector<typename Container::value_type> v;
    inline static Container make(std::size_t size){
        Container container;
        for(std::size_t i = 0; i < size; ++i){
            container_push_value(container, i);
        }

        return container;
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
struct FilledRandomInsert {
    static std::vector<typename Container::value_type> v;
    inline static Container make(std::size_t size){
        Container container;
        if constexpr (has_random_insert<Container>()) {
            // prepare randomized data that will have all the integers from the range
            if(v.size() != size){
                v.clear();
                v.reserve(size);
                for(std::size_t i = 0; i < size; ++i){
                    container_push_value(v, i);
                }
                std::shuffle(begin(v), end(v), std::mt19937());
            }

            // fill with randomized data
            for(std::size_t i = 0; i < size; ++i){
                container.insert(v[i]);
            }
        }

        return container;
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> FilledRandomInsert<Container>::v;

template<class Container>
struct SmartFilled {
    inline static std::unique_ptr<Container> make(std::size_t size){
        return std::unique_ptr<Container>(new Container(size, typename Container::value_type()));
    }

    inline static void clean(){}
};

template<class Container>
struct BackupSmartFilled {
    static std::vector<typename Container::value_type> v;
    inline static std::unique_ptr<Container> make(std::size_t size){
        bool size_changed = true;
        if constexpr (has_native_size<Container>())
            size_changed = v.size() != size;

        if(size_changed){
            v.clear();
            v.reserve(size);
            for(std::size_t i = 0; i < size; ++i){
                container_push_value(v, i);
            }
        }

        std::unique_ptr<Container> container(new Container());

        for(std::size_t i = 0; i < size; ++i){
            container_push_value(*container, v[i]);
        }

        return container;
    }

    inline static void clean(){
        v.clear();
        v.shrink_to_fit();
    }
};

template<class Container>
std::vector<typename Container::value_type> BackupSmartFilled<Container>::v;

// testing policies

template<class Container>
struct NoOp {
    inline static void run(Container &, std::size_t) {
        //Nothing
    }
};

template<class Container>
struct ReserveSize {
    inline static void run(Container &c, std::size_t size){
        c.reserve(size);
    }
};

template<class Container>
struct InsertSimple {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        if constexpr (has_random_insert<Container>()) {
            for(size_t i=0; i<size; ++i){
                c.insert(value);
            }
        }
    }
};

template<class Container>
const typename Container::value_type InsertSimple<Container>::value{};

template<class Container>
struct FillBack {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        if constexpr (can_push_back<Container>()) {
            for(size_t i=0; i<size; ++i){
                container_push_value(c, value);
            }
        }
    }
};

template <class Container>
const typename Container::value_type FillBack<Container>::value{};

template<class Container>
struct FillBackBackup {
    inline static void run(Container &c, std::size_t size){
        if constexpr (can_push_back<Container>()) {
            for(size_t i=0; i<size; ++i){
                container_push_value(c, EmptyPrepareBackup<Container>::v[i]);
            }
        }
    }
};

template<class Container>
struct FastestAddition {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            container_push_value_fastest(c, value);
        }
    }
};

template <class Container>
const typename Container::value_type FastestAddition<Container>::value{};

template<class Container>
struct FastestAdditionBackup {
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            container_push_value_fastest(c, EmptyPrepareBackup<Container>::v[i]);
        }
    }
};

template<class Container>
struct FillBackInserter {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        std::fill_n(std::back_inserter(c), size, value);
    }
};

template<class Container> const typename Container::value_type FillBackInserter<Container>::value{};

template<class Container>
struct EmplaceBack {
    inline static void run(Container &c, std::size_t size){
        if constexpr (can_push_back<Container>()) {
            for(size_t i=0; i<size; ++i){
                c.emplace_back();
            }
        }
    }
};

template<class Container>
struct EmplaceInsertSimple {
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.emplace();
        }
    }
};

template<class Container>
struct FillFront {
    static const typename Container::value_type value;
    inline static void run(Container &c, std::size_t size){
        std::fill_n(std::front_inserter(c), size, value);
    }
};

template<class Container> const typename Container::value_type FillFront<Container>::value{};

template<class T>
struct FillFront<std::vector<T> > {
    static const T value;
    inline static void run(std::vector<T> &c, std::size_t size){
        for(std::size_t i=0; i<size; ++i){
            c.insert(begin(c), value);
        }
    }
};

template<class T> const T FillFront<std::vector<T> >::value{};

template<class Container>
struct EmplaceFront {
    inline static void run(Container &c, std::size_t size){
        for(size_t i=0; i<size; ++i){
            c.emplace_front();
        }
    }
};

template<class T>
struct EmplaceFront<std::vector<T> > {
    inline static void run(std::vector<T> &c, std::size_t size){
        for(std::size_t i=0; i<size; ++i){
            c.emplace(begin(c));
        }
    }
};

template<class Container>
struct Find {
    static size_t X;
    inline static void run(Container &c, std::size_t size){
        for(std::size_t i=0; i<size; ++i) {
            // hand written comparison to eliminate temporary object creation
            if(std::find_if(std::begin(c), std::end(c), [&](decltype(*std::begin(c)) v){ return v.a == i; }) == std::end(c)){
                ++X;
            }
        }
    }
};

template<class Container>
size_t Find<Container>::X = 0;

template<class Container>
struct Insert {
    static std::array<typename Container::value_type, 1000> values;
    inline static void run(Container &c, std::size_t){
        if constexpr (has_random_insert<Container>()) {
            for(std::size_t i=0; i<1000; ++i) {
                // hand written comparison to eliminate temporary object creation
                auto it = std::find_if(std::begin(c), std::end(c), [&](decltype(*std::begin(c)) v){ return v.a == i; });
                c.insert(it, values[i]);
            }
        }
    }
};

template<class Container>
std::array<typename Container::value_type, 1000> Insert<Container>::values {};

template<class Container>
struct Write {
    inline static void run(Container &c, std::size_t){
        auto it = std::begin(c);
        auto end = std::end(c);

        for(; it != end; ++it){
            ++(it->a);
        }
    }
};

template<class Container>
struct Iterate {
    inline static void run(Container &c, std::size_t){
        auto it = std::begin(c);
        auto end = std::end(c);

        while(it != end){
            auto val = *it;
            (void)val;
            ++it;
        }
    }
};

template<class Container>
struct IterateAndClear : Iterate<Container> {
    inline static void run(Container &c, std::size_t size){
        Iterate<Container>::run(c, size);
        c.clear();
    }
};

template<class Container> using IterateAndClearShrink = Shrink<Container, IterateAndClear>;

template<class Container>
struct Erase {
    inline static void run(Container &c, std::size_t){
        for(std::size_t i=0; i<1000; ++i) {
            // hand written comparison to eliminate temporary object creation
            if constexpr (!has_random_insert<Container>())
                c.remove_if([&](decltype(*begin(c)) v){ return v.a == i; });
            else
                c.erase(std::find_if(std::begin(c), std::end(c), [&](decltype(*std::begin(c)) v){ return v.a == i; }));
        }
    }
};

template<class Container> using EraseShrink = Shrink<Container, Erase>;

template<class Container>
struct RemoveErase {
    inline static void run(Container &c, std::size_t){
        // hand written comparison to eliminate temporary object creation
        if constexpr (!has_random_insert<Container>())
            c.remove_if([&](decltype(*begin(c)) v){ return v.a < 1000; });
        else
            c.erase(std::remove_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a < 1000; }), end(c));
    }
};

template<class Container> using RemoveEraseShrink = Shrink<Container, RemoveErase>;

template<class Container>
struct EraseFront {
    inline static void run(Container &c, std::size_t){
        decltype(c.begin()) it;
        if constexpr (is_forward_list<Container>())
            it = c.before_begin();
        else
            it = c.begin();

        if constexpr (is_forward_list<Container>())
            c.erase_after(it);
        else
            c.erase(it);
    }
};

template<class Container> using EraseFrontShrink = Shrink<Container, EraseFront>;

template<class Container>
struct SwapAndErase {
    template<typename T>
    inline static void run(Container &c, T it) {
        std::swap(*it, c.back());
        c.pop_back();
    }
};

template<class Container>
struct EraseFrontSwap : SwapAndErase<Container> {
    inline static void run(Container &c, std::size_t) {
        SwapAndErase<Container>::run(c, c.begin());
    }
};

template<class Container> using EraseFrontSwapShrink = Shrink<Container, EraseFrontSwap>;

template<class Container>
struct EraseMiddle {
    inline static void run(Container &c, std::size_t size){
        decltype(c.begin()) it;
        if constexpr (is_forward_list<Container>())
            it = c.before_begin();
        else
            it = c.begin();

        std::advance(it, size/2);

        if constexpr (is_forward_list<Container>())
            c.erase_after(it);
        else
            c.erase(it);
    }
};

template<class Container> using EraseMiddleShrink = Shrink<Container, EraseMiddle>;

template<class Container>
struct EraseMiddleSwap : SwapAndErase<Container> {
    inline static void run(Container &c, std::size_t size){
        auto it = c.begin();
        std::advance(it, size/2);
        SwapAndErase<Container>::run(c, it);
    }
};

template<class Container> using EraseMiddleSwapShrink = Shrink<Container, EraseMiddleSwap>;

template<class Container>
struct EraseBack {
    inline static void run(Container &c, std::size_t){
        decltype(c.begin()) it;
        if constexpr (is_forward_list<Container>()) {
            auto it_before = c.before_begin();
            auto it_next = c.begin();
            for (++it_next; it_next != c.end(); ++it_next) {
                it_before++;
            }

            c.erase_after(it_before);
        }
        else {
            c.pop_back();
        }
    }
};

template<class Container> using EraseBackShrink = Shrink<Container, EraseBack>;

template<class Container>
struct EraseBackSwap : SwapAndErase<Container> {
    inline static void run(Container &c, std::size_t){
        SwapAndErase<Container>::run(c, c.rbegin());
    }
};

template<class Container> using EraseBackSwapShrink = Shrink<Container, EraseBackSwap>;

//Sort the container

template<class Container>
struct Sort {
    inline static void run(Container &c, std::size_t){
        std::sort(c.begin(), c.end());
    }
};

template<class T>
struct Sort<std::list<T> > {
    inline static void run(std::list<T> &c, std::size_t){
        c.sort();
    }
};

template<class Container>
struct TimSort {
    inline static void run(Container &c, std::size_t){
        c.timsort();
    }
};

//Reverse the container

template<class Container>
struct Reverse {
    inline static void run(Container &c, std::size_t){
        std::reverse(c.begin(), c.end());
    }
};

template<class T>
struct Reverse<std::list<T> > {
    inline static void run(std::list<T> &c, std::size_t){
        c.reverse();
    }
};

//Destroy the container

template<class Container>
struct SmartDelete {
    inline static void run(Container &c, std::size_t) { c.reset(); }
};

template<class Container>
struct RandomSortedInsert {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t size){
        if constexpr (has_random_insert<Container>()) {
            for(std::size_t i=0; i<size; ++i){
                auto val = distribution(generator);
                // hand written comparison to eliminate temporary object creation
                c.insert(std::find_if(begin(c), end(c), [&](decltype(*begin(c)) v){ return v.a >= val; }), {val});
            }
        }
    }
};

template<class Container> std::mt19937 RandomSortedInsert<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomSortedInsert<Container>::distribution(0, std::numeric_limits<std::size_t>::max() - 1);

template<class Container>
struct RandomErase1 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();
        decltype(c.begin()) before;

        if constexpr (is_forward_list<Container>())
            before = c.before_begin();

        while(it != c.end()){
            if(distribution(generator) > 9900){
                if constexpr (is_forward_list<Container>())
                    it = c.erase_after(before);
                else
                    it = c.erase(it);
            } else {
                before = it;
                ++it;
            }
        }

        if constexpr (has_native_size<Container>())
            std::cout << c.size() << std::endl;
    }
};

template<class Container> std::mt19937 RandomErase1<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase1<Container>::distribution(0, 10000);
template<class Container> using RandomErase1Shrink = Shrink<Container, RandomErase1>;

template<class Container>
struct RandomErase10 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();
        decltype(c.begin()) before;

        if constexpr (is_forward_list<Container>())
            before = c.before_begin();

        while(it != c.end()){
            if(distribution(generator) > 9000){
                if constexpr (is_forward_list<Container>())
                    it = c.erase_after(before);
                else
                    it = c.erase(it);
            } else {
                before = it;
                ++it;
            }
        }
    }
};

template<class Container> std::mt19937 RandomErase10<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase10<Container>::distribution(0, 10000);
template<class Container> using RandomErase10Shrink = Shrink<Container, RandomErase10>;

template<class Container>
struct RandomErase25 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();
        decltype(c.begin()) before;

        if constexpr (is_forward_list<Container>())
            before = c.before_begin();

        while(it != c.end()){
            if(distribution(generator) > 7500){
                if constexpr (is_forward_list<Container>())
                    it = c.erase_after(before);
                else
                    it = c.erase(it);
            } else {
                before = it;
                ++it;
            }
        }
    }
};

template<class Container> std::mt19937 RandomErase25<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase25<Container>::distribution(0, 10000);

template<class Container>
struct RandomErase50 {
    static std::mt19937 generator;
    static std::uniform_int_distribution<std::size_t> distribution;

    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();
        decltype(c.begin()) before;

        if constexpr (is_forward_list<Container>())
            before = c.before_begin();

        while(it != c.end()){
            if(distribution(generator) > 5000){
                if constexpr (is_forward_list<Container>())
                    it = c.erase_after(before);
                else
                    it = c.erase(it);
            } else {
                before = it;
                ++it;
            }
        }
    }
};

template<class Container> std::mt19937 RandomErase50<Container>::generator;
template<class Container> std::uniform_int_distribution<std::size_t> RandomErase50<Container>::distribution(0, 10000);

template<class Container>
struct FullErase {
    inline static void run(Container &c, std::size_t /*size*/){
        auto it = c.begin();
        decltype(c.begin()) before;

        if constexpr (is_forward_list<Container>())
            before = c.before_begin();

        while(it != c.end()){
            if constexpr (is_forward_list<Container>())
                it = c.erase_after(before);
            else
                it = c.erase(it);
        }
    }
};

template<class Container> using FullEraseShrink = Shrink<Container, FullErase>;

// Note: This is probably erased completely for a vector
template<class Container>
struct Traversal {
    inline static void run(Container &c, std::size_t size){
        auto it = c.begin();
        auto end = c.end();

        while(it != end){
            ++it;
        }
    }
};

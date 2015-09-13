#ifndef COUNTER_HPP_IROCZG2M
#define COUNTER_HPP_IROCZG2M
#include <map>
#include <algorithm>
#include <boost/serialization/map.hpp>

template <class E, class C>
class Counter {
public:
    void add(const E& element, const C& value) {
        if (_counter.find(element) == _counter.end()) {
            _counter[element] = value;
        } else {
            _counter[element] += value;
        }
    }

    void add_to_all(const C& value) {
        for(auto& it: _counter) {
            it.second += value;
        }
    }

    C get(const E& element) const {
        return _counter.at(element);
    }

    bool contains(const E& element) const {
        return _counter.find(element) != _counter.end();
    }

    C get_or_else(const E& element, const C& default_value) const {
        if (_counter.find(element) == _counter.end()) {
            return default_value;
        } else {
            return this->get(element);
        }
    }

    C sum() const {
        C sum = 0;
        for(const auto& it: _counter) {
            sum += it.second;
        }
        return sum;
    }

    size_t size() const {
        return _counter.size();
    }

    void clear() {
        _counter.clear();
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(_counter);
    }

    std::pair<E, C> max_element() const {
        auto max = std::max_element(_counter.begin(), _counter.end(),
                [](const std::pair<E, C>& p1, const std::pair<E, C>& p2) {
                    return p1.second < p2.second;
                });
        return std::make_pair(max->first, max->second);
    }

public:
    std::map<E, C> _counter;
};


template <class E1, class E2, class C>
class Counter2 {
public:
    void add(const E1& element1, const E2& element2, const C& value) {
        if (_counter2.find(element1) == _counter2.end()) {
            Counter<E2, C> counter;
            _counter2[element1] = counter;
        }
        _counter2[element1].add(element2, value);
    }

    C get(const E1& element1, const E2& element2) const {
        return _counter2.at(element1).get(element2);
    }

    C get_or_else(const E1& element1, const E2& element2, const C& default_value) const {
        if (_counter2.find(element1) == _counter2.end()) {
            return default_value;
        } else {
            const Counter<E2, C>& counter = _counter2.at(element1);
            return counter.get_or_else(element2, default_value);
        }
    }

    Counter<E2, C> get(const E1& element1) const {
        if (_counter2.find(element1) == _counter2.end()) {
            return Counter<E2, C>();
        } else {
            return _counter2.at(element1);
        }
    }

    size_t size() const {
        return _counter2.size();
    }

    size_t all_size() const {
        size_t sum = 0;
        for (const auto& it: _counter2) {
            sum += it.second.size();
        }
        return sum;
    }

    size_t all_sum() const {
        C sum = 0;
        for (const auto& it: _counter2) {
            sum += it.second.sum();
        }
        return sum;
    }

    void clear() {
        _counter2.clear();
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(_counter2);
    }

public:
    std::map<E1,Counter<E2,C>> _counter2;
};

#endif /* end of include guard: COUNTER_HPP_IROCZG2M */

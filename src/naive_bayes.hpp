// Implementation translated from Nak
// https://github.com/scalanlp/nak/tree/master/src/main/scala/nak
#ifndef NAIVE_BAYES_HPP_YCLH5EBR
#define NAIVE_BAYES_HPP_YCLH5EBR
#include <vector>
#include <cmath>
#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include "example.hpp"
#include "counter.hpp"


class NaiveBayes {
public:
    NaiveBayes(double word_smoothing=0.05, double class_smoothing=0.01) :
        _word_smoothing(word_smoothing),
        _class_smoothing(class_smoothing) {}

    void fit(const std::vector<Example>& examples) {
        clear();
        for (const auto& example: examples) {
            const auto label = example._label;
            _class_counter.add(label, 1);
            for (const auto& word: example._words) {
                _word_counter.add(label, word, 1);
                _all_words.insert(word);
            }
        }
        _class_counter.add_to_all(_class_smoothing);

        for(const auto& it: _word_counter._counter2) {
            _word_totals.add(it.first, it.second.sum());
        }
        _vocab_size = _all_words.size();
    }

    Counter<std::string, double> scores(const Example& example) {
        Counter<std::string, double> res;
        bool is_any_words_matched = false;
        for (const auto& cls: _class_counter._counter) {
            const auto& label = cls.first;
            double prior = static_cast<double>(cls.second);
            res.add(label, log(prior + _class_smoothing));
            double log_denom = log(_word_totals.get(label) + _vocab_size * _word_smoothing);
            double log_prob = 0.0;
            auto prob_wc = _word_counter.get(label);
            for (const auto& word: example._words) {
                if (prob_wc.contains(word)) {
                    is_any_words_matched = true;
                }
                log_prob += log(prob_wc.get_or_else(word, 0) + _word_smoothing) - log_denom;
            }
            res.add(label, log_prob);
        }
        if (is_any_words_matched) {
            return res;
        } else {
            return Counter<std::string, double>();
        }
    }

    // A simple algorithm with majority voting
    Counter<std::string, double> fast_scores(const Example& example) {
        Counter<std::string, double> res;
        for (const auto& it: _word_counter._counter2) {
            const auto& label = it.first;
            const auto& prob_wc = it.second;
            for (const auto& word: example._words) {
                if (prob_wc.contains(word)) {
                    res.add(label, 1);
                }
            }
        }
        return res;
    }

    static void normalize_scores_inplace(Counter<std::string, double>& counter) {
        double sum = 0.0;
        for (auto& it: counter._counter) {
            it.second = exp(it.second);
            sum += it.second;
        }
        for (auto& it: counter._counter) {
            it.second /= sum;
        }
    }

    Counter<std::string, double> normalize_scores(
            const Counter<std::string, double>& counter) {
        Counter<std::string, double> copy;
        for (const auto& it: counter._counter) {
            copy._counter[it.first] = it.second;
        }
        normalize_scores_inplace(copy);
        return copy;
    }

    void clear() {
        _class_counter.clear();
        _word_counter.clear();
        _all_words.clear();
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & _word_smoothing;
        ar & _class_smoothing;
        ar & _class_counter;
        ar & _word_counter;
        ar & _word_totals;
        ar & _all_words;
        ar & _vocab_size;
    }

private:
    double _word_smoothing;
    double _class_smoothing;
    Counter<std::string, double> _class_counter;
    Counter2<std::string, std::string, int> _word_counter;
    Counter<std::string, int> _word_totals;
    std::set<std::string> _all_words;
    int _vocab_size;
};

#endif /* end of include guard: NAIVE_BAYES_HPP_YCLH5EBR */

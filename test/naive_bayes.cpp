#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <vector>
#include "naive_bayes.hpp"
#include "example.hpp"

void print_scores(Counter<std::string, double> scores) {
    for (const auto& it: scores._counter) {
        std::cout<<it.first<<" ";
        std::cout<<it.second<<std::endl;
    }
    std::cout<<"---------"<<std::endl;
}


TEST(naive_bayes, example) {
    NaiveBayes bayes;
    std::vector<std::string> words1{"hello", "world", "world"};
    std::vector<std::string> words2{"fuck", "world", "world"};
    std::vector<Example> examples = {
        Example(words1, "positive"),
        Example(words2, "negative")
    };
    bayes.fit(examples);
    print_scores(bayes.scores(examples[0]));
    print_scores(bayes.scores(examples[1]));
}

TEST(naive_bayes, nomatching_words_example) {
    // When nothing in dict matches, we want an empty map returned,
    // rather than smoothed class distributions.
    NaiveBayes bayes;
    std::vector<std::string> words1{"hello", "world"};
    std::vector<std::string> words2{"good", "morning"};
    std::vector<Example> examples = {
        Example(words1, "positive"),
        Example(words2, "negative")
    };
    bayes.fit(examples);

    std::vector<std::string> missing_words{"escape", "notfound"};
    Example missing_example(missing_words, "positive");
    auto scores = bayes.scores(missing_example);
    EXPECT_EQ(0, scores.size());
    EXPECT_EQ(0, scores.sum());
}

TEST(naive_bayes, fast_scores) {
    NaiveBayes bayes;
    std::vector<std::string> words1{"hello", "world"};
    std::vector<std::string> words2{"good", "morning"};
    std::vector<Example> examples = {
        Example(words1, "positive"),
        Example(words2, "negative")
    };
    bayes.fit(examples);

    auto scores1 = bayes.fast_scores(examples[0]);
    EXPECT_EQ(1, scores1.size());
    EXPECT_EQ(2, scores1.get("positive"));

    auto scores2 = bayes.fast_scores(examples[1]);
    EXPECT_EQ(1, scores2.size());
    EXPECT_EQ(2, scores2.get("negative"));

    std::vector<std::string> missing_words{"escape", "notfound"};
    Example missing_example(missing_words, "positive");
    auto missing_scores = bayes.fast_scores(missing_example);
    EXPECT_EQ(0, missing_scores.size());
    EXPECT_EQ(0, missing_scores.sum());
}

class TempFile {
public:
    TempFile() {
        filename = tmpnam(NULL);
    }
    ~TempFile() {
        remove(filename);
    }
public:
    const char* filename;
};

TEST(naive_bayes, serialization) {
    NaiveBayes bayes;
    std::vector<std::string> words1{"hello", "world", "world"};
    std::vector<std::string> words2{"fuck", "world", "world"};
    std::vector<Example> examples = {
        Example(words1, "positive"),
        Example(words2, "negative")
    };
    bayes.fit(examples);
    auto scores = bayes.scores(examples[0]);

    TempFile tmp_file;
    std::ofstream ofs(tmp_file.filename);
    boost::archive::text_oarchive oa(ofs);
    oa << bayes;
    ofs.close();

    NaiveBayes bayes_restore;
    std::ifstream ifs(tmp_file.filename);
    boost::archive::text_iarchive ia(ifs);
    ia >> bayes_restore;

    auto scores_restore = bayes_restore.scores(examples[0]);

    EXPECT_EQ(scores.get("positive"), scores_restore.get("positive"));
    EXPECT_EQ(scores.get("negative"), scores_restore.get("negative"));
}

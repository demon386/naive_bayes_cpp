#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "counter.hpp"

TEST(counter, counter_example) {
    Counter<std::string, int> counter;
    counter.add("Hello", 3);
    counter.add("World", 4);
    counter.add("World", 4);
    EXPECT_EQ(3, counter.get("Hello"));
    EXPECT_EQ(8, counter.get("World"));
    EXPECT_EQ(3, counter.get_or_else("xxx", 3));
    EXPECT_EQ(11, counter.sum());
    EXPECT_EQ(2, counter.size());

    counter.add_to_all(10);
    EXPECT_EQ(11 + 10 + 10, counter.sum());

    counter.clear();
    EXPECT_EQ(0, counter.sum());
    EXPECT_EQ(0, counter.size());
}

TEST(counter, counter2_example) {
    Counter2<std::string, std::string, int> counter2;
    counter2.add("Hello", "World", 3);
    EXPECT_EQ(3, counter2.get_or_else("Hello", "World", 0));
    EXPECT_EQ(0, counter2.get_or_else("Hello", "xxx", 0));

    counter2.add("Hello", "Amazing", 5);
    EXPECT_EQ(8, counter2.get("Hello").sum());
    EXPECT_EQ(0, counter2.get("magic").sum());

    EXPECT_EQ(1, counter2.size());
    EXPECT_EQ(2, counter2.all_size());
    EXPECT_EQ(8, counter2.all_sum());

    counter2.clear();
    EXPECT_EQ(0, counter2.all_sum());
    EXPECT_EQ(0, counter2.all_size());
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

TEST(counter, serialization) {
    Counter<std::string, int> counter;
    counter.add("hello", 10);

    TempFile tmp_file;
    std::ofstream ofs(tmp_file.filename);
    boost::archive::text_oarchive oa(ofs);
    oa << counter;
    ofs.close();

    Counter<std::string, int> counter_restore;
    std::ifstream ifs(tmp_file.filename);
    boost::archive::text_iarchive ia(ifs);
    ia >> counter_restore;
    EXPECT_EQ(10, counter_restore.get("hello"));
}

TEST(counter2, serialization) {
    Counter2<std::string, std::string, int> counter2;
    counter2.add("hello", "world", 10);

    TempFile tmp_file;
    std::ofstream ofs(tmp_file.filename);
    boost::archive::text_oarchive oa(ofs);
    oa << counter2;
    ofs.close();

    Counter2<std::string, std::string, int> counter2_restore;
    std::ifstream ifs(tmp_file.filename);
    boost::archive::text_iarchive ia(ifs);
    ia >> counter2_restore;
    EXPECT_EQ(10, counter2_restore.get("hello", "world"));
}

TEST(counter, max_element) {
    Counter<std::string, int> counter;
    counter.add("hello", 2);
    counter.add("world", 10);
    counter.add("test", 1);

    auto max = counter.max_element();
    EXPECT_EQ("world", max.first);
    EXPECT_EQ(10, max.second);
}

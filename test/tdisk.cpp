#include <gtest/gtest.h>
#include "../disk.h"

TEST(Disk, Test1)
{
    Disk d;
    d.read();

    EXPECT_EQ ( 10.0, 10.0);
}

TEST(Disk, Test2)
{
    EXPECT_EQ(10.0, 10.0);
}


class TestPrinter :  public ::testing::EmptyTestEventListener
{
    virtual void OnTestStart(const ::testing::TestInfo& info) {
	//std::cout << "+ START " << info.test_case_name() << "." << info.name() << std::endl;
    }

    virtual void OnTestEnd(const ::testing::TestInfo& info) {
	std::cout << (info.result()->Passed() ? "[    ]" : "[FAIL]")
		  << " " << info.test_case_name() << "." << info.name()
		  << std::endl;
    }

    virtual void OnTestProgramEnd(const ::testing::UnitTest& utest) {
	std::cout << "passed:" << utest.successful_test_count() << "  "
		  << "failed:" << utest.failed_test_count() << "  "
		  << "total:"  << utest.total_test_count() << std::endl;
    }
};


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();

    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new TestPrinter);
    
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>

int divison(int a, int b)
{
    return a / b;
}

TEST(testCaseDeathTest, test_div)
{
    EXPECT_DEATH(divison(1, 0), "");
}
int main(int argc, char **argv)
{  
    testing::InitGoogleTest(&argc, argv);  
    return RUN_ALL_TESTS(); 
} 

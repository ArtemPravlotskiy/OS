//
// Created by pravl on 29.03.2025.
//

#include <gtest/gtest.h>
#include <fstream>
#include "../include/employee.h"

class EmployeeTest : public ::testing::Test {
protected:
    employee emp{1, "John", 40.0};
    const char* testFile = "test_employee.bin";

    void SetUp() override {
        std::ofstream out(testFile, std::ios::binary);
        out.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
        out.close();
    }

    void TearDown() override {
        remove(testFile);
    }
};

TEST_F(EmployeeTest, EmployeeStructSize) {
    EXPECT_EQ(sizeof(employee), sizeof(int) + 10 + sizeof(double));
}

TEST_F(EmployeeTest, WriteAndReadBinary) {
    std::ifstream in(testFile, std::ios::binary);
    ASSERT_TRUE(in.is_open());

    employee readEmp{};
    in.read(reinterpret_cast<char*>(&readEmp), sizeof(employee));
    in.close();

    EXPECT_EQ(readEmp.num, emp.num);
    EXPECT_STREQ(readEmp.name, emp.name);
    EXPECT_DOUBLE_EQ(readEmp.hours, emp.hours);
}

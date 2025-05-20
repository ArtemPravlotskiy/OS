//
// Created by pravl on 20.05.2025.
//

#include <gtest/gtest.h>
#include <vector>
#include <cstdio>
#include <cstring>
#include <string>

// Повторяем определение структуры
struct employee {
    int num;
    char name[10];
    double hours;
};

// Вспомогательные функции для тестов
namespace TestHelpers {
    // Записать в файл вектор записей
    bool writeEmployees(const std::string& filename, const std::vector<employee>& v) {
        FILE* f = fopen(filename.c_str(), "wb");
        if (!f) return false;
        for (size_t i = 0; i < v.size(); ++i) {
            if (fwrite(&v[i], sizeof(employee), 1, f) != 1) {
                fclose(f);
                return false;
            }
        }
        fclose(f);
        return true;
    }

    // Прочитать из файла все записи
    bool readEmployees(const std::string& filename, std::vector<employee>& out) {
        FILE* f = fopen(filename.c_str(), "rb");
        if (!f) return false;
        out.clear();
        employee e;
        while (fread(&e, sizeof(employee), 1, f) == 1) {
            out.push_back(e);
        }
        fclose(f);
        return true;
    }

    // Обновить одну запись по индексу
    bool updateEmployee(const std::string& filename, int idx, const employee& newData) {
        FILE* f = fopen(filename.c_str(), "rb+");
        if (!f) return false;
        if (fseek(f, idx * sizeof(employee), SEEK_SET) != 0) {
            fclose(f);
            return false;
        }
        if (fwrite(&newData, sizeof(employee), 1, f) != 1) {
            fclose(f);
            return false;
        }
        fclose(f);
        return true;
    }
}

// Тесты
TEST(FileIOTest, WriteAndReadBack) {
    std::string fn = "test_employees.bin";
    // Исходные данные
    std::vector<employee> input = {
        {0, "Alice", 8.5},
        {1, "Bob",   7.0},
        {2, "Carol", 9.25}
    };
    ASSERT_TRUE(TestHelpers::writeEmployees(fn, input));

    std::vector<employee> output;
    ASSERT_TRUE(TestHelpers::readEmployees(fn, output));
    ASSERT_EQ(output.size(), input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        EXPECT_EQ(output[i].num,   input[i].num);
        EXPECT_STREQ(output[i].name, input[i].name);
        EXPECT_DOUBLE_EQ(output[i].hours, input[i].hours);
    }

    // Чистим файл
    std::remove(fn.c_str());
}

TEST(FileIOTest, UpdateSingleRecord) {
    std::string fn = "test_update.bin";
    std::vector<employee> initial = {
        {0, "X", 1.0},
        {1, "Y", 2.0},
        {2, "Z", 3.0}
    };
    ASSERT_TRUE(TestHelpers::writeEmployees(fn, initial));

    // Новые данные для записи с индексом 1
    employee modified = {1, "ZZZZ", 4.5};
    ASSERT_TRUE(TestHelpers::updateEmployee(fn, 1, modified));

    // Считаем всё заново
    std::vector<employee> output;
    ASSERT_TRUE(TestHelpers::readEmployees(fn, output));
    ASSERT_EQ(output.size(), initial.size());

    // Проверяем, что только второй элемент изменился
    EXPECT_EQ(output[0].num, initial[0].num);
    EXPECT_STREQ(output[0].name, initial[0].name);
    EXPECT_DOUBLE_EQ(output[0].hours, initial[0].hours);

    EXPECT_EQ(output[1].num, modified.num);
    EXPECT_STREQ(output[1].name, modified.name);
    EXPECT_DOUBLE_EQ(output[1].hours, modified.hours);

    EXPECT_EQ(output[2].num, initial[2].num);
    EXPECT_STREQ(output[2].name, initial[2].name);
    EXPECT_DOUBLE_EQ(output[2].hours, initial[2].hours);

    std::remove(fn.c_str());
}

TEST(FileIOTest, InvalidFileHandling) {
    std::string fn = "nonexistent_dir/test.bin";
    std::vector<employee> data = {{0, "Name",  0.0}};
    // Попытка записи в несуществующую директорию должна провалиться
    EXPECT_FALSE(TestHelpers::writeEmployees(fn, data));

    std::vector<employee> out;
    // Чтение несуществующего файла тоже должно вернуть false
    EXPECT_FALSE(TestHelpers::readEmployees(fn, out));
}

// Опционально: можно добавить тест на чтение пустого файла
TEST(FileIOTest, ReadEmptyFile) {
    std::string fn = "empty.bin";
    // Создадим пустой файл
    {
        FILE* f = fopen(fn.c_str(), "wb");
        ASSERT_NE(f, nullptr);
        fclose(f);
    }
    std::vector<employee> out;
    // Чтение пустого файла возвращает true, но вектор пустой
    EXPECT_TRUE(TestHelpers::readEmployees(fn, out));
    EXPECT_TRUE(out.empty());
    std::remove(fn.c_str());
}
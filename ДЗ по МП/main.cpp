/**
 * \file sorting_employees.cpp
 * \brief Реализация различных алгоритмов сортировки для списка сотрудников и измерение времени их выполнения.
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif

/**
 * \struct Employee
 * \brief Представляет данные сотрудника.
 */
struct Employee {
    std::string fullName;    /**< ФИО сотрудника */
    std::string department;  /**< Название отдела */
    std::string position;    /**< Должность */
    std::string hireDate;    /**< Дата поступления в формате YYYY-MM-DD */

    /**
     * \brief Сравнение сотрудников по отделу, ФИО, дате поступления и должности.
     * \param rhs Правый операнд для сравнения
     * \return true, если текущий объект меньше rhs
     */
    bool operator<(const Employee &rhs) const {
        if (department != rhs.department) return department < rhs.department;
        if (fullName   != rhs.fullName)   return fullName   < rhs.fullName;
        if (hireDate   != rhs.hireDate)   return hireDate   < rhs.hireDate;
        return position < rhs.position;
    }

    /**
     * \brief Обратная проверка оператора <
     * \param rhs Правый операнд для сравнения
     * \return true, если текущий объект больше rhs
     */
    bool operator>(const Employee &rhs) const {
        return rhs < *this;
    }

    /**
     * \brief Проверка оператора <=
     * \param rhs Правый операнд для сравнения
     * \return true, если текущий объект меньше или равен rhs
     */
    bool operator<=(const Employee &rhs) const {
        return !(*this > rhs);
    }

    /**
     * \brief Проверка оператора >=
     * \param rhs Правый операнд для сравнения
     * \return true, если текущий объект больше или равен rhs
     */
    bool operator>=(const Employee &rhs) const {
        return !(*this < rhs);
    }
};

/**
 * \brief Сортировка пузырьком.
 * \tparam T Тип элементов массива
 * \param arr Массив для сортировки
 */
template <typename T>
void bubbleSort(std::vector<T> &arr) {
    bool swapped;
    size_t n = arr.size();
    for (size_t i = 0; i + 1 < n; ++i) {
        swapped = false;
        for (size_t j = 0; j + 1 < n - i; ++j) {
            if (arr[j+1] < arr[j]) {
                std::swap(arr[j], arr[j+1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

/**
 * \brief Вспомогательная функция для пирамидальной сортировки.
 */
template <typename T>
void heapify(std::vector<T> &arr, int n, int i) {
    int largest = i;
    int left    = 2*i + 1;
    int right   = 2*i + 2;
    if (left  < n && arr[left]  > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

/**
 * \brief Пирамидальная сортировка (Heap sort).
 */
template <typename T>
void heapSort(std::vector<T> &arr) {
    int n = static_cast<int>(arr.size());
    for (int i = n/2 - 1; i >= 0; --i) heapify(arr, n, i);
    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

/**
 * \brief Вспомогательная функция для слияния двух частей массива.
 */
template <typename T>
void merge(std::vector<T> &arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<T> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) arr[k++] = (L[i] <= R[j] ? L[i++] : R[j++]);
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

/**
 * \brief Рекурсивная часть сортировки слиянием.
 */
template <typename T>
void mergeSortHelper(std::vector<T> &arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortHelper(arr, left, mid);
    mergeSortHelper(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

/**
 * \brief Сортировка слиянием.
 */
template <typename T>
void mergeSort(std::vector<T> &arr) {
    mergeSortHelper(arr, 0, static_cast<int>(arr.size() - 1));
}

/**
 * \brief Чтение данных сотрудников из CSV-файла.
 * \param filename Имя входного файла
 * \return Вектор объектов Employee
 */
std::vector<Employee> readEmployeesFromCSV(const std::string &filename) {
    std::vector<Employee> employees;
    std::ifstream file(filename);
    if (!file.is_open()) return employees;
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        Employee emp;
        std::getline(ss, emp.fullName, ',');
        std::getline(ss, emp.department, ',');
        std::getline(ss, emp.position, ',');
        std::getline(ss, emp.hireDate, ',');
        employees.push_back(emp);
    }
    return employees;
}

/**
 * \brief Запись данных сотрудников в CSV-файл.
 * \param employees Вектор сотрудников
 * \param filename Имя выходного файла
 */
void writeEmployeesToCSV(const std::vector<Employee> &employees, const std::string &filename) {
    std::ofstream file(filename);
    file << "ФИО,Отдел,Должность,Дата поступления\n";
    for (const auto &e : employees) {
        file << e.fullName << ','
             << e.department << ','
             << e.position   << ','
             << e.hireDate   << '\n';
    }
}

/**
 * \brief Точка входа программы.
 */
int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
    auto employees = readEmployeesFromCSV("employees.csv");
    if (employees.empty()) return 1;
    std::vector<int> datasetSizes = {100, 1000, 3000, 5000, 7000, 10000, 20000, 30000, 50000, 70000, 100000};
    std::ofstream timingFile("sorting_times.csv");
    timingFile << "Size,Bubble(ms),Heap(ms),Merge(ms),Std(ms)\n";

    for (int size : datasetSizes) {
        if (static_cast<int>(employees.size()) < size) break;

        std::vector<Employee> subset(employees.begin(), employees.begin() + size);

        auto t1 = std::chrono::high_resolution_clock::now();
        bubbleSort(subset);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto tb = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        subset.assign(employees.begin(), employees.begin() + size);
        t1 = std::chrono::high_resolution_clock::now();
        heapSort(subset);
        t2 = std::chrono::high_resolution_clock::now();
        auto th = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        subset.assign(employees.begin(), employees.begin() + size);
        t1 = std::chrono::high_resolution_clock::now();
        mergeSort(subset);
        t2 = std::chrono::high_resolution_clock::now();
        auto tm = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        subset.assign(employees.begin(), employees.begin() + size);
        t1 = std::chrono::high_resolution_clock::now();
        std::sort(subset.begin(), subset.end());
        t2 = std::chrono::high_resolution_clock::now();
        auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        timingFile
                << size << ','
                << tb   << ','
                << th   << ','
                << tm   << ','
                << ts   << '\n';
    }

    return 0;
}
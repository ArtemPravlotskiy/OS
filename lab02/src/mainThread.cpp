//
// Created by pravl on 12.04.2025.
//

#include <iostream>
#include <vector>
#include "processArray.cpp"

using namespace std;

int main() {
    int n;
    cout << "Enter array size: ";
    cin >> n;
    if (n <= 0) {
        cout << "Array size must be greater than 0" << endl;
        return 1;
    }

    vector<int> array(n);
    cout << "Enter array elements: ";
    for (int i = 0; i < n; ++i) {
        cin >> array[i];
    }

    vector<int> result = processArray(array);

    cout << "New array elements: ";
    for (int x : result) {
        cout << x << " ";
    }
    cout << endl;

    return 0;
}
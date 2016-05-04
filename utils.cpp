#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>

#include "utils.h"

std::string get_statistic(char *buf, int size) {
    std::stringstream ss;
    std::vector<int> digits; 

    for (int i = 0; i < size; ++i) {
        if (buf[i] >= '0' && buf[i] <= '9') {
            digits.push_back(buf[i] - '0');
        }
    }

    if (!digits.size()) {
        return std::string();
    }

    std::sort(digits.begin(), digits.end());
    ss << "Sorted: ";
    for (int &d: digits) {
        ss << d << ' ';
    }
    ss << '\n';

    ss << "Sum: " << std::accumulate(digits.begin(), digits.end(), 0) << '\n';

    ss << "Minimum: " << *digits.begin() << "\nMaximum: " << *digits.end() << '\n'; 

    return ss.str();
}

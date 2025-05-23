#include <ranges>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    using namespace ranges;
    vector<string> v = { "hello", "world" };
    string s = v | ranges::views::join_with('/') | ranges::to<string>();

    cout << s << endl;
}

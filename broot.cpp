#include <iostream>
#include <string>
#include<bits/stdc++.h>

void polecenie_O(std::string *s, int j, int k) {
    size_t len = k - j + 1;
    std::string fragment = s->substr(j - 1, len);
    std::reverse(fragment.begin(), fragment.end());
    s->replace(j - 1, len, fragment);
}
void polecenie_P(std::string *s, int j, int k, int l) {
    size_t len = k - j + 1;
    std::string fragment = s->substr(j - 1, len);
    s->replace(j - 1, k - j + 1, "");
    s->replace(l - 1, 0, fragment);
}
void polecenie_N(std::string &s, int j, int k) {
    int maximum = 1;
    int licznik = 1;
    char poprzednia = s[j - 1];
    for (int i = j; i < k; i++) {
        if (poprzednia == s[i]) {
            licznik++;
        }
        else {
            poprzednia = s[i];
            maximum = std::max(maximum, licznik);
            licznik = 1;
        }
    }
    maximum = std::max(maximum, licznik);
    std::cout << maximum << '\n';
}

int main() {
    int n, m, j, k, l;
    std::string s;
    char typ;
    std::cin >> n;
    std::cin >> m;
    std::cin >> s;

    for (int i = 0; i < m; i++) {
        std::cin >> typ;
        std::cin >> j;
        std::cin >> k;
        switch (typ) {
            case 'O':
                polecenie_O(&s, j, k);
                break;
            case 'P':
                std::cin >> l;
                polecenie_P(&s, j, k, l);
                break;
            case 'N':
                polecenie_N(s, j, k);
        }
    }
    return 0;
}
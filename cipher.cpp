#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <queue>
#include <fstream>
#include <sstream>

std::string parse_text() {
   std::ifstream t("script.txt");
   std::stringstream buffer;
   buffer << t.rdbuf();
   return buffer.str();
}

std::string encrypt_ceaser(const std::string& plain_text, const int ceaser_number) {
    std::string new_text;
    for (char ch : plain_text) {
        if (ch <= 'z' && ch >= 'a') {
            char ch_new = (ch - 'a');
            ch_new += ceaser_number;
            ch_new %= 26;
            ch_new += 'a';
            new_text += ch_new;
        }
        else {
            new_text += ch;
        }
    }
    return new_text;
}
std::string decrypt_ceaser(const std::string& cipher_text, const int ceaser_number) {
    return encrypt_ceaser(cipher_text, 26-ceaser_number);
}

float score(const std::string& str) {
    //score = 1 - std::abs(0.0849 - 0.10))
    //score_e = 1 - std::abs(0.1116 - 0.01))
    //score_total = score_a + score_e;
    float score = 0;
    std::unordered_map<char, float> char_prob = { {'a', 8.49}, {'b', 2.07}, {'c', 2.78}, {'d', 4.25}, {'e', 12.7}, {'f', 0}, {'g', 2.01}, {'h', 6.09}, {'i', 6.96}, {'j', 0.153}, {'k', 0.772}, {'l', 4.02},
                                    {'m', 2.4}, {'n', 6.74}, {'o', 7.5}, {'p', 1.92}, {'q', 0.095}, {'r', 5.98}, {'s', 6.32}, {'t', 9.05}, {'u', 2.75}, {'v', 0.978}, {'w', 2.36}, {'x', 0.15}, {'y', 1.97}, {'z', 0.074} };
    std::unordered_map<std::string, float> bi_prob = { {"th", 3.88}, {"he", 3.68}, {"in", 2.28}, {"er", 2.17}, {"an", 2.14} };
    std::unordered_map<std::string, float> tri_prob = { {"the", 3.51}, {"and", 1.59}, {"ing", 1.14} };
    for (int i = 0; i < str.length(); i++) {
        std::string t = "";
        std::string s = "";
        s.push_back(str[i]);
        if (i < str.length()) {
            s.push_back(str[i + 1]);
        }
        t.push_back(str[i]);
        if (i < str.length() - 1) {
            t.push_back(str[i + 1]);
            t.push_back(str[i + 2]);
        }
        if (tri_prob.find(t) != tri_prob.end()) {
            score += tri_prob.find(t)->second * 10000;
        }
        if (bi_prob.find(s) != bi_prob.end()) {
            score += bi_prob.find(s)->second * 1000;
        }
        if (char_prob.find(str[i]) != char_prob.end()) {
            score += char_prob.find(str[i])->second;
        }
    }

    return score;
}

void smart_ceaser(const std::string& cipher_text) {
    float highest_score = std::numeric_limits<float>::min();
    std::priority_queue<std::pair<float, int>> top_three;
    for (int itx = 0; itx < 26; itx++) {
        top_three.push({score(decrypt_ceaser(cipher_text, itx)), itx});
    }
    
    for (int i = 0; i < 5; i++) {
        std::cout << decrypt_ceaser(cipher_text, top_three.top().second) << std::endl;
        top_three.pop();
    }

    
}

std::string encrypt_alpha(const std::string& plain_text, const std::unordered_map<char, char>& key) {
    std::string cipher_text = plain_text;
    for (int i = 0; i < plain_text.size(); i++) {
        if (key.find(plain_text[i]) != key.end()) {
        cipher_text[i] = key.at(plain_text[i]);
        }
    }
    return cipher_text;
}

std::string decrypt_alpha(const std::string& cipher_text, const std::unordered_map<char, char>& key) {
    std::unordered_map<char, char> reverse;
    for (auto flip : key) {
        reverse[flip.second] = flip.first;
    }
    return encrypt_alpha(cipher_text, reverse);
}



std::unordered_map<char, float> get_frequency(const std::string& str) {
    std::unordered_map<char, float> freq;
    for (auto ch : str) {
        if (ch < 'a' || ch > 'z') continue;
        if (freq.find(ch) != freq.end()) {
            freq[ch] += 1.0;
        }
        else {
            freq.insert({ch, 1});
        }
    }
    float total_char = 0.0;
    for (const auto& kv : freq) {
        total_char += kv.second;
    }

    for (auto& kv : freq) {
        kv.second /= total_char;
        kv.second *= 100.0;
    }
    return freq;
}

std::priority_queue<std::pair<float, char>> get_sorted_counts(std::unordered_map<char, float> freq) {
    std::priority_queue<std::pair<float, char>> sorted;
    for (auto pr : freq) {
        sorted.push({pr.second, pr.first});
    }
    return sorted;
}

std::string decrypt_alpha_smart(const std::string& cipher_text) {
    std::unordered_map<char, float> expected_prob = { {'a', 8.49}, {'b', 2.07}, {'c', 2.78}, {'d', 4.25}, {'e', 12.7}, {'f', 1.81}, {'g', 2.01}, {'h', 6.09}, {'i', 6.96}, {'j', 0.153}, {'k', 0.772}, {'l', 4.02},
                                   {'m', 2.4}, {'n', 6.74}, {'o', 7.5}, {'p', 1.92}, {'q', 0.095}, {'r', 5.98}, {'s', 6.32}, {'t', 9.05}, {'u', 2.75}, {'v', 0.978}, {'w', 2.36}, {'x', 0.15}, {'y', 1.97}, {'z', 0.074} };
    std::unordered_map<char, float> calc_prob = get_frequency(cipher_text);
    std::cout << "Hello" << std::endl;
    std::unordered_map<char, char> key;
    auto sorted_expected = get_sorted_counts(expected_prob);
    auto sorted_calc = get_sorted_counts(calc_prob);
    while (!sorted_expected.empty() && !sorted_calc.empty()) {
        std::cout << "Swapping: " << sorted_expected.top().second << " -- With: " << sorted_calc.top().second << std::endl;
        key.insert({ sorted_expected.top().second, sorted_calc.top().second});
        sorted_expected.pop();
        sorted_calc.pop();
    }
    return decrypt_alpha(cipher_text, key);
}

int main()
{

    //std::string input = "[1] Fourscore and seven years ago our fathers brought forth on this continent, a new nation, conceived in Liberty, and dedicated to the proposition that all men are created equal.Now we are engaged in a great civil war, testing whether that nation, or any nation so conceived and so dedicated, can long endure.We are met on a great battle - field of that war.We have come to dedicate a portion of that field, as a final resting place for those who here gave their lives that that nation might live.It is altogether fittingand proper that we should do this.But, in a larger sense, we can not dedicate - we can not consecrate - we can not hallow - this ground.The brave men, livingand dead, who struggled here, have consecrated it, far above our poor power to add or detract.The world will little note, nor long remember what we say here, but it can never forget what they did here.It is for us the living, rather, to be dedicated here to the unfinished work which they who fought here have thus far so nobly advanced.It is rather for us to be here dedicated to the great task remaining before us - that from these honored dead we take increased devotion to that cause for which they gave the last full measure of devotion - that we here highly resolve that these dead shall not have died in vain - that this nation, under God, shall have a new birth of freedom - and that government of the people, by the people, for the people shall not perish from the earth.";
    //std::string input = "hello from ten minutes ago this sometimes works but sometimes does not work";
    //std::string input = "how much wood could a woodchuck chuck if a wouldchuck could chuck wood";
    std::string input = encrypt_ceaser(parse_text(), 15);
    std::unordered_map<char, float> temp = get_frequency(input);
    auto ordered = get_sorted_counts(temp);

    for (const auto& kv : temp) {
        std::cout << kv.first << " " << kv.second << std::endl;
    }
    
    /*
    std::cout << "before" << std::endl;
    std::cout << encrypt_ceaser(input, 5) << std::endl;
    std::cout << "after" << std::endl;
    smart_ceaser(encrypt_ceaser(input, 5));
    */
    //std::unordered_map<char, char> abc = { {'a', 'b'}, {'b', 'a'}, {'c', 'c'} };
    std::cout << decrypt_alpha_smart(input) << std::endl;

    
}

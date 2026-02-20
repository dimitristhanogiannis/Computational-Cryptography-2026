#include <bits/stdc++.h>
using namespace std;

vector<long double> english_frequencies = { //english letter frequencies for comparison
    0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015,
    0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749,
    0.07507, 0.01929, 0.00095, 0.05987, 0.06327, 0.09056, 0.02758,
    0.00978, 0.02360, 0.00150, 0.01974, 0.00074
};

bool isletter(char c){
    return (c >= 'a' && c <= 'z');
}

long long int kasiski_test(vector<char>& s){

    vector<long long int> distances;
    long long int n = s.size();

    if(n < 3) return 0; //we only examine common strings of length >= 3 to limit randomness

    for(long long int i = 0; i + 3 <= n; i++){
        for(long long int j = i + 1; j + 3 <= n; j++){
            long long int count = 0;

            while (i + count < n && j + count < n && s[i + count] == s[j + count]) count++;
            if (count >= 3) distances.push_back(j-i); //as soon as we locate a common string of length >= 3 we add the distance between the first letters
        }
    }

    if (distances.empty()) return 0;

    long long int mkd = distances[0];

    for(int i = 1; i < distances.size(); i++){
        mkd = gcd(mkd, distances[i]); //we calculate the GCD of all the distances found above in pairs
    }

    return mkd; //this and its divisors will be the possible key lengths we will examine
}

long double ic(vector<char>& s){
    long long int n = s.size();
    long double ans = 0.0;
    vector<long long int> f(26);

    for(long long int i = 0; i < n; i++){
        char c = s[i];
        f[c - 'a']++;
    }

    for(int i = 0; i < 26; i++){
        ans += f[i]*(f[i] - 1);
    }

    if (n > 1) ans /= (n*(n-1));
    else ans = 0.0;

    return ans;
}

/*long double imc(vector<char>& col1, vector<char>& col2, long long int j){
    long long int n1 = col1.size();
    long long int n2 = col2.size();

    if(n1 == 0 || n2 == 0) return 0.0;

    vector<long long int> f1(26), f2(26);

    for(long long int i = 0; i < n1; i++){
        char c = col1[i];
        f1[c - 'a']++;
    }

    for(long long int i = 0; i < n2; i++){
        char c = col2[i];
        f2[c - 'a']++;
    }

    long double sum = 0.0;

    for(int i = 0; i < 26; i++){
        int idx = (i - j + 26) % 26;
        sum += f1[idx] * f2[i];
    }

    return sum / (n1 * n2);
}*/

vector<long long int> diairetes(long long int mkd){
    vector<long long int> d;

    if(mkd <= 0) return d;

    for(long long int i = 1; i <= mkd; i++){
        if (mkd % i == 0) d.push_back(i);
    }

    return d;
}

/*int find_relative_shift(vector<char>& col1, vector<char>& colm){
    long long int best_shift = 0;
    long double best_diff = 1e9, best_imc = 0.0;

    for(long long int j = 0; j < 26; j++){
        long double curr_imc = imc(col1, colm, j); //we find the imc between columns 1 and m
        long double diff = abs(curr_imc - 0.065);

        if(diff < best_diff){
            best_diff = diff;
            best_imc = curr_imc; //we store the one closest to 0.065 (the maximum)
            best_shift = j; //we consider the corresponding shift as the most appropriate
        }
    }

    return best_shift;
}*/

int find_shift(vector<char>& col){
    long long int n = col.size();

    if(n == 0) return 0;

    long long int best_shift = 0;
    long double best_x_squared = 1e9;

    for(int shift = 0; shift < 26; shift++){ //we try each shift to find the most suitable one
        vector<long long int> f(26);

        for(long long int i = 0; i < n; i++){
            char c = col[i];
            int shift_idx = (c - 'a' - shift + 26) % 26; 
            f[shift_idx]++;
        }

        long double x_squared = 0.0; //we use the statistical index x^2 = Σ ((Oi - Ei)^2 / Ei), O = observed, E = expected, which is the most suitable to see how well the frequencies match those of the English language

        for(int i = 0; i < 26; i++){
            long double expected = english_frequencies[i] * n;
            long double observed = f[i];

            if(expected > 0){
                x_squared += (observed - expected) * (observed - expected) / expected;
            }
        }    

        if(x_squared < best_x_squared){
            best_x_squared = x_squared;
            best_shift = shift; //we keep the shift with the smallest value of the x^2 index, as this adapts better to the English language frequencies
        }
    }

    return best_shift;
}

string minimize_key(string key){
    int n = key.size();

    for(int p = 1; p <= n; p++){
        if(n % p != 0) continue;

        bool ok = true;

        for(int i = 0; i < n; i++){
            if(key[i] != key[i % p]){ //function to check key repetition >= 2 times
                ok = false;
                break;
            }
        }
        if(ok) return key.substr(0, p); //if this happens, we keep only the 1st repetition period of the key
    }
    return key;
}

vector<pair<string, long double>> find_keys(vector<char>& s, vector<long long int>& divs){
    vector<tuple<long long int, long double, vector<vector<char>>>> candidates; //we store all candidate keys

    for(long long int i = 0; i < divs.size(); i++){
        long long int r = divs[i]; //we examine the divisors of each number, which to ensure we don't miss the key length are the numbers 1 - 30 

        if(r <= 0) continue;

        vector<vector<char>> cols(r);

        for(long long int j = 0; j < s.size(); j++){
            cols[j % r].push_back(s[j]); //we divide the text into r columns cyclically via mod
        }

        long double sum_ic = 0.0;
        long long int valid_cols = 0; //columns for which ic can be calculated 

        for(long long int j = 0; j < r; j++){
            if (cols[j].size() > 1){
                sum_ic += ic(cols[j]); //we add the ic of each column to calculate the average ic for the specific r
                valid_cols++;
            }
        }

        if(valid_cols > 0){
            long double avg_ic = sum_ic / valid_cols;
            candidates.push_back({r, avg_ic, cols}); 
        }
    }

    sort(candidates.begin(), candidates.end(), [](auto& a, auto& b){
        return abs(get<1>(a) - 0.065) < abs(get<1>(b) - 0.065);
    }); //we sort the candidate keys by distance from 0.065

    vector<pair<string, long double>> results;
    int maxcand = min((int)candidates.size(), 200); //we examine many keys, so the correct one is not lost due to period repetitions, but we will print only the 5 most appropriate and non-repeating ones

    for(int i = 0; i < maxcand; i++){
        long long int key_length = get<0>(candidates[i]);
        vector<vector<char>> best_cols = get<2>(candidates[i]);

        string key = "";
        for(long long int j = 0; j < key_length; j++){
            int shift = find_shift(best_cols[j]); //instead of finding the relative shift of the 1st with the remaining columns and then finding the shift of the 1st column, we find all shifts, as this method proved more accurate
            key += (char)('a' + shift);
        }

        results.push_back({key, get<1>(candidates[i])});
    }

    return results;
}

string vigenere_decrypt(string& s, string key){ //vigenere decryption with known key
    if(key.empty()) return "";

    string text = "";
    long long int key_length = key.length();
    long long int ki = 0;
    
    for(long long int i = 0; i < s.size(); i++){
        char c = s[i];
        
        if(isletter(c)){
            char k = key[ki % key_length];
            int decryption = (c - 'a' - (k - 'a') + 26) % 26;
            text += (char)('a' + decryption);
            ki++;
        } 
        else text += c;
    }

    return text;
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    string ciphertext;
    {
        ostringstream oss;
        oss << cin.rdbuf();
        ciphertext = oss.str(); //reading the input text as is (with punctuation marks and spaces)
    }

    vector<char> s;
    for(char c : ciphertext){
        if(isletter(c)) s.push_back(c); //we keep only the letters for cryptanalysis
    }

    long long int mkd = kasiski_test(s);

    vector<long long int> divs;

    if(mkd <= 0){ 
        for(int i = 1; i <= 30; i++){ //if the kasiski test yields no result we manually insert divisors
            divs.push_back(i);
        }
    }
    else{
        divs = diairetes(mkd);
        for(int i = 1; i <= 30; i++){ 
            if(find(divs.begin(), divs.end(), i) == divs.end()){
                divs.push_back(i); //the same for the case where it yields a result, as there is a possibility of coincidence, resulting in losing the key size, if it is larger
            }
        }
        sort(divs.begin(), divs.end());
    }

    vector<pair<string, long double>> key_candidates = find_keys(s, divs);

    unordered_set<string> printed_plaintexts; //we store plaintexts in a set so we don't print them 2 times
    int printed = 0;

    for(auto& candidate : key_candidates){
        if(printed >= 5) break; //up to 5 results, as we store more, so the correct one is not lost
        
        string key_full = candidate.first;
        string key = minimize_key(key_full); //check for key repetition >= 2 times 
        
        string plaintext = vigenere_decrypt(ciphertext, key); //decryption
        
        for(char& c : plaintext){
            if(c == '\n' || c == '\r') c = ' '; //conversion of text to a single line
        }
        
        if(printed_plaintexts.count(plaintext)) continue;
        printed_plaintexts.insert(plaintext);

        vector<char> text;
        for(char c : plaintext){
            if(isletter(c)) text.push_back(c);
        }
        long double ic_val = ic(text);

        cout << key << " " << plaintext << " " << fixed << setprecision(4) << ic_val << endl;
        printed++;
    }

    return 0;
}
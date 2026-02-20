#include <bits/stdc++.h>
using namespace std;

long long modular_exponentiation_square_and_multiply(long long m, long long e, long long n){
    long long result = 1;
    
    m %= n;

    while (e > 0){
        if(e % 2 == 1) result = ((__int128)result * m) % n; //casting to int128 bits to avoid overflow

        m = ((__int128)m * m) % n; 
        e /= 2;
    }
    return result;
}

long long extended_gcd(long long a, long long b, long long &x, long long &y) {
    if (b == 0) {
        x = 1;
        y = 0;

        return a;
    }

    long long x1, y1;
    long long d = extended_gcd(b, a % b, x1, y1);

    x = y1;
    y = x1 - y1 * (a / b);

    return d; //finding, through references, x, y, such that ax + by = gcd(a, b). (a (mod b) = a - ceil(a/b) * b)
}

long long modular_inverse(long long e, long long phi) {
    long long x, y;
    long long g = extended_gcd(e, phi, x, y);

    if (g != 1) return -1;

    return (x % phi + phi) % phi; //finding d using extended euclidean algorithm and shifting to [0, phi(n)-1] if it is negative
}

bool is_prime(long long n) {
    if (n <= 1) return false;

    for (long long i = 2; i * i <= n; i++) if (n % i == 0) return false; //check up to root n for optimization (if n is composite, it has 2 factors a,b : n = a * b, a,b < n^(1/2)

    return true;
}

long long get_random_prime() {
    long long p;

    do p = rand() % 50000 + 10000; while (!is_prime(p)); //finding prime in [10000, 60000), to fit in 64 bit long long variables

    return p;
}

bool loc_oracle(long long c, long long d, long long n){ 
    long long m = modular_exponentiation_square_and_multiply(c, d, n);

    return (m > n / 2); 
}

long long loc_attack(long long c, long long e, long long d, long long n){
    long long a = 0, b = n;
    long long cur_c = c;
    long long enc2 = modular_exponentiation_square_and_multiply(2, e, n);

    for(int i = 0; i < ceil(log2(n)); i++){
        if(i > 0) cur_c = ((__int128)cur_c * enc2) % n;

        long long mid = (a + b) / 2;
        
        if (mid == a) break;

        if(loc_oracle(cur_c, d, n)) a = mid;
        else b = mid;
    }
    
    long long range = 10; 

    for (long long candidate = b - range; candidate <= b + range; candidate++) {
        if (candidate < 0 || candidate >= n) continue;

        long long check_c = modular_exponentiation_square_and_multiply(candidate, e, n);
        
        if (check_c == c) return candidate; //we check in a neighborhood of +/- 10 integers if the resulting ciphertext matches the original, as significant precision is lost, since a, b are integers and not floating-point numbers
    }

    return b;
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    srand(time(0)); //seed for rand() is the current time

    long long p = get_random_prime();
    long long q = get_random_prime();

    while (p == q) q = get_random_prime(); 

    long long n = p * q;
    long long phi = (p - 1) * (q - 1); 
    long long e = 65537; //commonly used e

    while (gcd(e, phi) != 1) e = get_random_prime();

    cout << "n: " << n << endl;
    cout << "e: " << e << endl;

    long long d = modular_inverse(e, phi);

    long long plaintext = (rand() % (n - 2)) + 1; //random m in [1, n-1]

    cout << "Plaintext: " << plaintext << endl;

    long long ciphertext = modular_exponentiation_square_and_multiply(plaintext, e, n);

    long long m = loc_attack(ciphertext, e, d, n);

    cout << "Recovered plaintext: " << m << endl;

    if (plaintext == m) cout << "Success!" << endl;
    else cout << "Failure!" << endl;

    return 0;
}
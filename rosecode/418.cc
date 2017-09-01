#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>

using int64 = long long;
using int128 = __int128;

template<typename T>
T gcd(T a, T b) {
  return !b ? a : gcd(b, a % b);
}

// mod should be not greater than 4e18
// return a * b % mod when mod is less than 2^31
inline int64 mul_mod(int64 a, int64 b, int64 mod) {
  assert(0 <= a && a < mod);
  assert(0 <= b && b < mod);
  if (mod < int(1e9)) return a * b % mod;
  int64 k = (int64)((long double)a * b / mod);
  int64 res = a * b - k * mod;
  res %= mod;
  if (res < 0) res += mod;
  return res;
}

inline int64 add_mod(int64 x, int64 y, int64 mod) {
  return (x + y) % mod;
}

inline int64 sub_mod(int64 x, int64 y, int64 mod) {
  return (x - y + mod) % mod;
}

int64 pow_mod(int64 a, int64 n, int64 m) {
  int64 res = 1;
  for (a %= m; n; n >>= 1) {
    if (n & 1) res = mul_mod(res, a, m);
    a = mul_mod(a, a, m);
  }
  return res;
}

struct Primality {
public:
  // 用miller rabin素数测试判断n是否为质数
  bool is_prime(int64 n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (~n & 1) return false;
    const int u[] = {2,3,5,7,325,9375,28178,450775,9780504,1795265022,0};
    int64 e = n - 1, a, c = 0; // 原理：http://miller-rabin.appspot.com/
    while (~e & 1) e >>= 1, ++c;
    for (int i = 0; u[i]; ++i) {
      if (n <= u[i]) return true;
      a = pow_mod(u[i], e, n);
      if (a == 1) continue;
      for (int j = 1; a != n - 1; ++j) {
        if (j == c) return false;
        a = mul_mod(a, a, n);
      }
    }
    return true;
  }
  // 求一个小于n的因数，期望复杂度为O(n^0.25)，当n为非合数时返回n本身
  int64 pollard_rho(int64 n){
    if (n <= 3 || is_prime(n)) return n; // 保证n为合数时可去掉这行
    while (1) {
      int i = 1, cnt = 2;
      int64 x = rand() % n, y = x, c = rand() % n;
      if (!c || c == n - 2) ++c;
      do {
        int64 u = gcd(n - x + y, n);
        if (u > 1 && u < n) return u;
        if (++i == cnt) y = x, cnt <<= 1;
        x = (c + mul_mod(x, x, n)) % n;
      } while (x != y);
    }
    return n;
  }
  // 使用rho方法对n做质因数分解，建议先筛去小质因数后再用此函数
  std::vector<int64> factorize(int64 n){
    std::vector<int64> u;
    if (n > 1) u.push_back(n);
    for (size_t i = 0; i < u.size(); ++i){
      int64 x = pollard_rho(u[i]);
      if(x == u[i]) continue;
      u[i--] /= x;
      u.push_back(x);
    }
    std::sort(u.begin(), u.end());
    return u;
  }
} pp;

int128 dfs(int d, int64 now, std::vector<std::pair<int64, int>> &p) {
  if (d == p.size()) {
    if (now % 4) return now;
    else return 0;
  }
  int128 ret = 0;
  for (int i = 0; i <= p[d].second; ++i) {
    ret += dfs(d + 1, now, p);
    now *= p[d].first;
  }
  return ret;
}

int128 r4(int64 n) {
  auto u = pp.factorize(n);
  std::vector<std::pair<int64, int>> fac;
  for (size_t i = 0; i < u.size(); ++i) {
    if (fac.empty() || fac.back().first != u[i]) {
      fac.emplace_back(u[i], 0);
    }
    fac.back().second++;
  }
  int128 ret = 8 * dfs(0, 1, fac);
  return ret;
}

int main() {
  int128 ret = 0;
  for (int i = 0; i <= 100; ++i) {
    ret += r4(100000000000000000ll + i);
  }
  const int64 base = 1000000000000000000ll;
  printf("%lld%018lld\n", int64(ret / base), int64(ret % base));
  return 0;
}

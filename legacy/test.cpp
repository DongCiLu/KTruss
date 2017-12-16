#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <climits>

#define INTSIZE 31

using namespace std;

typedef pair<int, int> ptype;

inline long encode(int a, int b) {
    //return ((long)(a) << INTSIZE) + b;
    long c = INT_MAX;
    return a * (c + 1) + b;
}

int main() {
    cout << INT_MAX << " " << LONG_MAX  
        << " " << sizeof(int) 
        << " " << sizeof(int) 
        << " " << sizeof(long) << endl;
    int zzz = 3;
    int zz = 5;
    long x = encode(zzz, zz);
    cout << (zzz << 1) << endl;
    cout << x << " " << ((x - 5) >> INTSIZE)  << " " << int(x & INT_MAX) << endl;
    unordered_set<long> a;
    unordered_set<ptype, boost::hash<ptype> > b;
    unordered_map<long, int> c;
    unordered_map<ptype, int, boost::hash<ptype> > d;
    clock_t last_time = clock();
    for (int i = 0; i < 10000000; i++) {
        a.insert(encode(i, i));
    }
    cout << "unordered_set<int> a elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();
    for (int i = 0; i < 10000000; i++) {
        b.insert(make_pair(i, i));
    }
    cout << "unordered_set<ptype, boost::hash<ptype> b elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();
    for (int i = 0; i < 10000000; i++) {
        c.insert(make_pair(encode(i, i), i));
    }
    cout << "unordered_map<int, ptype> c elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();
    for (int i = 0; i < 10000000; i++) {
        d.insert(make_pair(make_pair(i, i), i));
    }
    cout << "unordered_map<ptype, int, boost::hash<ptype>> d elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();

    size_t nothing = 0;
    for (int i = 0; i < 10000000; i++) {
        if (a.find(encode(i, i)) != a.end())
            nothing ++;
    }
    cout << "unordered_set<int> a elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();
    for (int i = 0; i < 10000000; i++) {
        if (b.find(make_pair(i, i)) != b.end())
            nothing ++;
    }
    cout << "unordered_set<ptype, boost::hash<ptype> b elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();
    for (int i = 0; i < 10000000; i++) {
        if (c.find(encode(i, i)) != c.end())
            nothing ++;
    }
    cout << "unordered_map<int, ptype> c elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();
    for (int i = 0; i < 10000000; i++) {
        if (d.find(make_pair(i, i)) != d.end())
            nothing ++;
    }
    cout << "unordered_map<ptype, int, boost::hash<ptype>> d elapsed time: " << double(clock() - last_time) / CLOCKS_PER_SEC << endl;
    last_time = clock();

    cout << "nothing: " << nothing << endl;
    return 0;
}

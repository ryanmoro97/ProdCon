#include <string.h>
#include <string>
#include <chrono>
#include <sys/time.h>
using namespace std;

struct timeval tp;

string getTime(){
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    string time = to_string(ms);
    time.insert(time.length()-3, ".").pop_back();
    return time;
}
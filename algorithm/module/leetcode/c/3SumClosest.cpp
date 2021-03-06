// http://leetcode.com/2012/05/clone-graph-part-i.html
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

class Solution
{
 public:
  int threeSumClosest(vector<int> &num, int target)
  {
    vector<int>::size_type size = num.size();
    sort(num.begin(), num.end());
    int ret,cur_sum,diff;
    diff = INT_MAX;
    if (size < 3) {
      return LONG_MIN;
    }
    for (vector<int>::size_type i = 0; i != size-2 ; ++i) {
      vector<int>::size_type l = i + 1;
      vector<int>::size_type r = size - 1;
      cur_sum = num[i] + num[l] + num[r];
      while (l < r) {
        cur_sum = num[i] + num[l] + num[r];
        if (cur_sum < target) {
          int min = target - cur_sum;
          if (min < diff) {
            diff = min;
            ret = cur_sum;
          }
          ++l;
        }
        if (cur_sum > target) {
          int min = cur_sum - target;
          if (min < diff) {
            diff = min;
            ret = cur_sum;
          }
          --r;
        }
        if (cur_sum == target) {
          return cur_sum;
        }
     }
    }
    return ret;
  }
};

int main() {
  cout << "3SumClosest" << endl;
  int l[] = {0, 1, 2};
  vector<int> sum(l, l+3);
  Solution sln = Solution();
  int ret = sln.threeSumClosest(sum, 3);
  cout << ret << endl;
  return 0;
}

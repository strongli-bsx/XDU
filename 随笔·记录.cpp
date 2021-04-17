-------------------------4-17---------------------------------------------
class Solution {
public:
	int searchInsert(vector<int>& nums, int target) {
		int left = 0, right = nums.size() - 1;
		//找到最后，left 一定是 等于right的，且left一定是向右走，right向左走
		//当不满足条件的时候，right < left, 代表没有找到该元素
		//插入的位置，也就是 right + 1
		while (left <= right) {
			int index = (left + right) >> 1;
			if (nums[index] == target)	return index;
			if (nums[index] > target)  right = index - 1;
			if (nums[index] < target)  left = index + 1;
		}
		return right + 1;
	}
};

------------------------------------------------------------------------
class Solution{
public:
	int minSubArrayLen(int s, vector<int>& nums) {
		int res = INT_MAX;
		int n = nums.size();
		int sum = 0;
		//复习滑动窗口，前两天刚刚写过，今天写了双指针，就不会写了
		//窗口是循环，两层循环，先是右侧循环，当右侧到达临界点后，
		//左侧进入循环，到达临界点，右侧继续循环，以此往复
		for (int left = 0, right = 0; right < n; right++) {
			sum +=  nums[right];
			for (; sum >= s; left++) {
				sum -= nums[left];
				res = min(res, right - left + 1);
	
			}
		}
		return res == INT_MAX ? 0 : res;
	}
};
-----------------------------------------------------------------

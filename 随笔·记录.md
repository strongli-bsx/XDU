
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

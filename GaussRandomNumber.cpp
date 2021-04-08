#include <iostream>
#include <fstream>
using namespace std;
#include <ctime>
typedef int ElemType;

#define PI 3.141592653

ElemType guassrand(const ElemType mu,const ElemType sigma) {
	const double epsilon = std::numeric_limits<double>::min();
	const double two_pi = 2.0*3.14159265358979323846;
	


	static double z0, z1;
	static bool generate;
	generate = !generate;

	if (!generate)
		return z1 * sigma + mu;

	double u1, u2;
	do {
		u1 = rand() * (1.0 / RAND_MAX);
		u2 = rand() * (1.0 / RAND_MAX);
	} while (u1 <= epsilon);


	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
	return z0 * sigma + mu;
}

void createGaussRandomNum(const int num, const ElemType mu, const ElemType sigma) {
	srand((unsigned)time(NULL));
	ofstream ofs("³É¼¨.txt", ios::out);
	for (int i = 0; i < num; i++) {
		ElemType arr = guassrand(mu, sigma);
		if (arr >= 100)
			arr -= 10;
		ofs << arr << endl;
		cout << arr << endl;
	}
}



int main() {

	createGaussRandomNum(142,90, 5);


	system("pause");
	return 0;

}
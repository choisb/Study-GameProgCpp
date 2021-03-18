#include "Math.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{

	Vector2 a, b;
	float result;

	a = Vector2(2.0f, 4.0f) - Vector2(-1.0f, 1.0f);
	b = Vector2(3.0f, 3.0f) - Vector2(-1.0f, 1.0f);

	result = Math::Acos(
		Vector2::Dot(a, b) /
		(a.Length() * b.Length()));

	cout << "라디안: " << result << " / " << result / Math::Pi << "π" << endl
		<< "도: " << result / Math::Pi * 180 << "°"<< endl;

    return 0;
}

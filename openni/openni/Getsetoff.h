#pragma once
class MyClass
{
public:
	MyClass();
	~MyClass();
	void  Get_setoff(void);
	double tranxyz[3];
	int xtag = 0;
	int ytag = 0;
	int e = 0;

private:
	int depthindex;
	double rotation[3][3];
	void on_Mouse(int events, int x, int y);
	static void onMouse(int events, int x, int y, int, void* userdata);
	char temp[10] = { 0 };
	char tempa[10]= { 0 };
	char tempb[10] = { 0 };
	int i;
};
#include "pch.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define M_PI 3.141592653
#define COS(X)   cos( (X) * 3.14159/180.0 )
#define SIN(X)   sin( (X) * 3.14159/180.0 )
#define HEAD 1
#define SHOUDER 2
#define BODY 3
#define WAIST 4
#define UPLEG 5
#define LOWLEG 6
#define FOOT 7
#define UPARM 8
#define LOWARM 9
#define HAND 10
GLUquadricObj *qObj;
static float lightpos[] = { 0.f, 200.f, 10.f, 1.f };
double turnbody = 0;
double elevation = 0;
int turnbodytemp;
int iskick = 0;
int iswalk = 0;
int canexcludeball = 0;
int cancatchball = 1;
float G_vLitPosition[4] = { -5.0f, 0.0f, 5.0f, 1.0f };
int fk = 1;
static double v0, v1, v2;
static double a0, a1, a2;

struct {
	float pos[2];
	float dir[2];
}myrobot;

int mode = 0;
int ileg = 0;
int iarm = 0;
double turnfront = 0;
double turnleft = 0;
double turnupward = 0;
//头，肩，腰。
double turnHead = 0;
double turnShouder = 0;
double turnWaist = 0;
//手臂。
double turnUpArmR = 0;
double turnMiddleArmR = 0;
double turnHandR = 0;
double turnUpArmL = 0;
double turnMiddleArmL = 0;
double turnHandL = 0;
//脚。
double turnUpLegR = 0;
double turnMiddleLegR = 0;
double turnFootR = 0;
double turnUpLegL = 0;
double turnMiddleLegL = 0;
double turnFootL = 0;
//utils
float distance(float x1, float z1, float x2, float z2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (z1 - z2) * (z1 - z2));
}
float getMirrorAngle(float posx, float posz, float centerx, float centerz, float vx, float vz)
{	double theta;
	double fai;
	double alfa;
	/*atan2反正切值函数，得到的角度是指过原点和指定坐标点（vz,vx）的直线与x轴之间的夹角（用弧度表示）
	如果结果为正，表示此直线沿x轴逆时针旋转；如果结果为负，表示此直线沿x轴顺时针旋转。
	 fabs取绝对值函数*/
	double temptheta = atan2((double)fabs(vz), (double)fabs(vx));
	if (vx >= 0 && vz <= 0)
		theta = temptheta;
	if (vx <= 0 && vz <= 0)
		theta = M_PI - temptheta;
	if (vx <= 0 && vz >= 0)
		theta = M_PI + temptheta;
	if (vx >= 0 && vz >= 0)
		theta = 2 * M_PI - temptheta;

	double tempfai = atan2((double)fabs(posz - centerz), (double)fabs(posx - centerx));
	if (posz <= centerz && posx >= centerx)
		fai = tempfai;
	if (posz <= centerz && posx <= centerx)
		fai = M_PI - tempfai;
	if (posz >= centerz && posx <= centerx)
		fai = M_PI + tempfai;
	if (posz >= centerz && posx >= centerx)
		fai = 2 * M_PI - tempfai;

	alfa = theta - fai - M_PI;
	return (float)(fai - alfa);

}

void drawTorus(void)//手爪儿
{
	int numMajor = 32;
	int numMinor = 24;
	float majorRadius = 6.f;
	float minorRadius = 2.f;
	double majorStep = 2.0f*M_PI / numMajor;
	double minorStep = 2.0f*M_PI / numMinor;

	int i, j;

	for (i = 0; i < numMajor; ++i)
	{
		double a0 = i * majorStep;
		double a1 = a0 + majorStep;
		GLfloat x0 = (GLfloat)cos(a0);
		GLfloat y0 = (GLfloat)sin(a0);
		GLfloat x1 = (GLfloat)cos(a1);
		GLfloat y1 = (GLfloat)sin(a1);
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0; j < numMinor; ++j)
		{
			double b = j * minorStep;

			GLfloat c = (GLfloat)cos(b);
			GLfloat r = minorRadius * c + majorRadius;
			GLfloat z = minorRadius * (GLfloat)sin(b);

			glNormal3f(x0*c, y0*c, z / minorRadius);
			glVertex3f(x0*r, y0*r, z);
			glNormal3f(x1*c, y1*c, z / minorRadius);
			glVertex3f(x1*r, y1*r, z);

		}

		glEnd();

		if (i == 14)
			break;
	}
}

/*
glNewList，glEndList用于创建和替换一个显示列表函数原型。
显示列表的GL命令组已为后续执行存储。显示列表创建的glNewList。
放在显示列表中的所有后续命令，在发出的命令，直到glEndList被调用。
glNewList有两个参数。第一个参数列表，是一个正整数，成为唯一的名称显示列表。
名称可以创建和保留与glGenLists和独特性与glIsList测试。
第二个参数，模式，是一个符号常量。
*/
void head()//头
{
	glNewList(HEAD, GL_COMPILE);// //用于创建一个显示列表函数原型
	glColor3f(1, 1, 1);
	glPushMatrix();//在变换完毕后用glPopMatrix()记录变换后的位置
	gluSphere(qObj, 10, 10, 10); //（对象，球半径，Z片数,Y片数）
	glRotatef(-turnHead, 0, 1, 0); //（旋转角度，轴向量）
	glTranslatef(0, -10, 0); //平移函数（xyz）
	glRotatef(90, 1, 0, 0);
	gluCylinder(qObj, 5, 5, 5, 30, 30); //圆柱（对象，底半径，顶半径，**，**）
	gluDisk(qObj, 0, 5, 30, 30); /*同心圆（二次对象，内部半径，外部半径，围绕z轴的细分数，关于       圆盘被细分为原点的同心圆环的数目）*/
	glPopMatrix();//调用时返回原态
	glEndList();//替换一个显示列表函数原型
}
void shouder()//肩膀
{
	glNewList(SHOUDER, GL_COMPILE);
	glColor3f(0, 0.8, 1);
	glPushMatrix();//在变换完毕后用glPopMatrix()记录变换后的位置
	glRotatef(90, 1, 0, 0);//旋转
	gluCylinder(qObj, 8, 25, 8, 30, 30);//圆柱
	gluDisk(qObj, 0, 8, 30, 30);//同心圆
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, -8, 0);//平移
	glRotatef(90, 1, 0, 0);
	gluDisk(qObj, 0, 25, 30, 30);
	glPopMatrix();//调用时返回原态
	glColor3f(1, 1, 1);
	glEndList();
}

void body()//腰部和肩膀之间
{
	glNewList(BODY, GL_COMPILE);//在变换完毕后用glPopMatrix()记录变换后的位置
	glColor3f(0.8, 1, 0);
	gluCylinder(qObj, 6, 15, 25, 30, 30);
	glColor3f(1, 1, 1);
	glEndList();
}

void waist()//腰
{
	glNewList(WAIST, GL_COMPILE);
	glColor3f(0, 0, 1);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	gluCylinder(qObj, 6, 6, 2, 30, 30);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, -2, 0);
	glRotatef(90, 1, 0, 0);
	gluCylinder(qObj, 6, 10, 10, 10, 10);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, -10, 0);
	glRotatef(90, 1, 0, 0);
	gluDisk(qObj, 0, 10, 30, 30);
	glPopMatrix();
	glColor3f(0, 1, 1);
	glEndList();
}


void lowleg()//小腿
{   
	glColor3f(0.5, 0, 1);
	glRotatef(90, 1, 0, 0);
	gluCylinder(qObj, 3, 3, 50, 30, 30);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(-3, -50, 0);
	glRotatef(90, 0, 1, 0);
	gluCylinder(qObj, 3, 3, 6, 30, 30);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(3, -2, 6);

	glPushMatrix();
	if (!ileg % 2)
	{
		glRotatef(turnFootR, 1, 0, 0);
		
	}
	else
	{
		glRotatef(turnFootL, 1, 0, 0);
		
	}
	glPopMatrix();

}
void upleg()//腿
{
	glPushMatrix();
	for (ileg = 0; ileg < 2; ileg++)
	{
		if (ileg % 2)
			glTranslatef(-12, 0, 0);
		glColor3f(0, 0.5, 1);
		gluSphere(qObj, 4, 30, 30); //（对象，球半径，Z片数, Y片数）
		glPushMatrix();
		if (!ileg % 2)
			glRotatef(turnUpLegR, 1, 0, 0);
		else
			glRotatef(turnUpLegL, 1, 0, 0);
		if (!ileg % 2)
			glRotatef(5, 0, 0, 1);
		else
			glRotatef(-5, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 4, 4, 20, 30, 30);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(-4, -23, 0);
		glRotatef(90, 0, 1, 0);
		gluCylinder(qObj, 3, 3, 8, 30, 30);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(4, 0, 0);
		glColor3f(1, 0, 1);
		glPushMatrix();
		if (!ileg % 2)
		{
			glRotatef(turnMiddleLegR, 1, 0, 0);
			lowleg();
		}
		else
		{
			glRotatef(turnMiddleLegL, 1, 0, 0);
			lowleg();
		}
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();
}
void hand()//手
{
	glPushMatrix();
	glTranslatef(-1, -6, -6);
	glRotatef(90, 0, 1, 0);
	drawTorus();//调用手爪
	glPopMatrix();//调用时返回原态
}
void lowarm()//小臂
{
	glRotatef(90, 1, 0, 0);
	gluCylinder(qObj, 3, 3, 20, 30, 30);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(-2, -20, 0);
	glRotatef(90, 0, 1, 0);
	gluCylinder(qObj, 3, 3, 4, 30, 30);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(3, -2, 6);
	glPushMatrix();
	if (!iarm % 2)
	{
		glRotatef(turnHandR, 1, 0, 0);
		hand();
	}
	else
	{
		glRotatef(turnHandL, 1, 0, 0);
		hand();
		if (!cancatchball)
		{
			glTranslatef(-1, -6, -6);
		}
	}
	glPopMatrix();
}
void uparm()//胳膊
{
	glPushMatrix();
	for (iarm = 0; iarm < 2; iarm++)
	{
		if (iarm % 2)
			glTranslatef(-50, 0, 0);
		glColor3f(1, 0, 1);
		gluSphere(qObj, 5, 30, 30);
		glPushMatrix();
		if (!iarm % 2)
			glRotatef(turnUpArmR, 1, 0, 0);
		else
			glRotatef(turnUpArmL, 1, 0, 0);
		if (!iarm % 2)
			glRotatef(10, 0, 0, 1);
		else
			glRotatef(-10, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(qObj, 4, 4, 20, 30, 30);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(-4, -23, 0);
		glRotatef(90, 0, 1, 0);
		gluCylinder(qObj, 3, 3, 8, 30, 30);
		glColor3f(0, 1, 0.3);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(4, 0, 0);
		glPushMatrix();
		if (!iarm % 2)
		{
			glRotatef(turnMiddleArmR, 1, 0, 0);
			lowarm();
		}
		else
		{
			glRotatef(turnMiddleArmL, 1, 0, 0);
			lowarm();
		}
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();
}
//Robot functions end


void myinit()//初始化  
{
	qObj = gluNewQuadric();//创建二次对象
	glEnable(GL_DEPTH_TEST);//参数  跟据函数glAlphaFunc的条件要求来决定图形透明的层度是否显示
	glEnable(GL_COLOR_MATERIAL);//启用每一像素的色彩为位逻辑运算
	// 用于启用各种功能。功能由参数决定
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//init robot pos
	myrobot.dir[0] = 0;
	myrobot.dir[1] = 0;
	myrobot.pos[0] = 0;
	myrobot.pos[1] = 0;

	//Robot
	head();
	shouder();
    body();
	waist();
}


//Dynamic Object
void drawRobot()//画图
{
	glPushMatrix();

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glCallList(BODY);
	glPopMatrix();//一次动作

	glPushMatrix();
	glTranslatef(0, 33, 0);
	glRotatef(turnShouder, 0, 1, 0);
	glCallList(SHOUDER);
	glTranslatef(25, -7, 0);
	uparm();
	glPopMatrix();//二次变换

	glPushMatrix();
	glTranslatef(0, 45, 0);
	glRotatef(turnHead, 0, 1, 0);
	glCallList(HEAD);
	glPopMatrix();//三次变换

	glPushMatrix();
	glRotatef(turnWaist, 0, 1, 0);
	glCallList(WAIST);
	glTranslatef(6, -15, 0);
	upleg();
	glPopMatrix();//4次变换

	glPopMatrix();//收尾

}

//Animation动画
void  robotrun()
{
	static int  step = 1;
	static int stept = 7;
	double degree = 6;
	if (step == 1)
	{
		//left arm
		turnUpArmL -= (degree / 3);
		//right arm
		turnUpArmR += (degree / 3);
		//left leg
		turnUpLegL += (degree / 3);
		//right leg
		if (turnUpLegR >= -40)
		{
			turnUpLegR -= degree;
			turnMiddleLegR += degree * 60 / 40;
		}
		else
		{
			elevation = 2;
			step = 2;
		}
	}
	else if (step == 2)
	{
		//left arm
		turnUpArmL -= (degree / 3);
		//right arm
		turnUpArmR += (degree / 3);
		//left leg
		turnUpLegL += degree / 3;
		//right leg
		if (turnUpLegR <= -20)
		{
			turnUpLegR += degree;
			turnMiddleLegR -= degree * 2.5;
		}
		else
		{
			elevation = 0;
			step = 3;
		}
	}
	else if (step == 3)
	{
		//left arm
		turnUpArmL += (degree);
		//right arm
		turnUpArmR -= (degree);
		//left leg
		if (turnUpLegL >= 0 && stept != 8)
		{
			turnUpLegL -= degree * 3;
			turnMiddleLegL += degree;
		}
		else if (turnMiddleLegL >= 0 && stept != 8)
			turnMiddleLegL -= degree;
		else
		{
			stept = 8;
			if (turnUpLegL >= -35 && stept != 9)
			{
				turnUpLegL -= degree * 3;
				turnMiddleLegL += degree * 60 / 40;
			}
			else
			{
				stept = 9;
				turnUpLegL += degree * 3;
				turnMiddleLegL -= degree * 2.5;
			}
		}
		//right leg
		if (turnUpLegR <= 20)
		{
			turnUpLegR += degree;
		}
		else
			step = 4;
	}
	else if (step == 4)
	{
		//left arm
		turnUpArmL -= (degree / 2.5);
		//right arm
		turnUpArmR += (degree / 2.5);
		//left leg
		turnUpLegL += degree / 1.8;
		stept = 7;
		//right leg
		if (turnUpLegR >= 0)
		{
			turnUpLegR -= degree;
			turnMiddleLegR += degree;
		}
		else if (turnMiddleLegR >= 0)
		{
			turnMiddleLegR -= degree;
		}
		else
		{
			elevation = 2;
			step = 1;
		}
	}
	glutPostRedisplay();
	/*在图像绘制的所有操作之后，要加入 glutPostRedisplay() 来重绘图像，
	否则图像只有响应鼠标或键盘消息时才会更新图像。
	有点像窗口重绘函数一样。它要求当前的回调函数返回时执行显示回调函数*/
}
//Key Listener
void  key(unsigned char key, int x, int y)//功能键
{

	if (iskick == 2)
	{
		//clear();
		iskick = 1;
	}


	if (key == '\033')
		exit(0);//Esc退出

	if (key == 'm')  //  m   改变模式
		mode++;

	//direction
	if (key == 'd')//  d  逆时针转体
	{
		turnbody += 10;
	}
	else if (key == 'a')  //  顺时针转体
	{
		turnbody -= 10;
	}


	myrobot.dir[0] = sin(M_PI / 180 * turnbody);
	myrobot.dir[1] = cos(M_PI / 180 * turnbody);


	if (key == 'w')//向前走
	{
		iswalk = 1;

		float lastpos0 = myrobot.pos[0];
		float lastpos1 = myrobot.pos[1];

		myrobot.pos[0] += 5 * myrobot.dir[0];
		myrobot.pos[1] += 5 * myrobot.dir[1];
		//坐标前进

		//border deal
		if (myrobot.pos[0] >= 450 ||
			myrobot.pos[0] <= -450 ||
			myrobot.pos[1] <= -450 ||
			myrobot.pos[1] >= 450//wall
			)
		{
			myrobot.pos[0] = lastpos0;
			myrobot.pos[1] = lastpos1;
		}//更新坐标

		robotrun();//调转  至动画
	}

	if (key == 's')
	{
		iswalk = 1;

		float lastpos0 = myrobot.pos[0];
		float lastpos1 = myrobot.pos[1];

		myrobot.pos[0] -= 5 * myrobot.dir[0];
		myrobot.pos[1] -= 5 * myrobot.dir[1];
		//坐标后移

		//border deal
		if (myrobot.pos[0] >= 450 ||
			myrobot.pos[0] <= -450 ||
			myrobot.pos[1] <= -450 ||
			myrobot.pos[1] >= 450//wall
			)
		{
			myrobot.pos[0] = lastpos0;
			myrobot.pos[1] = lastpos1;
		}
		//更新坐标

		robotrun();
	}


	if (key == 'h')
	{
		turnHead += 10;
	} // h逆时针转动头部
	else if (key == 'j')
	{
		turnShouder += 10;
	}// j 逆时针转动肩膀
	else if (key == 'k')
	{
		turnWaist += 10;
	} // k 逆时针转动 腰部以下
	else if (key == 'H')
	{
		turnHead -= 10;
	}//H  顺时针转动头部
	else if (key == 'J')
	{
		turnShouder -= 10;
	} //J   逆时针转动肩膀
	else if (key == 'K')
	{
		turnWaist -= 10;
	}// K 逆时针转动 腰部以下
	if (key == 'u')
	{
		turnUpArmR += 10;
	} //逆时针转动整个手臂 u
	else if (key == 'i')
	{
		turnMiddleArmR += 10;
	} //  i  逆时针转动小臂
	else if (key == 'o')
	{
		turnHandR += 10;
	}//  逆时针 转手爪子 o
	else if (key == 'U')
	{
		turnUpArmR -= 10;
	}
	else if (key == 'I')
	{
		turnMiddleArmR -= 10;
	}
	else if (key == 'O')
	{
		turnHandR -= 10;
	}
	if (key == 'v')
	{
		turnUpArmL += 10;
	}
	else if (key == 'b')
	{
		turnMiddleArmL += 10;
	}
	else if (key == 'n')
	{
		turnHandL += 10;
	}
	else if (key == 'V')
	{
		turnUpArmL -= 10;
	}
	else if (key == 'B')
	{
		turnMiddleArmL -= 10;
	}
	else if (key == 'N')
	{
		turnHandL -= 10;
	}
	if (key == 'r')
	{
		turnUpLegR += 10;
	} //逆时针转动右腿
	else if (key == 't')
	{
		turnMiddleLegR += 10;
	}//逆时针转动右小腿
	/*else if (key == 'y')
	{
		turnFootR += 10;
	}*/
	else if (key == 'R')
	{
		turnUpLegR -= 10;
	}
	else if (key == 'T')
	{
		turnMiddleLegR -= 10;
	}
	/*else if (key == 'Y')
	{
		turnFootR -= 10;
	}*/
	if (key == 'l')
	{
		turnUpLegL += 10;
	}
	else if (key == 'f')
	{
		turnMiddleLegL += 10;
	}//逆时针左小腿  f
	/*else if (key == 'g')
	{
		turnFootL += 10;
	}*/
	else if (key == 'L')
	{
		turnUpLegL -= 10;
	}
	else if (key == 'F')
	{
		turnMiddleLegL -= 10;
	}

	glutPostRedisplay();

}




void  redraw(void)//作图
{ //设置清除屏幕的颜色，并清除屏幕和深度缓冲
	glClearColor(1.0, 1.0, 0.5, 1.0);//
	glClearDepth(2.0);//缓冲深度值
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//

	//变换并绘制物体
	glMatrixMode(GL_PROJECTION);//参数投影    
	/*在要做下一步之前告诉计算机我要对“什么”进行操作了
	要对投影相关进行操作，也就是把物体投影到一个平面上，就像我们照相一样，把3维物体投到2维的平面上。
	这样，接下来的语句可以是跟透视相关的函数
	*/
	glLoadIdentity();
	/*无论以前进行了多少次矩阵变换，在该命令执行后，当前矩阵均恢复成一个单位矩阵，
	即相当于没有进行任何矩阵变换状态。*/


	// gluPerspective(80.0, 1, 1,3000.0);
	gluPerspective(75.0, 2, 2.5, 3000.0);//（角度，宽高比，沿z轴方向的两裁面之间的距离的近处，沿z轴方向的两裁面之间的距离的远处）
	/**改变图形胖瘦，视图***********/
	glPushMatrix();//记录每次变换
	glLoadIdentity();
	/*每次建立顶点的时候，都是以当前物体的中心作为世界的原点的
	（换句话说，也就是上一轮物体变换之后，原点的位置作为新的物体的中心）*/
	glOrtho(0, 100, 0, 100, -1, 1);//（左，右，底，顶，近，远）
	/*平行修剪空间，无论物体距离相机多远，投影后的物体大小尺寸不变*/
	glMatrixMode(GL_MODELVIEW);
	/*功能是指定哪一个矩阵是当前矩阵。创建一个投影矩阵并且与当前矩阵相乘，得到的矩阵设定为当前变换
	GL_MODELVIEW,对模型视景矩阵堆栈应用随后的矩阵操作.
GL_PROJECTION,对投影矩阵应用随后的矩阵操作.
GL_TEXTURE,对纹理矩阵堆栈应用随后的矩阵操作.*/
	glLoadIdentity();
	glNormal3f(0, 0, 1);

	glTranslatef(37, 37, 0);
	// glTranslatef(37,37,600);
	glFlush();
	//用于强制刷新缓冲，保证绘图命令将被执行，而不是存储在缓冲区[2]  中等待其他的OpenGL命令。
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	/*回到glPushMatrix()之前的状态*/


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, -8.6, -300);//你绘点坐标的原点在当前原点的基础上平移一个(x,y,z)向量。
	if (mode % 2)
	{
		glDisable(GL_LIGHTING);//使用当前光照计算顶点颜色。否则只是将当前的颜色和顶点简单的关联在一起
		glDisable(GL_DEPTH_TEST);//进行深度比较和更新深度缓冲
		glDisable(GL_NORMALIZE);//在转换之后和光照之前将法线向量标准化成单位长度
	/*disable是关闭各种功能*/
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}/*（确定显示模式将适用于物体的哪些部分控制多边形的正面和背面的绘图模式，
		 mode这个参数确定选中的物体的面以何种方式显示）
		 GL_FRONT表示显示模式将适用于物体的前向面（也就是物体能看到的面）
		 GL_LINE表示显示线段，多边形用轮廓显示*/
	else
	{//启动模式
		glEnable(GL_LIGHTING);//启用灯源
		glEnable(GL_DEPTH_TEST);//启用深度测试根据坐标的远近自动隐藏被遮住的图形（材料）
		glEnable(GL_NORMALIZE);//根据函数glNormal的设置条件，启用法向量
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}



	glLoadIdentity();
	//gluLookAt(490,450,0,0,-85,myrobot.pos[1],0,1,0);
	gluLookAt(490, 450, 0,/*相机在世界坐标的位置*/ 0, -85, myrobot.pos[1],/*相机镜头对准的物体在世界坐标的位置*/
		0, 1, 0/*头顶朝向的方向*/);
	/*该函数定义一个视图矩阵，并与当前矩阵相乘。

	*/
	glPushMatrix();
	//glTranslatef(0,-8.6,0);
	glTranslatef(0, 300, 0);
	glRotatef(turnleft, 0, 1, 0);
	glRotatef(turnupward, 1, 0, 0);
	glRotatef(turnfront, 0, 0, 1);

	glPushMatrix();
	//glTranslatef(0,-200,0);
	glTranslatef(0, elevation, 0);
	//Robot
	glPushMatrix();
	if (iswalk)
		glTranslatef(myrobot.pos[0], 0, myrobot.pos[1]);
	glRotatef(turnbody, 0, 1, 0);
	drawRobot();
	glPopMatrix();

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
	/*交换前台后台缓冲区指针，解决了频繁刷新导致的画面闪烁问题*/


}


void  reshape(int w, int h)//更新图视
{
	glViewport(0, 0, w, h);
	/*把视景体截取的图像按照怎样的高和宽显示到屏幕上
	前两个参数所在坐标系的原点位于当前窗口的左下角，即整个窗口位于第一象限内，一般情况，前两个参数都是0,0
	后两个参数表示该视口的宽度和高度，可以把视口看做是一个矩形方框。
	*/
	//设定透视方式
	glMatrixMode(GL_PROJECTION);
	//制定后方功能
	glLoadIdentity();
	//复原
	gluPerspective(60.0, 1.0*(GLfloat)w / (GLfloat)h, 1.0, 30.0);
	//设置透视窗

}
/***************************主函数***********************************************************/
int main(int argc, char *argv[])
{
	printf("Esc  退出绘图");
	printf("m   改变机器人模式");
	printf("d   逆时针转体");
	printf("a   顺时针转体");
	printf("w   前进");
	printf("s   后退");
	printf("大写代表顺时针：  ");
	printf("h    逆时针转动头部  ");
	printf("j    逆时针转动肩膀  ");
	printf("h     k逆时针转动腰部及以下  ");
	
	printf("u    逆时针转动整条右手臂 ");
	printf("i    逆时针转动右小手臂 ");
	printf("o    逆时针转动右手爪  ");
	
	printf("v    顺时针转动整条左手臂  ");
	printf("b    顺时针转动左小手臂  ");
	printf("n    顺时针转动左手爪  ");
	
	printf("r    逆时针转动整条右腿  ");
	printf("t    逆时针转动右小腿  ");
	
	printf(" l   逆时针转动整条左腿  ");
	printf("f   逆时针转动左小腿 ");

	glutInit(&argc, argv);//传递一个正确的入口参数，使它能够正确地初始化OpenGL环境。
	glutInitWindowSize(800, 700);//设置窗口宽，高
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_ACCUM | GLUT_DOUBLE);
	//设置图形显示模式
	/*GLUT_RGBA：当未指明GLUT - RGBA或GLUT - INDEX时，是默认使用的模式。表明欲建立RGBA模式的窗口。
	 GLUT_DEPTH：使用深度缓存
	 GLUT_ACCUM：让窗口使用累加的缓存。
	 GLUT_DOUBLE：使用双缓存。以避免把计算机作图的过程都表现出来，或者为了平滑地实现动画。

*/
	(void)glutCreateWindow("Made by ddm");
	/*产生一个顶层的窗口。name 作为窗口的名字，也就是窗口标题栏显示的内容
	返回值是生成窗口的标记符，可用函数glutGetWindow()加以引用
	*/

	myinit();//初始化 准备函数
	glutDisplayFunc(redraw);//调用redraw函数
	glutReshapeFunc(reshape);//重新绘图，调用reshape
	glutKeyboardFunc(key);//注册当前窗口的键盘回调函数  调用key

	glMatrixMode(GL_MODELVIEW/*对模型视景的操作*/);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	/*启动*/
	
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	/*glLightfv（光源编号，光源特性，参数数据）来设置光源
	*/
	glutMainLoop();//循环
	return 0;
   }


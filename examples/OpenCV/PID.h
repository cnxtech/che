//
// Created by Kry·L on 2018/10/10.
//

#ifndef LANEDETECTION_DEMO_PID_H
#define LANEDETECTION_DEMO_PID_H

#include <iostream>

//位置式PID
class PID_position
{
private:
    float kp;//比例系数
    float ki;//积分系数
    float kd;//微分系数
    float target;//目标值
    float actual;//实际值
    float e;//误差
    float e_pre;//上一次误差
    float integral;//积分项
public:
    PID_position();
    ~PID_position(){};
    PID_position(float p,float i,float d);
    float pid_control(float tar,float act);//执行PID控制
    void pid_show();//显示PID控制器的内部参数
};

//位置PID
PID_position::PID_position():kp(0),ki(0),kd(0),target(0),actual(0),integral(0)
{
    e=target-actual;
    e_pre=e;
}
PID_position::PID_position(float p,float i,float d):kp(p),ki(i),kd(d),target(0),actual(0),integral(0)
{
    e=target-actual;
    e_pre=e;
}
float PID_position::pid_control(float tar,float act)
{
    float u;
    target=tar;
    actual=act;
    e=target-actual;
    integral+=e;
    u=kp*e+ki*integral+kd*(e-e_pre);
    e_pre=e;
    return u;
}
void PID_position::pid_show()
{
    using std::cout;
    using std::endl;
    cout<<"The infomation of this position PID controller is as following:"<<endl;
    cout<<"       Kp="<<kp<<endl;
    cout<<"       Ki="<<ki<<endl;
    cout<<"       Kd="<<kd<<endl;
    cout<<" integral="<<integral<<endl;
    cout<<"   target="<<target<<endl;
    cout<<"   actual="<<actual<<endl;
    cout<<"        e="<<e<<endl;
    cout<<"    e_pre="<<e_pre<<endl;
}

//增量式PID
class PID_incremental
{
private:
    float kp;
    float ki;
    float kd;
    float target;
    float actual;
    float e;
    float e_pre_1;
    float e_pre_2;
    float A;
    float B;
    float C;
public:
    PID_incremental();
    PID_incremental(float p,float i,float d);
    float pid_control(float tar,float act);
    void pid_show();
};

//增量PID
PID_incremental::PID_incremental():kp(0),ki(0),kd(0),e_pre_1(0),e_pre_2(0),target(0),actual(0)
{
    A=kp+ki+kd;
    B=-2*kd-kp;
    C=kd;
    e=target-actual;
}
PID_incremental::PID_incremental(float p,float i,float d):kp(p),ki(i),kd(d),e_pre_1(0),e_pre_2(0),target(0),actual(0)
{
    A=kp+ki+kd;
    B=-2*kd-kp;
    C=kd;
    e=target-actual;
}
float PID_incremental::pid_control(float tar,float act)
{
    float u_increment;
    target=tar;
    actual=act;
    e=target-actual;
    u_increment=A*e+B*e_pre_1+C*e_pre_2;
    e_pre_2=e_pre_1;
    e_pre_1=e;
    return u_increment;
}

void PID_incremental::pid_show()
{
    using std::cout;
    using std::endl;
    cout<<"The infomation of this incremental PID controller is as following:"<<endl;
    cout<<"     Kp="<<kp<<endl;
    cout<<"     Ki="<<ki<<endl;
    cout<<"     Kd="<<kd<<endl;
    cout<<" target="<<target<<endl;
    cout<<" actual="<<actual<<endl;
    cout<<"      e="<<e<<endl;
    cout<<"e_pre_1="<<e_pre_1<<endl;
    cout<<"e_pre_2="<<e_pre_2<<endl;
}

#endif //LANEDETECTION_DEMO_PID_H

#include <ros/ros.h>
#include "client.h"
#include "ros_dynamic_test/tutorialsConfig.h"
#include <boost/function.hpp>
#include <string>
#include <iostream>
#include <fstream>

typedef boost::function<void(const my_msgs::tutorialsConfig &)> CallBack;

void dynCallBack(const my_msgs::tutorialsConfig &data)
{
    ROS_INFO("int: %d, double: %f, bool: %d, string: %s", data.int_param, data.double_param,
             data.bool_param, data.str_param.c_str());
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "dyn_client");

    ROS_INFO("Spinning node");

    CallBack tmpdata;
    dynamic_reconfigure::Client<my_msgs::tutorialsConfig> client("dynamic_srv", dynCallBack);
    my_msgs::tutorialsConfig config;
    //tmpdata = boost::bind(dynCallBack, _1);

    ros::Rate loop_rate(10);

    int c = 0;
    while (ros::ok())
    {
        c++;
        static bool ret = true;
        static int cnt = 0;
        if(!(c%10))
        {
            config.bool_param = !ret;
            config.int_param = cnt;
            config.double_param = 1/((double)(cnt+1));
            client.setConfiguration(config);
            cnt++;
            ret = !ret;
            if(cnt > 10)
                cnt = 0;
        }

        ros::spinOnce();
        loop_rate.sleep();
    }
    ROS_INFO("Spinning node shutdown...");
    return 0;
}


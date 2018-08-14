#include "../include/rosthread.h"
#include <iostream>

RosThread::RosThread(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<RecfgMsgVal>("RecfgMsgVal");

    m_isExt = false; m_ros_thread = NULL; m_subMap.clear();
    n_ = new ros::NodeHandle;
    m_ros_thread = new boost::thread(boost::bind(&RosThread::rosrunThread, this));
}

RosThread::~RosThread()
{
    m_isExt = true;
    if(m_ros_thread)
    {
        m_ros_thread->interrupt();
        m_ros_thread->join();
        delete m_ros_thread;
    }
}

void RosThread::rosrunThread()
{
    ros::Duration initDur(0.2);
    while (ros::ok() && !m_isExt)
    {
        ros::spinOnce();
        initDur.sleep();
    }
    ROS_INFO("Ros shutdown, proceeding to close the gui.");
}

void RosThread::setServicesName(std::string srvname)
{
    if(!m_subMap.contains(srvname))
    {
        ros::Subscriber sub = n_->subscribe(srvname, 1000, &RosThread::subRecfgCallBack, this);
        m_subMap.insert(srvname, sub);
    }
    else
        return;
}

void RosThread::shutServicesUpdate(std::string srvname)
{
    if(m_subMap.contains(srvname))
    {
        m_subMap[srvname].shutdown();
        m_subMap.remove(srvname);
    }
    else
        return;
}

void RosThread::subRecfgCallBack(const ros::MessageEvent<dynamic_reconfigure::Config const> &event)
{
    const std::string &pub_name = event.getPublisherName();
    const dynamic_reconfigure::ConfigConstPtr &msg = event.getMessage();
    emit signal_recfg_msg(QString::fromStdString(pub_name), msg);
}

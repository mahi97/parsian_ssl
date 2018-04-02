//#line 2 "/opt/ros/kinetic/share/dynamic_reconfigure/cmake/../templates/ConfigType.h.template"
// *********************************************************
// 
// File autogenerated for the behavior_server_config package 
// by the dynamic_reconfigure package.
// Please do not edit.
// 
// ********************************************************/

#ifndef __behavior_server_config__BEHAVIOR_SERVERCONFIG_H__
#define __behavior_server_config__BEHAVIOR_SERVERCONFIG_H__

#include <dynamic_reconfigure/config_tools.h>
#include <limits>
#include <ros/node_handle.h>
#include <dynamic_reconfigure/ConfigDescription.h>
#include <dynamic_reconfigure/ParamDescription.h>
#include <dynamic_reconfigure/Group.h>
#include <dynamic_reconfigure/config_init_mutex.h>
#include <boost/any.hpp>

namespace behavior_server_config
{
  class behavior_serverConfigStatics;
  
  class behavior_serverConfig
  {
  public:
    class AbstractParamDescription : public dynamic_reconfigure::ParamDescription
    {
    public:
      AbstractParamDescription(std::string n, std::string t, uint32_t l, 
          std::string d, std::string e)
      {
        name = n;
        type = t;
        level = l;
        description = d;
        edit_method = e;
      }
      
      virtual void clamp(behavior_serverConfig &config, const behavior_serverConfig &max, const behavior_serverConfig &min) const = 0;
      virtual void calcLevel(uint32_t &level, const behavior_serverConfig &config1, const behavior_serverConfig &config2) const = 0;
      virtual void fromServer(const ros::NodeHandle &nh, behavior_serverConfig &config) const = 0;
      virtual void toServer(const ros::NodeHandle &nh, const behavior_serverConfig &config) const = 0;
      virtual bool fromMessage(const dynamic_reconfigure::Config &msg, behavior_serverConfig &config) const = 0;
      virtual void toMessage(dynamic_reconfigure::Config &msg, const behavior_serverConfig &config) const = 0;
      virtual void getValue(const behavior_serverConfig &config, boost::any &val) const = 0;
    };

    typedef boost::shared_ptr<AbstractParamDescription> AbstractParamDescriptionPtr;
    typedef boost::shared_ptr<const AbstractParamDescription> AbstractParamDescriptionConstPtr;
    
    template <class T>
    class ParamDescription : public AbstractParamDescription
    {
    public:
      ParamDescription(std::string a_name, std::string a_type, uint32_t a_level, 
          std::string a_description, std::string a_edit_method, T behavior_serverConfig::* a_f) :
        AbstractParamDescription(a_name, a_type, a_level, a_description, a_edit_method),
        field(a_f)
      {}

      T (behavior_serverConfig::* field);

      virtual void clamp(behavior_serverConfig &config, const behavior_serverConfig &max, const behavior_serverConfig &min) const
      {
        if (config.*field > max.*field)
          config.*field = max.*field;
        
        if (config.*field < min.*field)
          config.*field = min.*field;
      }

      virtual void calcLevel(uint32_t &comb_level, const behavior_serverConfig &config1, const behavior_serverConfig &config2) const
      {
        if (config1.*field != config2.*field)
          comb_level |= level;
      }

      virtual void fromServer(const ros::NodeHandle &nh, behavior_serverConfig &config) const
      {
        nh.getParam(name, config.*field);
      }

      virtual void toServer(const ros::NodeHandle &nh, const behavior_serverConfig &config) const
      {
        nh.setParam(name, config.*field);
      }

      virtual bool fromMessage(const dynamic_reconfigure::Config &msg, behavior_serverConfig &config) const
      {
        return dynamic_reconfigure::ConfigTools::getParameter(msg, name, config.*field);
      }

      virtual void toMessage(dynamic_reconfigure::Config &msg, const behavior_serverConfig &config) const
      {
        dynamic_reconfigure::ConfigTools::appendParameter(msg, name, config.*field);
      }

      virtual void getValue(const behavior_serverConfig &config, boost::any &val) const
      {
        val = config.*field;
      }
    };

    class AbstractGroupDescription : public dynamic_reconfigure::Group
    {
      public:
      AbstractGroupDescription(std::string n, std::string t, int p, int i, bool s)
      {
        name = n;
        type = t;
        parent = p;
        state = s;
        id = i;
      }

      std::vector<AbstractParamDescriptionConstPtr> abstract_parameters;
      bool state;

      virtual void toMessage(dynamic_reconfigure::Config &msg, const boost::any &config) const = 0;
      virtual bool fromMessage(const dynamic_reconfigure::Config &msg, boost::any &config) const =0;
      virtual void updateParams(boost::any &cfg, behavior_serverConfig &top) const= 0;
      virtual void setInitialState(boost::any &cfg) const = 0;


      void convertParams()
      {
        for(std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = abstract_parameters.begin(); i != abstract_parameters.end(); ++i)
        {
          parameters.push_back(dynamic_reconfigure::ParamDescription(**i));
        }
      }
    };

    typedef boost::shared_ptr<AbstractGroupDescription> AbstractGroupDescriptionPtr;
    typedef boost::shared_ptr<const AbstractGroupDescription> AbstractGroupDescriptionConstPtr;

    template<class T, class PT>
    class GroupDescription : public AbstractGroupDescription
    {
    public:
      GroupDescription(std::string a_name, std::string a_type, int a_parent, int a_id, bool a_s, T PT::* a_f) : AbstractGroupDescription(a_name, a_type, a_parent, a_id, a_s), field(a_f)
      {
      }

      GroupDescription(const GroupDescription<T, PT>& g): AbstractGroupDescription(g.name, g.type, g.parent, g.id, g.state), field(g.field), groups(g.groups)
      {
        parameters = g.parameters;
        abstract_parameters = g.abstract_parameters;
      }

      virtual bool fromMessage(const dynamic_reconfigure::Config &msg, boost::any &cfg) const
      {
        PT* config = boost::any_cast<PT*>(cfg);
        if(!dynamic_reconfigure::ConfigTools::getGroupState(msg, name, (*config).*field))
          return false;

        for(std::vector<AbstractGroupDescriptionConstPtr>::const_iterator i = groups.begin(); i != groups.end(); ++i)
        {
          boost::any n = &((*config).*field);
          if(!(*i)->fromMessage(msg, n))
            return false;
        }

        return true;
      }

      virtual void setInitialState(boost::any &cfg) const
      {
        PT* config = boost::any_cast<PT*>(cfg);
        T* group = &((*config).*field);
        group->state = state;

        for(std::vector<AbstractGroupDescriptionConstPtr>::const_iterator i = groups.begin(); i != groups.end(); ++i)
        {
          boost::any n = boost::any(&((*config).*field));
          (*i)->setInitialState(n);
        }

      }

      virtual void updateParams(boost::any &cfg, behavior_serverConfig &top) const
      {
        PT* config = boost::any_cast<PT*>(cfg);

        T* f = &((*config).*field);
        f->setParams(top, abstract_parameters);

        for(std::vector<AbstractGroupDescriptionConstPtr>::const_iterator i = groups.begin(); i != groups.end(); ++i)
        {
          boost::any n = &((*config).*field);
          (*i)->updateParams(n, top);
        }
      }

      virtual void toMessage(dynamic_reconfigure::Config &msg, const boost::any &cfg) const
      {
        const PT config = boost::any_cast<PT>(cfg);
        dynamic_reconfigure::ConfigTools::appendGroup<T>(msg, name, id, parent, config.*field);

        for(std::vector<AbstractGroupDescriptionConstPtr>::const_iterator i = groups.begin(); i != groups.end(); ++i)
        {
          (*i)->toMessage(msg, config.*field);
        }
      }

      T (PT::* field);
      std::vector<behavior_serverConfig::AbstractGroupDescriptionConstPtr> groups;
    };
    
class DEFAULT
{
  public:
    DEFAULT()
    {
      state = true;
      name = "Default";
    }

    void setParams(behavior_serverConfig &config, const std::vector<AbstractParamDescriptionConstPtr> params)
    {
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator _i = params.begin(); _i != params.end(); ++_i)
      {
        boost::any val;
        (*_i)->getValue(config, val);


      }
    }

    

    bool state;
    std::string name;

    class BEHAVIOR_SERVER
{
  public:
    BEHAVIOR_SERVER()
    {
      state = true;
      name = "Behavior_Server";
    }

    void setParams(behavior_serverConfig &config, const std::vector<AbstractParamDescriptionConstPtr> params)
    {
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator _i = params.begin(); _i != params.end(); ++_i)
      {
        boost::any val;
        (*_i)->getValue(config, val);

        if("threshold_amount"==(*_i)->name){threshold_amount = boost::any_cast<double>(val);}
        if("queue_size"==(*_i)->name){queue_size = boost::any_cast<double>(val);}
      }
    }

    double threshold_amount;
double queue_size;

    bool state;
    std::string name;

    
}behavior_server;

class BOUNDARIES
{
  public:
    BOUNDARIES()
    {
      state = true;
      name = "Boundaries";
    }

    void setParams(behavior_serverConfig &config, const std::vector<AbstractParamDescriptionConstPtr> params)
    {
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator _i = params.begin(); _i != params.end(); ++_i)
      {
        boost::any val;
        (*_i)->getValue(config, val);

        if("upper_boundary"==(*_i)->name){upper_boundary = boost::any_cast<double>(val);}
        if("lower_boundary"==(*_i)->name){lower_boundary = boost::any_cast<double>(val);}
      }
    }

    double upper_boundary;
double lower_boundary;

    bool state;
    std::string name;

    
}boundaries;

class MAHI
{
  public:
    MAHI()
    {
      state = true;
      name = "mahi";
    }

    void setParams(behavior_serverConfig &config, const std::vector<AbstractParamDescriptionConstPtr> params)
    {
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator _i = params.begin(); _i != params.end(); ++_i)
      {
        boost::any val;
        (*_i)->getValue(config, val);

        if("reward"==(*_i)->name){reward = boost::any_cast<double>(val);}
        if("penalty"==(*_i)->name){penalty = boost::any_cast<double>(val);}
      }
    }

    double reward;
double penalty;

    bool state;
    std::string name;

    
}mahi;

}groups;



//#line 9 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      double threshold_amount;
//#line 10 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      double queue_size;
//#line 13 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      double upper_boundary;
//#line 14 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      double lower_boundary;
//#line 17 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      double reward;
//#line 18 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      double penalty;
//#line 218 "/opt/ros/kinetic/share/dynamic_reconfigure/cmake/../templates/ConfigType.h.template"

    bool __fromMessage__(dynamic_reconfigure::Config &msg)
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      const std::vector<AbstractGroupDescriptionConstPtr> &__group_descriptions__ = __getGroupDescriptions__();

      int count = 0;
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); ++i)
        if ((*i)->fromMessage(msg, *this))
          count++;

      for (std::vector<AbstractGroupDescriptionConstPtr>::const_iterator i = __group_descriptions__.begin(); i != __group_descriptions__.end(); i ++)
      {
        if ((*i)->id == 0)
        {
          boost::any n = boost::any(this);
          (*i)->updateParams(n, *this);
          (*i)->fromMessage(msg, n);
        }
      }

      if (count != dynamic_reconfigure::ConfigTools::size(msg))
      {
        ROS_ERROR("behavior_serverConfig::__fromMessage__ called with an unexpected parameter.");
        ROS_ERROR("Booleans:");
        for (unsigned int i = 0; i < msg.bools.size(); i++)
          ROS_ERROR("  %s", msg.bools[i].name.c_str());
        ROS_ERROR("Integers:");
        for (unsigned int i = 0; i < msg.ints.size(); i++)
          ROS_ERROR("  %s", msg.ints[i].name.c_str());
        ROS_ERROR("Doubles:");
        for (unsigned int i = 0; i < msg.doubles.size(); i++)
          ROS_ERROR("  %s", msg.doubles[i].name.c_str());
        ROS_ERROR("Strings:");
        for (unsigned int i = 0; i < msg.strs.size(); i++)
          ROS_ERROR("  %s", msg.strs[i].name.c_str());
        // @todo Check that there are no duplicates. Make this error more
        // explicit.
        return false;
      }
      return true;
    }

    // This version of __toMessage__ is used during initialization of
    // statics when __getParamDescriptions__ can't be called yet.
    void __toMessage__(dynamic_reconfigure::Config &msg, const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__, const std::vector<AbstractGroupDescriptionConstPtr> &__group_descriptions__) const
    {
      dynamic_reconfigure::ConfigTools::clear(msg);
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); ++i)
        (*i)->toMessage(msg, *this);

      for (std::vector<AbstractGroupDescriptionConstPtr>::const_iterator i = __group_descriptions__.begin(); i != __group_descriptions__.end(); ++i)
      {
        if((*i)->id == 0)
        {
          (*i)->toMessage(msg, *this);
        }
      }
    }
    
    void __toMessage__(dynamic_reconfigure::Config &msg) const
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      const std::vector<AbstractGroupDescriptionConstPtr> &__group_descriptions__ = __getGroupDescriptions__();
      __toMessage__(msg, __param_descriptions__, __group_descriptions__);
    }
    
    void __toServer__(const ros::NodeHandle &nh) const
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); ++i)
        (*i)->toServer(nh, *this);
    }

    void __fromServer__(const ros::NodeHandle &nh)
    {
      static bool setup=false;

      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); ++i)
        (*i)->fromServer(nh, *this);

      const std::vector<AbstractGroupDescriptionConstPtr> &__group_descriptions__ = __getGroupDescriptions__();
      for (std::vector<AbstractGroupDescriptionConstPtr>::const_iterator i = __group_descriptions__.begin(); i != __group_descriptions__.end(); i++){
        if (!setup && (*i)->id == 0) {
          setup = true;
          boost::any n = boost::any(this);
          (*i)->setInitialState(n);
        }
      }
    }

    void __clamp__()
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      const behavior_serverConfig &__max__ = __getMax__();
      const behavior_serverConfig &__min__ = __getMin__();
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); ++i)
        (*i)->clamp(*this, __max__, __min__);
    }

    uint32_t __level__(const behavior_serverConfig &config) const
    {
      const std::vector<AbstractParamDescriptionConstPtr> &__param_descriptions__ = __getParamDescriptions__();
      uint32_t level = 0;
      for (std::vector<AbstractParamDescriptionConstPtr>::const_iterator i = __param_descriptions__.begin(); i != __param_descriptions__.end(); ++i)
        (*i)->calcLevel(level, config, *this);
      return level;
    }
    
    static const dynamic_reconfigure::ConfigDescription &__getDescriptionMessage__();
    static const behavior_serverConfig &__getDefault__();
    static const behavior_serverConfig &__getMax__();
    static const behavior_serverConfig &__getMin__();
    static const std::vector<AbstractParamDescriptionConstPtr> &__getParamDescriptions__();
    static const std::vector<AbstractGroupDescriptionConstPtr> &__getGroupDescriptions__();
    
  private:
    static const behavior_serverConfigStatics *__get_statics__();
  };
  
  template <> // Max and min are ignored for strings.
  inline void behavior_serverConfig::ParamDescription<std::string>::clamp(behavior_serverConfig &config, const behavior_serverConfig &max, const behavior_serverConfig &min) const
  {
    (void) config;
    (void) min;
    (void) max;
    return;
  }

  class behavior_serverConfigStatics
  {
    friend class behavior_serverConfig;
    
    behavior_serverConfigStatics()
    {
behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT, behavior_serverConfig> Default("Default", "", 0, 0, true, &behavior_serverConfig::groups);
behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::BEHAVIOR_SERVER, behavior_serverConfig::DEFAULT> Behavior_Server("Behavior_Server", "", 0, 1, true, &behavior_serverConfig::DEFAULT::behavior_server);
//#line 9 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __min__.threshold_amount = 0.0;
//#line 9 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __max__.threshold_amount = 1.0;
//#line 9 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __default__.threshold_amount = 0.1;
//#line 9 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      Behavior_Server.abstract_parameters.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("threshold_amount", "double", 0, "threshold amount", "", &behavior_serverConfig::threshold_amount)));
//#line 9 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __param_descriptions__.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("threshold_amount", "double", 0, "threshold amount", "", &behavior_serverConfig::threshold_amount)));
//#line 10 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __min__.queue_size = 1.0;
//#line 10 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __max__.queue_size = 60.0;
//#line 10 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __default__.queue_size = 5.0;
//#line 10 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      Behavior_Server.abstract_parameters.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("queue_size", "double", 0, "queue size", "", &behavior_serverConfig::queue_size)));
//#line 10 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __param_descriptions__.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("queue_size", "double", 0, "queue size", "", &behavior_serverConfig::queue_size)));
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      Behavior_Server.convertParams();
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      Default.groups.push_back(behavior_serverConfig::AbstractGroupDescriptionConstPtr(new behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::BEHAVIOR_SERVER, behavior_serverConfig::DEFAULT>(Behavior_Server)));
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      __group_descriptions__.push_back(behavior_serverConfig::AbstractGroupDescriptionConstPtr(new behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::BEHAVIOR_SERVER, behavior_serverConfig::DEFAULT>(Behavior_Server)));
behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::BOUNDARIES, behavior_serverConfig::DEFAULT> Boundaries("Boundaries", "", 0, 2, true, &behavior_serverConfig::DEFAULT::boundaries);
//#line 13 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __min__.upper_boundary = 0.6;
//#line 13 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __max__.upper_boundary = 1.0;
//#line 13 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __default__.upper_boundary = 0.9;
//#line 13 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      Boundaries.abstract_parameters.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("upper_boundary", "double", 0, "Upper Boundary for Plans Probability", "", &behavior_serverConfig::upper_boundary)));
//#line 13 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __param_descriptions__.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("upper_boundary", "double", 0, "Upper Boundary for Plans Probability", "", &behavior_serverConfig::upper_boundary)));
//#line 14 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __min__.lower_boundary = 0.0;
//#line 14 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __max__.lower_boundary = 0.5;
//#line 14 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __default__.lower_boundary = 0.1;
//#line 14 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      Boundaries.abstract_parameters.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("lower_boundary", "double", 0, "Lower Boundary for Plans Probability", "", &behavior_serverConfig::lower_boundary)));
//#line 14 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __param_descriptions__.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("lower_boundary", "double", 0, "Lower Boundary for Plans Probability", "", &behavior_serverConfig::lower_boundary)));
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      Boundaries.convertParams();
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      Default.groups.push_back(behavior_serverConfig::AbstractGroupDescriptionConstPtr(new behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::BOUNDARIES, behavior_serverConfig::DEFAULT>(Boundaries)));
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      __group_descriptions__.push_back(behavior_serverConfig::AbstractGroupDescriptionConstPtr(new behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::BOUNDARIES, behavior_serverConfig::DEFAULT>(Boundaries)));
behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::MAHI, behavior_serverConfig::DEFAULT> mahi("mahi", "", 0, 3, true, &behavior_serverConfig::DEFAULT::mahi);
//#line 17 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __min__.reward = 0.0;
//#line 17 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __max__.reward = 1.0;
//#line 17 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __default__.reward = 1.0;
//#line 17 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      mahi.abstract_parameters.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("reward", "double", 0, "reward", "", &behavior_serverConfig::reward)));
//#line 17 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __param_descriptions__.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("reward", "double", 0, "reward", "", &behavior_serverConfig::reward)));
//#line 18 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __min__.penalty = 0.0;
//#line 18 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __max__.penalty = 1.0;
//#line 18 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __default__.penalty = 0.0;
//#line 18 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      mahi.abstract_parameters.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("penalty", "double", 0, "penalty", "", &behavior_serverConfig::penalty)));
//#line 18 "/home/hamidhamidrezaroodabeh/Desktop/parsian_ws/src/parsian_ssl/parsian_ai/cfg/behavior_server.cfg"
      __param_descriptions__.push_back(behavior_serverConfig::AbstractParamDescriptionConstPtr(new behavior_serverConfig::ParamDescription<double>("penalty", "double", 0, "penalty", "", &behavior_serverConfig::penalty)));
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      mahi.convertParams();
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      Default.groups.push_back(behavior_serverConfig::AbstractGroupDescriptionConstPtr(new behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::MAHI, behavior_serverConfig::DEFAULT>(mahi)));
//#line 123 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      __group_descriptions__.push_back(behavior_serverConfig::AbstractGroupDescriptionConstPtr(new behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT::MAHI, behavior_serverConfig::DEFAULT>(mahi)));
//#line 246 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      Default.convertParams();
//#line 246 "/opt/ros/kinetic/lib/python2.7/dist-packages/dynamic_reconfigure/parameter_generator_catkin.py"
      __group_descriptions__.push_back(behavior_serverConfig::AbstractGroupDescriptionConstPtr(new behavior_serverConfig::GroupDescription<behavior_serverConfig::DEFAULT, behavior_serverConfig>(Default)));
//#line 356 "/opt/ros/kinetic/share/dynamic_reconfigure/cmake/../templates/ConfigType.h.template"

      for (std::vector<behavior_serverConfig::AbstractGroupDescriptionConstPtr>::const_iterator i = __group_descriptions__.begin(); i != __group_descriptions__.end(); ++i)
      {
        __description_message__.groups.push_back(**i);
      }
      __max__.__toMessage__(__description_message__.max, __param_descriptions__, __group_descriptions__); 
      __min__.__toMessage__(__description_message__.min, __param_descriptions__, __group_descriptions__); 
      __default__.__toMessage__(__description_message__.dflt, __param_descriptions__, __group_descriptions__); 
    }
    std::vector<behavior_serverConfig::AbstractParamDescriptionConstPtr> __param_descriptions__;
    std::vector<behavior_serverConfig::AbstractGroupDescriptionConstPtr> __group_descriptions__;
    behavior_serverConfig __max__;
    behavior_serverConfig __min__;
    behavior_serverConfig __default__;
    dynamic_reconfigure::ConfigDescription __description_message__;

    static const behavior_serverConfigStatics *get_instance()
    {
      // Split this off in a separate function because I know that
      // instance will get initialized the first time get_instance is
      // called, and I am guaranteeing that get_instance gets called at
      // most once.
      static behavior_serverConfigStatics instance;
      return &instance;
    }
  };

  inline const dynamic_reconfigure::ConfigDescription &behavior_serverConfig::__getDescriptionMessage__() 
  {
    return __get_statics__()->__description_message__;
  }

  inline const behavior_serverConfig &behavior_serverConfig::__getDefault__()
  {
    return __get_statics__()->__default__;
  }
  
  inline const behavior_serverConfig &behavior_serverConfig::__getMax__()
  {
    return __get_statics__()->__max__;
  }
  
  inline const behavior_serverConfig &behavior_serverConfig::__getMin__()
  {
    return __get_statics__()->__min__;
  }
  
  inline const std::vector<behavior_serverConfig::AbstractParamDescriptionConstPtr> &behavior_serverConfig::__getParamDescriptions__()
  {
    return __get_statics__()->__param_descriptions__;
  }

  inline const std::vector<behavior_serverConfig::AbstractGroupDescriptionConstPtr> &behavior_serverConfig::__getGroupDescriptions__()
  {
    return __get_statics__()->__group_descriptions__;
  }

  inline const behavior_serverConfigStatics *behavior_serverConfig::__get_statics__()
  {
    const static behavior_serverConfigStatics *statics;
  
    if (statics) // Common case
      return statics;

    boost::mutex::scoped_lock lock(dynamic_reconfigure::__init_mutex__);

    if (statics) // In case we lost a race.
      return statics;

    statics = behavior_serverConfigStatics::get_instance();
    
    return statics;
  }


}

#endif // __BEHAVIOR_SERVERRECONFIGURATOR_H__

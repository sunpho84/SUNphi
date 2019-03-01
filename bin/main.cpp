//#define DEBUG_TENS
//#define DEBUG_BINDER
//#define DEBUG_INDEXER
//#define DEBUG_REL_BINDER

#include <SUNphi.hpp>

#include <yaml-cpp/yaml.h>

#include <iostream>

using namespace std;
using namespace SUNphi;

namespace SUNphi
{
  /// Test1
  class Test
  {
  public:
    
    SERIALIZABLE_SCALAR(double,a,10.0);
    SERIALIZABLE_SCALAR(double,b,1);
    
    SERIALIZABLE_MEMBERS(a,b);
    
    Test()
    {
    }
  };
  
  /// Test2
  class Test2
  {
  public:
    
    SERIALIZABLE_SCALAR(Test,test,NO_DEFAULT);
    SERIALIZABLE_SCALAR(std::string,ciccio,"ciaccio");
    
    SERIALIZABLE_MEMBERS(test,ciccio);
  };
}

// namespace SUNphi
// {
//   DEFINE_HAS_MEMBER(serialRead);
// }

// template <typename...Ts>
// class SerializingList
//   : public Tuple<Ts...>
// {
// public:
//   size_t mappedSize=
//     (sizeof(RemRef<Ts>) + ...);
// };

// template <typename...Ts,
// 	  typename=EnableIf<(isSerializable<Ts> && ...)>>
// auto serializingList(Ts&&...ts)
// {
//   return
//     SerializingList<Ts...>{{ts...}};
// }

// class Test
// {
// public:
//   Serializable<double> d{"d",0.0};
//   Serializable<int> i{"i",0};
//   int j;
  
//   decltype(serializingList(d,i)) ser{serializingList(d,i)};
// };

// Test test;

template <typename T,
	  typename Tdef>
YAML::Node& operator<<(YAML::Node& node,
		       const SerializableScalar<T,Tdef>& t)
{
  if constexpr(isSerializableClass<T>)
    {
      YAML::Node subNode;
      
      forEach(t().serializableMembers,
	      [&subNode](auto s)
	      {
		subNode<<s;
	      });
      
      node[t.name]=
	subNode;
    }
  else
    node[t.name]=t();
  
  return node;
}

// template <typename T,
  // 	  typename=EnableIf<isSerializableMap<T>>>
  // YAML::Node& operator<<(YAML::Node& node,
  // 		       const T& t)
  // {
  //   if(not t.isDefault())
  //     node[t.name]<<static_cast<const SerializableMap<T>>(t());
  
  //   return node;
  // }
  
namespace YAML
{
  /// Serializable scalar conversion to YAML node
  template<typename T>
  struct convert<SerializableScalar<T>>
  {
    static Node encode(const SerializableScalar<T>& rhs)
    {
      Node node;
      node<<~rhs;
      
      return node;
    }
    
    static bool decode(const Node& node,SerializableScalar<T>& rhs)
    {
      node>>rhs.a;
      
      return true;
    }
  };
}

int main()
{
  // Serializable<double> ciccio("ciccio",10.0);
  // // (*static_cast<Logger::LoggerLine*>(nullptr))<<(*static_cast<RemRef<SerializeWrapper<double>>*>(nullptr));
  // ciccio=10.0;
  // // decltype(*static_cast<Logger*>(nullptr)<<double{}) &a="";
  
  // std::ostringstream os;
  // os<<std::mt19937_64{};
  // runLog()<<os.str();
  
  // runLog()<<ciccio;
  
  // runLog()<<canPrint<Logger,Serializable<double>>;
  
  // std::get<0>(test.ser)=
  //   10.0;
  
  // runLog()<<test.ser.mappedSize;
  // runLog()<<test.d;
  
  YAML::Node node;
  
  SerializableScalar<Test> test{"test",{}};
  SerializableScalar<Test2> t2{"t2",NO_DEFAULT};
  
  test().a=11.0;
  runLog()<<test().a();
  node<<t2;
  
  YAML::Emitter emitter;
  emitter<<node;
  
  runLog()<<emitter.c_str();
  
  // double d;
  // int i;

  // auto l=serList(d,"d",10.0,
  // 		 i,"i",10);
  // runLog()<<((&std::get<0>(std::get<0>(l)))!=&d);
  
  SerializableScalar<double> _aref("a",NO_DEFAULT);
  
  runLog()<<isSerializableScalar<SerializableScalar<double>>;
  
  return 0;
}

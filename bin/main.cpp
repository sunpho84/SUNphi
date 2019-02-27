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
  template <typename T,
	    typename Tdef>
  auto ser(T&& t,
	   const char* name,
	   const Tdef& def)
  {
    return
      make_tuple(std::ref(t),name,def);
  }
  
  template <typename Tp,
	    typename T,
	    typename Tdef,
	    typename...Tail>
  auto _serList(Tp&& tp,
	       T&& t,
	       const char* name,
	       const Tdef& def,
	       Tail&&...tail)
  {
    auto tmp=
      std::tuple_cat(tp,make_tuple(ser(forw<T>(t),name,def)));
    
    if constexpr(sizeof...(tail)>0)
		  {
		    return
		      _serList(tmp,forw<Tail>(tail)...);
		  }
    else
      return
	tmp;
  }
  
  template <typename...Args>
  auto serList(Args&&...args)
  {
    std::tuple<> tmp;
    
    if constexpr(sizeof...(args)>0)
		  return _serList(tmp,forw<Args>(args)...);
    else
      return
	tmp;
  }
  
  DEFINE_BASE_TYPE(SerializableMap);
  
  template <typename T>
  class SerializableMap
  {
  public:
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    const auto getSeri()
      const
    {
      return
	(~(*this)).getSeri();
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(getSeri);
    
    SerializableMap<T>()
    {
      std::apply([](auto...s){((std::get<0>(s)=std::get<2>(s)) , ...);},(~(*this)).getSeri());
    }
    
  };
  
  class Test
    : public SerializableMap<Test>
    , public BaseSerializableMap
  {
  public:
    double a;
    
    auto getSeri()
    {
      return
	serList(a,"a",10.0);
    }
    
    auto getSeri()
          const
    {
      return
	serList(a,"a",10.0);
    }
    
    //PROVIDE_ALSO_NON_CONST_METHOD(getSeri);
    
    // bool operator==(const Test& oth)
    //   const
    // {
    //   return
    //     getSeri()==oth.getSeri();
    // }
    
    Test()
    {
    }
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

//static_assert(isSerializable<SerializeWrapper<double>>,"The serializer is not such");

namespace YAML
{
  template <typename T>
  const auto& castToSerializableMap(const SerializableMap<T>& in)
  {
    return
      in;
  }
  
  
template <typename T>
YAML::Node& operator<<(YAML::Node& node,
		       const Serializable<T>& t)
{
  if constexpr(isSerializableMap<T>)
		node[t.name]=castToSerializableMap(t());
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

  template<typename T>
  struct convert<Serializable<T>>
  {
    static Node encode(const Serializable<T>& rhs)
    {
      Node node;
      node<<~rhs;
      
      return node;
    }
    
    static bool decode(const Node& node,Serializable<T>& rhs)
    {
      node>>rhs.a;
      
      return true;
    }
  };
  
  template<typename T>
  struct convert<SerializableMap<T>>
  {
    static Node encode(const SerializableMap<T>& rhs)
    {
      Node node;
      
      std::apply([&node](auto...s)
		 {
		   ((node[std::get<1>(s)]=std::get<0>(s)) , ...);
		 },
		 rhs.getSeri());
      
      return node;
    }
    
    static bool decode(const Node& node,SerializableMap<T>& rhs)
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
  
  Serializable<Test> test{"test",{}};
  
  test().a=11.0;
  
  node<<test;
  
  YAML::Emitter emitter;
  emitter<<node;
  
  runLog()<<emitter.c_str();
  
  // double d;
  // int i;

  // auto l=serList(d,"d",10.0,
  // 		 i,"i",10);
  // runLog()<<((&std::get<0>(std::get<0>(l)))!=&d);
  
  return 0;
}

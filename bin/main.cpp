//#define DEBUG_TENS
//#define DEBUG_BINDER
//#define DEBUG_INDEXER
//#define DEBUG_REL_BINDER

#include <SUNphi.hpp>

#include <iostream>

using namespace std;
using namespace SUNphi;

namespace SUNphi
{
  /// Test1
  class Test :
    public SerializableClass<Test>
  {
  public:
    using E=std::pair<double,double>;
    using F=std::vector<double>;
    //decltype(_serializableScalar(static_cast<E*>(nullptr),"a",10.0,20.0)) a{"a",10.0,20.0};

    SerializableScalar<E> a{"a",{10.0,'c'}};
    
    //SERIALIZABLE_SCALAR(F,c,{10,20});
    //SERIALIZABLE_VECTOR(double,v,10);
    SerializableScalar<double> b{"b",1};
    
    SERIALIZABLE_MEMBERS(a,// v,
			 b);
    
    Test()
    {
    }
  };
  
  /// Test2
  class Test2 :
    public SerializableClass<Test2>
  {
  public:
    
    SerializableScalar<Test,NO_DEFAULT> test{"test"};
    SerializableScalar<std::string> ciccio{"ciccio","ciaccio"};
    
    SERIALIZABLE_MEMBERS(test,ciccio);
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
  
  
  // SerializableScalar<Test> test{"test",{}};
  SerializableScalar<Test2,NO_DEFAULT> t2{"t2"};
  
  // test().a=11.0;
  // runLog()<<test().a();
  
  Serializer ser;
  ser<<t2;
  
  DeSerializer deSer(ser.get().c_str());
  
  deSer>>t2;
  
  Serializer ser2;
  ser2<<t2;
  
  runLog()<<t2;
  
  // Binarizer bin=t2.binarize();
  // runLog()<<bin.size();

  // t2().ciccio="scompigliato";
  // t2().test().a=1235;
  // ser2<<t2;
  // runLog()<<ser2.get();

  // 	    t2.deBinarize(bin);
  // Serializer ser3;
  // ser3<<t2;
  // runLog()<<ser3.get();
  
  
  // double d;
  // int i;

  // auto l=serList(d,"d",10.0,
  // 		 i,"i",10);
  // runLog()<<((&std::get<0>(std::get<0>(l)))!=&d);
  
  // SerializableScalar<double> _aref("a",NO_DEFAULT);
  
  // runLog()<<isSerializableScalar<SerializableScalar<double>>;
  
  return 0;
}

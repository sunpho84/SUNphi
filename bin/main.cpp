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
  //Test1
  SERIALIZABLE_CLASS(Test)
  {
  public:
    using P=std::pair<std::string,std::string>;
    using F=std::vector<double>;
    //decltype(_serializableScalar(static_cast<E*>(nullptr),"a",10.0,20.0)) a{"a",10.0,20.0};
    
    //SERIALIZABLE_SEQUENCE(P,a,{"fi","se"});
    
    //SERIALIZABLE_SCALAR(F,c,{10,20});
    SERIALIZABLE_VECTOR(double,v,10);
    SERIALIZABLE_SCALAR(double,b,1.0);
    
    LIST_SERIALIZABLE_MEMBERS(// a,
			      v,b);
    
    // Test& operator=(const Test& oth)
    // {
    //   serializableMembers()=
    // 	oth.serializableMembers();
      
    //   return
    // 	*this;
    // }
    
    Test()
    {
    }
  };
  
  /// Test2
  SERIALIZABLE_CLASS(Test2)
  {
  public:
    
    Serializable<Test> test{"test"};
    Serializable<std::string> ciccio{"ciccio","ciaccio"};
    
    //Serializable<std::vector<Test>> v{"v",std::vector<Test>(10)};
    //SerializableScalar<Test,NO_DEFAULT> v{"v"};
    LIST_SERIALIZABLE_MEMBERS(test,ciccio);//,v);
  };
}

int main()
{
  Test2 test1,test2;
  
  //test1.v().resize(1);
  
  runLog()<<"Txt";
  runLog()<<"Full:";
  {
    SCOPE_INDENT(runLog);
    runLog()<<test1;
  }
  runLog()<<"test1 isDefault: "<<test1.isDefault();
  
  runLog()<<"NonDef:";
  {
    SCOPE_INDENT(runLog);
    runLog()<<test1.serialize(ONLY_NON_DEFAULT);
  }
  
  test1.ciccio()=
    "first";
  
  runLog()<<"test1 isDefault: "<<test1.isDefault();
  
  runLog()<<"NonDef:";
  {
    SCOPE_INDENT(runLog);
    runLog()<<test1.serialize(ONLY_NON_DEFAULT);
  }
  
  test2.deSerialize(test1.serialize(ONLY_NON_DEFAULT));
  
  {
    SCOPE_INDENT(runLog);
    runLog()<<test2;
  }
  
  // test1.ciccio()=
  //    "second";
  
  test2.deBinarize(test1.binarize());
  
  runLog()<<"Bin";
  
  {
    SCOPE_INDENT(runLog);
    runLog()<<test2;
  }
  
  // runLog()<<(void*)&(test1.test().a().first[0])<<" "<<(void*)&(test2.test().a().first[0]);
  
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
  // Serializable<Test2,NO_DEFAULT> t2{"t2"};
  
  // YAML::Emitter out;
  
  // out<<t2.serialize();
  
  // runLog()<<out.c_str();

  // // test().a=11.0;
  // // runLog()<<test().a();
  
  // Serializer ser;
  // ser<<t2;
  
  // DeSerializer deSer(ser.get().c_str());
  
  // deSer>>t2;
  
  // Serializer ser2;
  // ser2<<t2;
  
  // //runLog()<<t2;
  
  // // Binarizer bin=t2.binarize();
  // // runLog()<<bin.size();

  // // t2().ciccio="scompigliato";
  // // t2().test().a=1235;
  // // ser2<<t2;
  // // runLog()<<ser2.get();

  // // 	    t2.deBinarize(bin);
  // // Serializer ser3;
  // // ser3<<t2;
  // // runLog()<<ser3.get();
  
  
  // // double d;
  // // int i;

  // // auto l=serList(d,"d",10.0,
  // // 		 i,"i",10);
  // // runLog()<<((&std::get<0>(std::get<0>(l)))!=&d);
  
  // // SerializableScalar<double> _aref("a",NO_DEFAULT);
  
  // // runLog()<<isSerializableScalar<SerializableScalar<double>>;
  
  return 0;
}
